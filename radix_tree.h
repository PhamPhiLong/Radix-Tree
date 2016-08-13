//
// Created by Pham Phi Long on 09/08/2016.
//

#ifndef PHAM_PHI_LONG_RADIX_TREE_H
#define PHAM_PHI_LONG_RADIX_TREE_H

#include "radix_tree_iterator.h"

namespace phamphilong {
    /**
     * Read:
     *      https://en.wikipedia.org/wiki/Radix_tree
     * for the definition.
     */
    template <
            typename Key,
            typename T,
            typename Split = split<Key>,
            typename Len = radix_len<Key>>
    class radix_tree {
    public:
        using mapped_type = T;
        using key_type = Key;
        using iterator = radix_tree_iterator<key_type, mapped_type, Split, Len>;
        using node_type = radix_tree_node<key_type , mapped_type , Split, Len>;
        using value_type = std::pair<const key_type , mapped_type>;


        iterator begin() noexcept {
            return ++iterator{root_node};
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

        std::pair<iterator, bool> insert(const value_type& value) {
            if (!root_node) {
                // add root node
                //root_node = std::make_shared<node_type>(split_key(value.first, 0, 0), nullptr);
                root_node = new node_type(
                        std::make_pair(key_type{}, mapped_type{}),      // value
                        nullptr,                                        // parent_node
                        static_cast<std::size_t >(0));                  // depth
            }

            auto parent_it = find_parent_node(value.first, 0, root_node);
            if (parent_it == end()) {
                return std::make_pair<iterator, bool>(std::move(parent_it), false);
            }

            bool found_common_branch{false};
            key_type sub_key = split_key(value.first, parent_it.pointed_node->depth);
            auto sub_key_len = get_key_len(sub_key);

            for (auto &child_node : parent_it.pointed_node->children) {
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
                     *      (step 2) add new parent node (ab) to current parent node (root)
                     *      (step 3) transform fond node (abc) to (c)
                     *      (step 4) add found node (c) to the children list of new parent node (ab)
                     *      (step 5) add new node (d) to (ab) children list
                     *
                     */

                    // (step 1) remove but not delete node (abc) from parent_it (root) children list
                    node_type* found_node = child_node.second;
                    parent_it.pointed_node->children.erase(child_node.first);

                    // (step 2) add new parent node (ab) to current parent node (root)
                    key_type new_parent_node_search_key = split_key(found_node->get_search_key(), 0, i);
                    key_type new_parent_node_key = split_key(found_node->value->first, 0, parent_it.pointed_node->depth + i);
                    auto new_parent_insert_result = parent_it.pointed_node->children.insert(std::pair<key_type, node_type*>(
                            new_parent_node_search_key,
                            new node_type{
                                    std::make_pair(new_parent_node_key, mapped_type{}),                // value
                                    parent_it.pointed_node,                                            // parent_node
                                    get_key_len(new_parent_node_key)                                   // depth
                            }
                    ));
                    node_type* new_parent_node = new_parent_insert_result.first->second;

                    // (step 3) transform found node (abc) to (c)
                    found_node->parent_node = new_parent_node;

                    // (step 4) add found node (c) to the children list of new parent node (ab)
                    new_parent_node->children.insert(std::pair<key_type, node_type*>(found_node->get_search_key(), found_node));

                    // (step 5) add new node (d) to (ab) children list
                    key_type new_node_search_key = split_key(sub_key, i);
                    auto new_node_insert_result = new_parent_node->children.insert(std::pair<key_type, node_type*>(
                            new_node_search_key,
                            new node_type{
                                    value,                      // value
                                    new_parent_node,            // parent_node
                                    get_key_len(value.first)    // depth
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
                if ((parent_it.pointed_node != root_node) && parent_it.pointed_node->is_leaf()) {
                    parent_it.pointed_node->children.insert(std::pair<key_type , node_type*>(
                            key_type{},
                            new node_type {
                                    *parent_it.pointed_node->value,            // value
                                    parent_it.pointed_node,                    // parent_node
                                    parent_it.pointed_node->depth              // depth
                            }
                    ));

                    parent_it.pointed_node->value->second = mapped_type{};
                }

                auto new_node = parent_it.pointed_node->children.insert(std::pair<Key, node_type*>(
                        sub_key,
                        new node_type{
                                value,                           // value
                                parent_it.pointed_node,          // parent_node
                                get_key_len(value.first)         // depth
                        }
                ));

                auto new_node_it = parent_it.pointed_node->children.find(sub_key);
                if (new_node.second && (new_node_it != parent_it.pointed_node->children.end())) {
                    return std::pair<iterator, bool>(new_node_it->second, true);
                }
            }

            return std::pair<iterator, bool>(end(), false);
        };

    private:
        node_type* root_node{nullptr};
        const Split split_key{};
        const Len get_key_len{};

        iterator find_node(const key_type& key, const std::size_t cur_key_depth, node_type* traverse_node) {
            if (traverse_node == nullptr) {
                return end();
            }

            if (traverse_node->is_leaf()) {
                if (cur_key_depth == get_key_len(key) && traverse_node->value->first == key) {
                    return iterator{traverse_node};
                }
            } else {
                for (auto &child_node : traverse_node->children) {
                    std::size_t child_key_len = get_key_len(child_node.first);
                    key_type sub_key = split_key(key, cur_key_depth, child_key_len);

                    if (child_key_len > 0 && child_node.first == sub_key) {
                        std::size_t new_key_depth = cur_key_depth + child_key_len;
                        return find_node(key, new_key_depth, child_node.second);
                    } else if (child_key_len == 0 && child_node.second->is_leaf() && child_node.second->depth == get_key_len(key) && child_node.second->value->first == key) {
                        return iterator{child_node.second};
                    }
                }
            }

            return end();       // cannot find
        }

        iterator find_parent_node(const key_type& key, const std::size_t cur_key_depth, node_type* parent_node) {
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
