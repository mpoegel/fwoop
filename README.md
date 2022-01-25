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

## Installing
```sh
cd build
sudo `which cmake` --install .
```

### Uninstalling
```sh
sudo xargs rm <install_manifest.txt
```
