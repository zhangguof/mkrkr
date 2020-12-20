# moblie krkrz
port krkrz to ios.
https://github.com/krkrz/krkrz


# depends
* build success with macos 10.14.6, xcode 11.3:
* freetype 2.9
* SDL2 2.0.8
* boost 1.69
* ffmpeg 4.0
* openal system default
* opengles system default

# build for ios
* just run  xcode
* config third-party include path
* config static lib path
  
# build for macos
**Old method to build in macos blew**
**Use xcode for newest.**
## build onig
cd src/external/oniguruma
mkdir build
cmake ../
cp libonig.a ../../../../libs/

## build boost
cd boost_1_67_0
./bootstrap.sh --with-libraries=filesystem,locale,system
./b2 toolset=clang


then copy boost_1_67_0/stage/lib/*.a to mkrkr/libs/

## build zlib
cd ext_libs_src
wget http://www.zlib.net/zlib-1.2.11.tar.gz
tar xvzf zlib-1.2.11.tar.gz
mv zlib-1.2.11 zlib
cd zlib
exprot CC=cc
exprot CXX=c++
./configure
make
cp libz.a ../../../libs/

## build freetype
cd ext_libs_src
wget https://sourceforge.net/projects/freetype/files/freetype2/2.9/freetype-2.9.tar.bz2/download
./configure CC=cc CXX=c++
make
cp objs/.libs/libfreetype.a ../../../libs/

## build libpng
cd ext_libs_src
wget https://download.sourceforge.net/libpng/libpng-1.6.34.tar.gz
....
like before

## build krkr
scons
