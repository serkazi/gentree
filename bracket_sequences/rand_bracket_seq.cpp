#include "rand_bracket_seq.h"

#include <cassert>
#include <optional>
#include <stack>

RandomBrackSeqImpl::RandomBrackSeqImpl(size_t n) : n_(n) {}

std::string RandomBrackSeqImpl::explicit_stack_phi(const std::string &w) {
  size_t n= w.size()/2, cur= 0;
  std::string sb; sb.resize(2*n);
  std::stack<std::optional<std::pair<size_t,size_t>>> post_action;
  std::stack<size_t> ls, rs;
  std::stack<bool> status;
#define enc(x,y,tf,opt) { ls.push(x),rs.push(y),status.push(tf),post_action.push(opt); }
  enc(0,2*n-1,false,std::nullopt);
  while ( not ls.empty() ) {
    auto left= ls.top(), right= rs.top();
    ls.pop(), rs.pop();
    std::int64_t partial_sum= 0,i,j,k,r= 0;
    bool done= status.top(); status.pop();
    auto action= post_action.top(); post_action.pop();
    if ( done ) {
      if ( action ) {
        sb[cur++]= ')';
        for ( auto k= action.value().first+1; k <= action.value().second-2; ++k )
          sb[cur++]= (w[k]=='('?')':'(');
      }
      continue ;
    }
    enc(left,right,true,action);
    for ( i= left; i <= right and r == 0; ++i )
      if ( (partial_sum+= (w[i]=='('?1:-1)) == 0 )
        r= i+1;
    if ( left > right ) continue ;
    assert( r > 0 );
    if ( w[left] == '(' ) {
      for ( i= left; i < r; ++i )
        sb[cur++]= w[i];
      enc(r,right,false,std::nullopt);
      continue ;
    }
    assert( w[left] == ')' );
    assert( w[r-1] == '(' );
    sb[cur++]= '(';
    enc(r,right,false,std::make_pair(left,r));
  }
#undef enc
  return sb;
}

std::string RandomBrackSeqImpl::random_bps(size_t n ) {
  auto L = utils_.rand_subset(2*n,n);
  std::string x{};
  x.resize(2*n);
  assert( L.size() == n );
  size_t i,k;
  for ( i= 0, k= 0; k < n; x[i++]= '(', ++k )
    for ( ;i < 2*n and i < L[k]; x[i++]= ')' ) ;
  assert( k == n );
  for ( ;i < 2*n; x[i++]= ')' ) ;
  const auto result = explicit_stack_phi(x);
  assert(is_balanced(result));
  return result;
}

void RandomBrackSeqImpl::generate(std::ostream &os) {
  os << '(' << random_bps(n_ - 1) << ')';
}

bool RandomBrackSeqImpl::is_balanced(const std::string &s) {
  std::int64_t balance= 0;
  for ( auto ch: s )
    if ( (balance+= (ch=='('?1:-1)) < 0 )
      return false ;
  return balance == 0;
}