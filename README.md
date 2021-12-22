# XOR cipher

[Symmetric XOR encryption / decryption](https://en.wikipedia.org/wiki/XOR_cipher) tool in C.

[![Makefile CI](https://github.com/koppi/xor/actions/workflows/makefile.yml/badge.svg)](https://github.com/koppi/xor/actions/workflows/makefile.yml)

### compile and install

```bash
git clone https://github.com/koppi/xor && sudo make -C xor install
```

or

```bash
curl -L 'https://github.com/koppi/xor/raw/master/xor.c' | gcc -O2 -x c -o /usr/local/bin/xor -
```

### Usage

```bash
xor -e abc.key -i abc.txt -o abc.enc # to encrypt
xor -d abc.key -i abc.enc -o abc.txt # to decrypt
```

where:

* abc.txt is the plaintext file.
* abc.key is the randomly generated symmetric XOR key.
* abc.enc is the encrypted file.

### Hints

* to speed up random XOR key generation on Linux, install haveged:

```bash
sudo apt-get -y install haveged
```

* make sure, that the XOR key generation is truely random.

### Author

* **Jakob Flierl** - [koppi](https://github.com/koppi)

