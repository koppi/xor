Symmetric xor encryption / decryption [![Build Status](https://travis-ci.com/koppi/xor.svg?branch=master)](https://travis-ci.com/koppi/xor)

### compile and install

```bash
git clone https://github.com/koppi/xor
cd xor
sudo make install
```

### usage

```bash
to encrypt, run: 'xor -e abc.key -i abc.txt -o abc.enc'
to decrypt, run: 'xor -d abc.key -i abc.enc -o abc.txt'
```

* abc.txt is the plaintext file.
* abc.key is the randomly generated symmetric xor key.
* abc.enc is the encrypted file.

### hints

* to speed up random xor key generation install haveged

```bash
sudo apt-get -y install haveged
```
