#include<vector>
#include<set>
#include<iostream>
#include "util.h"
#include<fstream>
#include "tree.h"
#include<map>
#include<queue>
#include<vector>
#include "bitbuffer.h"

struct Priority_Item {
    size_t priority {};
    std::shared_ptr<Tree> tree;
    
    bool operator==(const Priority_Item &item) const {
        return priority == item.priority;
    }
    bool operator>(const Priority_Item &item) const {
        if (priority > item.priority) {
            return true;
        } else if (priority == item.priority) {
            return (*tree).getFirstChar() < (*item.tree).getFirstChar();
        } else {
            return false;
        }
    }
    Priority_Item(Priority_Item &&copy) : priority(priority), tree(std::move(copy.tree)) {}
    Priority_Item& operator=(Priority_Item &&move_assignment) {
        tree = std::move(move_assignment.tree);
        priority = move_assignment.priority;
        return *this;
    }  
    Priority_Item(size_t priority, std::shared_ptr<Tree> &&tree) : priority(priority), tree(std::move(tree)) {}
    Priority_Item(size_t priority, std::shared_ptr<Tree> &tree) : priority(priority), tree(std::move(tree)) {}
};

std::map<unsigned char, size_t> getHuffman(const std::map<unsigned char, size_t> &count) {
    auto compare = [](const std::shared_ptr<Priority_Item> &lhs, const std::shared_ptr<Priority_Item> &rhs) {
        return *lhs > *rhs;
    };
    std::priority_queue<std::shared_ptr<Priority_Item>, std::vector<std::shared_ptr<Priority_Item>>, decltype(compare)> queue(compare);

    for (std::pair<char, size_t> item : count) {
        queue.push(std::make_shared<Priority_Item>(item.second, std::make_shared<Tree>(item.first)));
    }
    while (queue.size() > 1) {
        size_t total_priority {(*queue.top()).priority};
        std::shared_ptr<Tree> b = std::move((*queue.top()).tree);
        queue.pop();
        total_priority += (*queue.top()).priority;
        std::shared_ptr<Tree> a  = std::move((*queue.top()).tree);
        queue.pop();
        if ((*a).chars_size() < (*b).chars_size()) {
            std::shared_ptr<Tree> new_tree = std::make_shared<Tree>(a, b);
            queue.push(std::make_shared<Priority_Item>(total_priority, new_tree));
        } else  {
            std::shared_ptr<Tree> new_tree = std::make_shared<Tree>(b, a);
            queue.push(std::make_shared<Priority_Item>(total_priority, new_tree));
        }
    }
    return (*(*queue.top()).tree).serialize();
}

std::map<unsigned char, size_t> count_chars(std::ifstream &input) {
    std::map<unsigned char, size_t> count;
    size_t total_size {};
    char c;
    while (input.get(c)) {
        count[static_cast<unsigned char>(c)]++;
        total_size++;
    }
    input.clear();
    input.seekg (0, std::ios::beg);
    std::cout << "Uncompressed size is " << total_size << " bytes." << std::endl;
    return count;
}

std::map<unsigned char, std::vector<bool>> getDictionary(const std::map<unsigned char, size_t> &huffman) {
    std::map<size_t, size_t> bl_count;
    size_t max_code {};
    for (auto entry : huffman) {
        bl_count[entry.second]++;
        max_code = max_code > entry.second ? max_code : entry.second;
    }
    size_t code = 0;
    bl_count[0] = 0;
    std::map<size_t, size_t> next_code;
    for (size_t bits = 1; bits <= max_code; bits++) {
        code = (code + bl_count[bits-1]) << 1;
        next_code[bits] = code;
    }

    std::map<unsigned char, std::vector<bool>>  res;
    for (auto entry : huffman) {
        if (entry.second == 0) {
            continue;
        }
        std::vector<bool> bit_vector(entry.second, false);
        size_t to_put {next_code[entry.second]};
        size_t index{entry.second - 1};
        while (to_put != 0) {
            bit_vector.at(index) = (to_put & 1 == 1);
            to_put >>= 1;
            index--;
        }
        next_code[entry.second]++;
        res[entry.first] = bit_vector;
    }
    return res;
}

std::map<unsigned char, size_t> readHuffman(std::ifstream &input) {
    std::map<unsigned char, size_t> res;
    char c;
    for (size_t i {0}; i < 256; i++) {
        input.get(c);
        res[static_cast<unsigned char>(i)] = static_cast<size_t>(c);
    }
    return res;
}

void encode (std::string in_file, std::string out_file) {
    std::ifstream input(in_file, std::ios::binary | std::ios::in);
    if (!input) {
        std::cerr << "Could not open input!!" << std::endl;
        throw -1;
    }
    std::map<unsigned char, size_t> count {count_chars(input)};
    std::map<unsigned char, size_t> huffman = getHuffman(count);
    std::cout << "The dictionary is 256 bytes." << std::endl;
    std::ofstream output(out_file, std::ios::binary | std::ios::out);
    if (!output) {
        std::cerr << "Could not write output!!" << std::endl;
        throw -1;
    }
   
    std::map<unsigned char, std::vector<bool>> dictionary {getDictionary(huffman)};
    char c;
    BitBuffer buffer;
    size_t input_size {};
    while (input.get(c)) {
        std::vector<bool> bits = dictionary[c];
        for (const bool bit : bits) {
            bit >> buffer;
        }
        input_size++;
    }
    char trailing {0};
    while (buffer.size() % 8 != 0) {
        false >> buffer;
        trailing++;
    }
    std::cerr << "Compressed size is " << buffer.size()/8 << " bytes." << std::endl;
    std::cerr << "Total size is " << (256 + (buffer.size()/8)) << " bytes." << std::endl;
    std::cerr << "Storage reduced by " << 100*(1 - (256 + (buffer.size()/8)) / static_cast<double>(input_size)) << " %." << std::endl;
    
    output.put(trailing);
    for (unsigned int c = 0; c <256; c++) {
        output.put(huffman[static_cast<unsigned char>(c)]);
    }
    output << buffer;
}

void decode (std::string in_file, std::string out_file) {
    std::ifstream input(in_file, std::ios::binary | std::ios::in);
    if (!input) {
        std::cerr << "Could not open input!!" << std::endl;
        throw -1;
    }
    char trailingZeroes {};
    input.get(trailingZeroes);
    std::map<unsigned char, size_t> huffman = readHuffman(input);
    std::cout << "The dictionary is 256 bytes." << std::endl;
    std::ofstream output(out_file, std::ios::binary | std::ios::out);
    if (!output) {
        std::cerr << "Could not write output!!" << std::endl;
        throw -1;
    }
    std::map<unsigned char, std::vector<bool>> dictionary {getDictionary(huffman)};
    std::shared_ptr<Tree> root = std::make_shared<Tree>();
    for (std::pair<unsigned char, std::vector<bool>> entry : dictionary) {
        std::shared_ptr<Tree> currentTree = root;
        (*currentTree).addChar(entry.first);
        for (bool direction : entry.second) {
            if (!(*currentTree).hasChildren()) {
                (*currentTree).makeChildren();
            }
            if (direction) { //right == 1
                currentTree = (*currentTree).getRightChild();
            } else {
                currentTree = (*currentTree).getLeftChild();
            }
            (*currentTree).addChar(entry.first);
        }
    }
    char c;
    BitBuffer buffer;
    std::shared_ptr<Tree> currentTree = root;
    while (input.get(c)) {
        c >> buffer;
        while (buffer.hasNext()) {
            if (input.peek() && buffer.size() == trailingZeroes) {
                break;
            }
            bool nextBool;
            nextBool << buffer;
            currentTree = (nextBool ? (*currentTree).getRightChild() : (*currentTree).getLeftChild());
            if (!(*currentTree).hasChildren()) {
                output << (*currentTree).getFirstChar();
                currentTree = root;
            }
        }
    }
    if (input.peek()) {
        std::cout << "1" << std::endl;
    } else {
        std::cout << "2" << std::endl;
    }
    
    std::cout << "Decompressed." << std::endl;
}

int main() {
    encode("./to_compress.txt", "./compressed.huff");
    decode("./compressed.huff", "./decompressed.txt");
}