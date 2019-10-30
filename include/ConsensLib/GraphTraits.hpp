#pragma once

#include <cstddef>

namespace ConsensLib {

/**
 * @brief Traits for the graph used for enumeration.
 * This must be specified by the user for the graph structure used.
 *
 * Needs:
 *
 * 'Node' which is a typedef, class or struct for the type of node.
 *
 * 'adjancencyBegin' which is static, takes a node and the graph as arguments
 * and returns an iterator pointing to the begin of the adjacency list of the node.
 *
 * 'adjancencyEnd' which is static, takes a node and the graph as arguments
 * and returns an iterator pointing to the end of the adjacency list of the node.
 *
 * 'nodesBegin' which is static, takes a graph as argument
 * and returns an iterator pointing to the begin of the list of nodes.
 *
 * 'nodesEnd' which is static takes a graph as argument
 * and returns an iterator pointing to the end of the list of nodes.
 *
 * 'listsSorted' which is static, takes no arguments
 * and returns true if all adjacency lists are sorted.
 *
 * For example template specifications see the example and test files.
 */
template<typename Graph>
struct GraphTraits {
  using Node = std::nullptr_t;
  using AdjacencyIterator = std::nullptr_t;
  using NodeIterator = std::nullptr_t;
  static AdjacencyIterator adjancencyBegin(
      const Node& node,
      const Graph& graph)
  {
    static_assert(sizeof(Graph) == 0, "You must specify the GraphTraits for your type of graph. "
                                      "This function must take a node and a graph as arguments "
                                      "and return an iterator pointing to the begin "
                                      "of the adjacency list of that node");
  }

  static AdjacencyIterator adjancencyEnd(
      const Node& node,
      const Graph& graph)
  {
    static_assert(sizeof(Graph) == 0, "You must specify the GraphTraits for your type of graph. "
                                      "This function must take a node and a graph as arguments "
                                      "and return an iterator pointing to the end "
                                      "of the adjacency list of that node");
  }

  static NodeIterator nodesBegin(const Graph& graph)
  {
    static_assert(sizeof(Graph) == 0, "You must specify the GraphTraits for your type of graph. "
                                      "This function must take a graph as argument "
                                      "and return an iterator pointing to the begin "
                                      "of the range of nodes");
  }

  static NodeIterator nodesEnd(const Graph& graph)
  {
    static_assert(sizeof(Graph) == 0, "You must specify the GraphTraits for your type of graph. "
                                      "This function must take a graph as argument "
                                      "and return an iterator pointing to the end "
                                      "of the range of nodes");
  }

  static constexpr bool listsSorted() {
    static_assert(sizeof(Graph) == 0, "You must specify the GraphTraits for your type of graph. "
                                      "This function must return true if all adjacency lists "
                                      "are sorted and false otherwise");
    return false;
  }
};

} // end namespace ConsensLib
