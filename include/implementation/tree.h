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
            explicit _left_df_iterator() : data{nullptr}, search_stack{} {}
            explicit _left_df_iterator(tree<T>* e) : data{e}, search_stack{} {}
            void operator++() {
                if(!data)
                    throw std::out_of_range("tree iterator pointing to nothing");
                // begin/end maintenance notes:
                // The ENTIRE tree's idea of where "end()" is must be maintained by the way
                if(*this == data->end())
                    throw std::out_of_range("iterator already point at the end");

                if(data->children().empty()) {
                    search_stack.top().sibling++;
                    while(search_stack.top().reached_end()) {
                        search_stack.pop();
                        if(search_stack.empty()) {
                            data = &(*data->end());
                            return;
                        }
                        search_stack.top().sibling++;
                    }
                    data = &(*search_stack.top().sibling);
                } else {
                    search_stack.push({data->_children.begin(), data->_children.end()});
                    data = &(*search_stack.top().sibling);
                }
            }
            void operator+=(size_t index) {
                if(!data)
                    throw std::out_of_range("tree iterator pointing to nothing");
                for(size_t i = 0; i < index; i++)
                    this->operator++();
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
                return *data;
            }
            auto operator==(const _left_df_iterator& other) const -> bool {
                return data == other.data;
            }
            auto operator!=(const _left_df_iterator& o) const -> bool {
                return !this->operator==(o);
            }
        private:
            tree<T>* data;
            struct search_element {
                child_it_t sibling;
                child_it_t parent_end;
                auto reached_end() -> bool {
                    return sibling == parent_end;
                }
            };
            std::stack<search_element> search_stack;
        };
        tree() : node{}, _children{}, p{}, _begin{this}, _end{&(*_children.end())} {}
        explicit tree(const T &r) : node(r), _children{}, p{}, _begin{this}, _end{&(*_children.end())} {}
        explicit tree(T &&r) : node{std::forward<T>(r)}, _children{}, p{}, _begin{this}, _end{&(*_children.end())} {}
        tree(const tree<T>& o) : node{o.node}, _children{o.children()}, p{o.p}, _begin{this}, _end{o._end} {}
        tree(tree<T>&& o) noexcept : node{std::move(o.node)}, _children{std::move(o._children)}, p{std::move(o.p)}, _begin{this}, _end{std::move(o._end)} {}
        auto operator=(const tree<T>& o) -> tree<T>& {
            node = o.node;
            _children = o._children;
            p = o.p;
            _begin = _left_df_iterator{this};
            _end = o._end;
            return *this;
        }
        auto operator=(tree<T>&& o)  noexcept -> tree<T>& {
            node = std::move(o.node);
            _children = std::move(o._children);
            p = std::move(o.p);
            _begin = _left_df_iterator{this};
            _end = std::move(o._end);
            return *this;
        }

        auto begin() const -> _left_df_iterator {
            return _begin;
        }
        auto end() const -> _left_df_iterator {
            return _end;
        }

        template<typename... Args>
        auto emplace(Args &&... args) -> tree<T>& {
            _children.emplace_back(std::forward<Args...>(args)...);
            _children[_children.size()-1].set_parent(*this);
            set_end(_children[_children.size()-1]._end);
            return *this;
        }
        // TODO: Naming of concat/emplace is bad
        auto concat(const tree<T> &element) -> tree<T>& {
            _children.push_back(element);
            _children[_children.size()-1].set_parent(*this);
            set_end(_children[_children.size()-1]._end);
            return *this;
        }
        auto concat(tree<T>&& e) -> tree<T>& {
            _children.emplace_back(std::move(e));
            _children[_children.size()-1].set_parent(*this);
            set_end(_children[_children.size()-1]._end);
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
        auto children_end() -> child_it_t {
            return _children.end();
        }

        T node;
    private:
        children_t _children;
        std::optional<std::reference_wrapper<tree<T>>> p;
        _left_df_iterator _begin, _end;

        void set_parent(std::reference_wrapper<tree<T>> new_parent) {
            p = new_parent;
        }

        void set_end(_left_df_iterator new_end) {
            _end = new_end;
            if(parent().has_value())
                parent().value().get().set_end(new_end);
        }
    };

    template<typename T>
    auto operator+(tree<T> &a, const tree<T> &b) -> tree<T> & {
        return a += b;
    }
}

#endif
