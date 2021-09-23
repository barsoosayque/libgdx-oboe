#!/usr/bin/sh

# =============== Build definitions ===============

CORES=4
MAKE="make -j$CORES"

HOST_NAME=$(uname -s | tr '[:upper:]' '[:lower:]')
HOST_ARCH=$(uname -m)
HOST_TAG="$HOST_NAME-$HOST_ARCH"

# Directories
LIBMP3LAME_ROOT="build/libmp3lame"
LIBOGG_ROOT="build/libogg"
LIBWAVPACK_ROOT="build/libwavpack"
LIBVORBIS_ROOT="build/libvorbis"
FFMPEG_ROOT="dependencies/ffmpeg"
BUILD_ROOT="$(pwd)/build/libs"

# Consult library/build.gradle for those options
# android.defaultConfig.ndk.abiFilters
#ABI_FILTERS="x86_64"
ABI_FILTERS="x86 x86_64 armeabi-v7a arm64-v8a"
# android.defaultConfig.minSdkVersion
MIN_SDK_VERSION="16"

# Flags
FFMPEG_FLAGS="
--enable-cross-compile
--target-os=android
--pkg-config-flags=--static
--pkg-config=pkgconf
--disable-postproc
--disable-debug
--enable-hardcoded-tables
--enable-version3
--enable-x86asm
--enable-nonfree

--enable-shared
--disable-static
--enable-pic
--enable-small

--disable-everything
--disable-doc
--disable-avdevice
--disable-network
--disable-swscale
--disable-pthreads
--disable-programs
--disable-zlib
--enable-swresample
--enable-avformat
--enable-avcodec

--disable-protocols
--enable-protocol=file

--disable-encoders
--enable-libmp3lame
--enable-libvorbis
--enable-demuxer=wav,ogg,pcm*,mp3
--enable-decoder=vorbis,opus,wavpack,mp3*,pcm*
"
# static:
#--enable-statoc
#--disable-shared
#--enable-lto

# shared:
#--enable-shared
#--disable-static
#--enable-pic


# =============== Option handle ==============
while [[ $# -gt 0 ]]; do
    case "$1" in
        --help|-h)
            echo "build_ffmpeg.sh -- script to build shared ffmpeg libraries for android with support for mp3, wav and ogg."
            echo "usage: build_ffmpeg.sh (--ndk-dir <NDK_DIR>) (-h|--help) (--update) (--clear) (--ffmpeg-only) (--init)"
            echo "options:"
            echo "    -h, --help:          print this message and exit."
            echo "    --ndk-dir <NDK_DIR>: uses toolchain from provided ndk directory (or env var NDK_DIR otherwise)"
            echo "    --update:            copy built libraries to ./libs (this option assume that the library is built)."
            echo "    --clear:             clear build and temporary directories and exit."
            echo "    --ffmpeg-only:       only build ffmpeg (assuming that dependencies already built)"
            echo "    --init:              configure ffmpeg and generate required files (this implies --ffmpeg-only)"
            exit 0
            ;;
        --update)
            for ABI in $ABI_FILTERS; do
                DIR="$(pwd)/libs/$ABI"
                mkdir -p "$DIR"
                cp -av -t "$DIR" $BUILD_ROOT/$ABI/lib/lib{avformat,avcodec,swresample,avutil}.so
            done
            exit 0
            ;;
        --clear)
            rm -r "$BUILD_ROOT"
            rm -r "$LIBMP3LAME_ROOT"
            rm -r "$LIBOGG_ROOT"
            rm -r "$LIBWAVPACK_ROOT"
            rm -r "$LIBVORBIS_ROOT"
            exit 0
            ;;
        --init)
            INIT_ONLY=1
            FFMPEG_ONLY=1
            ;;
        --ffmpeg-only)
            FFMPEG_ONLY=1
            ;;
        --ndk-dir)
            NDK_DIR="$2"
            shift
            ;;
    esac
    shift
done

if [ -z "$NDK_DIR" ]; then
  echo "ERROR: define NDK_DIR env var or pass --ndk-dir to the script"
  exit 1
fi

TOOLCHAIN="$NDK_DIR/toolchains/llvm/prebuilt/$HOST_TAG"


# =============== Actual build ===============

if [ -z "INIT_ONLY" ]; then
  echo "*************** Prepare dependepcies ***************"

  mkdir -p "$BUILD_ROOT"

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
fi

echo "*************** FFmpeg cross-compilation ***************"
for ABI in $ABI_FILTERS; do
    echo "*************** $ABI ***************"
    CFLAGS="-O3"
    LDFLAGS="-lm"
    ABI_FLAGS=""
    case $ABI in
        x86)
            ARCH=x86
            TOOLCHAIN_DIR="$TOOLCHAIN/i686-linux-android"
            TOOLCHAIN_PREFIX="i686-linux-android"
            ABI_FLAGS="--disable-asm"
            CPU="i686"
            CFLAGS="$CFLAGS -march=i686"
            ;;
        x86_64)
            ARCH=x86_64
            TOOLCHAIN_DIR="$TOOLCHAIN/x86_64-linux-android"
            TOOLCHAIN_PREFIX="x86_64-linux-android"
            CPU="x86_64"
            ABI_FLAGS="--disable-x86asm"
            ;;
        armeabi-v7a)
            ARCH=arm
            TOOLCHAIN_DIR="$TOOLCHAIN/arm-linux-androideabi"
            TOOLCHAIN_PREFIX="arm-linux-androideabi"
            CPU="armv7a"
            ;;
        arm64-v8a)
            ARCH=aarch64
            TOOLCHAIN_DIR="$TOOLCHAIN/aarch64-linux-android"
            TOOLCHAIN_PREFIX="aarch64-linux-android"
            #LDFLAGS="-fuse-ld=gold "
            CPU="generic"
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
    export CXX=$CC++
    export LD=$CC
    export RANLIB=$TOOLCHAIN/bin/$TOOLCHAIN_PREFIX-ranlib
    export STRIP=$TOOLCHAIN/bin/$TOOLCHAIN_PREFIX-strip
    export NM=$TOOLCHAIN/bin/$TOOLCHAIN_PREFIX-nm

    export PKG_CONFIG_PATH=$BUILD_ROOT/$ABI/lib/pkgconfig

    export TOOLCHAIN=$TOOLCHAIN
    export TARGET=$TOOLCHAIN_PREFIX
    export API=$MIN_SDK_VERSION

    AUTOCONF_CROSS_FLAGS="
    --quiet
    --host=$TARGET
    --disable-shared
    --enable-static
    --disable-frontend
    --disable-docs
    --disable-examples
    --disable-encoder
    --prefix=$BUILD_ROOT/$ABI
    "
#      --with-sysroot=$TOOLCHAIN/sysroot

    if [ -z "$INIT_ONLY" ]; then
      BUILD_CMD="$MAKE clean && $MAKE && $MAKE install"
    else
      BUILD_CMD=":"
    fi

    if [ -z "$FFMPEG_ONLY" ]; then
        echo "[1/5] Build $ABI libmp3lame..."
        (cd $LIBMP3LAME_ROOT && ./configure $AUTOCONF_CROSS_FLAGS && $BUILD_CMD)

        echo "[2/5] Build $ABI libogg..."
        (cd $LIBOGG_ROOT && ./configure $AUTOCONF_CROSS_FLAGS && $BUILD_CMD)

        echo "[3/5] Build $ABI libvorbis..."
        LIBVORBIS_FLAGS="
        --disable-oggtest
        CFLAGS=-I$BUILD_ROOT/$ABI/include
        LDFLAGS=-L$BUILD_ROOT/$ABI/lib
        "
        (cd $LIBVORBIS_ROOT && ./configure $AUTOCONF_CROSS_FLAGS $LIBVORBIS_FLAGS && $BUILD_CMD)

        echo "[4/5] Build $ABI libwavpack..."
        LIBWAVPACK_FLAGS="
        --disable-asm
        --disable-apps
        --disable-dsd
        --enable-legacy
        "
        (cd $LIBWAVPACK_ROOT && ./configure $AUTOCONF_CROSS_FLAGS $LIBWAVPACK_FLAGS && $BUILD_CMD)

        echo "[5/5] Build $ABI ffmpeg..."
    else
        echo "[1/1] Build $ABI ffmpeg..."
    fi

    ABI_FLAGS="
    $ABI_FLAGS
    --cpu=$CPU
    --cc=$CC
    --cxx=$XX
    --ld=$LD
    --ar=$AR
    --as=$CC
    --nm=$NM
    --ranlib=$RANLIB
    --strip=$STRIP
    --arch=$ARCH
    --sysroot=$TOOLCHAIN/sysroot
    --prefix=$BUILD_ROOT/$ABI
    --cross-prefix=$TOOLCHAIN/bin/$TOOLCHAIN_PREFIX-
    --env=PKG_CONFIG_PATH=$BUILD_ROOT/$ABI/lib/pkgconfig
    "
    CFLAGS="$CFLAGS -I$BUILD_ROOT/$ABI/include"
    LDFLAGS="$LDFLAGS -L$BUILD_ROOT/$ABI/lib"
    (cd "$FFMPEG_ROOT" && \
    ./configure \
        --extra-cflags="$CFLAGS" \
        --extra-ldflags="$LDFLAGS" \
        $FFMPEG_FLAGS \
        $ABI_FLAGS \
    && $BUILD_CMD)
done
