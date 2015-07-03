secure xor encryption for dummies

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
