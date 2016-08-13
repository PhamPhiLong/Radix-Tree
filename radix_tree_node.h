//
// Created by Pham Phi Long on 09/08/2016.
//

#ifndef PHAM_PHI_LONG_RADIX_TREE_NODE_H
#define PHAM_PHI_LONG_RADIX_TREE_NODE_H

#include <unordered_map>
#include <memory>

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

    template <typename Key, typename T, typename Split, typename Len> class radix_tree;
    template <typename Key, typename T, typename Split, typename Len> class radix_tree_iterator;

    template <typename Key, typename T, typename Split, typename Len>
    class radix_tree_node {
        friend class radix_tree<Key, T, Split, Len>;
        friend class radix_tree_iterator<Key, T, Split, Len>;

    private:
        using mapped_type = T;
        using key_type = Key;
        using iterator = radix_tree_iterator<key_type, mapped_type, Split, Len>;
        using node_type = radix_tree_node<key_type , mapped_type , Split, Len>;
        using value_type = std::pair<const key_type , mapped_type>;
        using size_type = std::size_t;

        radix_tree_node(value_type value, radix_tree_node* parent_node, const size_type depth)
                : value{new value_type(value)}, parent_node{parent_node}, depth{depth} {}

        const Split split_key{};
        std::unique_ptr<value_type> value{nullptr};
        radix_tree_node* parent_node{nullptr};
        size_type depth{0};
        std::unordered_map<key_type, radix_tree_node<key_type, T, Split, Len>*> children{};

        bool is_leaf() {
            return children.empty();
        }

        bool is_root() {
            return nullptr == parent_node;
        }

        key_type get_search_key() {
            // return the key of this node in parent's children map
            if (is_root()) {
                return split_key(value->first, 0, depth);
            }

            return split_key(value->first, parent_node->depth, depth);
        }

        ~radix_tree_node() {
            for (auto& child : children) {
                delete child.second;
            }
        }
    };
}

#endif //PHAM_PHI_LONG_RADIX_TREE_NODE_H
