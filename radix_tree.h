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
     * for the definition.
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
                root_node = new node(key_type{}, mapped_type{}, nullptr, false, static_cast<std::size_t >(0));
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
                    /**
                     * Current keys : (abc), (abcedf)
                     *
                     * (root)
                     *   |____ (abc)
                     *           |____ ()
                     *           |____ (ef)
                     *
                     * insert key (abd)
                     *
                     * (root)
                     *   |____ (ab)
                     *           |____ (c)
                     *           |      |____ ()
                     *           |      |____ (ef)
                     *           |____ (d)
                     *
                     *  parent_it = (root)
                     *  found_node = (abc)
                     *  new_node = (d)
                     *  new_parent_node = (ab)
                     *  common_branch = (root) ____ (abc)
                     *
                     *  Algorithm:
                     *      (step 1) remove but not delete node (abc) from parent_it (root) children list
                     *      (step 2) add new parent node (ab) to parent_it (root)
                     *      (step 3) transform (abc) to (c)
                     *      (step 4) add (c) to (ab) children list
                     *      (step 5) add new node (d) to (ab) children list
                     *
                     */

                    // (step 1) remove but not delete node (abc) from parent_it (root) children list
                    node* found_node = child_node.second;
                    parent_it->children.erase(child_node.first);

                    // (step 2) add new parent node (ab) to parent_it (root)
                    key_type new_parent_node_key = split_key(found_node->key, 0, i);
                    std::size_t new_parent_node_depth = parent_it->depth + get_key_len(new_parent_node_key);
                    auto new_parent_insert_result = parent_it->children.insert(std::pair<key_type, node*>(
                            new_parent_node_key,
                            new node{
                                    new_parent_node_key,          // key
                                    mapped_type{},                // value
                                    parent_it.pointed_node,       // parent_node
                                    false,                        // is_leaf
                                    new_parent_node_depth         // depth
                            }
                    ));

                    // (step 3) transform (abc) to (c)
                    found_node->key = split_key(found_node->key, i);

                    // (step 4) add (c) to (ab) children list
                    node* new_parent_node = new_parent_insert_result.first->second;
                    new_parent_node->children.insert(std::pair<key_type, node*>(found_node->key, found_node));

                    // (step 5) add new node (d) to (ab) children list
                    key_type new_node_key = split_key(sub_key, i);
                    std::size_t new_node_depth = new_parent_node->depth + get_key_len(new_node_key);
                    auto new_node_insert_result = new_parent_node->children.insert(std::pair<key_type, node*>(
                            new_node_key,
                            new node{
                                    new_node_key,                   // key
                                    val.second,                     // value
                                    new_parent_node,                // parent_node
                                    true,                           // is_leaf
                                    new_node_depth                  // depth
                            }
                    ));

                    return std::pair<iterator, bool>(new_node_insert_result.first->second, true);
                }
            }

            if (!found_common_branch) {
                /**
                 * Current keys : (abc)
                 *
                 * (root)
                 *   |____ (abc)
                 *
                 * insert key (abcef)
                 *
                 * (root)
                 *   |____ (abc)
                 *           |____ ()
                 *           |____ (ef)
                 *
                 */
                if (parent_it->is_leaf) {
                    parent_it->children.insert(std::pair<Key, node *>(
                            key_type{},
                            new node{
                                    key_type{},                   // key
                                    parent_it->value,             // value
                                    parent_it.pointed_node,       // parent_node
                                    true,                         // is_leaf
                                    parent_it->depth              // depth
                            }
                    ));

                    parent_it->value = mapped_type{};
                    parent_it->is_leaf = false;
                }

                auto new_node = parent_it->children.insert(std::pair<Key, node *>(
                        sub_key,
                        new node{
                                sub_key,                      // key
                                val.second,                   // value
                                parent_it.pointed_node,       // parent_node
                                true,                         // is_leaf
                                get_key_len(val.first)        // depth
                        }
                ));

                auto new_node_it = parent_it->children.find(sub_key);
                if (new_node.second && (new_node_it != parent_it->children.end())) {
                    return std::pair<iterator, bool>(new_node_it->second, true);
                }
            }

            return std::pair<iterator, bool>(end(), false);
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
                if (cur_key_depth == get_key_len(key) && traverse_node->key == split_key(key, cur_key_depth)) {
                    return iterator{traverse_node};
                }
            } else {
                for (auto &child_node : traverse_node->children) {
                    std::size_t child_key_len = get_key_len(child_node.first);
                    key_type sub_key = split_key(key, cur_key_depth, child_key_len);

                    if (child_key_len > 0 && child_node.second->key == sub_key) {
                        std::size_t new_key_depth = cur_key_depth + child_key_len;
                        return find_node(key, new_key_depth, child_node.second);
                    } else if (child_key_len == 0 && child_node.second->is_leaf && child_node.second->depth == get_key_len(key)) {
                        return iterator{child_node.second};
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

                if (child_key_len > 0 && child_node.first == sub_key) {      // matched path
                    std::size_t new_key_depth = cur_key_depth + child_key_len;
                    return find_parent_node(key, new_key_depth, child_node.second);
                }
            }

            return parent_node;
        }
    };
}

#endif //PHAM_PHI_LONG_RADIX_TREE_H
