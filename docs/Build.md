# Build

### Pre requirements

- CMake >= 3.7.1
- C++ compiler which supports C++17 (Preferably, Clang)
- Android NDK

### Build steps

```bash
$ ./gradlew :library:build
```

And you got yourself two AAR-files in `library/build/outputs/aar`.