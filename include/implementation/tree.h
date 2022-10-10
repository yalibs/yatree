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
#include <stack>

namespace ya {
    template<typename T>
    struct tree {
        using children_t = std::vector<tree<T>>;
        using child_it_t = typename children_t::iterator;
        // TODO: Out of scope iterator features
        //       - implement swap
        //       - implement operator=(const _left_df_iterator&)
        //       - implement operator--
        struct _left_df_iterator {
            explicit _left_df_iterator(tree<T>& root) : root{root}, indices{} {}
            void operator++() {
                if(indices.empty()) {
                    if(root._children.empty())
                        throw std::out_of_range("root node has no children");
                    indices.push_back(0);
                    return;
                }
                if(indices[0] >= root._children.size()) {

                }
            }
            void operator+=(size_t index) {
                for(size_t i = 0; i < index; i++)
                    this->operator++();
            }
            void operator--() {

            }
            void operator-=(size_t index) {
                for(size_t i = 0; i < index; i++)
                    this->operator--();
            }
            auto operator*() const -> tree<T>& {
                tree<T>& e = root;
                for(auto& i : indices)
                    e = e[i];
                return e;
            }
            auto operator==(const _left_df_iterator& other) const -> bool {
            }
            auto operator!=(const _left_df_iterator& o) const -> bool {
            }
            auto operator=(const _left_df_iterator& o) -> _left_df_iterator& {
                root = o.root;
                indices = o.indices;
                return *this;
            }
        private:
            tree<T>& root;
            std::vector<size_t> indices;
        };
        tree() : node{}, _children{}, p{} {}
        explicit tree(const T &r) : node(r), _children{}, p{} {}
        explicit tree(T &&r) : node{std::forward<T>(r)}, _children{}, p{} {}
        tree(const tree<T>& o) : node{o.node}, _children{o.children()}, p{o.p} {}
        tree(tree<T>&& o) noexcept : node{std::move(o.node)}, _children{std::move(o._children)}, p{std::move(o.p)} {}

        auto operator[](size_t i) -> tree<T>& {
            if(i >= _children.size())
                throw std::out_of_range("tree index our of range");
            return _children[i];
        }

        template<typename... Args>
        auto emplace(Args &&... args) -> tree<T>& {
            _children.emplace_back(std::forward<Args...>(args)...);
            _children[_children.size()-1].set_parent(*this);
            return *this;
        }
        // TODO: Naming of concat/emplace is bad
        auto concat(const tree<T> &element) -> tree<T>& {
            _children.push_back(element);
            _children[_children.size()-1].set_parent(*this);
            return *this;
        }
        auto concat(tree<T>&& e) -> tree<T>& {
            _children.emplace_back(std::move(e));
            _children[_children.size()-1].set_parent(*this);
            return *this;
        }
        auto operator+=(const tree<T> &element) -> tree<T> & {
            return concat(element);
        }

        void apply_dfs(std::function<void(T &)> f) {
            f(node);
            for (auto &c: _children)
                c.apply_dfs(f);
        }
        void apply_dfs(std::function<void(const T &)> f) const {
            f(node);
            for (auto &c: _children)
                c.apply_dfs(f);
        }
        void apply_dfs(std::function<void(const tree<T> &)> f) const {
            f(*this);
            for (auto &c: _children)
                c.apply_dfs(f);
        }
        auto parent() const -> std::optional<std::reference_wrapper<tree<T>>> {
            return p;
        }
        auto children() const -> const children_t& {
            return _children;
        }

        T node;
    private:
        children_t _children;
        std::optional<std::reference_wrapper<tree<T>>> p;

        void set_parent(std::reference_wrapper<tree<T>> new_parent) {
            p = new_parent;
        }
    };

    template<typename T>
    auto operator+(tree<T> &a, const tree<T> &b) -> tree<T> & {
        return a += b;
    }
}

#endif
