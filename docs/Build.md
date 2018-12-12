## Build

### Pre requirements:

- CMake >= 3.7.1
- C++ compiler which supports C++17 (Preferably, Clang)
- Android NDK

### Build steps:

```bash
// Clone repository with dependencies
$ git clone https://gitlab.com/barsoosayque/libgdx-oboe.git --recurse-submodules

// Cd in and build
$ cd libgdx-oboe
$ ./gradlew :library:build
```

And you got yourself two AAR-files in `library/build/outputs/aar`.