#include "sha256.h"
#include <iostream>
#include <stdio.h>
#include <string.h>

static unsigned int endian(unsigned int x) {
    return (x << 24) | ((x << 8) & 0x00ff0000) | ((x >> 8) & 0x0000ff00) | (x >> 24);
}

unsigned int SHA::checksum(unsigned int *input) {
    unsigned int msg[16] = {0};
    unsigned int digest[8] = {0};

    msg[0] = 0x00;
    msg[0] |= input[0] >> 8;
    msg[1] = (input[0] << 24) | (input[1] >> 8);
    msg[2] = (input[1] << 24) | (input[2] >> 8);
    msg[3] = (input[2] << 24) | (input[3] >> 8);
    msg[4] = (input[3] << 24) | (input[4] >> 8);
    msg[5] = (input[4] << 24) | 0x00800000;

    msg[15] = 168;

    init(digest);
    sha256(msg, digest);

    memset(msg, 0, 16 * 4);
    for (int i = 0; i < 8; i++) {
        msg[i] = digest[i];
    }

    msg[8] = 0x80000000;
    msg[15] = 256;

    init(digest);
    sha256(msg, digest);

    return digest[0];
}
