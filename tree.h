#include <memory>
#include <set>
#include <vector>
#include <map>

#ifndef _TREE_H_
#define _TREE_H_


class Tree {
    std::shared_ptr<Tree> lhs;
    std::shared_ptr<Tree> rhs;
    std::set<unsigned char> content;
    bool has_children;
    public:
    Tree();

    Tree(unsigned char content);

    Tree& operator=(Tree &from) {
        return from;
    }
    Tree(std::shared_ptr<Tree> &lhs, std::shared_ptr<Tree> &rhs);
    ~Tree() = default;

    bool hasChildren() const;
    std::shared_ptr<Tree> getLeftChild() const;
    std::shared_ptr<Tree> getRightChild() const;

    bool hasChar(const unsigned char c) const;
    void addChar(const unsigned char c);
    unsigned char getFirstChar() const;
    unsigned char getLastChar() const;
    void makeChildren();
    size_t chars_size() const;
    size_t depth() const;
    std::map<unsigned char, long unsigned int> serialize(bool top_level = true) const;
};

#endif