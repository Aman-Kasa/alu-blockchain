# Blockchain CLI wallet

An interactive command-line wallet built on top of the blockchain library from
the previous projects (`crypto/` and `blockchain/v0.3/`). It lets you create a
wallet, mine blocks to earn coins, send coins to another wallet through a
transaction pool, check your balance, and save/load both the wallet and the
blockchain to disk.

## Build

```sh
cd cli
make
```

`make` first builds the two libraries the CLI depends on
(`crypto/libhblk_crypto.a` and `blockchain/v0.3/libhblk_blockchain.a`), then
links the `cli` program. It needs OpenSSL and the `llist` library.

## Commands

| Command | Description |
| --- | --- |
| `address` | Show the public address of the current wallet (130 hex characters) |
| `info` | Show blocks, unspent outputs, pending transactions, total coins and **your balance** |
| `mine` | Mine a block. Includes every valid pending transaction and pays you the 50-coin reward |
| `send AMOUNT ADDRESS` | Send coins to an address. The transaction waits in the pool until the next `mine` |
| `save PATH` / `load PATH` | Save / load the blockchain to / from a file |
| `wallet_save DIR` / `wallet_load DIR` | Save / load the wallet keys (`key.pem`, `key_pub.pem`) |
| `help [COMMAND]` | List commands, or show help for one |
| `exit` | Quit (Ctrl-D also works) |

A new wallet is created automatically when the CLI starts.

## Example: Alice pays Bob

**1. Bob creates a wallet and shares his address**

```
$ ./cli
> wallet_save bob
Saved wallet to bob
> address
040b47ac9f4e5d098c5943be66084a3ec96dd28ff586128fbe944c739c9839725bc57434a1708630e63fbb8e8416473707f358be36cf6432fecbbaad365e396eee
> exit
```

**2. Alice mines some coins, sends 30 to Bob, and mines a block to confirm it**

```
$ ./cli
> wallet_save alice
Saved wallet to alice
> mine
Successfully mined a block
> mine
Successfully mined a block
> send 30 040b47ac9f4e5d...396eee
Transaction added to local transaction pool
> info
Blocks:  3
UTXOs:   2
TX Pool: 1
Coins:   100
Balance: 100
> mine
Successfully mined a block
> info
Blocks:  4
UTXOs:   3
TX Pool: 0
Coins:   150
Balance: 120
> save chain.hblk
Saved blockchain to chain.hblk
> exit
```

Alice had 100 coins, sent 30 to Bob and earned 50 for mining the block that
confirmed the transfer: 100 - 30 + 50 = **120**.

**3. Bob loads the chain and sees the coins**

```
$ ./cli
> wallet_load bob
Loaded wallet from bob
> load chain.hblk
Loaded blockchain from chain.hblk
> info
Blocks:  4
UTXOs:   3
TX Pool: 0
Coins:   150
Balance: 30
```

## How it works

- **Wallet**: an EC key pair on the `secp256k1` curve. The address is the
  uncompressed public key.
- **Sending**: `send` picks unspent outputs owned by your wallet, creates a
  transaction (with change sent back to you), signs every input with your
  private key, checks it is valid, and adds it to the local transaction pool.
- **Mining**: `mine` drops any pool transaction that is no longer valid,
  puts a coinbase transaction (the reward) plus the pool into a new block,
  adjusts the difficulty, finds a nonce by proof of work, validates the
  block, then updates the list of unspent outputs.
- **Balance**: the sum of the unspent outputs locked to your public key.
  `Coins` is the total supply in circulation.
- **Safety checks**: invalid amounts, malformed addresses, overspending and
  double spending are all rejected.

## Files

| File | Role |
| --- | --- |
| `cli.c` | Prompt loop, tokenizer and command dispatch |
| `cli.h` | Shared state, command table entries and prototypes |
| `get_commands.c`, `find_command.c` | Command table and lookup |
| `cli_address.c`, `cli_info.c` | Wallet address and blockchain/balance summary |
| `cli_send.c`, `cli_mine.c` | Transactions and mining |
| `cli_save.c`, `cli_load.c` | Blockchain persistence |
| `cli_wallet_save.c`, `cli_wallet_load.c` | Wallet persistence |
| `state_init.c`, `state_clear.c` | Set up and free the CLI state |
