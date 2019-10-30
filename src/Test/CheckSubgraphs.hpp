#pragma once

#include <algorithm>
#include <limits>
#include <vector>

#include <gtest/gtest.h>

#include "ConsensLib/GraphTraits.hpp"
#include "ConsensLib/Types.hpp"

template<typename Graph,
         typename Node = typename ConsensLib::GraphTraits<Graph>::Node,
         typename FilterFunc = ConsensLib::NoFilter,
         typename Compare = std::less<Node>>
void checkValidity(
    const std::vector<std::vector<Node>>& subgraphs,
    const Graph& graph,
    size_t upper = std::numeric_limits<size_t>::max(),
    const FilterFunc& filter = FilterFunc(),
    const Compare& compare = Compare())
{
  for (unsigned i = 0; i < subgraphs.size(); ++i) {
    const std::vector<Node>& subgraph = subgraphs.at(i);
    // check size
    EXPECT_LE(subgraph.size(), upper);

    // check filter criteria
    EXPECT_TRUE(filter(subgraph));

    // check connectivity
    if (subgraph.size() > 1) {
      for (const Node& node : subgraph) {
        auto begin = ConsensLib::GraphTraits<Graph>::adjancencyBegin(node, graph);
        auto end = ConsensLib::GraphTraits<Graph>::adjancencyEnd(node, graph);
        bool hasNeighbor = false;
        for (auto iter = begin; iter != end; ++iter) {
          auto foundIter = std::lower_bound(subgraph.begin(), subgraph.end(), *iter, compare);
          // !compare(*foundIter, *iter) already checked by lower_bound
          if (foundIter != subgraph.end() && !compare(*iter, *foundIter)) {
            hasNeighbor = true;
            break;
          }
        }
        EXPECT_TRUE(hasNeighbor);
      }
    }

    // check uniqueness
    for (unsigned j = i + 1; j < subgraphs.size(); ++j) {
      const std::vector<Node>& other = subgraphs.at(j);
      bool same = subgraph.size() == other.size();
      if (same) {
        for (unsigned idx = 0; idx < subgraph.size(); ++idx) {
          const Node& n1 = subgraph.at(idx);
          const Node& n2 = other.at(idx);
          if (compare(n1, n2) || compare(n2, n1)) {
            same = false;
            break;
          }
        }
      }
      EXPECT_FALSE(same);
    }
  }
}
