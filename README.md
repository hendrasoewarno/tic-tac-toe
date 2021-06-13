# Exercise Smart Contract 101

Create a Tic Tac Toe 
- Welcome action can only be taken by contract account
- create( const name &challenger, name &host );
- close( const name &challenger, const name &host );
- restart( const name &challenger, const name &host, const name &by );
- move( const name &challenger, const name &host, const name &by, const uint16_t &row, const uint16_t &column ); 
- Create an appropriate Ricardian Contract for each of your contract's actions.
- Win/loss tracking and leaderboard 

Require the host’s authorization, and make sure the host and challenger aren’t the same player – and that a game doesn’t already exist between them. Game flow: the host user creates a game – which sets them as host, with the challenger they input  – and then posts a move Submit your expanded Tic Tac Toe smart contract. Include one or more of the following additional features:

    Time limit for moves
    Win/loss tracking and leaderboard
    "Best of" game sets

# Setup

1. Fork this repo to your personal GitHub account so that you can save your work into your personal Github account.

2. Open https://gitpod.io#your_github_repo

## Building sample contract

The source code for the sample smartcontract is at `contract/tictactoe.cpp` within the IDE. To compile the contract, run this in a terminal:

```
cd contract
eosio-cpp tictactoe.cpp

```

This will produce `tictactoe.abi` and `tictactoe.wasm`.

## Installing the contract

Run this in a terminal:

```
rm -rf ~/eosio/chain
nodeos --config-dir ~/eosio/chain/config --data-dir ~/eosio/chain/data -e -p eosio --plugin eosio::chain_api_plugin --contracts-console

cleos create account eosio tictactoe EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos set code tictactoe tictactoe.wasm
cleos set abi tictactoe tictactoe.abi

```

## Creating users and using the contract

Run this in a terminal:
```
cleos create account eosio bob EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio jane EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV

#Exercise 1
#Ok
cleos push action tictactoe welcome '[tictactoe, bob]' -p tictactoe
#Missing required authority
cleos push action tictactoe welcome '[bob, jane]' -p bob

#Exercise 2
#Ok
cleos push action tictactoe create '[jane, bob]' -p bob
cleos get table tictactoe tictactoe game

#game already exists
cleos push action tictactoe create '[jane, bob]' -p bob
#pair already exists
cleos push action tictactoe create '[bob, jane]' -p jane

cleos get table tictactoe tictactoe game

#start game
cleos push action tictactoe restart '[jane, bob, bob]' -p bob

cleos get table tictactoe tictactoe game

#move Ok
cleos push action tictactoe move '[jane, bob, bob, 1,0]' -p bob

#invalid move
cleos push action tictactoe move '[jane, bob, bob, 1,0]' -p bob

#move Ok
cleos push action tictactoe move '[jane, bob, jane, 0,0]' -p jane
cleos push action tictactoe move '[jane, bob, bob, 1,1]' -p bob
cleos push action tictactoe move '[jane, bob, jane, 0,1]' -p jane
cleos push action tictactoe move '[jane, bob, bob, 1,2]' -p bob

#game over!
cleos push action tictactoe move '[jane, bob, jane, 0,2]' -p jane

cleos get table tictactoe tictactoe leader

#Ok
cleos push action tictactoe close '[jane, bob]' -p bob
or
cleos push action tictactoe close '[bob, jane]' -p jane
#pair not found
cleos push action tictactoe close '[jane, bob]' -p bob

```
