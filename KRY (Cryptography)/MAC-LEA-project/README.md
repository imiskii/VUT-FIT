# SHA-256 & Length Extension Attack

This program can create SHA-256 hash of a message, create a naive Message Authentication Code (MAC), and also perform
length extension attack on created MAC.

## Implementation

The SHA-256 algorithm is implemented according to the NIST standard _http://dx.doi.org/10.6028/NIST.FIPS.180-4_. The naive MAC is computed as _MAC = (SECRET_KEY + MESSAGE)_. Where `+` is the concatenation. There are four main objects:

### Objects:

+ `word` - Wrapper around `std::uint32_t`. It is used in hash computation.
+ `bit256_t` - Representation of 256-bit long buffer for storing checksums (hashes) and MACs.
+ `sha256` - Object with implementation of SHA-256.
+ `LEA` - Length Extension Attack object which is derived from `sha256` object and it is adjusted to perform attack on MAC.


### Files:

|**File** | **Content**|
|---|---|
| _argparser.cpp/hpp_ | Implementation for processing program arguments. |
| _utils.cpp/hpp_ | Help function and classes as `word` and `bit256_t`, etc. |
| _sha256.cpp/hpp_ | Implementation of SHA-256 |
| _lea.cpp/hpp_ | Implementation of length extension attack |
| _kry.cpp/hpp_ | The main function |


## How to Use

### Compilation

With provided Makefile it is enough to execute `make` command. For compilation is used **gcc** compiler with flags `-pedantic`, `-Wall`, `-Wextra`. There are also leak and address sanitezers `-fsanitize=address,leak`. Additional make commands are:

+ `make clean` - delete all temporary files
+ `make zip` - pack source files with the Makefile and README

### Run

Program help can be printed with the single `-h` option or with executing the program without any options.

+ `./kry -h`
+ `./kry`

The program reads a message from STDIN and can be executed with one of the following switches. Each switch changes the functionality of the application:

+ `-c` Computes and prints the SHA-256 checksum of the input message to STDOUT.
+ `-s` Computes a MAC for the input message and prints the result to STDOUT. Must be used with the `-k` parameter.
+ `-v` Verifies the MAC for a given key and input message. The program returns 0 in case of a valid MAC, otherwise 1. Must be used in combination with the `-k` and `-m` parameters.
+ `-e` Performs a length extension attack on the MAC and input message. Must be used in combination with the `-m`, `-n`, and `-a` parameters. It prints the recomputed checksum (hash) and the new message to STDOUT.

The program may require additional parameters depending on the selected functionality. These parameters specify additional input information:

+ `-k` <KEY> Specifies the secret key for MAC calculation.
+ `-m` <CHS> Specifies the MAC of the input message for its verification or attack.
+ `-n` <NUM> Specifies the length of the secret key (necessary for performing the attack).
+ `-a` <MSG> Specifies the extension of the input message for performing the attack.

## Examples 

**Computing SHA-256 checksum:**
```
~> echo -ne "zprava" | ./kry -c
d8305a064cd0f827df85ae5a7732bf25d578b746b8434871704e98cde3208ddf
```

**Computing MAC:**
```
~> echo -ne "zprava" | ./kry -s -k heslo
23158796a45a9392951d9a72dffd6a539b14a07832390b937b94a80ddb6dc18e
```

**Verifying MAC:**
```
~> echo -ne "zprava" | ./kry -v -k heslo -m 23158796a45a9392951d9a72dffd6a539b14a07832390b937b94a80ddb6dc18e
~> echo $?
0
```

```
~> echo -ne "message" | ./kry -v -k heslo -m 23158796a45a9392951d9a72dffd6a539b14a07832390b937b94a80ddb6dc18e
~> echo $?
1
```

**Length extension attack:**
```
~> echo -ne "zprava" | ./kry -e -n 5 -a ==message -m 23158796a45a9392951d9a72dffd6a539b14a07832390b937b94a80ddb6dc18ea3b205a7ebb070c26910e1028322e99b35e846d5db399aae295082ddecf3edd3
zprava\x80\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x
00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x
00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x
00\x00\x00\x00\x00\x00\x00\x58==message
```