#include "radix_tree.h"
#include <string>
#include <iostream>

using namespace phamphilong;

int main() {
    radix_tree<std::string, std::string> radix_tree;
    radix_tree.insert(std::make_pair<>("abc", "abc"));
    radix_tree.insert(std::make_pair<>("abcef", "abcef"));
    radix_tree.insert(std::make_pair<>("abd", "abd"));
    radix_tree.insert(std::make_pair<>("ab", "ab"));
    radix_tree.insert(std::make_pair<>("a", "a"));
    auto it = radix_tree.find("abz");
    if (it != radix_tree.end()) {
        std::cout << it->value << std::endl;
    }

    for (auto it = radix_tree.begin(); it != radix_tree.end(); ++it) {

    }
    return -1;
}
//no matching function for call to ‘make_pair(std::pair<std::__detail::_Node_iterator<std::pair<const std::__cxx11::basic_string<char>, std::shared_ptr<phamphilong::radix_tree_node<std::__cxx11::basic_string<char>, int> > >, false, true>, bool>&, bool)’