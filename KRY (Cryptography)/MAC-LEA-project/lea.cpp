/**
 * @file lea.cpp
 * @author Michal Ľaš (xlasmi00)
 * @brief Lengt Extension Attack tools
 * @date 2024-04-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "lea.hpp"


size_t LEA::_countOriginalMessageBlockSize(size_t originalMessageSize){
    originalMessageSize += 1; // 0x80 byte
    size_t blockNum = _countNumberOfBlocks(originalMessageSize * 8);
    return blockNum * 64; // each block has 512 bits
}

std::string LEA::_getNewMsg(std::string &msg, std::uint32_t secretLen, size_t originalBlocksSize, std::string &extension){
    std::string newMsg = "";
    std::uint64_t msgLen = (msg.length() + secretLen) * 8; // in bits
    size_t paddingSize = originalBlocksSize - (secretLen + msg.length() + 1 + sizeof(msgLen));
    newMsg += msg;
    newMsg += "\\x80";
    for (size_t i = 0; i < paddingSize; ++i){
        newMsg += "\\x00";
    }
    newMsg += getHexOfUint64(msgLen);
    newMsg += extension;
    return newMsg;
}

bit256_t LEA::resumMAC(std::string &msg, bit256_t &mac, std::uint32_t secretLen, std::string &extension){
    // Initialize partial hashes with given MAC (_tmpH)
    _tmpH = mac.getWords();

    // Compute padding for extension
    size_t extraLen = _countOriginalMessageBlockSize(secretLen + msg.length());
    _pad(extension, _exBlocks, extraLen);

    // Loop _exBlocks and compute hashes
    for (auto &block : _exBlocks){
        _compute(block);
    }

    // Get final hash and message
    _newMsg = _getNewMsg(msg, secretLen, extraLen, extension);
    return bit256_t(_tmpH);
}

void LEA::printLastAlteredMessage(){
    std::cout << _newMsg << std::endl;
}
