// SPDX-License-Identifier: MIT
pragma solidity ^0.8.28;

// Import the ERC20 from OpenZeppelin
import {ERC20} from "@openzeppelin/contracts/token/ERC20/ERC20.sol";
import {AccessControlEnumerable} from "@openzeppelin/contracts/access/extensions/AccessControlEnumerable.sol";
import {ECDSA} from "@openzeppelin/contracts/utils/cryptography/ECDSA.sol";
import {MessageHashUtils} from "@openzeppelin/contracts/utils/cryptography/MessageHashUtils.sol";
import {Strings} from "@openzeppelin/contracts/utils/Strings.sol";

// Contract extending ERC20 toke, limits the maximum amount of tokens in circulation
contract ExtendedERC20 is ERC20, AccessControlEnumerable {
    event TokensMinted(address recipient, uint amount);
    event TokensTransferred(address sender, address recipient, uint amount);
    event TransferRestrictionCreated(address user, uint newLimit);
    event UserBlocked(address user);
    event UserUnblocked(address user);
    event NewVote(address target, bytes32 role, bool isAdd);
    event VoteExecuted(address target, bytes32 role, bool isAdd);

    uint public immutable maxSupply; /// maximum amount of tokens in circulation
    uint public immutable maxDailyMint; /// maximum amount of tokens minted in one day

    uint public lastMintedDay; /// timestamp for checking amount of tokens minted in one day
    uint public mintedToday; /// amount of minted tokens in current day, The reset is based on UTC 00:00, not 24h since last mint!

    /// Role for minting admin - only minting admin can mint new tokens
    bytes32 public constant MINTING_ADMIN_ROLE =
        keccak256("MINTING_ADMIN_ROLE");

    /// Role for restriction admin - restriction admin can set daily limits to specific addresses
    bytes32 public constant RESTR_ADMIN_ROLE = keccak256("RESTR_ADMIN_ROLE");

    /// Role for identity providers - identity providers can add/remove centralized IDP
    bytes32 public constant IDENTITY_PROVIDER_ADMIN_ROLE =
        keccak256("IDENTITY_PROVIDER_ADMIN_ROLE");

    /// Tranfer limit info
    struct TransferLimit {
        uint maxDaily;
        uint transferredToday;
        uint lastTransferDay;
        bool isSet; // tells if the limitInfo was set or not
    }

    /// Tracking transfer limits of users
    mapping(address => TransferLimit) public transferLimits;

    /// Verification info
    struct Verification {
        bool wasVerified; // if true, user was ever verified and can own tokens
        bool isVerified; // if true, user verification is up to date
        bool isBlocked; // if true, user has blocked account
        uint timestamp; // timestamp of the last verification, this is also used to check if verification is up to date
    }

    /// Tracking verified users (including all roles)
    mapping(address => Verification) public verifiedUsers;
    /// Tracking centralized identity providers
    mapping(address => bool) public isTrustedIDP;

    /// Number of hours for verification to expire
    uint public expirationTime; // in hours

    /// Role vote proposal
    struct RoleProposal {
        bool initialized; //
        address targetUser; // User on which proposal is aimed
        bytes32 role; // Role that user may gain / lose
        bool isAdd; // if true user is gonna gain role, else user gonna lose role
        uint voteCount; // Number of votes
        mapping(address => bool) votes; // votes given by other users of the same role
    }

    /// Mapping of role proposals
    mapping(bytes32 => RoleProposal) public roleProposals;

    // ===================
    //      Modifiers
    // ===================

    /// @dev Check if maximal supplay wasn't exceeded
    modifier checkMaxSupply(uint amount) {
        if (totalSupply() + amount > maxSupply) {
            revert("Max supply exceeded!");
        }
        _;
    }

    /// @dev Check if user has Minting Admin role
    modifier isMintingAdmin(address user) {
        if (!hasRole(MINTING_ADMIN_ROLE, user)) {
            revert("Not minting user!");
        }
        _;
    }

    /// @dev Check if user has RestrAdmin role
    modifier isRestrAdmin(address user) {
        if (!hasRole(RESTR_ADMIN_ROLE, user)) {
            revert("Not restriction user!");
        }
        _;
    }

    /// @dev Just check if the given user was ever verified and can own tokens
    modifier wasVerified(address user) {
        if (user == address(0)) {
            revert("Zero address was not verified!");
        }

        Verification memory verificationInfo = verifiedUsers[user];

        if (!verificationInfo.wasVerified) {
            revert("User was never verified!");
        }
        _;
    }

    /// @dev Check if user is currently verified and can send tokens
    modifier isVerified(address user) {
        /// Note: address(0) does not pass the `_update()` function
        /// Because of minting this has to pass the `isVerified()` check
        if (user != address(0)) {
            Verification memory verificationInfo = verifiedUsers[user];

            if (!verificationInfo.wasVerified) {
                revert("User was never verified!");
            } else if (!verificationInfo.isVerified) {
                revert("User is not verified!");
            } else if (verificationInfo.isBlocked) {
                revert("User is blocked!");
            } else if (
                block.timestamp >
                verificationInfo.timestamp + expirationTime * 1 hours
            ) {
                revert("User verification expired!");
            }
        }
        _;
    }

    modifier isIdpAdmin(address user) {
        if (!hasRole(IDENTITY_PROVIDER_ADMIN_ROLE, user)) {
            revert("Not IDP admin user!");
        }
        _;
    }

    // ======================================================
    // Public functions callable from outside of the contract
    // ======================================================

    constructor(
        string memory name,
        string memory symbol,
        uint _maxSupply,
        uint _maxDailyMint,
        address[] memory initialMintingAdmins,
        address[] memory initialRestrAdmins,
        address[] memory initialIDP,
        address[] memory initialIdpAdmins,
        uint _expirationTime
    ) ERC20(name, symbol) {
        maxSupply = _maxSupply;
        maxDailyMint = _maxDailyMint;
        expirationTime = _expirationTime;

        // Grant user roles
        for (uint i = 0; i < initialMintingAdmins.length; ++i) {
            _grantRole(MINTING_ADMIN_ROLE, initialMintingAdmins[i]);
        }

        for (uint i = 0; i < initialRestrAdmins.length; ++i) {
            _grantRole(RESTR_ADMIN_ROLE, initialRestrAdmins[i]);
        }

        for (uint i = 0; i < initialIDP.length; ++i) {
            isTrustedIDP[initialIDP[i]] = true;
        }

        for (uint i = 0; i < initialIdpAdmins.length; ++i) {
            _grantRole(IDENTITY_PROVIDER_ADMIN_ROLE, initialIdpAdmins[i]);
        }
    }

    /// @dev Minting function, restricted to admin role
    /// @param to Receiver address must be verified in the past
    /// @param amount Amount of minted tokens
    function mint(
        address to,
        uint amount
    ) public checkMaxSupply(amount) isMintingAdmin(msg.sender) wasVerified(to) {
        // Timestamp reset
        uint today = block.timestamp - (block.timestamp % 1 days); // UTC midnight alignment
        if (today - lastMintedDay >= 1 days) {
            lastMintedDay = today;
            mintedToday = 0;
        }

        // Can not be as modifier, because it depends on the previous reset
        require(
            mintedToday + amount <= maxDailyMint,
            "Minting level reached for today!"
        );

        mintedToday += amount;
        _mint(to, amount);
        emit TokensMinted(to, amount);
    }

    /// @dev Set daily limit on transfers for a user
    /// @param user for wich the limit will be set up
    /// @param limit limit that will be set
    function setTransferDailyLimit(
        address user,
        uint limit
    ) public isRestrAdmin(msg.sender) {
        TransferLimit storage limitInfo = transferLimits[user];
        limitInfo.isSet = true;
        limitInfo.maxDaily = limit;
        emit TransferRestrictionCreated(user, limit);
    }

    /// @dev Verify identity of the message sender with central IDP signature
    /// @param timestamp of the veridication by IDP
    /// @param signature of the central IDP
    function verifyIdentity(uint timestamp, bytes calldata signature) public {
        string memory message = string(
            abi.encodePacked(
                "User with address ",
                Strings.toHexString(msg.sender),
                " has verified their identity at ",
                Strings.toString(timestamp)
            )
        );

        bytes32 messageHash = keccak256(bytes(message));
        bytes32 ethSignedHash = MessageHashUtils.toEthSignedMessageHash(
            messageHash
        );
        address signer = ECDSA.recover(ethSignedHash, signature);
        require(
            isTrustedIDP[signer],
            "Signer not trusted IDP or invalid signature!"
        );
        verifiedUsers[msg.sender] = Verification(
            true, // Verification provided
            (block.timestamp <= timestamp + expirationTime * 1 hours), // If timestamp is good, user is also currently verified
            false, // User is not blocked
            timestamp // Save the last timestamp
        );
    }

    /// @dev Add trusted centralized IDP, restricted to IdpAdmin role
    /// @param newIdp that will be added
    function addTrustedIdp(address newIdp) public isIdpAdmin(msg.sender) {
        require(newIdp != address(0), "Zero address can not be IDP!");
        isTrustedIDP[newIdp] = true;
    }

    /// @dev Remove currently trusted IDP, restricted to IdpAmin role
    /// @param idp that will be removed
    function removeTrustedIdp(address idp) public isIdpAdmin(msg.sender) {
        require(isTrustedIDP[idp], "IDP not found!");
        isTrustedIDP[idp] = false;
    }

    /// @dev Manual verification of a user, restricted to IdpAdmin role
    /// @param user that will be verified
    function manualVerify(address user) public isIdpAdmin(msg.sender) {
        Verification memory verificationInfo = verifiedUsers[user];

        verifiedUsers[user] = Verification(
            true, // Verification provided / stays the same
            true, // Currently verified
            verificationInfo.isBlocked, // Stays the same
            block.timestamp // Save the new timestamp
        );
    }

    /// @dev Revokation of user verification, restricted to IdpAdmin role
    /// @param user that verification will be revoked
    function revokeVerification(address user) public isIdpAdmin(msg.sender) {
        Verification memory verificationInfo = verifiedUsers[user];
        if (verificationInfo.wasVerified) {
            verifiedUsers[user] = Verification(
                true, // Stays the same
                false, // Remove verification
                verificationInfo.isBlocked, // Stays the same
                verificationInfo.timestamp // Stays the same
            );
        }
    }

    /// @dev Block of a user, the user will net be able to manipulate his tokens until unblocking
    /// restricted to IdpAdmin role
    /// @param user that will be blocked
    function blockUser(address user) public isIdpAdmin(msg.sender) {
        Verification memory verificationInfo = verifiedUsers[user];
        if (verificationInfo.wasVerified) {
            verifiedUsers[user] = Verification(
                true, // Stays the same
                verificationInfo.isVerified, // Stays the same
                true, // Apply block
                verificationInfo.timestamp // Stays the same
            );
            emit UserBlocked(user);
        }
    }

    /// @dev Unblock of a user, restricted to IdpAdmin role
    /// @param user that will be unblocked
    function unblockUser(address user) public isIdpAdmin(msg.sender) {
        Verification memory verificationInfo = verifiedUsers[user];
        if (verificationInfo.wasVerified) {
            verifiedUsers[user] = Verification(
                true, // Stays the same
                verificationInfo.isVerified, // Stays the same
                false, // Remove block
                verificationInfo.timestamp // Stays the same
            );
            emit UserUnblocked(user);
        }
    }

    /// @dev Users with a specific role can vote other users to optain
    /// the some role or lose this role
    /// @param targetUser on which the voting is aimed
    /// @param role that will be gaind / lost
    /// @param isAdd determines if role will be gaind (added, true) or lost (removed, false)
    function voteOnRoleChange(
        address targetUser,
        bytes32 role,
        bool isAdd
    ) public onlyRole(role) {
        // If user already has the role and voting is for adding it OR
        // user does not have the role and voting is for removing it, then skip
        if (
            (hasRole(role, targetUser) && isAdd) ||
            (!hasRole(role, targetUser) && !isAdd)
        ) {
            return;
        }

        // Create has of the proposal as identifier
        bytes32 proposalId = keccak256(
            abi.encodePacked(targetUser, role, isAdd)
        );
        RoleProposal storage proposal = roleProposals[proposalId];

        // If proposal does not exists initialize new
        if (!proposal.initialized) {
            proposal.initialized = true;
            proposal.role = role;
            proposal.targetUser = targetUser;
            proposal.isAdd = isAdd;

            emit NewVote(targetUser, role, isAdd);
        }

        // Assign new vote
        require(!proposal.votes[msg.sender], "Already voted");
        proposal.votes[msg.sender] = true;
        proposal.voteCount++;

        // Evaluate the voting
        if (proposal.voteCount > (getRoleMemberCount(role) / 2)) {
            if (isAdd) {
                _grantRole(role, targetUser);
            } else {
                _revokeRole(role, targetUser);
            }

            delete roleProposals[proposalId];
            emit VoteExecuted(targetUser, role, isAdd);
        }
    }

    // ================================================================
    // Internal functions called only from inside of the smart contract
    // ================================================================

    /// @dev Function executed before transfer, checks if sender is verified and reciever
    /// has to be verified in the past (not necessary currently verified)
    /// @param from sender address
    /// @param to receiver address
    /// @param amount that will be transfered
    function _update(
        address from,
        address to,
        uint256 amount
    ) internal virtual override isVerified(from) wasVerified(to) {
        if (from != address(0)) {
            TransferLimit storage limitInfo = transferLimits[from];

            if (limitInfo.isSet) {
                uint today = block.timestamp - (block.timestamp % 1 days);
                if (today - limitInfo.lastTransferDay >= 1 days) {
                    limitInfo.lastTransferDay = today;
                    limitInfo.transferredToday = 0;
                }

                require(
                    limitInfo.transferredToday + amount <= limitInfo.maxDaily,
                    "Transfer exceeds daily limit!"
                );

                limitInfo.transferredToday += amount;
            }
        }
        emit TokensTransferred(from, to, amount);
        super._update(from, to, amount);
    }

    // ===========================================================
    // Public functions for DAPP client (i.e., simulated by tests)
    // ===========================================================

    /// @dev Check if a user is currently verified
    /// @param user that will be checked
    /// @return bool true if is currently verified else false
    function isUserVerified(address user) public view returns (bool) {
        if (user == address(0)) {
            return false;
        }

        Verification memory verificationInfo = verifiedUsers[user];

        if (
            !verificationInfo.wasVerified ||
            !verificationInfo.isVerified ||
            verificationInfo.isBlocked ||
            (block.timestamp >
                verificationInfo.timestamp + expirationTime * 1 hours)
        ) {
            return false;
        }

        return true;
    }

    /// @dev Check if a user is currently blocked
    /// @param user that will be checked
    function isUserBlocked(address user) public view returns (bool) {
        Verification memory verificationInfo = verifiedUsers[user];
        if (verificationInfo.isBlocked) {
            return true;
        }
        return false;
    }
}
