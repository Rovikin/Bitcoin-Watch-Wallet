# Bitcoin-Watch-Wallet
Creating a watch wallet using python
Instead of importing a seed phrase which actually increases the vulnerability of your wallet when there is an unintentional oversight, this wallet only reads the address that has been entered in "wallet.txt"

## Features
available features: mempool, balance in smallest unit (sat/satoshi), balance in Rupiah format, mempool fee rates, Bitcoin fear and greed index, exchange rate in USD
## Termux Installation
follow the command line below:
```
pkg update && pkg upgrade -y && pkg install python -y && pip install requests
pkg install git
git clone https://github.com/Rovikin/Bitcoin-Watch-Wallet.git
cd Bitcoin-Watch-Wallet
python btc.py
```
## Ubuntu
For Ubuntu, you can simply replace the "pkg" command with the "sudo apt" or "apt" command if you are already a root user.

## How to run?
you just run this command:
```
python btc.py
```
For the wallet.txt file, you can replace it with the address you want. You can fill in 1 or as many addresses as you want.
This wallet will add up all the balances you input in the wallet.txt file.
The wallet.txt file I included is just an example.
