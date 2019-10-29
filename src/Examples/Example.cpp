#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

#include "ConsensLib/Consens.hpp"

class Graph {

public:

  Graph(const std::vector<std::vector<size_t>>& adjacency)
    : m_adjacency(adjacency)
  {
    std::vector<size_t> tempNodes(adjacency.size());
    std::iota(tempNodes.begin(), tempNodes.end(), 0);
    m_nodes = std::move(tempNodes);
  }

  const std::vector<size_t>& getNodes() const
  {
    return m_nodes;
  }

  const std::vector<size_t>& getNeighbors(size_t pos) const
  {
    return m_adjacency.at(pos);
  }

private:

  std::vector<size_t> m_nodes;
  std::vector<std::vector<size_t>> m_adjacency;

};

namespace ConsensLib {
template<>
struct GraphTraits<Graph> {
  using Node = size_t;
  using Iterator = std::vector<size_t>::const_iterator;
  static Iterator adjancencyBegin(
      const Node& node,
      const Graph& graph)
  {
    return graph.getNeighbors(node).begin();
  }

  static Iterator adjancencyEnd(
      const Node& node,
      const Graph& graph)
  {
    return graph.getNeighbors(node).end();
  }

  static Iterator nodesBegin(const Graph& graph)
  {
    return graph.getNodes().begin();
  }

  static Iterator nodesEnd(const Graph& graph)
  {
    return graph.getNodes().end();
  }

  static constexpr bool listsSorted() {
    return true;
  }
};
}

struct IsPathFilter {

  bool operator()(const std::vector<size_t>& subgraph) const
  {
    if (subgraph.size() < 3) {
      return true;
    }
    unsigned degreeSum = 0;
    for (size_t node : subgraph) {
      unsigned degree = 0;
      for (size_t neighbor : graph->getNeighbors(node)) {
        std::vector<size_t>::const_iterator iter = std::lower_bound(subgraph.begin(),
                                                                    subgraph.end(),
                                                                    neighbor);
        if (iter != subgraph.end() && *iter == neighbor) {
          ++degree;
        }
      }
      if (degree > 2) {
        return false;
      }
      degreeSum += degree;
    }
    if (degreeSum > 2 * subgraph.size() - 2) {
      return false;
    }
    return true;
  }

  Graph* graph;
};

int main()
{
  std::vector<std::vector<size_t>> adjacency = {{1}, {0, 2, 3, 4}, {1, 3}, {1, 2}, {1}};
  Graph graph(adjacency);

  std::cout << "--INPUT GRAPH--\n\n"
               "    0          \n"
               "     \\        \n"
               "      1--4     \n"
               "     / \\      \n"
               "    2---3      \n\n";

  std::vector<std::vector<size_t>> subgraphs = ConsensLib::runConsens(graph);

  std::cout << "Node sets of all connected induced subgraphs are\n\n";
  for (const std::vector<size_t>& subgraph : subgraphs) {
    std::cout << "{";
    for (unsigned i = 0; i < subgraph.size(); ++i) {
      if (i != 0) {
        std::cout << ", ";
      }
      std::cout << subgraph.at(i);
    }
    std::cout << "}\n";
  }

  IsPathFilter filter{&graph};

  std::vector<std::vector<size_t>> paths = ConsensLib::runConsens(graph,
                                                                  std::numeric_limits<size_t>::max(),
                                                                  filter);

  std::cout << "\nNode sets of all induced paths are\n\n";
  for (const std::vector<size_t>& path : paths) {
    std::cout << "{";
    for (unsigned i = 0; i < path.size(); ++i) {
      if (i != 0) {
        std::cout << ", ";
      }
      std::cout << path.at(i);
    }
    std::cout << "}\n";
  }

  return 0;
}



