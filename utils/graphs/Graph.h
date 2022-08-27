#ifndef GENTREE_UTILS_GRAPHS_GRAPH_H_
#define GENTREE_UTILS_GRAPHS_GRAPH_H_

#include <cstdint>
#include <istream>
#include <ostream>
#include <stack>
#include <string>
#include <vector>

using node_type= std::int64_t;
using size_type= std::int64_t;

/**
 */
class Graph {
 private:
  std::vector<std::vector<node_type>> adj;
  size_type V,E;
  void _serialize( std::ostream &os, node_type x ) const;
  void init( const std::string &s );
 public:
  node_type new_node();
  void add_arcs( node_type from, node_type to );
  Graph();
  explicit Graph(const std::string &s);
  explicit Graph(std::istream &is);
  [[nodiscard]] size_type size() const;
  [[nodiscard]] const std::vector<node_type> &children( node_type x ) const;
  std::vector<node_type> rest();
  void add_node( node_type x );
  void transform( node_type x, const Graph &src );
  void serialize( std::ostream &os ) const;
};

#endif //GENTREE_UTILS_GRAPHS_GRAPH_H_