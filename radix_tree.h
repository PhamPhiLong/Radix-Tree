//
// Created by Pham Phi Long on 09/08/2016.
//

#ifndef PHAM_PHI_LONG_RADIX_TREE_H
#define PHAM_PHI_LONG_RADIX_TREE_H

#include "radix_tree_iterator.h"
#include <memory>

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

            return find_node(key, 0, root_node);
        }

        std::pair<iterator, bool> insert(const value_type& val) {
            if (!root_node) {
                // add root node
                //root_node = std::make_shared<radix_tree_node<key_type, mapped_type>>(split_key(val.first, 0, 0), nullptr);
                root_node = new node(split_key(val.first, 0, 0), val.second, nullptr, false, static_cast<std::size_t >(0));
            }

            auto parent_it = find_parent_node(val.first, 0, root_node);
            if (parent_it == end()) {
                return std::make_pair<iterator, bool>(std::move(parent_it), false);
            }

            bool found_common_branch{false};
            key_type sub_key = split_key(val.first, parent_it->depth);
            auto sub_key_len = get_key_len(sub_key);

            for (auto &child_node : parent_it->children) {
                std::size_t child_key_len = get_key_len(child_node.first);
                std::size_t i=0;
                for (; (i < child_key_len) && (i < sub_key_len) && (child_node.first[i] == sub_key[i]); ++i) {
                    found_common_branch = true;
                }

                if (found_common_branch) {

                }
            }

            if (parent_it->is_leaf) {
                parent_it.pointed_node->children.insert(std::make_pair<Key, node *>(
                        key_type{},
                        new node{
                                key_type{},                   // key
                                parent_it->value,             // value
                                parent_it.pointed_node,       // parent_node
                                true,                         // is_leaf
                                parent_it->depth              // depth
                        }
                ));
            }

            auto new_node = parent_it.pointed_node->children.insert(std::make_pair<Key, node*>(
                    key_type{sub_key},
                    new node{
                            sub_key,                      // key
                            val.second,                   // value
                            parent_it.pointed_node,       // parent_node
                            true,                         // is_leaf
                            get_key_len(val.first)        // depth
                    }
            ));

            auto new_node_it = parent_it.pointed_node->children.find(sub_key);
            if (new_node.second && (new_node_it != parent_it.pointed_node->children.end())) {
                return std::make_pair<iterator, bool>(iterator(new_node_it->second), true);
            }

            return std::make_pair<iterator, bool>(end(), false);
        };

    private:
        node* root_node{nullptr};
        const Split split_key{};
        const Len get_key_len{};

        iterator find_node(const key_type& key, const std::size_t cur_key_depth, radix_tree_node<key_type, mapped_type>* traverse_node) {
            if (traverse_node == nullptr) {
                return end();
            }

            if (traverse_node->is_leaf) {
                if (cur_key_depth == get_key_len(key)) {
                    return iterator{traverse_node};
                }
            } else {
                for (auto &child_node : traverse_node->children) {
                    std::size_t child_key_len = get_key_len(child_node.first);
                    key_type sub_key = split_key(key, cur_key_depth, child_key_len);

                    if (child_node.second->key == sub_key) {
                        std::size_t new_key_depth = cur_key_depth + child_key_len;
                        return find_node(key, new_key_depth, child_node.second);
                    }
                }
            }

            return end();       // cannot find
        }

        iterator find_parent_node(const key_type& key, const std::size_t cur_key_depth, radix_tree_node<Key, mapped_type>* parent_node) {
            if (parent_node == nullptr) {
                return end();
            }

            for (auto &child_node : parent_node->children) {
                std::size_t child_key_len = get_key_len(child_node.first);
                key_type sub_key = split_key(key, cur_key_depth, child_key_len);

                if (child_node.first == sub_key) {      // matched path
                    std::size_t new_key_depth = cur_key_depth + child_key_len;
                    return find_parent_node(key, new_key_depth, child_node.second);
                }
            }

            return parent_node;
        }
    };
}

#endif //PHAM_PHI_LONG_RADIX_TREE_H
