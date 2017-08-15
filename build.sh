#!/bin/bash
echo "Please make sure you have at least cmake version 3.8 or the build will fail."
rm lib/* -rf
echo "Updating submodules..."
git submodule update --init --recursive --force --remote --depth 1
echo "Making build directory..."
mkdir -v build
cd build
#if [ ! -f cmake-3.9.0-Linux-x86_64.tar.gz ]; then
#  echo "downloading teh cmake"
#  wget https://cmake.org/files/v3.9/cmake-3.9.0-Linux-x86_64.tar.gz
#  echo "extracting teh cmake"
#  tar xf cmake-3.9.0-Linux-x86_64.tar.gz
#fi
if [ -f CMakeCache.txt ]; then
    echo "Cleaning up..."
    rm * -rf
fi
echo "Running cmake..."
cmake .. -DTEST=yes
echo "Cleaning build directory."
make clean
echo "Making bot..."
make -j`nproc` VERBOSE=1
echo "Done! The executable will be in the build directory."
#if make -j`nproc` VERBOSE=1 ; then
#	redis-server &
#	sleep 5
#	./KittehBot++
#        sleep 1
#else
#	exit 1
#fi
