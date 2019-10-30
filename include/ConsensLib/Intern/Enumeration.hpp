#pragma once

#include <algorithm>
#include <vector>

#include "../GraphTraits.hpp"
#include "../Types.hpp"

namespace ConsensLib {

namespace Intern {

/**
 * @brief Perform a recursive call for adding a subgraph together with all its super graphs
 *        following the restriction defined by the forbidden nodes.
 *
 * @tparam Graph Type of graph for enumeration.
 * @tparam Node Type of node contained in the graph.
 * @tparam FilterFunc Type of filter for the option of filtering the generated node sets.
 * @tparam Compare Type of compare function that defines a strict total ordering in the nodes.
 *
 * @param graph The input graph
 * @param upper Optional upper bound for the size of the subgraphs.
 * @param filter Optional filter criteria applied to the subgraphs.
 *        Must accept std::vector<Node> as input and return a boolean.
 * @param current Currently considered subgraph.
 * @param candidates Neighboring nodes that can be added to the subgraph.
 * @param forbidden Forbidden nodes that can never be added to this subgraph.
 * @param subgraphs the output container for all connected induced subgraphs
 *        that fulfill the filter criteria.
 * @param compare The compare function defining a strict total ordering on the nodes of the graph.
 *
 * Adds the currently considered subgraph to the output container if it fulfills the filter criteria.
 * For each candidate node the set of forbidden nodes is updated by adding all candidates
 * that are smaller than the chosen candidate itself. The candidate set is updated by excluding
 * all newly added forbidden nodes and adding neighbors of the currently considered candidate
 * that are not forbidden. All these operations can be done in asymptotic linear time with respect
 * to the number of nodes contained in the input graph.
 */
template<typename Graph,
         typename Node,
         typename FilterFunc,
         typename Compare>
void generateRecursiveLinear(
    const Graph& graph,
    size_t upper,
    const FilterFunc& filter,
    std::vector<Node>& current,
    const std::vector<Node>& candidates,
    const std::vector<Node>& forbidden,
    std::vector<std::vector<Node>>& subgraphs,
    const Compare& compare)
{
  if (filter(current)) {
    subgraphs.push_back(current);
  }
  if (current.size() < upper) {
    for (auto candidateIter = candidates.begin(); candidateIter != candidates.end(); ++candidateIter) {
      auto subgraphIter = std::lower_bound(current.begin(), current.end(), *candidateIter, compare);
      current.insert(subgraphIter, *candidateIter);
      std::vector<Node> nextForbidden;
      std::set_union(candidates.begin(), candidateIter,
                     forbidden.begin(), forbidden.end(),
                     std::back_inserter(nextForbidden), compare);
      std::vector<Node> tempComplement;
      auto begin = GraphTraits<Graph>::adjancencyBegin(*candidateIter, graph);
      auto end = GraphTraits<Graph>::adjancencyEnd(*candidateIter, graph);
      std::set_difference(begin, end,
                          current.begin(), current.end(),
                          std::back_inserter(tempComplement), compare);
      std::vector<Node> complement;
      std::set_difference(tempComplement.begin(), tempComplement.end(),
                          nextForbidden.begin(), nextForbidden.end(),
                          std::back_inserter(complement), compare);
      std::vector<Node> nextCandidates;
      std::set_union(candidateIter + 1, candidates.end(),
                     complement.begin(), complement.end(),
                     std::back_inserter(nextCandidates), compare);
      generateRecursiveLinear(graph, upper, filter, current, nextCandidates, nextForbidden, subgraphs, compare);
      auto eraseIter = std::lower_bound(current.begin(), current.end(), *candidateIter, compare);
      current.erase(eraseIter);
    }
  }
}

/**
 * @brief Perform a recursive call for adding a subgraph together with all its super graphs
 *        following the restriction defined by the forbidden nodes.
 *
 * @tparam Graph Type of graph for enumeration.
 * @tparam Node Type of node contained in the graph.
 * @tparam FilterFunc Type of filter for the option of filtering the generated node sets.
 * @tparam Compare Type of compare function that defines a strict total ordering in the nodes.
 *
 * @param graph The input graph
 * @param upper Optional upper bound for the size of the subgraphs.
 * @param filter Optional filter criteria applied to the subgraphs.
 *        Must accept std::vector<Node> as input and return a boolean.
 * @param current Currently considered subgraph.
 * @param candidates Neighboring nodes that can be added to the subgraph.
 * @param forbidden Forbidden nodes that can never be added to this subgraph.
 * @param subgraphs the output container for all connected induced subgraphs
 *        that fulfill the filter criteria.
 * @param compare The compare function defining a strict total ordering on the nodes of the graph.
 *
 * Adds the currently considered subgraph to the output container if it fulfills the filter criteria.
 * For each candidate node the set of forbidden nodes is updated by adding all candidates
 * that are smaller than the chosen candidate itself. The candidate set is updated by excluding
 * all newly added forbidden nodes and adding neighbors of the currently considered candidate
 * that are not forbidden. The update of the candidate set has worst-case time complexity O(n log n)
 * where n is the number of nodes contained in the query graph.
 */
template<typename Graph,
         typename Node,
         typename FilterFunc,
         typename Compare>
void generateRecursiveNonLinear(
    const Graph& graph,
    size_t upper,
    const FilterFunc& filter,
    std::vector<Node>& current,
    const std::vector<Node>& candidates,
    const std::vector<Node>& forbidden,
    std::vector<std::vector<Node>>& subgraphs,
    const Compare& compare)
{
  if (filter(current)) {
    subgraphs.push_back(current);
  }
  if (current.size() < upper) {
    for (auto candidateIter = candidates.begin(); candidateIter != candidates.end(); ++candidateIter) {
      auto subgraphIter = std::lower_bound(current.begin(), current.end(), *candidateIter, compare);
      current.insert(subgraphIter, *candidateIter);
      std::vector<Node> nextCandidates(candidateIter + 1, candidates.end());
      std::vector<Node> nextForbidden;
      std::set_union(forbidden.begin(), forbidden.end(),
                     candidates.begin(), candidateIter,
                     std::back_inserter(nextForbidden), compare);
      auto begin = GraphTraits<Graph>::adjancencyBegin(*candidateIter, graph);
      auto end = GraphTraits<Graph>::adjancencyEnd(*candidateIter, graph);
      for (auto neighborIter = begin; neighborIter != end; ++ neighborIter) {
        auto foundIter = std::lower_bound(nextForbidden.begin(), nextForbidden.end(), *neighborIter, compare);
        if (foundIter != nextForbidden.end() && !compare(*neighborIter, *foundIter)) {
          continue;
        }
        auto currentIter = std::lower_bound(current.begin(), current.end(), *neighborIter, compare);
        if (currentIter != current.end()
            && !compare(*currentIter, *neighborIter)
            && !compare(*neighborIter, *currentIter)) {
          continue;
        }
        auto insertIter = std::lower_bound(nextCandidates.begin(), nextCandidates.end(), *neighborIter, compare);
        if (insertIter == nextCandidates.end() || compare(*neighborIter, *insertIter)) {
          nextCandidates.insert(insertIter, *neighborIter);
        }
      }
      generateRecursiveNonLinear(graph,upper, filter, current, nextCandidates, nextForbidden, subgraphs, compare);
      auto eraseIter = std::lower_bound(current.begin(), current.end(), *candidateIter, compare);
      current.erase(eraseIter);
    }
  }
}

/**
 * @brief Perform the actual enumeration of subgraphs. Depending on wether or not the adjacency lists
 *        are sorted a diferent function is called
 *
 * @tparam Graph Type of graph for enumeration.
 * @tparam Node Type of node contained in the graph.
 * @tparam FilterFunc Type of filter for the option of filtering the generated node sets.
 * @tparam Compare Type of compare function that defines a strict total ordering in the nodes.
 *
 * @param graph The input graph
 * @param upper Optional upper bound for the size of the subgraphs.
 * @param filter Optional filter criteria applied to the subgraphs.
 *        Must accept std::vector<Node> as input and return a boolean.
 * @param compare The compare function defining a strict total ordering on the nodes of the graph.
 *
 * If all adjacency lists are sorted it is possible to apply set operations such as
 * 'union', 'intersection' and 'difference' in asymptotic linear time with respect to the number of
 * nodes contained in the query graph. When the adjacency lists are not sorted this is not possible
 * and the asymptotic runtime of one recursive call is O(n log n) where n is the number of nodes contained
 * in the query graph.
 */
template<typename Graph,
         typename Node,
         typename FilterFunc,
         typename Compare>
std::vector<std::vector<Node>> runEnumeration(
    const Graph& graph,
    size_t upper,
    const FilterFunc& filter,
    const Compare& compare)
{
  std::vector<std::vector<Node>> subgraphs;
  if (upper == 0) {
    return subgraphs;
  }
  auto nodesBegin = GraphTraits<Graph>::nodesBegin(graph);
  auto nodesEnd = GraphTraits<Graph>::nodesEnd(graph);
  std::vector<Node> nodesVector(nodesBegin, nodesEnd);
  std::sort(nodesVector.begin(), nodesVector.end(), compare);
  for (auto iter = nodesVector.begin(); iter != nodesVector.end(); ++iter) {
    std::vector<Node> current({*iter});
    std::vector<Node> candidates;
    std::vector<Node> forbidden(nodesVector.begin(), iter);
    auto begin = GraphTraits<Graph>::adjancencyBegin(*iter, graph);
    auto end = GraphTraits<Graph>::adjancencyEnd(*iter, graph);
    if (GraphTraits<Graph>::listsSorted()) {
      std::set_difference(begin, end, forbidden.begin(), forbidden.end(), std::back_inserter(candidates), compare);
      generateRecursiveLinear(graph, upper, filter, current, candidates, forbidden, subgraphs, compare);
    }
    else {
      for (auto neighborIter = begin; neighborIter != end; ++neighborIter) {
        auto foundIter = std::lower_bound(forbidden.begin(), forbidden.end(), *neighborIter, compare);
        if (foundIter == forbidden.end() || compare(*neighborIter, *foundIter)) {
          candidates.push_back(*neighborIter);
        }
      }
      std::sort(candidates.begin(), candidates.end(), compare);
      generateRecursiveNonLinear(graph, upper, filter, current, candidates, forbidden, subgraphs, compare);
    }
  }
  return subgraphs;
}

} // end namespace Intern
} // end namespace ConsensLib
