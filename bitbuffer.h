#ifndef _BITBUFFER_H_
#define _BITBUFFER_H_

#include <queue>
#include <ostream>

class OffsetException : std::exception {
   const char* what () const throw () {
      return "There was still an offset!";
   }
};

class BitBuffer {
    friend std::ostream& operator<<(std::ostream &os, BitBuffer &buffer);
    friend bool& operator<<(bool &extract, BitBuffer &buffer);
    friend void operator>>(bool bitvalue, BitBuffer &buffer);
    friend void operator>>(char data, BitBuffer &buffer);

    std::queue<bool> content;
    size_t bits {};

    public:
    size_t size() {
        return content.size();
    }
    bool hasNext() {
        return !content.empty();
    }
    
};






#endif
