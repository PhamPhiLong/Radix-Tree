#include "radix_tree.h"
#include <string>
#include <iostream>
#include <map>

using namespace phamphilong;

int main () {
    radix_tree<std::string, std::string> radix_tree;
    radix_tree.insert(std::make_pair<>("abc", "abc"));
    radix_tree.insert(std::make_pair<>("abcef", "abcef"));
    radix_tree.insert(std::make_pair<>("abd", "abd"));
    radix_tree.insert(std::make_pair<>("ab", "ab"));
    radix_tree.insert(std::make_pair<>("a", "a"));
    auto it = radix_tree.find("abz");
    if (it != radix_tree.end()) {
        std::cout << "ghgj" << std::endl;
    }

    for (auto it = radix_tree.begin(); it != radix_tree.end(); ++it) {
        std::cout << "key: " << it->first << "value: " << it->second << std::endl;
    }

    return 0;
}