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

## down depend:  
you  can down 3rd part library include files and static lib here:  

* baidu pan:   
https://pan.baidu.com/s/1EAnHR64if3C1pXUtXxNXGQ   p5dm  

* google drive:  
https://drive.google.com/drive/folders/1SgNTvspty6F09dlLW-hyh9_z7yYdcJBr?usp=sharing 


down in mkrkr folder:  

* tar xvzf libs_ios.tar.gz  
* tar xvzf thirdparty_boost_1_67_0.tar  
  
if you want build in macos:  

* tar xvzf libs_macos.tar.gz  


# build for ios
* just run  xcode
* config third-party include path
* config static lib path
  
# build for macos

just for reference, you can down thrid part library above link.  
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

# Run mkrkr
## Macos 
* scons
* bin/krkr -data test-data
