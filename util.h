#include <iostream>
#include <vector>
#include <string>
#include <set>

template <typename It>
std::ostream & print_collection(std::ostream &os, It start, It end,char startChar, char endChar) {
    os << startChar;
    if (start == end) {
        return os << endChar;
    }
    os << *start;
    start++;
    while (start != end) {
        os << "," << *start;
        start++;
    }
    return os << endChar;
}

template <typename T>
std::ostream & operator<<(std::ostream& os, std::vector<T> vec) {
    return print_collection(os, vec.begin(), vec.end(), '[', ']');
}

template <typename T>
std::ostream & operator<<(std::ostream& os, std::set<T> set) {
    return print_collection(os, set.begin(), set.end(), '{', '}');
}


