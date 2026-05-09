A C implementation of the [simple XOR cipher](https://en.wikipedia.org/wiki/XOR_cipher) for encrypting and decrypting arbitrary files.

## Clone, compile and install

```bash
git clone https://github.com/koppi/xor && sudo make -C xor release install
```

(Note: using simply `make` instead of `make release` will produce debug versions of the tools with the address sanitizer and undefined behavior sanitizer enabled.)

or

```bash
curl -L 'https://github.com/koppi/xor/raw/master/xor.c' | gcc -O3 -x c -o /usr/local/bin/xor -
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

## Hint

To speed up the key stream generation of the Linux pseudo-random number generator, install ```haveged```:

```bash
sudo apt -y install haveged
```

With a key that is truly random, the result is a one-time pad, which is unbreakable in theory.

## Author

* **Jakob Flierl** - [koppi](https://github.com/koppi)

