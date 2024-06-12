/**
 * @file sha256.cpp
 * @author Michal Ľaš (xlasmi00)
 * @brief Implementation of SHA-256
 * @date 2024-04-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include "sha256.hpp"


void shaBlock::set(std::string &rawBlock){
    assert (rawBlock.size() == 64);
    for (size_t i = 0, j = 0; i < _block.size(); ++i, j += 4){
        _block[i] = word(rawBlock.substr(j, 4));
    }
}

size_t sha256::_countNumberOfBlocks(size_t msgBits){
    size_t reqBits = msgBits + 64;
    if ((reqBits % 512) != 0){
        return (reqBits / 512) + 1;
    } else {
        return reqBits / 512;
    }
}


void sha256::_pad(std::string msg, std::vector<shaBlock> &blocks, size_t extraLen){
    std::uint64_t msgLen = reverse_uint64_t_endians((msg.length() + extraLen) * 8); // message length in bits, big endian
    msg += static_cast<char>(0x80); // Add 0x80 byte at the end | 0b1000 0000
    size_t bytesToAdd = msg.length();
    size_t blockNum = _countNumberOfBlocks(msg.length() * 8);
    blocks.reserve(blockNum);
    // Create blocks
    for (size_t i = 0; i < (blockNum - 1); ++i){
        std::string rawBlock;
        if (bytesToAdd > 64){ // full block
            rawBlock = msg.substr((i * 64), 64);
            bytesToAdd -= 64; 
        } else {
            rawBlock = msg.substr((i* 64), bytesToAdd);
            rawBlock.resize(64, 0);
            bytesToAdd = 0;
        }
        blocks.push_back(shaBlock(rawBlock));
    }
    std::string rawBlock;
    if (bytesToAdd != 0){
        rawBlock = msg.substr(((blockNum - 1) * 64), bytesToAdd);
    }
    rawBlock.resize(64, 0);
    // Set length
    std::memcpy((void*) (rawBlock.data() + 56), &msgLen, sizeof(msgLen)); // 56 = 64 - sizeof(msgLen)
    blocks.push_back(shaBlock(rawBlock));
}

void sha256::_preprocessing(std::string &msg){
    // Padding + parsing
    _pad(msg, _blocks);
    // Initialize start hashes
    for (size_t i = 0; i < _tmpH.size(); ++i){
        _tmpH[i] = word(HashStarts[i]);
    }
}

void sha256::_loadBlock(shaBlock &block){
    for (size_t i = 0; i < block.size(); ++i){
        _w[i] = std::move(block[i]);
    }
}

void sha256::_compute(shaBlock &block){
    // Prepare the message schedule
    _loadBlock(block);

    for (size_t i = 16; i < 64; ++i){
        _w[i] = _countWt(i);
    }

    // Initialize working variables
    for (size_t i = 0; i < _vars.size(); ++i){
        _vars[i] = _tmpH[i];
    }

    // Loop compute
    word T1;
    word T2;
    for (size_t t = 0; t < 64; ++t){
        T1 = _vars[h] + _CapSigma1(_vars[e]) + _Ch(_vars[e], _vars[f], _vars[g]) + word(K_consts[t]) + _w[t];
        T2 = _CapSigma0(_vars[a]) + _Maj(_vars[a], _vars[b], _vars[c]);
        _vars[h] = _vars[g];
        _vars[g] = _vars[f];
        _vars[f] = _vars[e];
        _vars[e] = _vars[d] + T1;
        _vars[d] = _vars[c];
        _vars[c] = _vars[b];
        _vars[b] = _vars[a];
        _vars[a] = T1 + T2; 
    }
    // Compute the i-th intermediate hash value H(i)
    for (size_t i = 0; i < _tmpH.size(); ++i){
        _tmpH[i] = _vars[i] + _tmpH[i];
    }
}

bit256_t sha256::hash(std::string &msg){
    // Make preprocessing
    _preprocessing(msg);
    
    // Computation
    for (auto &block : _blocks){
        _compute(block);
    }

    // Get hash by concatenating `_tmpH` (partial hashes)
    return bit256_t(_tmpH);
}
