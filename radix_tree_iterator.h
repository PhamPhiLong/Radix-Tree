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
            return *pointed_node;
        }

        radix_tree_node<Key, T>* operator-> () const {
            return pointed_node;
        }

        const radix_tree_iterator<Key, T>& operator++ () {

        }

        radix_tree_iterator<Key, T> operator++ (int) {

        }

        bool operator!= (const radix_tree_iterator<Key, T> &lhs) const {
            return pointed_node != lhs.pointed_node;
        }

        bool operator== (const radix_tree_iterator<Key, T> &lhs) const {
            return pointed_node == lhs.pointed_node;
        }


    private:
        radix_tree_iterator(const radix_tree_node<Key, T>* pointed_node) : pointed_node{pointed_node} {}
        radix_tree_node<Key, T>* pointed_node;
    };
}

#endif //PHAM_PHI_LONG_RADIX_TREE_ITERATOR_H
