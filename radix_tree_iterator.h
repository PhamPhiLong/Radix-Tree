//
// Created by Pham Phi Long on 09/08/2016.
//

#ifndef PHAM_PHI_LONG_RADIX_TREE_ITERATOR_H
#define PHAM_PHI_LONG_RADIX_TREE_ITERATOR_H

#include "radix_tree_node.h"

namespace phamphilong {
    template <typename Key, typename T>
    class radix_tree_iterator {
        friend class radix_tree<Key, T>;

    public:
        radix_tree_node<Key, T>& operator*  () const {
            return pointed_node;
        }

        radix_tree_node<Key, T>* operator-> () const {
            return pointed_node;
        }

        const radix_tree_iterator<Key, T>& operator++ () {
            // find first children node
            auto child_it = pointed_node->children.begin();
            if (child_it != pointed_node->children.end()) {
                pointed_node = child_it->second;
            } else {
                // cannot find any children, then find sibling
                auto parent_node = pointed_node->parent_node;
                while (parent_node != nullptr) {
                    auto it = pointed_node->parent_node->children.find(pointed_node->key);
                    if (it != pointed_node->parent_node->children.end()) {
                        ++it;
                        if (it != pointed_node->parent_node->children.end()) {
                            // found a sibling, jump to it
                            pointed_node = it->second;
                            break;
                        }
                    } else {
                        // something wrong here, it's impossible in theory
                        break;
                    }

                    // there is no sibling, then go upward
                    pointed_node = parent_node;
                    parent_node = pointed_node->parent_node;
                }
            }

            if (pointed_node->is_leaf) {
                return *this;
            } else {
                return this->operator++();
            }
        }

        radix_tree_iterator<Key, T> operator++ (int) {
            auto temp = *this;
            ++*this;
            return temp;
        }

        bool operator!= (const radix_tree_iterator<Key, T> &lhs) const {
            return pointed_node != lhs.pointed_node;
        }

        bool operator== (const radix_tree_iterator<Key, T> &lhs) const {
            return pointed_node == lhs.pointed_node;
        }


    private:
        radix_tree_iterator(radix_tree_node<Key, T>* pointed_node) : pointed_node{pointed_node} {}
        radix_tree_node<Key, T>* pointed_node;
    };
}

#endif //PHAM_PHI_LONG_RADIX_TREE_ITERATOR_H
