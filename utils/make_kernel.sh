#!/bin/bash

echo 'Making Kernel'

make clean
make mrproper
make clean
wget --no-check-certificate https://www.cs.drexel.edu/~wmm24/cs370/resources/config-2.6.22.19-debian -O .config
make oldconfig
