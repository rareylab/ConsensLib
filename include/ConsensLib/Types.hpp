#pragma once

#include <vector>

namespace ConsensLib {

/**
 * @brief The default filter returning true for every subgraph.
 * All custom filter should have the same interface.
 */
struct NoFilter
{
  template<typename T>
  bool operator()(const std::vector<T>& subgraph) const
  {
    return true;
  }
};
} // end namespace ConsensLib
