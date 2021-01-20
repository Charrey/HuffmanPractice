#include "tree.h"
#include <iostream>
#include <algorithm>
#include <map>

Tree::Tree() : has_children(false) {}

Tree::Tree(std::shared_ptr<Tree> &new_lhs, std::shared_ptr<Tree> &new_rhs)
    : lhs(new_lhs), rhs(new_rhs), has_children(true) {
        std::copy((*lhs).content.begin(), (*lhs).content.end(), std::inserter(content, content.begin()));
        std::copy((*rhs).content.begin(), (*rhs).content.end(), std::inserter(content, content.begin()));
}

Tree::Tree(unsigned char content) : content({content}), has_children(false) {}

bool Tree::hasChar(const unsigned char c) const {
    return content.find(c) != content.end();
}

void Tree::addChar(const unsigned char c) {
    content.insert(c);
}

unsigned char Tree::getFirstChar() const {
    return *(content.begin());
}
unsigned char Tree::getLastChar() const {
    return *(content.rbegin());
}

size_t Tree::chars_size() const {
    return content.size();
}

size_t Tree::depth() const {
    if (this->hasChildren()) {
        size_t left_depth = (*getLeftChild()).depth();
        size_t right_depth = (*getRightChild()).depth();
        if (left_depth > right_depth) {
            return left_depth + 1;
        } else {
            return right_depth + 1;
        }
    } else {
        return 1;
    }
}

std::map<unsigned char, long unsigned int> Tree::serialize(bool top_level) const {
    std::map<unsigned char, long unsigned int> res;
    if (this->hasChildren()) {
        std::map<unsigned char, long unsigned int> left_serialized = (*this->getLeftChild()).serialize(false);
        std::map<unsigned char, long unsigned int> right_serialized = (*this->getRightChild()).serialize(false);
        for (auto entry : left_serialized) {
            res[entry.first] = entry.second + (top_level ? 0 : 1);
        }
        for (auto entry : right_serialized) {
            res[entry.first] = entry.second + (top_level ? 0 : 1);
        }
    } else {
        res[getFirstChar()] = 1;
    }
    return res;
}


bool Tree::hasChildren() const {
    return has_children;
}

void Tree::makeChildren() {
    lhs = std::make_shared<Tree>();
    rhs = std::make_shared<Tree>();
    has_children = true;
}

std::shared_ptr<Tree> Tree::getLeftChild() const {
    return lhs;
}

std::shared_ptr<Tree> Tree::getRightChild() const {
    return rhs;
}
