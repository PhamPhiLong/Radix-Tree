//
// Created by Pham Phi Long on 09/08/2016.
//

#ifndef PHAM_PHI_LONG_RADIX_TREE_H
#define PHAM_PHI_LONG_RADIX_TREE_H

#include <memory>
#include "radix_tree_iterator.h"

namespace phamphilong {
    /**
     * Read:
     *      https://en.wikipedia.org/wiki/Radix_tree
     * for algorithm details.
     */
    template <typename Key, typename T, typename Split, typename Len>
    class radix_tree {
        using iterator = radix_tree_iterator<Key, T>;
        using node = radix_tree_node<Key, T>;
        using mapped_type = T;
        using value_type = std::pair<Key, T>;
        using key_type = Key;

    public:

        iterator begin() noexcept {
            return iterator{root_node.get()};
        }

        iterator end() noexcept {
            return iterator{nullptr};
        }

        iterator find(const key_type& key) {
            if (!root_node) {
                // empty radix-tree
                return end();
            }

            return find_node(key, 0, root_node.get());
        }

        std::pair<iterator, bool> insert(const value_type& val) {
            key_type& key = val.first;
            mapped_type& value = val.second;

            if (!root_node) {
                // add root node
                root_node = std::make_shared<radix_tree_node<key_type, T>>(split_key(key, 0, 0), nullptr);
            }

            auto parent_it = find_parent_node(key, 0, root_node.get());
            if (parent_it == end()) {
                return std::make_pair<iterator, bool>(parent_it, false);
            }

            bool found_common_branch{false};
            key_type sub_key = split_key(key, parent_it->depth);

            for (auto &child_node : parent_it->children) {
                std::size_t child_key_len = get_key_len(child_node->key);

                if (child_node.key == sub_key) {
                }
            }

            if (!found_common_branch) {
                auto new_node_it = parent_it.pointed_node->children.insert(std::make_shared<node >(
                        sub_key,                       // key
                        value,                        // value
                        parent_it.pointed_node,       // parent_node
                        true,                         // is_leaf
                        get_key_len(key)              // depth
                ));

                return std::make_pair<iterator, bool>(new_node_it, true);
            }
        };

    private:
        std::shared_ptr<radix_tree_node<key_type, T>> root_node{nullptr};
        const Split split_key{};
        const Len get_key_len{};

        iterator find_node(const key_type& key, const std::size_t cur_key_depth, const radix_tree_node<key_type, T>* traverse_node) {
            if (traverse_node == nullptr) {
                return end();
            }

            if (traverse_node->is_leaf) {
                if (cur_key_depth == get_key_len(key)) {
                    return iterator{traverse_node};
                }
            } else {
                for (auto &child_node : traverse_node->children) {
                    std::size_t child_key_len = get_key_len(child_node->key);
                    key_type sub_key = split_key(key, cur_key_depth, child_key_len);

                    if (child_node.second->key == sub_key) {
                        std::size_t new_key_depth = cur_key_depth + child_key_len;
                        return find_node(key, new_key_depth, child_node);
                    }
                }
            }

            return end();       // cannot find
        }

        iterator find_parent_node(const key_type& key, const std::size_t cur_key_depth, const radix_tree_node<Key, T>* parent_node) {
            if (parent_node == nullptr) {
                return end();
            }

            for (auto &child_node : parent_node->children) {
                std::size_t child_key_len = get_key_len(child_node->key);
                key_type sub_key = split_key(key, cur_key_depth, child_key_len);

                if (child_node.key == sub_key) {      // matched path
                    std::size_t new_key_depth = cur_key_depth + child_key_len;
                    return find_parent_node(key, new_key_depth, child_node);
                }
            }

            return parent_node;
        }
    };
}

#endif //PHAM_PHI_LONG_RADIX_TREE_H
