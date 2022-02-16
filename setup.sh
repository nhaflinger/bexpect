#!/usr/bin/bash

sudo apt-get update --yes
sudo apt-get install --yes cmake
sudo apt-get install --yes flex bison
sudo apt-get install --yes libgtest-dev
/usr/bin/rm CMakeCache.txt
cmake ./
make
