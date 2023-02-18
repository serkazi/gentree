//
//
#include "tree_covering.h"

#include <array>
#include <cassert>
#include <map>
#include <set>
#include <unordered_set>
#include <utility>
#include <vector>

using node_type = int;
using size_type = int;
using arc_t = std::pair<int,int>;

#define N (100'000)

namespace {

  enum class Type {
    kTemporary,
    kPermanent,
    kFinalized
  };

  struct Component {
    node_type root;
    std::unordered_set<node_type> bag{};
    std::vector<size_type> edge_idx{};
    Type type = Type::kTemporary;
    size_t size() const {
      return edge_idx.size();
    }
    void add(node_type x) {
      bag.insert(x);
    }
    bool contains(node_type x) const {
      return bag.count(x) > 0;
    }
    void addEdge(size_type idx) {
      edge_idx.push_back(idx);
    }
    Component& operator += (const Component& other) {
      for (auto x : other.bag) {
        this->bag.insert(x);
      }
      edge_idx.insert(edge_idx.end(), other.edge_idx.begin(),
                      other.edge_idx.end());
      return *this;
    }
  };

  class ArcManager {
    std::map<arc_t, size_type> m_arc_id;
    std::map<size_type, arc_t> m_rev_map;
   public:
    size_type add(arc_t arc) {
      if (m_arc_id.count(arc)) {
        return m_arc_id.at(arc);
      }
      auto k = m_arc_id.size();
      m_rev_map[m_arc_id[arc] = k++] = arc;
      return k-1;
    }
    void clear() {
      m_arc_id.clear();
    }
    size_type lookup(const arc_t& arc) const {
      if (not m_arc_id.count(arc)) {
        return -1;
      }
      return m_arc_id.at(arc);
    }
    node_type destinationOf(size_type idx) const {
      return m_rev_map.at(idx).second;
    }
    node_type sourceOf(size_type idx) const {
      return m_rev_map.at(idx).first;
    }
  };

  class TreeCovering : public ITreeCovering {
    size_t n;
    ArcManager m_manager;
    std::vector<size_type> m_adj[N];
    std::array<size_type, N> m_card;
    std::array<size_type, N> m_parent;
    std::array<bool, N> m_seen;

    size_type calcCard(node_type x) {
      if (m_seen[x]) {
        return m_card[x];
      }
      m_seen[x] = true, m_card[x] = 1; // including itself
      for (const auto pr : m_adj[x]) {
        const auto y = m_manager.destinationOf(pr);
        if (m_seen[y]) {
          continue ;
        }
        m_parent[y] = pr, m_card[x] += calcCard(y);
      }
      return m_card[x];
    }

    std::vector<size_type> childrenOf(node_type x) {
      std::vector<size_type> children;
      for (const auto pr : m_adj[x]) {
        const auto y = m_manager.destinationOf(pr);
        assert(x != y);
        if (m_parent[y] == pr) {
          children.push_back(pr);
        }
      }
      return children;
    }

    static Component singleton(node_type src, Type type) {
      Component cmp;
      cmp.type = type;
      cmp.add(src);
      cmp.root = src;
      return cmp;
    }

    std::vector<Component> caseOne(const node_type src,
                                   std::vector<std::pair<size_type, Component>> temporary,
                                   const size_t L) {

      if (temporary.empty()) {
        return {singleton(src, Type::kTemporary)};
      }

      // Normalize:
      std::vector<std::pair<size_type, Component>> temp;
      for (int i = 0, j; i < temporary.size(); i = j) {
        auto pr = temporary[i];
        for (j = i+1; j < temporary.size() and temporary[j].first == temporary[i].first; ++j) {
          pr.second += temporary[j].second;
        }
        temp.push_back(std::move(pr));
      }
      std::swap(temp, temporary);

      std::vector<Component> components;
      for (int i = 0, j; i < temporary.size(); i = j) {
        size_type acc = 0;
        for (j = i; j < temporary.size() and acc <= L; ++j) {
          acc += 1, acc += temporary[j].second.size();
        }
        Component c;
        c.root = src, c.add(src), c.type = Type::kPermanent;
        for (int k = i; k < j; ++k) {
          c += temporary[k].second;
          // Add the edge that leads to a child:
          c.addEdge(temporary[k].first);
        }
        components.push_back(std::move(c));
      }
      assert(!components.empty());
      if (components.size() == 1) {
          components.back().type = Type::kTemporary;
      }
      return components;
    }

    // Each recursive call decomposes a tree rooted at a node
    // and returns component subtrees
    std::vector<Component> decompose(node_type src, const size_type L) {
      std::vector<Component> result;

      // Indices of arcs leading to children:
      auto children = childrenOf(src);
      // Find the heavy children
      std::vector<size_type> heavy_child_idx;
      std::set<node_type> heavies;
      for (auto ch : children) {
        const auto y = m_manager.destinationOf(ch);
        if (m_card[y] >= L) {
          heavy_child_idx.push_back(ch);
          heavies.insert(y);
        }
      }
      // First, we recursively decompose the trees rooted at the children:
      std::vector<std::pair<size_type, Component>> temporary;
      for (const auto idx : children) {
        const auto y = m_manager.destinationOf(idx);
        auto components = decompose(y,L);
        for (auto &c : components) {
          // If the component contains the child, we declare it "temporary":
          if (c.contains(y) and c.type != Type::kFinalized) {
            c.type = Type::kTemporary;
            temporary.emplace_back(idx, std::move(c));
          } else {
            // We finalize the components not containing the children
            // of "src", i.e. the roots of the subtrees:
            c.type = Type::kFinalized;
            result.push_back(std::move(c));
          }
        }
      }

      // Case 1: "src" has no heavy children
      if (heavies.empty()) {
        const auto res = caseOne(src, std::move(temporary), L);
        result.insert(result.end(), res.begin(), res.end());
        return result;
      }

      // Case 2: "src" has only one heavy child
      if (heavies.size() == 1) {
        // We proceed analogously to Case 1, except for the situation
        // when the component containing the only heavy child u[i] has been
        // declared permanent. In this case, we simply ignore it and proceed
        // directly from the u[i-1] to u[i+1]
        const auto idx = *(heavy_child_idx.begin());
        auto filtr = [idx](const auto& pr) {
          return pr.first == idx;
        };
        auto it = std::find_if(temporary.begin(), temporary.end(), filtr);
        if (it != temporary.end()) {
          if (it->second.type == Type::kPermanent || it->second.type == Type::kFinalized) {
            temporary.erase(std::remove_if(temporary.begin(), temporary.end(), filtr),
                            temporary.end());
          }
        }

        const auto res = caseOne(src, std::move(temporary), L);
        result.insert(result.end(), res.begin(), res.end());
        return result;
      }

      // Case 3: Node "src" is a branching node.
      //
      // The situation here is that the root has several heavy children.
      // First, we make all the components containing these heavy nodes
      // as permanent. If after this no child is left, the root itself
      // becomes a permanent singleton component. Otherwise, we process
      // each of the remaining ranges as in Case 1.

      assert(heavies.size() >= 2);
      // First, we declare permanent all the components containing the heavy nodes:
      for (auto &[idx,b] : temporary) {
        const auto y = m_manager.destinationOf(idx);
        if (heavies.count(y)) {
          b.type = Type::kPermanent;
          result.push_back(b);
        }
      }

      // If in fact all the children left were heavy, "src" is by itself
      // is a permanent single-node component:
      if (children.size() == heavies.size()) {
        result.push_back(singleton(src, Type::kPermanent));
        return result;
      }

      auto fltr = [this, &heavies](const auto& pr) {
        return heavies.count(this->m_manager.destinationOf(pr.first)) > 0;
      };
      // Otherwise, the remaining children of "src" are broken by the heavy
      // nodes into intervals of consecutive non-heavy nodes:
      for(int i = 0, j; i < temporary.size(); i = j) {
        int k = i;
        for (;k < temporary.size() and fltr(temporary[k]); ++k);
        for (j = k; j < temporary.size() and !fltr(temporary[j]); ++j);
        // "children[j]" is a heavy child, hence we do not include it:
        std::vector<std::pair<size_type, Component>> rng(temporary.begin()+k, temporary.begin()+j);
        if (!rng.empty()) {
          auto ret = caseOne(src, std::move(rng), L);
          for (auto &cmp : ret) {
            cmp.type = Type::kFinalized;
          }
          result.insert(result.end(), ret.begin(), ret.end());
        }
      }

      return result;
    }

   public:

    ~TreeCovering() override = default;

    explicit TreeCovering(std::istream& is) {
      is >> n;
      for(auto i = 0; i < n; ++i) {
        m_adj[i].clear();
      }
      for (int k = 0; k < n-1; ++k) {
        node_type i, j;
        is >> i >> j; // 1-based index
        --i, --j;
        auto idxF = m_manager.add({i,j});
        auto idxB = m_manager.add({j,i});
        m_adj[i].push_back(idxF);
        m_adj[j].push_back(idxB);
      }
      std::fill(m_seen.begin(), m_seen.begin()+n, false);
      std::fill(m_parent.begin(), m_parent.begin()+n, -1);
      calcCard(0);
    }

    void print(std::ostream& os, const size_t L) override {
      const auto components = decompose(0, L);
      int component = 0;
      for (const auto &c : components) {
        os << "[Component No] " << ++component << '\n';
        if (c.size() == 0) {
          os << "Single-node cluster: " << 1 + (c.root) << '\n';
          continue;
        }
        for (auto idx : c.edge_idx) {
          const auto x = m_manager.sourceOf(idx);
          const auto y = m_manager.destinationOf(idx);
          os << (x+1) << "->" << (y+1) << '\n';
        }
      }
    }

  };
} // namespace

std::shared_ptr<ITreeCovering> createTreeCovering(std::istream& is) {
  return std::make_shared<TreeCovering>(is);
}