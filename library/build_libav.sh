#!/usr/bin/sh

# =============== Build definitions ===============

HOST_NAME=$(uname -s | tr '[:upper:]' '[:lower:]')
HOST_ARCH=$(uname -m)
HOST_TAG="$HOST_NAME-$HOST_ARCH"

# Directories
LIBMP3LAME_ROOT="build/libmp3lame"
LIBOGG_ROOT="build/libogg"
LIBWAVPACK_ROOT="build/libwavpack"
LIBVORBIS_ROOT="build/libvorbis"
LIBAV_ROOT="dependencies/libav"
BUILD_ROOT="$(pwd)/build/libs"
NDK="$ANDROID_HOME/ndk-bundle"
TOOLCHAIN="$NDK/toolchains/llvm/prebuilt/$HOST_TAG"

# Consult library/build.gradle for those options
# android.defaultConfig.ndk.abiFilters
ABI_FILTERS="x86 x86_64 armeabi-v7a arm64-v8a"
# android.defaultConfig.minSdkVersion
MIN_SDK_VERSION="16"

# Flags
LIBAV_FLAGS="
--logfile=build.log
--enable-cross-compile
--target-os=android
--enable-shared
--disable-static
--disable-pthreads
--disable-programs
--disable-doc
--disable-avdevice
--disable-avformat
--disable-avfilter
--disable-avresample
--disable-network
--disable-swscale
--disable-everything
--enable-libmp3lame
--enable-libvorbis
--enable-libwavpack
"

# =============== Option handle ==============
while test $# -gt 0; do
    case "$1" in
        --help|-h)
            echo "build_libav.sh -- script to build libav for android with support for mp3, wav and ogg."
            echo "usage: build_libav.sh (-h|--help) (--update) (--clear)"
            echo "options:"
            echo "    -h, --help:  print this message and exit."
            echo "    --update:    copy built libraries to ./libs (this option assume that the library is built)."
            echo "    --clear:     clear build and temporary directories before any action."
            exit 0;
            ;;
        --update)
            for ABI in $ABI_FILTERS; do
                DIR="$(pwd)/libs/$ABI"
                mkdir -p "$DIR"
                cp -t "$DIR" \
                $BUILD_ROOT/$ABI/lib/libavcodec.so \
                $BUILD_ROOT/$ABI/lib/libavutil.so \
                $BUILD_ROOT/$ABI/lib/libmp3lame.so \
                $BUILD_ROOT/$ABI/lib/libogg.so \
                $BUILD_ROOT/$ABI/lib/libvorbis.so \
                $BUILD_ROOT/$ABI/lib/libwavpack.so
            done
            exit 0;
            ;;
        --clear)
            rm -r "$BUILD_ROOT"
            rm -r "$LIBMP3LAME_ROOT"
            rm -r "$LIBOGG_ROOT"
            rm -r "$LIBWAVPACK_ROOT"
            rm -r "$LIBVORBIS_ROOT"
            ;;
    esac
    shift
done

# =============== Actual build ===============

echo "*************** Prepare dependepcies ***************"

if [ ! -e $LIBMP3LAME_ROOT ]; then
    curl -L "https://netcologne.dl.sourceforge.net/project/lame/lame/3.100/lame-3.100.tar.gz" | tar xz
    mv lame-3.100 "$LIBMP3LAME_ROOT"
fi

if [ ! -e $LIBOGG_ROOT ]; then
    curl -L "http://downloads.xiph.org/releases/ogg/libogg-1.3.4.tar.gz" | tar xz
    mv libogg-1.3.4 "$LIBOGG_ROOT"
fi

if [ ! -e $LIBVORBIS_ROOT ]; then
    curl -L "http://downloads.xiph.org/releases/vorbis/libvorbis-1.3.5.tar.gz" | tar xz
    mv libvorbis-1.3.5 "$LIBVORBIS_ROOT"
    patch $LIBVORBIS_ROOT/configure < dependencies/libvorbis-clang.patch
fi

if [ ! -e $LIBWAVPACK_ROOT ]; then
    curl -L "http://www.wavpack.com/wavpack-5.2.0.tar.xz" | tar xJ
    mv wavpack-5.2.0 "$LIBWAVPACK_ROOT"
fi

echo "*************** Libav cross-compilation ***************"
for ABI in $ABI_FILTERS; do
    echo "*************** $ABI ***************"
    CFLAGS=""
    LDFLAGS=""
    ABI_FLAGS=""
    case $ABI in
        x86)
            ARCH=x86
            TOOLCHAIN_DIR="$TOOLCHAIN/i686-linux-android"
            TOOLCHAIN_PREFIX="i686-linux-android"
            ABI_FLAGS="--disable-asm"
            ;;
        x86_64)
            ARCH=x86_64
            TOOLCHAIN_DIR="$TOOLCHAIN/x86_64-linux-android"
            TOOLCHAIN_PREFIX="x86_64-linux-android"
            ABI_FLAGS="--disable-x86asm"
            ;;
        armeabi-v7a)
            ARCH=arm
            TOOLCHAIN_DIR="$TOOLCHAIN/arm-linux-androideabi"
            TOOLCHAIN_PREFIX="arm-linux-androideabi"
            ABI_FLAGS="--cpu=armv7-a"
            ;;
        arm64-v8a)
            ARCH=aarch64
            TOOLCHAIN_DIR="$TOOLCHAIN/aarch64-linux-android"
            TOOLCHAIN_PREFIX="aarch64-linux-android"
            LDFLAGS="-fuse-ld=gold"
            ;;
    esac

    case $ARCH in
        arm)
            CC="armv7a-linux-androideabi$MIN_SDK_VERSION-clang"
            ;;
        *64)
            CC="${TOOLCHAIN_PREFIX}21-clang"
            ;;
        *)
            CC="$TOOLCHAIN_PREFIX$MIN_SDK_VERSION-clang"
            ;;
    esac

    export AR=$TOOLCHAIN/bin/$TOOLCHAIN_PREFIX-ar
    export AS=$TOOLCHAIN/bin/$TOOLCHAIN_PREFIX-as
    export CC=$TOOLCHAIN/bin/$CC
    export LD=$TOOLCHAIN/bin/$TOOLCHAIN_PREFIX-ld
    export RANLIB=$TOOLCHAIN/bin/$TOOLCHAIN_PREFIX-ranlib
    export STRIP=$TOOLCHAIN/bin/$TOOLCHAIN_PREFIX-strip
    AUTOCONF_CROSS_FLAGS="
    --quiet
    --with-pic
    --with-sysroot=$TOOLCHAIN/sysroot
    --host=$TOOLCHAIN_PREFIX
    --enable-shared
    --disable-static
    --disable-frontend
    --disable-docs
    --disable-examples
    --prefix=$BUILD_ROOT/$ABI
    "

    echo "[1/5] Build $ABI libmp3lame..."
    (cd $LIBMP3LAME_ROOT && ./configure $AUTOCONF_CROSS_FLAGS && make clean && make && make install)

    echo "[2/5] Build $ABI libogg..."
    (cd $LIBOGG_ROOT && ./configure $AUTOCONF_CROSS_FLAGS && make clean && make && make install)

    echo "[3/5] Build $ABI libvorbis..."
    LIBVORBIS_FLAGS="
    --disable-oggtest
    CFLAGS=-I$BUILD_ROOT/$ABI/include
    LDFLAGS=-L$BUILD_ROOT/$ABI/lib
    "
    (cd $LIBVORBIS_ROOT && ./configure $AUTOCONF_CROSS_FLAGS $LIBVORBIS_FLAGS && make clean && make && make install)

    echo "[4/5] Build $ABI libwavpack..."
    LIBWAVPACK_FLAGS="
    --disable-asm
    --disable-apps
    --disable-dsd
    --enable-legacy
    "
    (cd $LIBWAVPACK_ROOT && ./configure $AUTOCONF_CROSS_FLAGS $LIBWAVPACK_FLAGS && make clean && make && make install)

    echo "[5/5] Build $ABI libav..."
    ABI_FLAGS="
    $ABI_FLAGS
    --cc=$CC
    --arch=$ARCH
    --sysroot=$TOOLCHAIN/sysroot
    --prefix=$BUILD_ROOT/$ABI
    --cross-prefix=$TOOLCHAIN/bin/$TOOLCHAIN_PREFIX-
    "
    CFLAGS="$CFLAGS -I$BUILD_ROOT/$ABI/include"
    LDFLAGS="$LDFLAGS -L$BUILD_ROOT/$ABI/lib -Wl,-rpath -Wl,$BUILD_ROOT/$ABI/lib"
    (cd "$LIBAV_ROOT" && ./configure --extra-cflags="$CFLAGS" --extra-ldflags="$LDFLAGS" $LIBAV_FLAGS $ABI_FLAGS && make clean && make && make install)
done
