#include "radix_tree.h"

using namespace phamphilong;

int main() {
    radix_tree<std::string, int> radix_tree;
    auto it = radix_tree.find("aha");
    //radix_tree.insert(std::make_pair<std::string, int>("ahihi", 1));
    return 0;
}