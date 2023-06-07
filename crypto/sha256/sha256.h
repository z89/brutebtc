#ifndef _SHA_H
#define _SHA_H

#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>

namespace SHA {
    void init(unsigned int *digest);
    void sha256(unsigned int *input, unsigned int *digest);
    unsigned int checksum(unsigned int *input);
}; // namespace SHA

#endif