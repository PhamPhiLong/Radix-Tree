//
// Created by Pham Phi Long on 09/08/2016.
//

#ifndef PHAM_PHI_LONG_RADIX_TREE_ITERATOR_H
#define PHAM_PHI_LONG_RADIX_TREE_ITERATOR_H

#include "radix_tree_node.h"

namespace phamphilong {
    template <typename Key, typename T, typename Split, typename Len>
    class radix_tree_iterator {
        friend class radix_tree<Key, T, Split, Len>;

    public:
        using mapped_type = T;
        using key_type = Key;
        using iterator = radix_tree_iterator<key_type, mapped_type, Split, Len>;
        using node_type = radix_tree_node<key_type , mapped_type , Split, Len>;
        using value_type = std::pair<const key_type , mapped_type>;

        value_type& operator* () const;
        value_type* operator-> () const;
        const iterator& operator++ ();
        iterator operator++ (int);
        bool operator!= (const iterator& lhs) const;
        bool operator== (const iterator& lhs) const;

    private:
        radix_tree_iterator(node_type * pointed_node) : pointed_node{pointed_node} {}
        node_type * pointed_node;
    };

    template <typename Key, typename T, typename Split, typename Len>
    inline typename radix_tree_iterator<Key, T, Split, Len>::value_type& radix_tree_iterator<Key, T, Split, Len>::operator* () const {
        return *pointed_node->value;
    }

    template <typename Key, typename T, typename Split, typename Len>
    inline typename radix_tree_iterator<Key, T, Split, Len>::value_type* radix_tree_iterator<Key, T, Split, Len>::operator-> () const {
        return pointed_node->value.get();
    }

    template <typename Key, typename T, typename Split, typename Len>
    const typename radix_tree_iterator<Key, T, Split, Len>::iterator& radix_tree_iterator<Key, T, Split, Len>::operator++ () {
        // find first children node
        auto child_it = pointed_node->children.begin();
        if (child_it != pointed_node->children.end()) {
            pointed_node = child_it->second;
        } else {
            // cannot find any children, then find sibling
            auto parent_node = pointed_node->parent_node;
            while (parent_node != nullptr) {
                auto it = pointed_node->parent_node->children.find(pointed_node->get_search_key());
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

        if (pointed_node->parent_node == nullptr) {
            // already reached final node
            this->pointed_node = nullptr;
            return *this;
        } else if (pointed_node->is_leaf()) {
            return *this;
        } else {
            return this->operator++();
        }
    }

    template <typename Key, typename T, typename Split, typename Len>
    typename radix_tree_iterator<Key, T, Split, Len>::iterator radix_tree_iterator<Key, T, Split, Len>::operator++ (int) {
        auto temp = *this;
        ++*this;
        return temp;
    }

    template <typename Key, typename T, typename Split, typename Len>
    inline bool radix_tree_iterator<Key, T, Split, Len>::operator!= (const iterator& lhs) const {
        return pointed_node != lhs.pointed_node;
    }

    template <typename Key, typename T, typename Split, typename Len>
    inline bool radix_tree_iterator<Key, T, Split, Len>::operator== (const iterator& lhs) const {
        return pointed_node == lhs.pointed_node;
    }
}

#endif //PHAM_PHI_LONG_RADIX_TREE_ITERATOR_H
