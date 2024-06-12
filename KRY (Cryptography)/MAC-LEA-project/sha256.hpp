/**
 * @file sha256.hpp
 * @author Michal Ľaš (xlasmi00)
 * @brief Implementation of SHA-256
 * @date 2024-04-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include <cstdint>
#include <vector>
#include <array>
#include <cstring>
#include <utility>
#include "utils.hpp"


#define a 0
#define b 1
#define c 2
#define d 3
#define e 4
#define f 5
#define g 6
#define h 7


class shaBlock{
private:
    static constexpr size_t WORDS = 16;
    std::array<word, WORDS> _block;

public:

    shaBlock() : _block() {}
    shaBlock(std::string &rawBlock){ set(rawBlock); }

    word& operator[](const size_t n) { return _block[n]; }

    void set(std::string &rawBlock);

    size_t size(){
        return WORDS;
    }

};

class sha256{

friend class LEA;

private:
    std::vector<shaBlock> _blocks;
    std::array<word, 64> _w;    ///< W0, W1, ..., W63
    std::array<word, 8> _tmpH;  ///< H0, H1, ..., H7
    std::array<word, 8> _vars;  ///< a,b,c,d,e,f,g,h

    /// @brief Make padding for messages and fill `blocks` with 512-bits blocks
    /// @param extraLen Serves to add extra size in bytes. It is used for length extension attack. By default it is 0.
    void _pad(std::string msg, std::vector<shaBlock> &blocks, size_t extraLen = 0);

    /// @brief Set shaBlock `block` at the begining of `_w`. shaBlock words are moved! They can not be longer used.
    void _loadBlock(shaBlock &block);

    /// @brief Count the number of 512bits blocks for computing SHA-256
    /// @param msgBits number of message bits
    size_t _countNumberOfBlocks(size_t msgBits);

    /// @brief SHA-256 preprocessing step
    void _preprocessing(std::string &msg);

    /// @brief Count sigma0 of given word
    word _sigma0(word x){
        return x.ROTR(7) ^ x.ROTR(18) ^ x.rShift(3);
    }

    /// @brief Count sigma1 of given word
    word _sigma1(word x){
        return x.ROTR(17) ^ x.ROTR(19) ^ x.rShift(10);
    }

    /// @brief Count capital sigma0 of given word
    word _CapSigma0(word x){
        return x.ROTR(2) ^ x.ROTR(13) ^ x.ROTR(22);
    }

    /// @brief Count capital sigma1 of given word
    word _CapSigma1(word x){
        return x.ROTR(6) ^ x.ROTR(11) ^ x.ROTR(25);
    }

    /// @brief Count Ch function from SHA-256 standard
    word _Ch(word x, word y, word z){
        return (x & y) ^ ((~x) & z);
    }

    /// @brief Count Maj function from SHA-256 standard
    word _Maj(word x, word y, word z){
        return (x & y) ^ (x & z) ^ (y & z);
    }

    /// @brief Count word on given index `t`
    word _countWt(size_t t){
        assert(t >= 16);
        return _sigma1(_w[t-2]) + _w[t-7] + _sigma0(_w[t-15]) + _w[t-16];
    }

    /// @brief Compute hash of one block
    void _compute(shaBlock &block);

public:

    sha256() : _w(), _tmpH(), _vars() {
        _blocks.clear();
    }

    /// @brief Create SHA-256 hash of given `msg`
    bit256_t hash(std::string &msg);

};

/// @brief SHA-256 initial hash values constants based on standard
const std::uint32_t HashStarts[8] = {
    0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
    0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
};

/// @brief SHA-256 K constants based on standard
const std::uint32_t K_consts[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};