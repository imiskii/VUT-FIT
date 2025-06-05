// SPDX-License-Identifier: MIT
pragma solidity ^0.8.28;

import {Test, console} from "forge-std/Test.sol";
import {StdCheats} from "forge-std/StdCheats.sol";
import {ExtendedERC20} from "../src/ExtendedERC20.sol";
import {MessageHashUtils} from "@openzeppelin/contracts/utils/cryptography/MessageHashUtils.sol";
import {Strings} from "@openzeppelin/contracts/utils/Strings.sol";

contract ExtendedERC20Test is Test {
    ExtendedERC20 public token;
    address[] public users;
    address[] public admins;
    address[] public restrAdmins;
    address[] public idpAdmins;

    struct UserSignatureInfo {
        uint timestamp;
        bytes signature;
    }
    mapping(address => UserSignatureInfo) userSignatures;

    // IDP private key (simulated off-chain signer)
    uint256 internal constant IDP_PRIVATE_KEY = 0xA11CE;
    address[] public IDPs;

    uint constant MAX_SUPPLY = 1_000_000 ether;
    uint constant MAX_DAILY_MINT = 100_000 ether;
    uint constant VIRIFICATION_EXPIRATION = 48; // in hours

    function _signUser(
        address user,
        uint timestamp
    ) internal pure returns (bytes memory) {
        string memory message = string(
            abi.encodePacked(
                "User with address ",
                Strings.toHexString(user),
                " has verified their identity at ",
                Strings.toString(timestamp)
            )
        );
        bytes32 messageHash = keccak256(bytes(message));
        bytes32 ethSignedHash = MessageHashUtils.toEthSignedMessageHash(
            messageHash
        );
        (uint8 v, bytes32 r, bytes32 s) = vm.sign(
            IDP_PRIVATE_KEY,
            ethSignedHash
        );
        return abi.encodePacked(r, s, v);
    }

    function _generateRoleData(
        uint count,
        uint offset,
        uint timestamp,
        address[] storage targetList
    ) internal {
        for (uint i = 0; i < count; ++i) {
            address addr = vm.addr(i + 1 + offset);
            targetList.push(addr);
            userSignatures[addr] = UserSignatureInfo(
                timestamp,
                _signUser(addr, timestamp)
            );
        }
    }

    function _verifyUsers(address[] storage targetList) internal {
        for (uint i = 0; i < targetList.length; ++i) {
            UserSignatureInfo memory sig = userSignatures[targetList[i]];
            vm.prank(targetList[i]);
            token.verifyIdentity(sig.timestamp, sig.signature);
        }
    }

    function initERC20(
        uint usersCount,
        uint adminCounts,
        uint restrAdminCount,
        uint idpAdminCount,
        uint maxSupply,
        uint maxDailyMint,
        bool verifyUsers
    ) public {
        delete users;
        delete admins;
        delete restrAdmins;
        delete idpAdmins;

        uint offset = 0;
        uint timestamp = block.timestamp;

        /// Generate authority
        IDPs.push(vm.addr(IDP_PRIVATE_KEY));

        /// Generate users for all roles
        _generateRoleData(usersCount, offset, timestamp, users);
        offset += usersCount;

        _generateRoleData(adminCounts, offset, timestamp, admins);
        offset += adminCounts;

        _generateRoleData(restrAdminCount, offset, timestamp, restrAdmins);
        offset += adminCounts;

        _generateRoleData(idpAdminCount, offset, timestamp, idpAdmins);

        /// Create ExtendedERC20
        token = new ExtendedERC20(
            "TestToken",
            "TT",
            maxSupply,
            maxDailyMint,
            admins,
            restrAdmins,
            IDPs,
            idpAdmins,
            VIRIFICATION_EXPIRATION
        );

        /// Make initial users verification
        if (verifyUsers) {
            _verifyUsers(users);
            _verifyUsers(admins);
            _verifyUsers(restrAdmins);
            _verifyUsers(idpAdmins);
        }
    }

    // ===================
    //        TESTS
    // ===================

    function testMintFailsForUnverifiedUser() public {
        initERC20(1, 1, 0, 0, MAX_SUPPLY, MAX_DAILY_MINT, false);
        address admin = admins[0];

        vm.prank(admin);
        vm.expectRevert("User was never verified!");
        token.mint(admin, 1_000 ether);
        assertEq(token.balanceOf(admin), 0 ether);
    }

    function testMintUnderLimitByAdmin() public {
        initERC20(0, 1, 0, 0, MAX_SUPPLY, MAX_DAILY_MINT, true);
        address admin = admins[0];
        uint amount = 50_000 ether;

        vm.expectEmit();
        emit ExtendedERC20.TokensMinted(admin, amount);
        vm.prank(admin);
        token.mint(admin, amount);
        assertEq(token.totalSupply(), amount);
        assertEq(token.balanceOf(admin), amount);
    }

    function testMintFailsByNonAdmin() public {
        initERC20(1, 0, 0, 0, MAX_SUPPLY, MAX_DAILY_MINT, true);
        address user = users[0];

        vm.prank(user);
        vm.expectRevert("Not minting user!"); // Not minting admin
        token.mint(user, 1 ether);
        assertEq(token.balanceOf(user), 0 ether);
    }

    function testMintFailsIfExceedsMaxSupply() public {
        initERC20(0, 1, 0, 0, MAX_SUPPLY, MAX_SUPPLY * 2, true);
        address admin = admins[0];

        vm.prank(admin);
        vm.expectRevert("Max supply exceeded!"); // Max supply exceeded
        token.mint(admin, MAX_SUPPLY + 1 ether);
        assertEq(token.balanceOf(admin), 0 ether);
    }

    function testMintFailsIfExceedsDailyLimit() public {
        initERC20(0, 1, 0, 0, MAX_SUPPLY, MAX_DAILY_MINT, true);
        address admin = admins[0];

        vm.prank(admin);
        token.mint(admin, MAX_DAILY_MINT);

        vm.prank(admin);
        vm.expectRevert("Minting level reached for today!"); // Daily mint limit reached
        token.mint(admin, 1 ether);
        assertEq(token.balanceOf(admin), MAX_DAILY_MINT);
    }

    function testDailyLimitResets() public {
        initERC20(0, 1, 0, 0, MAX_SUPPLY, MAX_DAILY_MINT, true);
        address admin = admins[0];

        uint currentTime = block.timestamp;
        uint today = currentTime - (currentTime % 1 days);
        vm.warp(today);

        vm.prank(admin);
        token.mint(admin, MAX_DAILY_MINT);
        assertEq(token.balanceOf(admin), MAX_DAILY_MINT);

        vm.warp(today + 1 days + 1); // Change block timestamp to next day

        vm.prank(admin);
        token.mint(admin, 1 ether);
        assertEq(token.balanceOf(admin), MAX_DAILY_MINT + 1 ether);
    }

    function testTransferAllowedUnderLimit() public {
        initERC20(1, 1, 1, 0, MAX_SUPPLY, MAX_DAILY_MINT, true);
        address restrAdmin = restrAdmins[0];
        address admin = admins[0];
        address user = users[0];

        vm.expectEmit();
        emit ExtendedERC20.TransferRestrictionCreated(admin, 50_000 ether);
        vm.prank(restrAdmin);
        token.setTransferDailyLimit(admin, 50_000 ether);

        vm.prank(admin);
        token.mint(admin, 50_000 ether);

        vm.expectEmit();
        emit ExtendedERC20.TokensTransferred(admin, user, 40_000 ether);
        vm.prank(admin);
        token.transfer(user, 40_000 ether);

        assertEq(token.balanceOf(user), 40_000 ether);
        assertEq(token.balanceOf(admin), 10_000 ether);
    }

    function testTransferFailsOverLimit() public {
        initERC20(1, 1, 1, 0, MAX_SUPPLY, MAX_DAILY_MINT, true);
        address restrAdmin = restrAdmins[0];
        address admin = admins[0];
        address user = users[0];

        vm.expectEmit();
        emit ExtendedERC20.TransferRestrictionCreated(admin, 20_000 ether);
        vm.prank(restrAdmin);
        token.setTransferDailyLimit(admin, 20_000 ether);

        vm.prank(admin);
        token.mint(admin, 50_000 ether);

        vm.prank(admin);
        vm.expectRevert("Transfer exceeds daily limit!"); // Daily limit exceeded
        token.transfer(user, 40_000 ether);

        assertEq(token.balanceOf(user), 0 ether);
        assertEq(token.balanceOf(admin), 50_000 ether);
    }

    function testTransferResetNextDay() public {
        initERC20(1, 1, 1, 0, MAX_SUPPLY, MAX_DAILY_MINT, true);
        address restrAdmin = restrAdmins[0];
        address admin = admins[0];
        address user = users[0];

        vm.expectEmit();
        emit ExtendedERC20.TransferRestrictionCreated(admin, 20_000 ether);
        vm.prank(restrAdmin);
        token.setTransferDailyLimit(admin, 20_000 ether);

        vm.prank(admin);
        token.mint(admin, 50_000 ether);

        vm.expectEmit();
        emit ExtendedERC20.TokensTransferred(admin, user, 20_000 ether);
        vm.prank(admin);
        token.transfer(user, 20_000 ether); // Should pass

        // simulate time travel to next day
        vm.warp(block.timestamp + 1 days + 1);

        vm.expectEmit();
        emit ExtendedERC20.TokensTransferred(admin, user, 20_000 ether);
        vm.prank(admin);
        token.transfer(user, 20_000 ether); // Should pass

        assertEq(token.balanceOf(user), 40_000 ether);
        assertEq(token.balanceOf(admin), 10_000 ether);
    }

    function testVerificationWithValidSignature() public {
        initERC20(1, 0, 0, 0, MAX_SUPPLY, MAX_DAILY_MINT, false);
        address user = users[0];
        uint timestamp = block.timestamp;

        bytes memory signature = _signUser(user, timestamp);

        vm.prank(user);
        token.verifyIdentity(timestamp, signature);
        assertTrue(token.isUserVerified(user));
    }

    function testVerificationWithInvalidSignature() public {
        initERC20(2, 0, 0, 0, MAX_SUPPLY, MAX_DAILY_MINT, false);
        address user1 = users[0];
        address user2 = users[1];
        uint timestamp = block.timestamp;

        // Signature for different user
        bytes memory invalidSignature = _signUser(user1, timestamp);

        vm.prank(user2);
        vm.expectRevert("Signer not trusted IDP or invalid signature!");
        token.verifyIdentity(timestamp, invalidSignature);
        assertFalse(token.isUserVerified(user2));
    }

    function testVerificationWithUntrustedSigner() public {
        initERC20(1, 0, 0, 0, MAX_SUPPLY, MAX_DAILY_MINT, false);
        address user = users[0];
        uint timestamp = block.timestamp;

        uint untrustedPrivateKey = 0x123456; // Not in trusted IDP list

        string memory message = string(
            abi.encodePacked(
                "User with address ",
                Strings.toHexString(user),
                " has verified their identity at ",
                Strings.toString(timestamp)
            )
        );
        bytes32 messageHash = keccak256(bytes(message));
        bytes32 ethSignedHash = MessageHashUtils.toEthSignedMessageHash(
            messageHash
        );

        (uint8 v, bytes32 r, bytes32 s) = vm.sign(
            untrustedPrivateKey,
            ethSignedHash
        );
        bytes memory signature = abi.encodePacked(r, s, v);

        vm.prank(user);
        vm.expectRevert("Signer not trusted IDP or invalid signature!");
        token.verifyIdentity(timestamp, signature);
        assertFalse(token.isUserVerified(user));
    }

    function testVerificationExpiration() public {
        initERC20(1, 1, 0, 0, MAX_SUPPLY, MAX_DAILY_MINT, true);
        address admin = admins[0];
        address user = users[0];
        uint amount = 5_000 ether;

        vm.prank(admin);
        token.mint(admin, amount);

        vm.warp(block.timestamp + 2 days + 1);
        vm.prank(admin);
        vm.expectRevert("User verification expired!");
        token.transfer(user, amount); // Tokens can not be manipulated, bcs. of verification expiration

        assertEq(token.balanceOf(admin), amount);
        assertEq(token.balanceOf(user), 0 ether);
    }

    function testVerificationExpirationButCanMint() public {
        initERC20(0, 1, 0, 0, MAX_SUPPLY, MAX_DAILY_MINT, true);
        address admin = admins[0];
        uint amount = 5_000 ether;

        vm.warp(block.timestamp + 2 days + 1);
        vm.prank(admin);
        token.mint(admin, amount);
        assertEq(token.balanceOf(admin), amount);
    }

    function testReverification() public {
        initERC20(1, 0, 0, 0, MAX_SUPPLY, MAX_DAILY_MINT, true);
        address user = users[0];

        uint newTimestamp = block.timestamp + 2 days + 1;
        vm.warp(newTimestamp);
        assertFalse(token.isUserVerified(user));

        bytes memory signature = _signUser(user, newTimestamp);
        vm.prank(user);
        token.verifyIdentity(newTimestamp, signature);

        vm.prank(user);
        assertTrue(token.isUserVerified(user));
    }

    function testAddTrustedIdp() public {
        initERC20(0, 0, 0, 1, MAX_SUPPLY, MAX_DAILY_MINT, false);
        address idpAdmin = idpAdmins[0];
        address newIdp = vm.addr(0x123456);

        vm.prank(idpAdmin);
        token.addTrustedIdp(newIdp);
        assertTrue(token.isTrustedIDP(newIdp));
    }

    function testAddTrustedIdpByNonIdpAdmin() public {
        initERC20(1, 0, 0, 0, MAX_SUPPLY, MAX_DAILY_MINT, false);
        address noIdpAdmin = users[0];
        address newIdp = vm.addr(0x123456);

        vm.prank(noIdpAdmin);
        vm.expectRevert("Not IDP admin user!");
        token.addTrustedIdp(newIdp);
        assertFalse(token.isTrustedIDP(newIdp));
    }

    function testRemoveTrustedIdp() public {
        initERC20(0, 0, 0, 1, MAX_SUPPLY, MAX_DAILY_MINT, false);
        address idpAdmin = idpAdmins[0];
        address usedIdp = IDPs[0];

        vm.prank(idpAdmin);
        token.removeTrustedIdp(usedIdp);
        assertFalse(token.isTrustedIDP(usedIdp));
    }

    function testRemoveTrustedIdpByNonIdpAdmin() public {
        initERC20(1, 0, 0, 0, MAX_SUPPLY, MAX_DAILY_MINT, false);
        address noIdpAdmin = users[0];
        address usedIdp = IDPs[0];

        vm.prank(noIdpAdmin);
        vm.expectRevert("Not IDP admin user!");
        token.removeTrustedIdp(usedIdp);
        assertTrue(token.isTrustedIDP(usedIdp));
    }

    function testRemoveUntrustedIdp() public {
        initERC20(0, 0, 0, 1, MAX_SUPPLY, MAX_DAILY_MINT, false);
        address idpAdmin = idpAdmins[0];
        address noUsedIdp = vm.addr(0x123456);

        vm.prank(idpAdmin);
        vm.expectRevert("IDP not found!");
        token.removeTrustedIdp(noUsedIdp);
        assertFalse(token.isTrustedIDP(noUsedIdp));
    }

    function testManualUserVerification() public {
        initERC20(0, 0, 0, 1, MAX_SUPPLY, MAX_DAILY_MINT, true);
        address idpAdmin = idpAdmins[0];
        address user = vm.addr(0x123456); // some random user with no verification

        assertFalse(token.isUserVerified(user));

        vm.prank(idpAdmin);
        token.manualVerify(user);
        assertTrue(token.isUserVerified(user));
    }

    function testRemoveUserVerification() public {
        initERC20(1, 0, 0, 1, MAX_SUPPLY, MAX_DAILY_MINT, true);
        address idpAdmin = idpAdmins[0];
        address user = users[0]; // default verification

        assertTrue(token.isUserVerified(user));

        vm.prank(idpAdmin);
        token.revokeVerification(user);
        assertFalse(token.isUserVerified(user));
    }

    function testBlockUser() public {
        initERC20(1, 0, 0, 1, MAX_SUPPLY, MAX_DAILY_MINT, true);
        address idpAdmin = idpAdmins[0];
        address user = users[0];

        assertFalse(token.isUserBlocked(user));

        vm.expectEmit();
        emit ExtendedERC20.UserBlocked(user);
        vm.prank(idpAdmin);
        token.blockUser(user);
        assertTrue(token.isUserBlocked(user));
    }

    function testUnblockUser() public {
        initERC20(1, 0, 0, 1, MAX_SUPPLY, MAX_DAILY_MINT, true);
        address idpAdmin = idpAdmins[0];
        address user = users[0];

        vm.expectEmit();
        emit ExtendedERC20.UserBlocked(user);
        vm.prank(idpAdmin);
        token.blockUser(user);
        assertTrue(token.isUserBlocked(user));

        vm.expectEmit();
        emit ExtendedERC20.UserUnblocked(user);
        vm.prank(idpAdmin);
        token.unblockUser(user);
        assertFalse(token.isUserBlocked(user));
    }

    function testRoleVoteNotInRole() public {
        initERC20(2, 0, 0, 0, MAX_SUPPLY, MAX_DAILY_MINT, true);
        address user1 = users[0];
        address user2 = users[1];
        bytes32 role = token.MINTING_ADMIN_ROLE();

        vm.prank(user1);
        vm.expectRevert();
        token.voteOnRoleChange(user2, role, true);
        assertFalse(token.hasRole(role, user2));
    }

    function testRoleVoteNotEnough() public {
        initERC20(1, 4, 0, 0, MAX_SUPPLY, MAX_DAILY_MINT, true);
        address user = users[0];
        address admin1 = admins[0];
        address admin2 = admins[1];
        bytes32 role = token.MINTING_ADMIN_ROLE();

        vm.expectEmit();
        emit ExtendedERC20.NewVote(user, role, true);
        vm.prank(admin1);
        token.voteOnRoleChange(user, role, true);
        vm.prank(admin2);
        token.voteOnRoleChange(user, role, true);
        assertFalse(token.hasRole(role, user));
    }

    function testRoleVoteAddsRole() public {
        initERC20(1, 4, 0, 0, MAX_SUPPLY, MAX_DAILY_MINT, true);
        address user = users[0];
        address admin1 = admins[0];
        address admin2 = admins[1];
        address admin3 = admins[2];
        bytes32 role = token.MINTING_ADMIN_ROLE();

        vm.expectEmit();
        emit ExtendedERC20.NewVote(user, role, true);
        vm.prank(admin1);
        token.voteOnRoleChange(user, role, true);
        vm.prank(admin2);
        token.voteOnRoleChange(user, role, true);
        vm.expectEmit();
        emit ExtendedERC20.VoteExecuted(user, role, true);
        vm.prank(admin3);
        token.voteOnRoleChange(user, role, true);
        assertTrue(token.hasRole(role, user));
    }

    function testRoleVoteRemovesRole() public {
        initERC20(0, 4, 0, 0, MAX_SUPPLY, MAX_DAILY_MINT, true);
        address admin1 = admins[0];
        address admin2 = admins[1];
        address admin3 = admins[2];
        address admin4 = admins[3];
        bytes32 role = token.MINTING_ADMIN_ROLE();

        vm.expectEmit();
        emit ExtendedERC20.NewVote(admin4, role, false);
        vm.prank(admin1);
        token.voteOnRoleChange(admin4, role, false);
        vm.prank(admin2);
        token.voteOnRoleChange(admin4, role, false);
        vm.expectEmit();
        emit ExtendedERC20.VoteExecuted(admin4, role, false);
        vm.prank(admin3);
        token.voteOnRoleChange(admin4, role, false);
        assertFalse(token.hasRole(role, admin4));
    }

    function testRoleVoteDoubleVote() public {
        initERC20(1, 3, 0, 0, MAX_SUPPLY, MAX_DAILY_MINT, true);
        address user = users[0];
        address admin = admins[0];
        bytes32 role = token.MINTING_ADMIN_ROLE();

        vm.expectEmit();
        emit ExtendedERC20.NewVote(user, role, true);
        vm.prank(admin);
        token.voteOnRoleChange(user, role, true);
        vm.prank(admin);
        vm.expectRevert("Already voted");
        token.voteOnRoleChange(user, role, true);
        assertFalse(token.hasRole(role, user));
    }

    function testRoleDeleteAfterExecution() public {
        initERC20(1, 3, 0, 0, MAX_SUPPLY, MAX_DAILY_MINT, true);
        address user = users[0];
        address admin1 = admins[0];
        address admin2 = admins[1];
        bytes32 role = token.MINTING_ADMIN_ROLE();
        bytes32 proposalId = keccak256(abi.encodePacked(user, role, true));

        vm.expectEmit();
        emit ExtendedERC20.NewVote(user, role, true);
        vm.prank(admin1);
        token.voteOnRoleChange(user, role, true);

        (, , , , uint voteCount) = token.roleProposals(proposalId);
        assertEq(voteCount, 1);

        vm.expectEmit();
        emit ExtendedERC20.VoteExecuted(user, role, true);
        vm.prank(admin2);
        token.voteOnRoleChange(user, role, true);
        assertTrue(token.hasRole(role, user)); // user was voted as admin

        // the votes counter is rest
        (, , , , uint votesRst) = token.roleProposals(proposalId);
        assertEq(votesRst, 0);
    }

    /* FUZZING */

    function testFuzzVerifyIdentity(
        address fuzzUser,
        uint fuzzTimestamp
    ) public {
        initERC20(0, 0, 0, 0, MAX_SUPPLY, MAX_DAILY_MINT, false);
        // omit zero address
        vm.assume(fuzzUser != address(0));

        bytes memory signature = _signUser(fuzzUser, fuzzTimestamp);
        vm.prank(fuzzUser);
        try token.verifyIdentity(fuzzTimestamp, signature) {
            (bool wasVerified, , , ) = token.verifiedUsers(fuzzUser);
            assertTrue(wasVerified);
        } catch {
            (bool wasVerified, , , ) = token.verifiedUsers(fuzzUser);
            assertFalse(wasVerified);
        }
    }
}
