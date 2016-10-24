#!/bin/bash

gtest_src=/usr/src/gtest
if [ -d "$gtest_src" ]
then
    cd $gtest_src
else
    echo "$gtest_src is not a directory!" > /dev/stderr
fi

#dpkg -L libgtest-dev | grep src 

# Compile the library
sudo cmake CMakeLists.txt
sudo make
# Move it in the library path
sudo cp *.a /usr/lib
