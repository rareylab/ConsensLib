#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

#include <gtest/gtest.h>

#include "ConsensLib/Consens.hpp"

#include "CheckSubgraphs.hpp"

class GraphNode;
template<bool sort>
class Graph;

using SortedGraph = Graph<true>;
using UnsortedGraph = Graph<false>;


class GraphNode {

  template<bool sort>
  friend class ::Graph;

public:

  unsigned getIndex() const
  {
    return m_idx;
  }

  const std::vector<const GraphNode*>& getNeigbors() const
  {
    return reinterpret_cast<const std::vector<const GraphNode*>&>(m_neighbors);
  }

private:

  GraphNode(unsigned idx)
    : m_idx(idx) {}

  void setNeighbors(const std::vector<GraphNode*>& neighbors)
  {
    m_neighbors = neighbors;
  }

  unsigned m_idx;
  std::vector<GraphNode*> m_neighbors;
};

bool operator==(std::vector<const GraphNode*> v1, std::vector<const GraphNode*> v2)
{
  if (v1.size() != v2.size()) {
    return false;
  }

  for (unsigned i = 0; i < v1.size(); ++i) {
    const GraphNode* n1 = v1.at(i);
    const GraphNode* n2 = v2.at(i);
    if (!n1 || !n2 || n1->getIndex() != n2->getIndex()) {
      return false;
    }
  }

  return true;
}

template<bool sort>
class Graph {

public:

  Graph(const std::unordered_map<unsigned, std::vector<unsigned>>& adjacency)
  {
    std::unordered_map<unsigned, GraphNode*> indexMap;
    indexMap.reserve(adjacency.size());
    m_nodes.reserve(adjacency.size());

    // iterate first time to add nodes
    for (const std::pair<unsigned, std::vector<unsigned>>& nodeAdjacency : adjacency) {
      GraphNode* node = new GraphNode(nodeAdjacency.first);
      indexMap.insert(std::make_pair(nodeAdjacency.first, node));
      m_nodes.push_back(node);
    }

    // iterate second time to add adjacency
    for (const std::pair<unsigned, std::vector<unsigned>>& nodeAdjacency : adjacency) {
      std::vector<GraphNode*> neighbors;
      neighbors.reserve(nodeAdjacency.second.size());
      if (sort) {
        std::vector<unsigned> copyVec = nodeAdjacency.second;
        std::sort(copyVec.begin(), copyVec.end());
        for (unsigned idx : copyVec) {
          neighbors.push_back(indexMap.at(idx));
        }
      }
      else {
        for (unsigned idx : nodeAdjacency.second) {
          neighbors.push_back(indexMap.at(idx));
        }
      }
      indexMap.at(nodeAdjacency.first)->setNeighbors(neighbors);
    }
  }

  const std::vector<const GraphNode*>& getNodes() const
  {
    return reinterpret_cast<const std::vector<const GraphNode*>&>(m_nodes);
  }

  ~Graph()
  {
    for (GraphNode* node : m_nodes) {
      delete (node);
    }
  }

private:

  std::vector<GraphNode*> m_nodes;

};

struct GraphNodeCompare {
  bool operator()(const GraphNode* n1, const GraphNode* n2) const
  {
    return n1->getIndex() < n2->getIndex();
  }
};

namespace ConsensLib {

template<bool sorted>
struct GraphTraits<Graph<sorted>> {

  using Node = const GraphNode*;
  using Iterator = std::vector<const GraphNode*>::const_iterator;

  static Iterator adjancencyBegin(
      const Node& node,
      const Graph<sorted>& graph)
  {
    return node->getNeigbors().begin();
  }

  static Iterator adjancencyEnd(
      const Node& node,
      const Graph<sorted>& graph)
  {
    return node->getNeigbors().end();
  }

  static Iterator nodesBegin(const Graph<sorted>& graph)
  {
    return graph.getNodes().begin();
  }

  static Iterator nodesEnd(const Graph<sorted>& graph)
  {
    return graph.getNodes().end();
  }

  static constexpr bool listsSorted() {
    return sorted;
  }
};
}

std::unordered_map<unsigned, std::vector<unsigned>> getAdjacency(size_t nofNodes)
{
  std::unordered_map<unsigned, std::vector<unsigned>> adjacency;
  adjacency.reserve(nofNodes);
  for (unsigned idx = 0; idx < nofNodes; ++idx) {
    std::vector<unsigned> neighbors;
    neighbors.reserve(nofNodes-1);
    for (unsigned neighborIdx = 0; neighborIdx < nofNodes; ++neighborIdx) {
      // neighbors with ascending index
      if (nofNodes - (neighborIdx + 1) != idx) {
        neighbors.push_back(nofNodes - (neighborIdx + 1));
      }
    }
    adjacency.insert(std::make_pair(idx, std::move(neighbors)));
  }
  return adjacency;
}

struct LessOrEqualFilter {
  bool operator()(const std::vector<const GraphNode*>& subgraph) const
  {
    for (const GraphNode* node : subgraph) {
      if (node->getIndex() > bound) {
        return false;
      }
    }
    return true;
  }

  unsigned bound;
};

struct BiggerOrEqualFilter {
  bool operator()(const std::vector<const GraphNode*>& subgraph) const
  {
    for (const GraphNode* node : subgraph) {
      if (node->getIndex() < bound) {
        return false;
      }
    }
    return true;
  }

  unsigned bound;
};

struct CustomizedGraphTestRow {
  size_t upperBound;
  size_t nofNodes;
  unsigned Bound;
  size_t nofResultsLess;
  size_t nofResultsBigger;
};

class CustomizedGraphTest : public ::testing::TestWithParam<CustomizedGraphTestRow> {
  protected:
    void SetUp() override
    {
      sorted_graph = std::make_unique<Graph<true>>(getAdjacency(GetParam().nofNodes));
      unsorted_graph = std::make_unique<Graph<false>>(getAdjacency(GetParam().nofNodes));
    }

    void TearDown() override
    {
      sorted_graph.reset();
      unsorted_graph.reset();
    }

    std::unique_ptr<SortedGraph> sorted_graph;
    std::unique_ptr<UnsortedGraph> unsorted_graph;
};


TEST_P(CustomizedGraphTest, TestCustomConsens) {

  auto test_params = GetParam();

  GraphNodeCompare compare;
  LessOrEqualFilter lessFilter{test_params.Bound};
  BiggerOrEqualFilter biggerFilter{test_params.Bound};

  std::vector<std::vector<const GraphNode*>> resultLessSorted
      = ConsensLib::runConsens(*sorted_graph, test_params.upperBound, lessFilter, compare);

  std::vector<std::vector<const GraphNode*>> resultBiggerSorted
      = ConsensLib::runConsens(*sorted_graph, test_params.upperBound, biggerFilter, compare);

  std::vector<std::vector<const GraphNode*>> resultLessUnsorted
      = ConsensLib::runConsens(*unsorted_graph, test_params.upperBound, lessFilter, compare);

  std::vector<std::vector<const GraphNode*>> resultBiggerUnsorted
      = ConsensLib::runConsens(*unsorted_graph, test_params.upperBound, biggerFilter, compare);

  EXPECT_EQ(resultLessSorted.size(), test_params.nofResultsLess);
  EXPECT_EQ(resultBiggerSorted.size(), test_params.nofResultsBigger);

  EXPECT_EQ(resultLessSorted, resultLessUnsorted);
  EXPECT_EQ(resultBiggerSorted, resultBiggerUnsorted);

  checkValidity(resultLessSorted, *sorted_graph, test_params.upperBound, lessFilter, compare);
  checkValidity(resultBiggerSorted, *sorted_graph, test_params.upperBound, biggerFilter, compare);
}

INSTANTIATE_TEST_SUITE_P(CustomizedTester, CustomizedGraphTest, ::testing::Values(
    CustomizedGraphTestRow{std::numeric_limits<size_t>::max(), 2, 0u, 1, 3},
    CustomizedGraphTestRow{std::numeric_limits<size_t>::max(), 4, 1u, 3, 7},
    CustomizedGraphTestRow{8, 8, 6u, 127, 3},
    CustomizedGraphTestRow{8, 8, 7u, 255, 1},
    CustomizedGraphTestRow{3, 4, 2u, 7, 3}
));
