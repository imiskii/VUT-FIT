/**
 * @file lea.hpp
 * @author Michal Ľaš (xlasmi00)
 * @brief Lengt Extension Attack tools
 * @date 2024-04-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include <array>
#include <vector>
#include "utils.hpp"
#include "sha256.hpp"


/// @brief Length Extension Attack Object
class LEA : public sha256{
private:
    std::vector<shaBlock> _exBlocks; ///< SHA blocks with extended message
    std::string _newMsg;             /// Representation of message after the length extension attack

    /// @brief Count size of original message blocks in bytes
    /// @param originalMessageSize size of secret key + message in bytes
    /// @return Blocks size of original message in bytes
    size_t _countOriginalMessageBlockSize(size_t originalMessageSize);

    /// @brief Create string representation of new message after the length extension attack.
    /// The padded bytes are marked as 0x00, the message separator is 0x80
    /// @param msg Original message
    /// @param secretLen Length of the secret
    /// @param originalBitsSize Size of SHA blocks in original message in bytes
    /// @param extension Added extension
    /// @return String representing the new message after the length extension attack
    std::string _getNewMsg(std::string &msg, std::uint32_t secretLen, size_t originalBlocksSize, std::string &extension);

public:

    LEA(){
        _exBlocks.clear();
        _newMsg = "";
    }

    /// @brief Perform attack on given `mac`
    /// @param msg Original message
    /// @param mac MAC that will be attacked in 64 chars long hexadecimal string
    /// @param secretLen Length of the secret key
    /// @param extension Message that will be append to the MAC
    bit256_t resumMAC(std::string &msg, bit256_t &mac, std::uint32_t secretLen, std::string &extension);

    /// @brief Print last message that was computed with this object. Empty string if no messages was computed yet.
    void printLastAlteredMessage();

};