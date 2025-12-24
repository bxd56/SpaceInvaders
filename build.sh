#!/bin/bash

INIT_PATH=$PWD

if [ ! -d 3rdParty/SDL3-3.2.24/build ]
then
cd 3rdParty/SDL3-3.2.24/
mkdir build
cd build
cmake ..
make -j
fi

if [ ! -d $INIT_PATH/3rdParty/SDL3_image-3.2.4/build ]
then
cd $INIT_PATH/3rdParty/SDL3_image-3.2.4/
mkdir build
cd build
cmake .. -DSDL3_DIR=../SDL3-3.2.24/build
make -j
fi

if [ ! -d $INIT_PATH/3rdParty/SDL3_ttf-3.2.2/build ]
then
cd $INIT_PATH/3rdParty/SDL3_ttf-3.2.2/
mkdir build
cd build
cmake .. -DSDL3_DIR=../SDL3-3.2.24/build 
make -j
fi

cd $INIT_PATH
make -j
