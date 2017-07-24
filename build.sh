#!/bin/bash
echo `whoami`
echo "doing teh git"
git submodule update --init --recursive --remote --force
echo "making teh build"
mkdir build
echo "going into build"
cd build
if [ ! -f cmake-3.9.0-Linux-x86_64.tar.gz ]; then
  echo "downloading teh cmake"
  wget https://cmake.org/files/v3.9/cmake-3.9.0-Linux-x86_64.tar.gz
  echo "extracting teh cmake"
  tar xf cmake-3.9.0-Linux-x86_64.tar.gz
fi
if [ -f CMakeCache.txt ]; then
    rm CMakeCache.txt
fi
echo "using teh cmake"
cmake-3.9.0-Linux-x86_64/bin/cmake ..
echo "using make"
make clean
make -j`grep -c '^processor' /proc/cpuinfo`
echo "o.o it is done o.o"
cd ..
