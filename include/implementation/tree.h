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
#include <utility>
#include <vector>
#include <optional>
#include <functional> // std::reference_wrapper
#include <stack>

namespace ya {
    template<typename T>
    struct tree {
        using children_t = std::vector<tree<T>>;
        // TODO: Out of scope iterator features
        //       - implement swap
        //       - implement operator=(const _left_df_iterator&)
        //       - implement operator--
        //       - end should be indices = [ root.children.size() ] <--
        struct _left_df_iterator {
            explicit _left_df_iterator(tree<T>& root) : root{root}, indices{} {}
            _left_df_iterator(tree<T>& root, std::vector<size_t> indices) : root{root}, indices{std::move(indices)} {}
            auto operator++() -> _left_df_iterator& {
                if(indices.empty()) {
                    if(root._children.empty())
                        throw std::out_of_range("root node has no children");
                    indices.push_back(0);
                    return *this;
                }
                if(indices[0] >= root._children.size())
                    throw std::out_of_range("cannot increment iterator to end");
                auto& e = operator*();
                if(!e.children().empty()) {
                    indices.push_back(0);
                    return *this;
                }
                auto parent = e.parent();
                while(!indices.empty() && parent.has_value()) {
                    auto has_sibling = indices[indices.size()-1]+1 < parent.value()->children().size();
                    if(has_sibling) {
                        indices[indices.size()-1]++;
                        return *this;
                    }
                    indices.erase(indices.end()-1);
                    parent = operator*().parent();
                }
                indices.push_back(root._children.size());
                return *this;
            }
            auto operator++(int) -> _left_df_iterator {
                auto x = *this;
                operator++();
                return x;
            }
            auto operator+=(size_t index) -> _left_df_iterator& {
                for(size_t i = 0; i < index; i++)
                    this->operator++();
                return *this;
            }
            auto operator*() const -> tree<T>& {
                tree<T>* e = &root;
                for(auto& i : indices) {
                    e = &(e->operator[](i));
                }
                return *e;
            }
            auto operator->() const -> tree<T>* {
                tree<T>* e = &root;
                for(auto& i : indices) {
                    e = &(e->operator[](i));
                }
                return e;
            }
            auto operator==(const _left_df_iterator& other) const -> bool {
                if(&root != &other.root)
                    return false;
                if(indices.size() != other.indices.size())
                    return false;
                for(auto i = 0; i < indices.size(); i++) {
                    if(indices[i] != other.indices[i])
                        return false;
                }
                return true;
            }
            auto operator!=(const _left_df_iterator& o) const -> bool {
                return ! this->operator==(o);
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
        tree() : node{}, _children{}, p{nullptr} {}
        explicit tree(const T &r) : node(r), _children{}, p{nullptr} {}
        explicit tree(T &&r) : node{std::forward<T>(r)}, _children{}, p{nullptr} {}
        tree(const tree<T>& o) : node{o.node}, _children{o.children()}, p{o.p} {}
        tree(tree<T>&& o) noexcept : node{std::move(o.node)}, _children{std::move(o._children)}, p{std::move(o.p)} {}
        auto operator=(const tree<T>& o) -> tree<T>& {
            if(this == &o)
                return *this;
            node = o.node;
            _children = o._children;
            p = nullptr;
            return *this;
        }
        auto operator=(tree<T>&& o) noexcept -> tree<T>& {
            if(this == &o)
                return *this;
            node = o.node;
            _children = o._children;
            p = nullptr;
            return *this;
        }

        auto operator[](size_t i) -> tree<T>& {
            if(i >= _children.size())
                throw std::out_of_range("tree index out of range");
            return _children[i];
        }

        template<typename... Args>
        void emplace(Args &&... args) {
            _children.emplace_back(std::forward<Args...>(args)...).set_parent(this);
        }
        // TODO: Naming of concat/emplace is bad
        void concat(const tree<T> &element) {
            _children.push_back(element);
            (_children.end()-1)->set_parent(this);
        }
        void concat(tree<T>&& e) {
            _children.emplace_back(std::move(e));
            (_children.end()-1)->set_parent(this);
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
        auto parent() const -> std::optional<tree<T>*> {
            if(p) return {p};
            return {};
        }
        auto children() const -> const children_t& {
            return _children;
        }
        auto begin() -> _left_df_iterator {
            return _left_df_iterator{*this};
        }
        auto end() -> _left_df_iterator {
            return _left_df_iterator{*this, std::vector<size_t>{{_children.size()}}};
        }

        T node;
    private:
        children_t _children;
        tree<T>* p;

        void set_parent(tree<T>* new_parent) {
            p = new_parent;
        }
    };

    template<typename T>
    auto operator+(tree<T> &a, const tree<T> &b) -> tree<T> & {
        return a += b;
    }
}

#endif
