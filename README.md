# Bitcoin-Watch-Wallet
Creating a watch wallet using python
Instead of importing a seed phrase which actually increases the vulnerability of your wallet when there is an unintentional oversight, this wallet only reads the address that has been entered in "wallet.txt"

## Features
available features: balance in smallest unit (sat/satoshi), balance in Rupiah format, mempool fee rates, Bitcoin fear and greed index, exchange rate in USD
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
## installation for C++
In my repository I added for C++ version. You can compile it yourself with the following command:

```
g++ -std=c++17 -o btc btc.cpp -lcurl
```

If it doesn't work, please move btc.cpp and json.hpp to the home directory with the command:

```
cp -rf btc.cpp json.hpp ~/
g++ -std=c++17 -o btc btc.cpp -lcurl
```

Make sure you are still in the project directory.

```
cd Bitcoin-Watch-Wallet
```
Or if you don't want to compile, you can directly use the executable with the command:

```
./btc
```
this can only run on termux armeabi-v7a (arm 32 bit). For universal, please compile it with the command above.
