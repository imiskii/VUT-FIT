

.PHONY: network deploy test

network:
	anvil


deploy:
	forge script script/ExtendedERC20.s.sol:ExtendedERC20Script --rpc-url http://127.0.0.1:8545 --private-key 0xac0974bec39a17e36ba4a6b4d238ff944bacb478cbed5efcae784d7bf4f2ff80 --broadcast


test:
	forge test