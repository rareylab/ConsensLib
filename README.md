# ConsensLib

[![Build Status](https://travis-ci.org/lbellmann/ConsensLib.svg?branch=master)](https://travis-ci.org/lbellmann/ConsensLib)
[![Documentation Status](https://readthedocs.org/projects/consenslib/badge/?version=latest)](https://consenslib.readthedocs.io/en/latest/?badge=latest)

ConsensLib is a header-only C++ library for efficient enumeration of connected induced subgraphs.
The CONSENS (Connected Subgraph Enumeration Strategy) algorithm enumerates all node sets
that form a connected subgraph of a given query graph. During enumeration a pair of forbidden
and candidate node sets is used to avoid duplicates.

It has been developed at the
[University of Hamburg](https://www.uni-hamburg.de/),
[ZBH - Center for Bioinformatics](http://www.zbh.uni-hamburg.de),
[Research Group for Computational Molecular Design](http://www.zbh.uni-hamburg.de/amd)
by Louis Bellmann and Matthias Rarey.

The library is distributed under BSD New license,
see the file [LICENSE](LICENSE) and [BSD 3-clause](https://opensource.org/licenses/BSD-3-Clause).

## Installation

See [INSTALL](INSTALL) or the [documentation](https://consenslib.readthedocs.io/en/latest/installation.html) for detailed installation instructions.

## Documentation

The documentation is available on [ReadTheDocs](https://readthedocs.org/) as a [HTML](https://consenslib.readthedocs.io/en/latest/)
and as a [PDF](https://consenslib.readthedocs.io/en/latest/refman.pdf) document.

## Getting Started

In order to run the CONSENS enumeration algorithm you must first define traits for your graph structure.
See the [documentation](https://consenslib.readthedocs.io/en/latest/structConsensLib_1_1GraphTraits.html) for detailed instructions.
Below a traits definition taken from the [example file](src/Examples/Example.cpp) is shown.

```cpp
using GraphMap = std::pair<std::vector<unsigned>,
                           std::unordered_map<unsigned, std::vector<unsigned>>>;

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
```

For dense graphs the number of connected induced subgraphs can be quite large. If your node type takes a considerable amount of memory
this might lead to long run-times and large quantities of memory needed. Consider using indices or pointers instead.

The CONSENS algorithm specifically enumerates only connected induced subgraphs. If you want to consider all connected subgraphs you can apply the enumeration to the [line graph](https://en.wikipedia.org/wiki/Line_graph). Its nodes are defined by the edges of the original graph and two nodes are adjacent if the two underlying edges share a node. The connected induced subgraphs of the line graph correspond to the connected subgraphs of the original graph.
