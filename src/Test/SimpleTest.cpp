#include <memory>
#include <unordered_map>
#include <vector>

#include <gtest/gtest.h>

#include "ConsensLib/Consens.hpp"

#include "CheckSubgraphs.hpp"

using GraphMap = std::pair<std::vector<unsigned>, std::unordered_map<unsigned, std::vector<unsigned>>>;

namespace ConsensLib {
template<>
struct GraphTraits<GraphMap> {
  using Node = unsigned;
  using Iterator = std::vector<unsigned>::const_iterator;
  static Iterator adjancencyBegin(
      const Node& node,
      const GraphMap& graph)
  {
    return graph.second.at(node).begin();
  }

  static Iterator adjancencyEnd(
      const Node& node,
      const GraphMap& graph)
  {
    return graph.second.at(node).end();
  }

  static Iterator nodesBegin(const GraphMap& graph)
  {
    return graph.first.begin();
  }

  static Iterator nodesEnd(const GraphMap& graph)
  {
    return graph.first.end();
  }

  static constexpr bool listsSorted() {
    return true;
  }
};
}

enum class SimpleGraphEnum {
  CLIQUE,
  CYCLE,
  DISCONNECTED,
  EMPTY,
  PATH
};

GraphMap getSimpleGraph(SimpleGraphEnum graphEnum)
{
  std::vector<unsigned> nodes({1, 2, 3, 4, 5});
  std::unordered_map<unsigned, std::vector<unsigned>> graph;
  switch (graphEnum) {
  case SimpleGraphEnum::CLIQUE:
    graph.insert(std::make_pair(1, std::vector<unsigned>({2, 3, 4, 5})));
    graph.insert(std::make_pair(2, std::vector<unsigned>({1, 3, 4, 5})));
    graph.insert(std::make_pair(3, std::vector<unsigned>({1, 2, 4, 5})));
    graph.insert(std::make_pair(4, std::vector<unsigned>({1, 2, 3, 5})));
    graph.insert(std::make_pair(5, std::vector<unsigned>({1, 2, 3, 4})));
    break;
  case SimpleGraphEnum::CYCLE:
    graph.insert(std::make_pair(1, std::vector<unsigned>({2, 5})));
    graph.insert(std::make_pair(2, std::vector<unsigned>({1, 3})));
    graph.insert(std::make_pair(3, std::vector<unsigned>({2, 4})));
    graph.insert(std::make_pair(4, std::vector<unsigned>({3, 5})));
    graph.insert(std::make_pair(5, std::vector<unsigned>({1, 4})));
    break;
  case SimpleGraphEnum::DISCONNECTED:
    graph.insert(std::make_pair(1, std::vector<unsigned>({2})));
    graph.insert(std::make_pair(2, std::vector<unsigned>({1})));
    graph.insert(std::make_pair(3, std::vector<unsigned>({4, 5})));
    graph.insert(std::make_pair(4, std::vector<unsigned>({3, 5})));
    graph.insert(std::make_pair(5, std::vector<unsigned>({3, 4})));
    break;
  case SimpleGraphEnum::EMPTY:
    return std::make_pair(std::vector<unsigned>(), graph);
  default:
    graph.insert(std::make_pair(1, std::vector<unsigned>({2})));
    graph.insert(std::make_pair(2, std::vector<unsigned>({1, 3})));
    graph.insert(std::make_pair(3, std::vector<unsigned>({2, 4})));
    graph.insert(std::make_pair(4, std::vector<unsigned>({3, 5})));
    graph.insert(std::make_pair(5, std::vector<unsigned>({4})));
  }
  return std::make_pair(nodes, graph);
}

struct SimpleGraphTestRow {
  size_t upperBound;
  SimpleGraphEnum graphEnum;
  size_t nofResults;
};

class SimpleGraphTest : public ::testing::TestWithParam<SimpleGraphTestRow> {
  protected:
    void SetUp() override
    {
      graph = std::make_unique<GraphMap>(getSimpleGraph(GetParam().graphEnum));
    }

    void TearDown() override
    {
      graph.reset();
    }

    std::unique_ptr<GraphMap> graph;
};

TEST_P(SimpleGraphTest, TestEnumeration) {

  auto test_params = GetParam();

  std::vector<std::vector<unsigned>> result = ConsensLib::runConsens(*graph, test_params.upperBound);

  EXPECT_EQ(result.size(), test_params.nofResults);

  checkValidity(result, *graph, test_params.upperBound);
}

INSTANTIATE_TEST_SUITE_P(CliqueTester, SimpleGraphTest, ::testing::Values(
    SimpleGraphTestRow{std::numeric_limits<size_t>::max(), SimpleGraphEnum::CLIQUE, 31},
    SimpleGraphTestRow{0, SimpleGraphEnum::CLIQUE, 0},
    SimpleGraphTestRow{1, SimpleGraphEnum::CLIQUE, 5},
    SimpleGraphTestRow{2, SimpleGraphEnum::CLIQUE, 15},
    SimpleGraphTestRow{3, SimpleGraphEnum::CLIQUE, 25},
    SimpleGraphTestRow{4, SimpleGraphEnum::CLIQUE, 30},
    SimpleGraphTestRow{6, SimpleGraphEnum::CLIQUE, 31}
));

INSTANTIATE_TEST_SUITE_P(CycleTester, SimpleGraphTest, ::testing::Values(
    SimpleGraphTestRow{std::numeric_limits<size_t>::max(), SimpleGraphEnum::CYCLE, 21},
    SimpleGraphTestRow{0, SimpleGraphEnum::CYCLE, 0},
    SimpleGraphTestRow{1, SimpleGraphEnum::CYCLE, 5},
    SimpleGraphTestRow{2, SimpleGraphEnum::CYCLE, 10},
    SimpleGraphTestRow{3, SimpleGraphEnum::CYCLE, 15},
    SimpleGraphTestRow{4, SimpleGraphEnum::CYCLE, 20},
    SimpleGraphTestRow{10, SimpleGraphEnum::CYCLE, 21}
));

INSTANTIATE_TEST_SUITE_P(DisconnectedTester, SimpleGraphTest, ::testing::Values(
    SimpleGraphTestRow{std::numeric_limits<size_t>::max(), SimpleGraphEnum::DISCONNECTED, 10},
    SimpleGraphTestRow{0, SimpleGraphEnum::DISCONNECTED, 0},
    SimpleGraphTestRow{1, SimpleGraphEnum::DISCONNECTED, 5},
    SimpleGraphTestRow{2, SimpleGraphEnum::DISCONNECTED, 9},
    SimpleGraphTestRow{3, SimpleGraphEnum::DISCONNECTED, 10},
    SimpleGraphTestRow{4, SimpleGraphEnum::DISCONNECTED, 10},
    SimpleGraphTestRow{10, SimpleGraphEnum::DISCONNECTED, 10}
));

INSTANTIATE_TEST_SUITE_P(EmptyTester, SimpleGraphTest, ::testing::Values(
    SimpleGraphTestRow{std::numeric_limits<size_t>::max(), SimpleGraphEnum::EMPTY, 0},
    SimpleGraphTestRow{0, SimpleGraphEnum::EMPTY, 0},
    SimpleGraphTestRow{1, SimpleGraphEnum::EMPTY, 0}
));

INSTANTIATE_TEST_SUITE_P(PathTester, SimpleGraphTest, ::testing::Values(
    SimpleGraphTestRow{std::numeric_limits<size_t>::max(), SimpleGraphEnum::PATH, 15},
    SimpleGraphTestRow{0, SimpleGraphEnum::PATH, 0},
    SimpleGraphTestRow{1, SimpleGraphEnum::PATH, 5},
    SimpleGraphTestRow{2, SimpleGraphEnum::PATH, 9},
    SimpleGraphTestRow{3, SimpleGraphEnum::PATH, 12},
    SimpleGraphTestRow{4, SimpleGraphEnum::PATH, 14},
    SimpleGraphTestRow{10, SimpleGraphEnum::PATH, 15}
));

