# fwoop

## Building
Requires `gtest` to build unit tests.

```sh
mkdir build
cd build
cmake ../
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
