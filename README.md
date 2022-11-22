# fwoop

## Building
Requires `gtest` to build unit tests.

```sh
mkdir build
cd build
# Build with OpenSSL
cmake -DUSE_OPENSSL=1 ..
# Or build with fwoop crypto
cmake -DUSE_OPENSSL=0 ..
cmake --build .
```

### Testing
```sh
cd build
ctest
```

### Documenting
```sh
doxygen
python3.9 -m sphinx -b html docs/source docs/html
```

## Installing
```sh
cd build
sudo `which cmake` --install .
```

### Uninstalling
```sh
sudo xargs rm <install_manifest.txt
```
