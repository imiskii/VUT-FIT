/**
 * @file argparser.hpp
 * @author Michal Ľaš (xlasmi00)
 * @brief Header file for argparse.cpp - Class for parsing program arguments
 * @date 2024-01-16
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include <getopt.h>
#include <vector>
#include <regex>
#include <string>
#include <iostream>
#include <cstring>


#pragma once


using namespace std;


/******************** MACROS ********************/


/****************** DATATYPES *******************/


/// @brief Possible program modes
enum class RunMode{
    NONE,
    CHECKSUM,   ///< count SHA-256 hash
    MAKE_MAC,   ///< create MAC
    VERIFY_MAC, ///< verify MAC
    ATTACK_MAC  ///< execute length extension attack on MAC
};


/**
 * @brief Structure with parsed program arguments
 * 
 */
struct args_t 
{
    RunMode mode;                   ///< Selected program mode
    std::string key;                ///< Key
    std::string mac;                ///< MAC of inputed message
    std::string msgExtension;       ///< Specifies extension of inputed message
    std::uint32_t keyLength;        ///< Length of secret key
};


/**
 * @brief Argument flags (tells if program argument was given)
 * 
 */
struct arg_flags_t
{
    bool h_flag;    ///< help flag
    bool e_flag;    ///< length extension attack mode
    bool k_flag;    ///< key
    bool m_flag;    ///< message
    bool n_flag;    ///< key length
    bool a_flag;    ///< length of extended message
};

/*********** FUNCTIONS AND CLASSES **************/

/**
 * @brief Class representing program arguments and its parsing methods
 * 
 */
class ArgParser
{
protected:
    bool __parsed; ///< Flag telling if program arguments were already parsed

    /**
     * @brief Construct a new Arg Parser object
     *
     * @param argc number of arguments
     * @param argv pointer to array with arguments
     */
    ArgParser(int argc, char **argv);

private:
    /* Private Attributes */

    vector<option> _long_opt;   ///< Long program options
    string _short_opt;          ///< Short program options
    int _argCount;              ///< number of arguments
    char **_progArgs;           ///< program arguments

    /* Private Methods */

    /**
     * @brief print program help info
     *
     */
    void _printHelp();
    

    /// @brief Check if all required arguments were provided
    bool _checkArgs();


    /* Methods for processing program arguments */

    void _proc_c_opt();
    void _proc_s_opt();
    void _proc_v_opt();
    void _proc_e_opt();
    void _proc_k_opt(char *arg);
    void _proc_m_opt(char *arg);
    void _proc_n_opt(char *arg);
    void _proc_a_opt(char *arg);


public:
    ArgParser(ArgParser &other) = delete;
    void operator=(const ArgParser &) = delete;

    /* Public Attributes */
    args_t args;          ///< Parsed program arguments
    arg_flags_t flags;    ///< Program arguments flags telling if the argument was given

    /* Public Methods */

    /**
     * @brief Get the Instance object
     *
     * @param argc number of program arguments
     * @param argv program arguments
     * @return ArgParser& ArgParser class instance
     */
    static ArgParser& GetInstance(int argc, char **argv);

    /**
     * @brief Parse program arguments
     *
     * @return arguments struct with parsed program arguments
     */
    args_t getArguments();
};


/* END OF FILE */
