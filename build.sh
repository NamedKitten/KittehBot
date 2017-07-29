#!/bin/bash
echo `whoami`
echo `nproc`
rm lib/* -rf
echo "doing teh git"
git submodule update --init --recursive --remote --force
echo "making teh build"
mkdir build
echo "going into build"
cd build
#if [ ! -f cmake-3.9.0-Linux-x86_64.tar.gz ]; then
#  echo "downloading teh cmake"
#  wget https://cmake.org/files/v3.9/cmake-3.9.0-Linux-x86_64.tar.gz
#  echo "extracting teh cmake"
#  tar xf cmake-3.9.0-Linux-x86_64.tar.gz
#fi
if [ -f CMakeCache.txt ]; then
    rm * -rf
fi
export CC=clang-5.0
export CXX=clang++-5.0
echo "using teh cmake"
cmake ..
echo "using make"
make clean
if make -j`nproc` ; then
	echo "o.o it is done o.o"
else
	exit 1
fi
