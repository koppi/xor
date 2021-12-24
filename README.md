# XOR cipher

A C implementation of the [simple XOR cipher](https://en.wikipedia.org/wiki/XOR_cipher) for encrypring and decrypting arbitrary files.

[![Makefile CI](https://github.com/koppi/xor/actions/workflows/makefile.yml/badge.svg)](https://github.com/koppi/xor/actions/workflows/makefile.yml)

## Clone, compile and install

```bash
git clone https://github.com/koppi/xor && sudo make -C xor install
```

or

```bash
curl -L 'https://github.com/koppi/xor/raw/master/xor.c' | gcc -O2 -x c -o /usr/local/bin/xor -
```

## Usage

```bash
xor -e abc.key -i abc.txt -o abc.enc # to encrypt
xor -d abc.key -i abc.enc -o abc.txt # to decrypt
```

where:

* ```abc.txt``` is the plaintext file,
* ```abc.key``` is the single-use pre-shared key,
* ```abc.enc``` is the encrypted file.

## Hints

* to speed up the key stream generation of the Linux pseudo-random number generator, install ```haveged```:

```bash
sudo apt -y install haveged
```

With a key that is truly random, the result is a one-time pad, which is unbreakable in theory.

## Author

* **Jakob Flierl** - [koppi](https://github.com/koppi)

