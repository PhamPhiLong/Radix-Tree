//
// Created by Pham Phi Long on 09/08/2016.
//

#ifndef PHAM_PHI_LONG_RADIX_TREE_NODE_H
#define PHAM_PHI_LONG_RADIX_TREE_NODE_H

#include <unordered_map>
#include <vector>

namespace phamphilong {
    template <typename K, typename V>
    class radix_tree_node {
        friend class radix_tree;
        friend class radix_tree_iterator;
    public:

    private:
        radix_tree_node() {

        }

        K key;
        V value;
        radix_tree_node* parent;
        std::unordered_map<K, std::shared_ptr<radix_tree_node>> children;
        std::vector<int>
    };
}

#endif //PHAM_PHI_LONG_RADIX_TREE_NODE_H
