#ifndef YATREE_TREE_H
#define YATREE_TREE_H
#include <vector>

template<typename T>
class tree {
public:
    tree() = delete;
    explicit tree(const T& r) : root(r), children{} {}
    explicit tree(T&& r) : root{std::forward<T>(r)}, children{} {}
    template<typename... Args>
    void emplace(Args&&... args) {
        children.emplace_back(std::forward(args)...);
    }
    auto concat(const tree<T>& element) -> tree<T>& {
        children.push_back(element);
        return *this;
    }
    auto operator+=(const tree<T>& element) -> tree<T>& {
        return concat(element);
    }
    void apply(std::function<void(T&)> f) {
        f(root);
        for(auto& c : children)
            c.apply(f);
    }
    void apply(std::function<void(const T&)> f) const {
        f(root);
        for(auto& c : children)
            c.apply(f);
    }
    void apply(std::function<void(const tree<T>&)> f) const {
        f(*this);
        for(auto& c : children)
            c.tree_apply(f);
    }
private:
    T root;
    std::vector<tree<T>> children;
};

template<typename T>
auto operator+(tree<T>& a, const tree<T>& b) -> tree<T>& {
    return a += b;
}

#endif
