# fwoop

## Building
Requires `gtest` to build unit tests. Requires [Botan](https://botan.randombit.net/) for crypto.

```sh
mkdir build
cmake  build/
cmake --build build/ -j4
```

### Testing
```sh
ctest --test-dir build/
```

### Documenting
```sh
doxygen
python3.9 -m sphinx -b html docs/source docs/html
```

## Installing
```sh
sudo `which cmake` --install build/
```

### Uninstalling
```sh
cd build
sudo xargs rm <install_manifest.txt
```
