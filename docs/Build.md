## Build

### Pre requirements:

- CMake >= 3.7.1
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

## Libav

Shared library files for Libav are distributed prebuilt, so this section is mostly for the future development of the library.

Note, that building those \*.so files only works on Unix systems because of the *configure* scripts.
There is no point to use some cross-platform tools to build the library, therefore you can just run shell script to build the library:

```bash
$ cd library
$ sh build_libav.sh
```

Hopefully, this will build you a bunch of new \*.so libs in `build/libs`.
If you want to update library `libs` directory with prebuilt libraries, you can pass `--update` flag to the build script after you performed a build.

A few notes:
- Requires NDK >= r19 as it is uses it's clang toolchains.
- Environmental variable `ANDROID_HOME` should be defined and must be absolute.
- Scrip also support `--clear` flag to wipe any build folders before starting a build.