#include "radix_tree.h"
#include <string>

using namespace phamphilong;

int main() {
    radix_tree<std::string, int> radix_tree;
    auto it = radix_tree.find("aha");
    radix_tree.insert(std::make_pair<std::string, int>("ahihi", 1));
    return 0;
}
//no matching function for call to ‘make_pair(std::pair<std::__detail::_Node_iterator<std::pair<const std::__cxx11::basic_string<char>, std::shared_ptr<phamphilong::radix_tree_node<std::__cxx11::basic_string<char>, int> > >, false, true>, bool>&, bool)’