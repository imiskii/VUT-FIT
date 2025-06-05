import { ethers } from "ethers";

async function signVerificationMessage(wallet, userAddress, timestamp) {
    const message = `User with address ${userAddress} has verified their identity at ${timestamp}`;
    const signature = await wallet.signMessage(message);
    return signature;
}

const PRIVATE_KEY = "0x00000000000000000000000000000000000000000000000000000000000a11ce";
const USER_ADDRESS = "0x7e5f4552091a69125d5dfcb7b8c2659029395bdf"; /// vm.addr(1)
const idpWallet = new ethers.Wallet(PRIVATE_KEY.toLowerCase());
const timestamp = Math.floor(Date.now() / 1000);

const sig = await signVerificationMessage(idpWallet, USER_ADDRESS.toLowerCase(), timestamp);
console.log("Wallet address:", idpWallet.address); // matches vm.addr(0xa11ce)
console.log("Timestamp: ", timestamp);
console.log("Signature: ", sig);
