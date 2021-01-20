#include "bitbuffer.h"
#include <iostream>

std::ostream& operator<<(std::ostream &os, BitBuffer &buffer) {
    if (buffer.content.size() % 8 != 0) {
        throw OffsetException();
    }
    while (!buffer.content.empty()) {
        char res {0};
        for (int i = 0; i < 8; i++) {
            unsigned char mask = buffer.content.front() << (7 - i);
            res |= mask;
            buffer.content.pop();
        }
        os.put(res);
    }
    return os;
}

bool& operator<<(bool &extract, BitBuffer &buffer) {
    extract = buffer.content.front();
    buffer.content.pop();
    return extract;
}


void operator>>(bool bitvalue, BitBuffer &buffer) {
    buffer.content.push(bitvalue);
}

void operator>>(char bits, BitBuffer &buffer) {
    for (int i = 7; i >=0; i--) {
        operator>>(((bits >> i) & 1) == 1, buffer);
    }
}