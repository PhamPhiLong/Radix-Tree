//
// Created by Pham Phi Long on 09/08/2016.
//

#ifndef PHAM_PHI_LONG_RADIX_TREE_NODE_H
#define PHAM_PHI_LONG_RADIX_TREE_NODE_H

#include <unordered_map>

namespace phamphilong {
    template <typename Key> struct split;
    template <typename Key> struct radix_len;

    template <>
    struct split<std::string> {
        std::string operator()(const std::string& key, std::size_t start, std::size_t len) const {
            return key.substr(start, len);
        }

        std::string operator()(const std::string& key, std::size_t start) const {
            return key.substr(start);
        }
    };

    template <>
    struct radix_len<std::string> {
         std::size_t operator()(const std::string& key) const {
            return key.length();
        }
    };

    template <
            typename Key,
            typename T,
            typename Split = split<Key>,
            typename Len = radix_len<Key>
    > class radix_tree;

    template <typename Key, typename T> class radix_tree_iterator;

    template <typename Key, typename T>
    class radix_tree_node {
        friend class radix_tree<Key, T>;
        friend class radix_tree_iterator<Key, T>;
    public:

    private:
        radix_tree_node(Key key, T value, radix_tree_node* parent_node, const bool is_leaf, const std::size_t depth)
                : key{std::move(key)}, value{std::move(value)}, parent_node{parent_node}, is_leaf{is_leaf}, depth{depth} {}

        Key key;
        T value;
        radix_tree_node* parent_node{nullptr};
        bool is_leaf{false};
        std::size_t depth{0};
        std::unordered_map<Key, radix_tree_node<Key, T>*> children{};
    };
}

#endif //PHAM_PHI_LONG_RADIX_TREE_NODE_H
