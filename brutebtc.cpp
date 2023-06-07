#include <assert.h>
#include <chrono>
#include <string.h>

#include "./crypto/base58/base58.h"
#include "./crypto/ripemd160/ripemd160.h"
#include "./crypto/secp256k1/secp256k1.h"
#include "./crypto/sha256/sha256.h"

#include <cstring>
#include <iomanip>
#include <iostream>

using namespace std;
using namespace std::chrono;

static unsigned int endian(unsigned int x) {
    return (x << 24) | ((x << 8) & 0x00ff0000) | ((x >> 8) & 0x0000ff00) | (x >> 24);
}

int main(void) {
    auto start = high_resolution_clock::now();
    secp256k1::uint256 key = 0x0000000000000000000000000000000000000000000000000000000000000001;
    secp256k1::ecpoint p = secp256k1::multiplyPoint(key);

    unsigned int xWords[8] = {0};
    unsigned int yWords[8] = {0};

    p.x.exportWords(xWords, 8, secp256k1::uint256::BigEndian);
    p.y.exportWords(yWords, 8, secp256k1::uint256::BigEndian);

    unsigned int digest[5];

    unsigned int msg[16] = {0};
    unsigned int shaDigest[8];

    // Compressed public key format
    msg[15] = 33 * 8;

    msg[8] = (xWords[7] << 24) | 0x00800000;
    msg[7] = (xWords[7] >> 8) | (xWords[6] << 24);
    msg[6] = (xWords[6] >> 8) | (xWords[5] << 24);
    msg[5] = (xWords[5] >> 8) | (xWords[4] << 24);
    msg[4] = (xWords[4] >> 8) | (xWords[3] << 24);
    msg[3] = (xWords[3] >> 8) | (xWords[2] << 24);
    msg[2] = (xWords[2] >> 8) | (xWords[1] << 24);
    msg[1] = (xWords[1] >> 8) | (xWords[0] << 24);

    if (yWords[7] & 0x01) {
        msg[0] = (xWords[0] >> 8) | 0x03000000;
    } else {
        msg[0] = (xWords[0] >> 8) | 0x02000000;
    }

    SHA::init(shaDigest);
    SHA::sha256(msg, shaDigest);

    for (int i = 0; i < 16; i++) {
        msg[i] = 0;
    }

    // little endian
    for (int i = 0; i < 8; i++) {
        msg[i] = (shaDigest[i] << 24) | ((shaDigest[i] << 8) & 0x00ff0000) | ((shaDigest[i] >> 8) & 0x0000ff00) | (shaDigest[i] >> 24);
    }

    msg[8] = 0x00000080;
    msg[14] = 256;
    msg[15] = 0;

    RIPEMD::ripemd160(msg, digest);
    unsigned int checksum = SHA::checksum(digest);

    unsigned int addressWords[8] = {0};

    for (int i = 0; i < 5; i++) {
        addressWords[2 + i] = digest[i];
    }

    addressWords[7] = checksum;

    secp256k1::uint256 addressBigInt(addressWords, secp256k1::uint256::BigEndian);

    auto end = high_resolution_clock::now();

    cout << "address: 1" + BASE::toBase58(addressBigInt) << endl;

    duration<double, milli> ms = end - start;
    printf("execution time: %fms\n", ms.count());

    return 0;
}