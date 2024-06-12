/**
 * @file kry.hpp
 * @author Michal Ľaš (xlasmi00)
 * @brief Main of KRY project
 * @date 2024-04-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#pragma once


#include "argparser.hpp"
#include "sha256.hpp"
#include "lea.hpp"

#include <cstdio>
#include <string>

/// @brief Compute SHA-256 hash of given `message`
void checksum(sha256 &sha, std::string &message);

/// @brief Compute MAC of given `message` with given `key`
void makeMAC(sha256 &sha, std::string &message, std::string &key);

/// @brief Verify MAC (`mac`) of given `message` with given `key`
/// @return 0 if MAC was valid, 1 if MAC was invalid
int verifyMAC(sha256 &sha, std::string &message, std::string &key, std::string &mac);

/// @brief Perform a length extension attack on given MAC (`mac`) with given `message`, `keyLength` and specified `extension`
void attackMAC(LEA &lea, std::string &message, std::string &mac, std::uint32_t keyLength, std::string &extension);
