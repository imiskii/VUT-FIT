/**
 * @file kry.cpp
 * @author Michal Ľaš (xlasmi00)
 * @brief Main of KRY project
 * @date 2024-04-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include "kry.hpp"


int main(int argc, char* argv[]){

    ArgParser &AP = ArgParser::GetInstance(argc, argv);
    args_t args = AP.getArguments();
    sha256 sha;
    LEA lea;

    int symbol;
    std::string input = "";
    while ((symbol = getchar()) != EOF) {
        input += static_cast<char>(symbol);
    }

    switch (args.mode)
    {
    case RunMode::CHECKSUM:
        checksum(sha, input);
        break;
    case RunMode::MAKE_MAC:
        makeMAC(sha, input, args.key);
        break;
    case RunMode::VERIFY_MAC:
        return verifyMAC(sha, input, args.key, args.mac);
        break;
    case RunMode::ATTACK_MAC:
        attackMAC(lea, input, args.mac, args.keyLength, args.msgExtension);
        break;
    default:
        break;
    }

    return 0;
}


void checksum(sha256 &sha, std::string &message){
    bit256_t finalHash;
    finalHash = sha.hash(message);
    finalHash.printHex();
}

void makeMAC(sha256 &sha, std::string &message, std::string &key){
    bit256_t mac;
    std::string key_msg = key + message;
    mac = sha.hash(key_msg);
    mac.printHex();
}

int verifyMAC(sha256 &sha, std::string &message, std::string &key, std::string &mac){
    bit256_t macBits_1(mac);
    bit256_t macBits_2;
    std::string key_msg = key + message;
    macBits_2 = sha.hash(key_msg);
    if (macBits_1 == macBits_2){
        return 0;
    } else {
        return 1;
    }
}

void attackMAC(LEA &lea, std::string &message, std::string &mac, std::uint32_t keyLength, std::string &extension){
    bit256_t originalMAC(mac);
    bit256_t alteredMAC = lea.resumMAC(message, originalMAC, keyLength, extension);
    alteredMAC.printHex();
    lea.printLastAlteredMessage();
}
