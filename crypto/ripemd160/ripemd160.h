#ifndef _RIPEMD_H
#define _RIPEMD_H

#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>

namespace RIPEMD {
    void ripemd160(unsigned int *msg, unsigned int *digest);
};

#endif