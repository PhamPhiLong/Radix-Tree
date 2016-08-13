#include "radix_tree.h"
#include <string>
#include <iostream>
#include <map>

using namespace phamphilong;

int main () {
    radix_tree<std::string, std::string> radix_tree;


    radix_tree.insert(std::make_pair<>("abc", "abc"));
    std::cout << radix_tree.size() << std::endl;
    radix_tree.insert(std::make_pair<>("abcef", "abcef"));
    std::cout << radix_tree.size() << std::endl;
    radix_tree.insert(std::make_pair<>("abd", "abd"));
    std::cout << radix_tree.size() << std::endl;
    radix_tree.insert(std::make_pair<>("abg", "abg"));
    std::cout << radix_tree.size() << std::endl;
    for (auto it = radix_tree.begin(); it != radix_tree.end(); ++it) {
        std::cout << "key: " << it->first << ";        value: " << it->second << std::endl;
    }

    radix_tree.erase("abg");
    std::cout << radix_tree.size() << std::endl;
    for (auto it = radix_tree.begin(); it != radix_tree.end(); ++it) {
        std::cout << "key: " << it->first << ";        value: " << it->second << std::endl;
    }

    radix_tree.erase("abc");
    std::cout << radix_tree.size() << std::endl;
    for (auto it = radix_tree.begin(); it != radix_tree.end(); ++it) {
        std::cout << "key: " << it->first << ";        value: " << it->second << std::endl;
    }

    auto it = radix_tree.find("abz");
    if (it != radix_tree.end()) {
        std::cout << "ghgj" << std::endl;
    }

    radix_tree.clear();
    std::cout << radix_tree.size() << std::endl;
    for (auto it = radix_tree.begin(); it != radix_tree.end(); ++it) {
        std::cout << "key: " << it->first << ";        value: " << it->second << std::endl;
    }

    return 0;
}