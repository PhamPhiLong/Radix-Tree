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
        using size_type = std::size_t;

        iterator begin() const noexcept;
        iterator end() const noexcept;
        iterator find(const key_type& key) const noexcept;
        std::pair<iterator, bool> insert(const value_type& value);
        size_type erase(const key_type& key);
        size_type size() const noexcept;
        void clear() noexcept;

    private:
        node_type* root_node{nullptr};
        size_type tree_size{};
        const Split split_key{};
        const Len get_key_len{};

        iterator find_node(const key_type& key, const size_type cur_key_depth, node_type* traverse_node) const {
            if (traverse_node == nullptr) {
                return end();
            }

            if (traverse_node->is_leaf()) {
                if (cur_key_depth == get_key_len(key) && traverse_node->value->first == key) {
                    return iterator{traverse_node};
                }
            } else {
                for (auto &child_node : traverse_node->children) {
                    size_type child_key_len = get_key_len(child_node.first);
                    key_type sub_key = split_key(key, cur_key_depth, child_key_len);

                    if (child_key_len > 0 && child_node.first == sub_key) {
                        size_type new_key_depth = cur_key_depth + child_key_len;
                        return find_node(key, new_key_depth, child_node.second);
                    } else if (child_key_len == 0 && child_node.second->is_leaf() && child_node.second->depth == get_key_len(key) && child_node.second->value->first == key) {
                        return iterator{child_node.second};
                    }
                }
            }

            return end();       // cannot find
        }

        iterator find_parent_node(const key_type& key, const size_type cur_key_depth, node_type* parent_node) const {
            if (parent_node == nullptr) {
                return end();
            }

            for (auto &child_node : parent_node->children) {
                size_type child_key_len = get_key_len(child_node.first);
                key_type sub_key = split_key(key, cur_key_depth, child_key_len);

                if (child_key_len > 0 && child_node.first == sub_key) {      // matched path
                    size_type new_key_depth = cur_key_depth + child_key_len;
                    return find_parent_node(key, new_key_depth, child_node.second);
                }
            }

            return parent_node;
        }
    };

    template <typename Key, typename T, typename Split, typename Len>
    inline typename radix_tree<Key, T, Split, Len>::iterator radix_tree<Key, T, Split, Len>::begin() const noexcept {
        if (root_node == nullptr) {
            return end();
        }

        return ++iterator{root_node};
    }

    template <typename Key, typename T, typename Split, typename Len>
    inline typename radix_tree<Key, T, Split, Len>::iterator radix_tree<Key, T, Split, Len>::end() const noexcept {
        return iterator{nullptr};
    }

    template <typename Key, typename T, typename Split, typename Len>
    typename radix_tree<Key, T, Split, Len>::iterator radix_tree<Key, T, Split, Len>::find(const key_type& key) const noexcept {
        if (!root_node) {
            // empty radix-tree
            return end();
        }

        return find_node(key, 0, root_node);
    }

    template <typename Key, typename T, typename Split, typename Len>
    std::pair<typename radix_tree<Key, T, Split, Len>::iterator, bool> radix_tree<Key, T, Split, Len>::insert(const value_type& value) {
        if (!root_node) {
            // add root node
            //root_node = std::make_shared<node_type>(split_key(value.first, 0, 0), nullptr);
            root_node = new node_type(
                    std::make_pair(key_type{}, mapped_type{}),      // value
                    nullptr,                                        // parent_node
                    static_cast<size_type >(0));                    // depth
        }

        auto parent_it = find_parent_node(value.first, 0, root_node);
        if (parent_it == end()) {
            return std::make_pair<iterator, bool>(std::move(parent_it), false);
        }

        bool found_common_branch{false};
        key_type sub_key = split_key(value.first, parent_it.pointed_node->depth);
        auto sub_key_len = get_key_len(sub_key);

        for (auto &child_node : parent_it.pointed_node->children) {
            size_type child_key_len = get_key_len(child_node.first);
            size_type i=0;
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

                tree_size++;
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
                tree_size++;
                return std::pair<iterator, bool>(new_node_it->second, true);
            }
        }

        return std::pair<iterator, bool>(end(), false);
    }

    template <typename Key, typename T, typename Split, typename Len>
    typename radix_tree<Key, T, Split, Len>::size_type radix_tree<Key, T, Split, Len>::size() const noexcept {
        return tree_size;
    };

    template <typename Key, typename T, typename Split, typename Len>
    typename radix_tree<Key, T, Split, Len>::size_type radix_tree<Key, T, Split, Len>::erase(const key_type& key) {
        if (root_node == nullptr) {
            return 0;
        }

        auto found_node_it = find_node(key, 0, root_node);
        if (found_node_it == end()) {
            return 0;
        }

        /**
         * Current keys : (abc), (abcedf), (abd), (abg)
         *
         * (root)
         *   |____ (ab)
         *           |____ (c)
         *           |      |____ ()
         *           |      |____ (ef)
         *           |____ (d)
         *           |____ (g)
         *
         *   delete (abg) : simply delete (g)
         *   delete (abc) : delete () then merge (c) and (ef) to form (cef)
         *
         */
        node_type* parent_node = found_node_it.pointed_node->parent_node;
        parent_node->children.erase(found_node_it.pointed_node->get_search_key());
        delete found_node_it.pointed_node;

        if (parent_node->children.size() == 1 && parent_node->parent_node != nullptr) {
            // merge the parent node with its now only child (the only sibling of the erased node)
            node_type* grand_parent_node = parent_node->parent_node;
            node_type* sibling_node = parent_node->children.begin()->second;
            parent_node->children.erase(sibling_node->get_search_key());
            grand_parent_node->children.erase(parent_node->get_search_key());
            delete parent_node;
            sibling_node->parent_node = grand_parent_node;
            grand_parent_node->children.insert(std::make_pair(sibling_node->get_search_key(), sibling_node));
        }

        tree_size--;
        return 1;
    }

    template <typename Key, typename T, typename Split, typename Len>
    void radix_tree<Key, T, Split, Len>::clear() noexcept {
        delete root_node;
        root_node = nullptr;
        tree_size = 0;
    }
}

#endif //PHAM_PHI_LONG_RADIX_TREE_H
