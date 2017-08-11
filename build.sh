#!/bin/bash
echo `whoami`
echo `nproc`
ls -lah
echo potato
rm lib/* -rf
echo "doing teh git"
git submodule update --init --recursive --force --remote
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
export VERBOSE=1
echo "using teh cmake"
cmake .. -DTEST=yes
echo "using make"
make clean

#if make -j`nproc` VERBOSE=1 ; then
#	redis-server &
#	sleep 5
#	./KittehBot++
#else
#	exit 1
#fi
