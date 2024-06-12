/**
 * @file utils.cpp
 * @author Michal Ľaš (xlasmi00)
 * @brief Common functions and datatypes
 * @date 2024-04-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include "utils.hpp"

word::word(std::string val) : _data(0){
    assert(val.size() <= 4);
    std::reverse(val.begin(), val.end());
    std::memcpy(&_data, val.data(), val.size());
}

void word::printBin() const{
    for (int i = 31; i >= 0; --i){
        std::cout << ((_data >> i) & 1);
        if (i % 8 == 0)
            std::cout << " ";
    }
    std::cout << std::endl;
}


void bit256_t::printHex() const{
    for (const auto &byte : _data)
        std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<std::uint32_t>(byte);
    std::cout << std::endl;
}

void bit256_t::setByHexString(std::string &str){
    assert(str.length() == (WIDTH * 2));
    for (size_t i = 0; i < str.length(); i += 2) {
        std::string byteString = str.substr(i, 2);
        _data[i/2] = static_cast<std::uint8_t>(strtol(byteString.c_str(), NULL, 16));
    }
}

void bit256_t::setByWords(std::array<word, 8> words){
    for (size_t i = 0; i < words.size(); ++i){
        _data[i * 4 + 3] = words[i].get() & 0xFF;
        _data[i * 4 + 2] = (words[i].get() >> 8) & 0xFF;
        _data[i * 4 + 1] = (words[i].get() >> 16) & 0xFF;
        _data[i * 4] = (words[i].get() >> 24) & 0xFF;
    }
}

std::array<word, 8> bit256_t::getWords(){
    std::array<word, 8> words;
    for (size_t i = 0; i < words.size(); ++i){
        std::uint32_t rawWord = 0;
        rawWord |= static_cast<std::uint32_t>(_data[i * 4 + 3]);
        rawWord |= static_cast<std::uint32_t>(_data[i * 4 + 2]) << 8;
        rawWord |= static_cast<std::uint32_t>(_data[i * 4 + 1]) << 16;
        rawWord |= static_cast<std::uint32_t>(_data[i * 4]) << 24;
        words[i] = word(rawWord);
    }
    return words;
}

std::string getHex(const unsigned char c){
    std::stringstream ss;
    ss << "\\x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<std::uint32_t>(c);
    return ss.str();
}

std::string getHexOfUint64(std::uint64_t val){
    std::string ret = "";
    for (int i = 7; i >= 0; --i){
        std::uint8_t byte = (val >> (i * 8)) & 0xFF;
        ret += getHex(byte);
    }
    return ret;
}

std::uint64_t reverse_uint64_t_endians(std::uint64_t value){
    return ((value & 0x00000000000000FF) << 56) |
           ((value & 0x000000000000FF00) << 40) |
           ((value & 0x0000000000FF0000) << 24) |
           ((value & 0x00000000FF000000) << 8)  |
           ((value & 0x000000FF00000000) >> 8)  |
           ((value & 0x0000FF0000000000) >> 24) |
           ((value & 0x00FF000000000000) >> 40) |
           ((value & 0xFF00000000000000) >> 56);
}
