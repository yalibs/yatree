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
#ifndef YATREE_TREE_H
#define YATREE_TREE_H
#include <vector>
#include <optional>
#include <functional> // std::reference_wrapper

namespace ya {
    template<typename T>
    struct tree {
        tree() : node{}, children{}, parent{} {}
        explicit tree(const T &r) : node(r), children{}, parent{} {}
        explicit tree(T &&r) : node{std::forward<T>(r)}, children{}, parent{} {}

        template<typename... Args>
        auto emplace(Args &&... args) -> tree<T> & {
            children.emplace_back(std::forward<Args...>(args)...);
            for(auto& c : children)
                c.set_parent(*this);
            return *this;
        }
        auto concat(const tree<T> &element) -> tree<T> & {
            children.push_back(element);
            for(auto& c : children)
                c.set_parent(*this);
            return *this;
        }
        auto operator+=(const tree<T> &element) -> tree<T> & {
            return concat(element);
        }

        void apply_dfs(std::function<void(T &)> f) {
            f(node);
            for (auto &c: children)
                c.apply_dfs(f);
        }
        void apply_dfs(std::function<void(const T &)> f) const {
            f(node);
            for (auto &c: children)
                c.apply_dfs(f);
        }
        void apply_dfs(std::function<void(const tree<T> &)> f) const {
            f(*this);
            for (auto &c: children)
                c.apply_dfs(f);
        }

        T node;
        std::vector<tree<T>> children;
        std::optional<std::reference_wrapper<tree<T>>> parent;

    private:
        void set_parent(std::reference_wrapper<tree<T>> new_parent) {
            parent = new_parent;
        }
    };

    template<typename T>
    auto operator+(tree<T> &a, const tree<T> &b) -> tree<T> & {
        return a += b;
    }
}

#endif
