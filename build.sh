#!/bin/bash
echo "doing teh git"
git submodule update --init --recursive
echo "boop" > token.dat
echo "making teh build"
mkdir build
echo "going into build"
cd build
echo "downloading teh cmake"
wget https://cmake.org/files/v3.9/cmake-3.9.0-Linux-x86_64.tar.gz
echo "extracting teh cmake"
tar xf cmake-3.9.0-Linux-x86_64.tar.gz
echo "using teh cmake"
cmake-3.9.0-Linux-x86_64/bin/cmake ..
echo "using make"
make -j7
echo "o.o it is done o.o"
