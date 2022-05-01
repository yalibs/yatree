#include <iostream>
#include <tree>

void print_node(const std::string& node) {
    std::cout << node << " ";
}

int main(int argc, char** argv) {
    //// You can create trees constructor-pattern style
    ////   +
    ////  / \
    //// 1   2
    auto my_tree = tree<std::string>{"+"}
                    .emplace("1")
                    .emplace("2");
    my_tree.apply_dfs(print_node); // + 1 2
    std::cout << std::endl;

    //// You can even concatenate trees together
    ////    *
    ////   / \
    ////  3   +
    ////     / \
    ////    1   2
    auto my_tree2 = tree<std::string>{"*"}
                    .emplace("3")
                    .concat(my_tree);
    my_tree2.apply_dfs(print_node); // * 3 + 1 2
    std::cout << std::endl;
    return 0;
}
