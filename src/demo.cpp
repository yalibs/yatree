/* MIT License
 *
 * Copyright (c) 2022 Asger Gitz-Johansen
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <iostream>
#include <stack>
#include <tree>

void print_tree(const ya::tree<std::string>& tree);
void print_tree_manually(const ya::tree<std::string>& tree);

int main(int argc, char** argv) {
    auto printer_function = [](const std::string& node){ std::cout << node << " "; };
    //// You can create trees constructor-pattern style
    ////   +
    ////  / \
    //// 1   2
    auto my_tree = ya::tree<std::string>{"+"}
                    .emplace("1")
                    .emplace("2");
    my_tree.apply_dfs(printer_function); // + 1 2
    std::cout << std::endl;

    //// You can even concatenate trees together
    ////    *
    ////   / \
    ////  3   +
    ////     / \
    ////    1   2
    auto my_tree2 = ya::tree<std::string>{"*"}
                    .emplace("3")
                    .concat(my_tree);
    my_tree2.apply_dfs(printer_function); // * 3 + 1 2
    std::cout << std::endl;

    //// You dont have to use the in-built dfs. It is only meant to be a shortcut
    //// for very simple operations.
    /// If you need more control, just define your own function(s)
    print_tree_manually(my_tree2); // (3*(1+2))
    std::cout << std::endl;
    my_tree2.apply_dfs(print_tree); // also (3*(1+2)) - just a lot more manual stack maintenance
    std::cout << std::endl;
    return 0;
}

//// Prints a tree as if it was regular arithmetic ( 1 + 2 + 3 )
//// instead of reverse polish notation ( + 1 2 3 )
std::stack<std::pair<std::string, unsigned int>> counter_stack{};
void print_tree(const ya::tree<std::string>& tree) {
    if(tree.children.empty()) {
        std::cout << tree.node;
        while(!counter_stack.empty()) {
            if(--counter_stack.top().second <= 0) {
                std::cout << ")";
                counter_stack.pop();
            } else {
                std::cout << counter_stack.top().first;
                return;
            }
        }
        return;
    }
    std::cout << "(";
    counter_stack.push(std::make_pair(tree.node, tree.children.size()));
}

//// A much easier implementation of print_tree
void print_tree_manually(const ya::tree<std::string>& tree) {
    if(tree.children.empty()) {
        std::cout << tree.node;
        return;
    }
    std::cout << "(";
    std::string sep{};
    for(auto& c : tree.children) {
        std::cout << sep;
        print_tree_manually(c);
        sep = tree.node;
    }
    std::cout << ")";
}
