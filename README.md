# Exercise

Create a Tic Tac Toe Smart Contract

- For now, one single action:
welcome (name host, name opponent)

- Welcome action can only be taken by contract account

- welcome only prints a simple message:
"Welcome, <host> and <opponent>!"

# Setup

1. Fork this repo to your personal GitHub account so that you can save your work into your personal Github account.

2. Point your browser to the following URL https://gitpod.io/#https://github.com/your-github-account/tic-tac-toe to start the IDE. You will be automatically prompted to create a Gitpod account (all types of Gitpod accounts (including free) will work). You can also choose to provide multiple developers push access to your personal github fork of this repo to collaborate with them (one developer working on the smart contract (C++) while the other working on the front-end decentralized application (EOSJS), etc.). Each such developer sharing access to the forked repo will get their own copy of the EOSIO blockchain components to enable independent development.

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
cleos create account eosio tictactoe EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos set code tictactoe tictactoe.wasm
cleos set abi tictactoe tictactoe.abi

```

## Creating users and using the contract

Run this in a terminal:
```
cleos create account eosio bob EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio jane EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
#Ok
cleos push action tictactoe welcome '[tictactoe, bob]' -p tictactoe
#Missing required authority
cleos push action tictactoe welcome '[bob, jane]' -p bob

```

## Viewing the front-end decentralized web app (EOSJS):

The source code for the React WebApp is at `webapp/src/index.tsx` within the IDE. To preview the WebApp run this in a terminal:

```
gp preview $(gp url 8000)

```