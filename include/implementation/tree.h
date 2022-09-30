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
        // TODO: Out of scope iterator features
        //       - implement swap
        //       - implement operator=(const _left_df_interator&)
        struct _left_df_iterator {
            explicit _left_df_iterator() : data{nullptr} {}
            explicit _left_df_iterator(tree<T>* e) : data{e} {}
            void operator++() {
                if(!data)
                    throw std::out_of_range("tree iterator pointing to nothing");

            }
            void operator--() {
                // TODO: Implement
            }
            void operator+=(size_t index) {
                if(!data)
                    throw std::out_of_range("tree iterator pointing to nothing");
                for(size_t i = 0; i < index; i++) {
                    if(data->end() == *this)
                        throw std::out_of_range("access of non-existent tree nodes");
                    this->operator++();
                }
            }
            void operator-=(size_t index) {
                if(!data)
                    throw std::out_of_range("tree iterator pointing to nothing");
                for(size_t i = 0; i < index; i++) {
                    if(data->end() == *this)
                        throw std::out_of_range("access of non-existent tree nodes");
                    this->operator--();
                }
            }
            auto operator*() -> tree<T>& {
                if(!data)
                    throw std::out_of_range("tree iterator pointing to nothing");
                return *data;
            }
            auto operator==(const _left_df_iterator& other) -> bool {
                return data == other.data;
            }
            auto operator!=(const _left_df_iterator& o) -> bool {
                return !this->operator==(o);
            }
        private:
            tree<T>* data;
        };

        tree() : node{}, _children{}, p{}, _begin{this}, _end{} {}
        explicit tree(const T &r) : node(r), _children{}, p{}, _begin{this}, _end{} {}
        explicit tree(T &&r) : node{std::forward<T>(r)}, _children{}, p{}, _begin{this}, _end{} {}

        auto begin() const -> _left_df_iterator {
            return _begin;
        }
        auto end() const -> _left_df_iterator {
            return _end;
        }

        template<typename... Args>
        auto emplace(Args &&... args) -> tree<T> & {
            _children.emplace_back(std::forward<Args...>(args)...);
            for(auto& c : _children)
                c.set_parent(*this);
            return *this;
        }
        auto concat(const tree<T> &element) -> tree<T> & {
            _children.push_back(element);
            for(auto& c : _children)
                c.set_parent(*this);
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
        auto children() const -> const std::vector<tree<T>>& {
            return _children;
        }

        T node;
    private:
        std::vector<tree<T>> _children;
        std::optional<std::reference_wrapper<tree<T>>> p;
        _left_df_iterator _begin, _end;

        void set_parent(std::reference_wrapper<tree<T>> new_parent) {
            p = new_parent;
        }
        void set_end() {

        }
    };

    template<typename T>
    auto operator+(tree<T> &a, const tree<T> &b) -> tree<T> & {
        return a += b;
    }
}

#endif
