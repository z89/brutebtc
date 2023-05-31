#include <assert.h>
#include <chrono>
#include <string.h>

#include <iomanip>
#include <iostream>
#include <openssl/ripemd.h>
#include <openssl/sha.h>

#include "secp256k1.h"

using namespace std;
using namespace std::chrono;

// convert hex string to a specific length with leading zeros
string padHexString(string hexString, size_t length) {
    stringstream paddedString;

    // pad the hex string with leading zeros
    paddedString << setw(length) << setfill('0') << hexString;

    return paddedString.str();
}

// convert a byte array to a hex string
string byteArrayToHex(const unsigned char *byteArray, size_t length) {
    stringstream hexString;

    // set the string stream type to base-16 hexidecimal
    hexString << hex;

    // convert each unsigned char from byte array to decimal
    for (int i(0); i < length; ++i) {
        hexString << (int)byteArray[i];
    }

    // print the result for the hex string for debugging
    // cout << "byte array to hex result: " << hexString.str() << endl;

    return hexString.str();
}

// convert a hex string to a byte array
unsigned char *hexToByteArray(const string hexString) {
    //  define a fixed length byte array for conversion
    unsigned char *byteArray = new unsigned char[32];

    for (size_t i = 0; i < 32; i++) {
        // conver the two hexdecimal characters to decimal
        int decimalValue = stoi(hexString.substr(i * 2, 2), nullptr, 16);

        // change the decimal value to an unsigned char & add it to the byte array
        byteArray[i] = static_cast<unsigned char>(decimalValue);
    }

    // print the results for the byte array for debugging
    // for (size_t i = 0; i < 32; i++) {
    //     cout << "byteArray[" << i << "]: " << (int)byteArray[i] << endl;
    // }

    return byteArray;
}

// encode a byte array to base58
char *base58(unsigned char *s, int s_size, char *out, int out_size) {
    static const char *base_chars = "123456789"
                                    "ABCDEFGHJKLMNPQRSTUVWXYZ"
                                    "abcdefghijkmnopqrstuvwxyz";

    unsigned char s_cp[s_size];
    memcpy(s_cp, s, s_size);

    int c, i, n;

    out[n = out_size] = 0;
    while (n--) {
        for (c = i = 0; i < s_size; i++) {
            c = c * 256 + s_cp[i];
            s_cp[i] = c / 58;
            c %= 58;
        }
        out[n] = base_chars[c];
    }

    return out;
}

int main(void) {
    secp256k1_pubkey public_key;
    string private_key = padHexString("0000000000000000000000000000000000000000000000000000000000000001", 64);

    // create the secp256k1 context
    secp256k1_context *ctx = secp256k1_context_create(SECP256K1_CONTEXT_NONE);
    if (ctx == NULL) {
        printf("failed to create secp256k1 context\n");
        return 1;
    }

    auto start = high_resolution_clock::now();

    // convert the hex string to a byte array
    const unsigned char *private_key_byte_array = hexToByteArray(private_key);

    // verify the private key is valid
    int return_val = secp256k1_ec_seckey_verify(ctx, private_key_byte_array);
    assert(return_val);

    // create the public key from private key
    return_val = secp256k1_ec_pubkey_create(ctx, &public_key, private_key_byte_array);
    assert(return_val);

    unsigned char serialize_public_key[34];
    size_t serialize_public_key_len = 33;

    // serialize the public key with compression (33 bytes)
    return_val = secp256k1_ec_pubkey_serialize(ctx, serialize_public_key, &serialize_public_key_len, &public_key, SECP256K1_EC_COMPRESSED);
    assert(return_val);

    unsigned char serialized_clone[serialize_public_key_len];
    unsigned char hashed_key[5 + RIPEMD160_DIGEST_LENGTH];

    // copy the serialized public key to the new array
    memcpy(serialized_clone, serialize_public_key, serialize_public_key_len);

    // set the first byte to 0x00 for a mainnet address
    hashed_key[0] = 0;

    // hash the serialized public key with SHA256 and then RIPEMD160
    RIPEMD160(SHA256(serialized_clone, serialize_public_key_len, 0), SHA256_DIGEST_LENGTH, hashed_key + 1);

    // create the checksum, then get the last 4 checksum bytes & append it after the first 21 bytes of the hashed key
    memcpy(hashed_key + 21, SHA256(SHA256(hashed_key, 21, 0), SHA256_DIGEST_LENGTH, 0), 4);

    // convert the hashed key byte array to a base58 string
    char address[34];
    base58(hashed_key, 25, address, 34);

    auto end = high_resolution_clock::now();

    cout << "address: " << address << endl;

    duration<double, milli> ms = end - start;
    cout << "execution time: " << ms.count() << "ms\n";

    secp256k1_context_destroy(ctx);

    return 0;
}