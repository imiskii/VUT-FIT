/**
 * @file utils.hpp
 * @author Michal Ľaš (xlasmi00)
 * @brief Common functions and datatypes
 * @date 2024-04-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#pragma once

#include <span>
#include <array>
#include <algorithm>
#include <cstdint>
#include <cassert>
#include <cstring>
#include <ostream>
#include <iostream>
#include <iomanip>

class word{
private:
    static constexpr int BITS = 32;
    std::uint32_t _data;

public:

    constexpr word() : _data(0) {}
    constexpr explicit word(std::uint32_t val) : _data(val) {}
    explicit word(std::string val);

    friend constexpr word operator+(const word &a, const word &b) { return word(a._data + b._data); }
    friend constexpr word operator^(const word &a, const word &b) { return word(a._data ^ b._data); }
    friend constexpr word operator&(const word &a, const word &b) { return word(a._data & b._data); }
    friend constexpr word operator~(const word &a) { return word(~a._data); }
    friend constexpr bool operator==(const word &a, const word &b) { return a._data == b._data; }
    friend std::ostream& operator<<(std::ostream& os, const word &w) { return os << w._data; }

    /// @brief Get pointer to stored data
    std::uint32_t* data(){
        return &_data;
    }

    std::uint32_t get(){
        return _data;
    }

    size_t size(){
        return BITS / 8;
    }

    void set(std::uint32_t val){
        _data = val;
    }

    /// @brief Shift by `n` to right
    word rShift(std::uint32_t n){
        return word(_data >> n);
    }

    /// @brief Shift by `n` to left
    word lShift(std::uint32_t n){
        return word(_data << n);
    }

    /// @brief Circular shift by `n` bits to right
    word ROTR(std::uint32_t n){
        assert(n <= BITS);
        return word(((_data >> n) | (_data << (BITS - n))));
    }

    /// @brief Circular shift by `n` bits to left
    word ROTL(std::uint32_t n){
        assert(n <= BITS);
        return word(((_data << n) | (_data >> (BITS - n))));
    }

    /// @brief Returns value of bit on `n`-th position indexed from 0 to 32
    std::uint32_t getBit(std::uint32_t n) const{
        assert(n <= BITS);
        return (_data >> n) & 1UL;
    }

    /// @brief Print object value in binary form to STDOUT
    void printBin() const;

};


class bit256_t{
private:
    static constexpr int WIDTH = 256 / 8;
    std::array<std::uint8_t, WIDTH> _data;

public:
    /// @brief construct 0 value by default
    constexpr bit256_t() : _data() {}

    /// @brief Explicit constructor with string value
    explicit bit256_t(std::string &str){
        setByHexString(str);
    }

    /// @brief Explicit constructor with a array of eight words
    explicit bit256_t(std::array<word, 8> &words){
        setByWords(words);
    }

    /// @brief Get array stored bits as array of eight words
    std::array<word, 8> getWords();

    constexpr int Compare(const bit256_t& other) const { return std::memcmp(_data.data(), other._data.data(), WIDTH); }
    friend constexpr bool operator==(const bit256_t &a, const bit256_t &b) { return a.Compare(b) == 0; }
    friend constexpr bool operator!=(const bit256_t& a, const bit256_t& b) { return a.Compare(b) != 0; }
    friend constexpr bool operator<(const bit256_t& a, const bit256_t& b) { return a.Compare(b) < 0; }

    bool isNull() const{
        return std::all_of(_data.begin(), _data.end(), [](std::uint8_t val) {
            return val == 0;
        });
    }

    void setNull(){
        std::fill(_data.begin(), _data.end(), 0);
    }

    /// @brief Print object value in hexadecimal form to STDOUT
    void printHex() const;

    /// @brief Set the value of this object with given hexadecimal string `str` 
    void setByHexString(std::string &str);

    void setByWords(std::array<word, 8> words);
};

/// @brief Get hexadecimal value of a character `c` in form \\xXX
std::string getHex(const char c);

/// @brief Get hexadecimal value of std::uint64_t number
std::string getHexOfUint64(std::uint64_t val);

/// @brief Reverse endianity of given std::uint64_t `value`
std::uint64_t reverse_uint64_t_endians(std::uint64_t value);
