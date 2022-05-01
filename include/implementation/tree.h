#ifndef YATREE_TREE_H
#define YATREE_TREE_H
#include <vector>

template<typename T>
struct tree {
    tree() = delete;
    tree(const T& r) : node(r), children{} {}
    tree(T&& r) : node{std::forward<T>(r)}, children{} {}
    template<typename... Args>
    auto emplace(Args&&... args) -> tree<T>& {
        children.emplace_back(std::forward<Args...>(args)...);
        return *this;
    }
    auto concat(const tree<T>& element) -> tree<T>& {
        children.push_back(element);
        return *this;
    }
    auto operator+=(const tree<T>& element) -> tree<T>& {
        return concat(element);
    }
    void apply_dfs(std::function<void(T&)> f) {
        f(node);
        for(auto& c : children)
            c.apply_dfs(f);
    }
    void apply_dfs(std::function<void(const T&)> f) const {
        f(node);
        for(auto& c : children)
            c.apply_dfs(f);
    }
    void apply_dfs(std::function<void(const tree<T>&)> f) const {
        f(*this);
        for(auto& c : children)
            c.apply_dfs(f);
    }

    T node;
    std::vector<tree<T>> children;
};

template<typename T>
auto operator+(tree<T>& a, const tree<T>& b) -> tree<T>& {
    return a += b;
}

#endif
