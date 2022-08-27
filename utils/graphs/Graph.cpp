#include "Graph.h"

#include <cassert>

void Graph::_serialize(std::ostream &os, node_type x) const {
  os << '(';
  for ( auto z: adj[x] )
    _serialize(os,z);
  os << ')';
}

void Graph::init(const std::string &s) {
  std::stack<node_type> st{};
  node_type x,y;
  size_type n= s.size()/2;
  adj.resize(n), V= 0, E= n-1;
  for ( auto &v: adj )
    v.clear();
  for (auto ch: s) {
    switch (ch) {
      case ')': x = st.top(); st.pop();
          if(not st.empty()) {
            add_arcs(st.top(), x);
          }
          break ;
      case '(': st.push(new_node());
      break ;
      default:
        assert(false);
    }
  }
  assert( st.empty() );
}

node_type Graph::new_node() {
  auto res= V++;
  return res;
}

void Graph::add_arcs( node_type from, node_type to ) {
  adj[from].push_back(to);
}

Graph::Graph() { V= E= 0, adj.clear(); }

Graph::Graph( const std::string &s ) : Graph() { init(s); }

Graph::Graph( std::istream &is ) {
  std::string s;
  is >> s, init(s);
}

size_type Graph::size() const {
  return V;
}

const std::vector<node_type> &Graph::children( node_type x ) const {
  return adj[x];
}

std::vector<node_type> Graph::rest() {
  return V==0?std::vector<node_type>{}:adj[0];
}

void Graph::add_node( node_type x ) {
  for (;x >= size(); new_node() ) ;
}

void Graph::transform( node_type x, const Graph &src ) {
  auto &dst= *this;
  const auto &adj= src.children(x);
  // we assert this is a binary tree
  assert( adj.size() <= 2 );
  Graph lft{},rgt{};
  if ( adj.size() >= 1 )
    transform(adj.front(),lft);
  if ( adj.size() >= 2 )
    transform(adj.back(),rgt);
  auto root= dst.new_node();
  size_type offset= dst.V;
  {
    auto left_forest= lft.rest();
    for ( auto z= 0; z < lft.V; ++z ) {
      for ( auto &t: lft.adj[z] )
        t+= offset;
    }
    for ( auto z= 0; z < lft.size(); ++z ) {
      auto new_z= z+offset;
      dst.add_node(new_z);
      dst.adj[new_z]= std::move(lft.adj[z]);
    }
    if ( lft.size() )
      dst.adj[root].push_back(0+offset);
    offset+= lft.size();
  }
  {
    auto right_forest= rgt.rest();
    for ( auto z= 0; z < rgt.size(); ++z ) {
      for ( auto &t: rgt.adj[z] )
        --t, t+= offset;
    }
    for ( auto z= 1; z < rgt.size(); ++z ) {
      auto new_z= (z-1)+offset;
      dst.add_node(new_z);
      dst.adj[new_z]= std::move(rgt.adj[z]);
    }
    if ( rgt.size() ) {
      for ( auto &t: rgt.adj.front() )
        t+= offset-1;
      dst.adj[root].insert(dst.adj[root].end(),rgt.adj.front().begin(),rgt.adj.front().end());
    }
  }
}

void Graph::serialize(std::ostream &os) const {
  _serialize(os,0);
}