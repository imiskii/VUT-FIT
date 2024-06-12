/**
 * @file argparser.cpp
 * @author Michal Ľaš (xlasmi00)
 * @brief Class for parsing program arguments
 * @date 2024-01-16
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include "argparser.hpp"


using namespace std;


ArgParser::ArgParser(int argc, char **argv): _argCount(argc), _progArgs(argv){
    // set arguments options
    this->_short_opt = "hcsvek:m:n:a:";
    // set implicit arguments values
    this->flags = {false, false, false, false, false, false};
    this->args = {.mode=RunMode::NONE, .key="", .mac="", .msgExtension="", .keyLength=0};
    // set flag to not parsed arguments yet
    this->__parsed = false;
}

ArgParser& ArgParser::GetInstance(int argc, char **argv){
    static ArgParser __instance(argc,argv);
    return __instance;
}


args_t ArgParser::getArguments(){
    // If arguments were already parsed just return them
    if (this->__parsed){
        return this->args;
    }

    if (this->_argCount == 1){
        _printHelp();
        exit(EXIT_FAILURE);
    }

    int op = 0; // option

    // process program options (arguments)
    while((op = getopt(this->_argCount, this->_progArgs, this->_short_opt.c_str())) != -1)
    {
        switch (op)
        {
        // help option
        case 'h':
            // single argument '-h' is used
            if (this->_argCount == 2) {
                this->_printHelp();
                exit(EXIT_SUCCESS);
            } else {
                cerr << "No additional arguments allowed with -h." << endl;
                exit(EXIT_FAILURE);
            }
            break;
        case 'c':
            _proc_c_opt();
            break;
        case 's':
            _proc_s_opt();
            break;
        case 'v':
            _proc_v_opt();
            break;
        case 'e':
            _proc_e_opt();
            break;
        case 'k':
            _proc_k_opt(optarg);
            break;
        case 'm':
            _proc_m_opt(optarg);
            break;
        case 'n':
            _proc_n_opt(optarg);
            break;
        case 'a':
            _proc_a_opt(optarg);
            break;
        default:
            exit(EXIT_FAILURE); // error message comes from getopt
            break;
        }
    }

    if (!_checkArgs()){
        cerr << "Missing required argument/s! See help with option -h." << endl;
        exit(EXIT_FAILURE);
    }

    this->__parsed = true;
    return this->args;
}


void ArgParser::_printHelp()
{
    cout << "KRY - project for SHA-256 MAC and length extension attacks." << endl << endl
         << "The program can be executed with exactly one of the following switches. Each switch changes the functionality of the application:" << endl
         << "\t-c Computes and prints the SHA-256 checksum of the input message to STDOUT." << endl
         << "\t-s Computes a MAC for the input message and prints the result to STDOUT. Must be used with the -k parameter." << endl
         << "\t-v Verifies the MAC for a given key and input message. Returns 0 in case of a valid MAC, otherwise 1. Must be used in combination with the -k and -m parameters." << endl
         << "\t-e Performs a length extension attack on the MAC and input message. Must be used in combination with the -m, -n, and -a parameters." << endl
         << endl
         << "The program may require additional parameters depending on the selected functionality. These parameters specify additional input information:" << endl
         << "\t-k <KEY> Specifies the secret key for MAC calculation." << endl
         << "\t-m <CHS> Specifies the MAC of the input message for its verification or attack." << endl
         << "\t-n <NUM> Specifies the length of the secret key (necessary for performing the attack)." << endl
         << "\t-a <MSG> Specifies the extension of the input message for performing the attack." << endl
         << endl;
}


bool ArgParser::_checkArgs(){
    switch (args.mode){
    case RunMode::NONE:
        return false;
    case RunMode::MAKE_MAC:
        if (!flags.k_flag)
            return false;
        break;
    case RunMode::VERIFY_MAC:
        if (!flags.k_flag || !flags.m_flag)
            return false;
        break;
    case RunMode::ATTACK_MAC:
        if (!flags.n_flag || !flags.m_flag || !flags.a_flag)
            return false;
        break;
    default:
        break;
    }

    return true;
}


void ArgParser::_proc_c_opt(){
    if (args.mode == RunMode::NONE){
        args.mode = RunMode::CHECKSUM;
    } else {
        cerr << "Multiple run modes specified. Only one run mode option (-c/-s/-v/-e) is allowed!" << endl;
        exit(EXIT_FAILURE);
    }
}

void ArgParser::_proc_s_opt(){
    if (args.mode == RunMode::NONE){
        args.mode = RunMode::MAKE_MAC;
    } else {
        cerr << "Multiple run modes specified. Only one run mode option (-c/-s/-v/-e) is allowed!" << endl;
        exit(EXIT_FAILURE);
    }
}

void ArgParser::_proc_v_opt(){
    if (args.mode == RunMode::NONE){
        args.mode = RunMode::VERIFY_MAC;
    } else {
        cerr << "Multiple run modes specified. Only one run mode option (-c/-s/-v/-e) is allowed!" << endl;
        exit(EXIT_FAILURE);
    }
}

void ArgParser::_proc_e_opt(){
    if (args.mode == RunMode::NONE){
        args.mode = RunMode::ATTACK_MAC;
    } else {
        cerr << "Multiple run modes specified. Only one run mode option (-c/-s/-v/-e) is allowed!" << endl;
        exit(EXIT_FAILURE);
    }
}

void ArgParser::_proc_k_opt(char *arg){
    regex rKey("^[A-Za-z0-9]*$");
    if (!regex_match(arg, rKey)){
        cerr << "Invalid key (-k) argument: " << arg << "!" << endl;
        exit(EXIT_FAILURE);
    }

    if (flags.k_flag){
        cerr << "Only one -k argument is allowed!" << endl;
        exit(EXIT_FAILURE);
    }

    flags.k_flag = true;
    args.key = arg;
}

void ArgParser::_proc_m_opt(char *arg){
    regex rMac("^[A-Fa-f0-9]{64}$");
    if (!regex_match(arg, rMac)){
        cerr << "Invalid MAC (-m) argument: " << arg << "!" << endl;
        exit(EXIT_FAILURE);
    }

    if (flags.m_flag){
        cerr << "Only one -m argument is allowed!" << endl;
        exit(EXIT_FAILURE);
    }

    flags.m_flag = true;
    args.mac = arg;
}

void ArgParser::_proc_n_opt(char *arg){
    try {
        args.keyLength = std::stoul(arg);

        if (flags.n_flag){
            cerr << "Only one -n argument is allowed!" << endl;
            exit(EXIT_FAILURE);
        }

        flags.n_flag = true;
    } catch (...) {
        cerr << "Invalid key length (-n) argument: " << arg << "!" << endl;
        exit(EXIT_FAILURE);
    }
}

void ArgParser::_proc_a_opt(char *arg){
    regex rMsg("^[a-zA-Z0-9!#$%&'\"()*+,\\-.\\/:;<>=?@[\\]\\\\^_{}|~]*$");
    if (!regex_match(arg, rMsg)){
        cerr << "Invalid message extension (-a) argument: " << arg << "!" << endl;
        exit(EXIT_FAILURE);
    }

    if (flags.a_flag){
        cerr << "Only one -a argument is allowed!" << endl;
        exit(EXIT_FAILURE);
    }

    flags.a_flag = true;
    args.msgExtension = arg;
}

/* END OF FILE */
