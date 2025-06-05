// SPDX-License-Identifier: UNLICENSED
pragma solidity ^0.8.28;

import {Script, console} from "forge-std/Script.sol";
import {ExtendedERC20} from "../src/ExtendedERC20.sol";

contract ExtendedERC20Script is Script {
    ExtendedERC20 public token;
    uint constant MAX_SUPPLY = 1_000_000 ether;
    uint constant MAX_DAILY_MINT = 100_000 ether;
    uint constant VIRIFICATION_EXPIRATION = 48; // in hours

    address[] ADMINS = [
        0x9965507D1a55bcC2695C58ba16FB37d819B0A4dc,
        0x976EA74026E726554dB657fA54763abd0C3a0aa9,
        0x14dC79964da2C08b23698B3D3cc7Ca32193d9955
    ]; // 5,6,7
    address[] RestrADMINS = [
        0x70997970C51812dc3A010C7d01b50e0d17dc79C8,
        0x3C44CdDdB6a900fa2b585dd299e03d12FA4293BC,
        0x90F79bf6EB2c4f870365E785982E1f101E93b906,
        0x15d34AAf54267DB7D7c367839AAf71A00a2C6A65
    ]; // 1,2,3,4
    address[] IdpADMINS = [0xf39Fd6e51aad88F6F4ce6aB8827279cffFb92266]; // 0
    address[] DIPs = [0xe05fcC23807536bEe418f142D19fa0d21BB0cfF7];

    function run() public {
        vm.startBroadcast();

        token = new ExtendedERC20(
            "TestToken",
            "TT",
            MAX_SUPPLY,
            MAX_DAILY_MINT,
            ADMINS,
            RestrADMINS,
            DIPs,
            IdpADMINS,
            VIRIFICATION_EXPIRATION
        );

        vm.stopBroadcast();
    }
}
