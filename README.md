# YA-TREE
Yet another C/C++ tree implementation. Header-only implementation of a simple tree structure and a DFS algorithm.

# Example Usage
```c++
auto printer_function = [](const std::string& node){ std::cout << node << " "; };
//// You can create trees constructor-pattern style
////   +
////  / \.
//// 1   2
auto my_tree = tree<std::string>{"+"}
                .emplace("1")
                .emplace("2");
my_tree.apply_dfs(printer_function); // + 1 2

//// You can even concatenate trees together
////    *
////   / \.
////  3   +
////     / \.
////    1   2
auto my_tree2 = tree<std::string>{"*"}
                .emplace("3")
                .concat(my_tree);
my_tree2.apply_dfs(printer_function); // * 3 + 1 2

//// trees are even forward iterable in a DFS manner as well.
for(auto& n : my_tree2)
    std::cout << n.node << " "; // * 3 + 1 2
```
