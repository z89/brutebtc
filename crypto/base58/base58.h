#ifndef _BASE_H
#define _BASE_H

#include "../secp256k1/secp256k1.h"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>

namespace BASE {
    std::string toBase58(const secp256k1::uint256 &x);
    secp256k1::uint256 toBigInt(const std::string &s);
    void getMinMaxFromPrefix(const std::string &prefix, secp256k1::uint256 &minValueOut, secp256k1::uint256 &maxValueOut);

    void toHash160(const std::string &s, unsigned int hash[5]);

    bool isBase58(std::string s);
}; // namespace BASE

#endif