#!/bin/bash

echo 'Making Kernel'

make clean
make mrproper
make clean
wget --no-check-certificate https://www.cs.drexel.edu/~wmm24/cs370/resources/config-2.6.22.19-debian -O .config
make oldconfig

echo 'Getting'

wget https://www.cs.drexel.edu/~wmm24/cs370/resources/kdb-2.6.22.19/kdb-v4.4-2.6.22-rc7-common-1.bz2
wget https://www.cs.drexel.edu/~wmm24/cs370/resources/kdb-2.6.22.19/kdb-v4.4-2.6.22-rc7-i386-1.bz2
wget https://www.cs.drexel.edu/~wmm24/cs370/resources/kdb-2.6.22.19/kdb-v4.4-2.6.22-rc7-x86_64-1.bz2

bunzip2 kdb-v4.4-2.6.22-rc7-common-1.bz2
bunzip2 kdb-v4.4-2.6.22-rc7-i386-1.bz2
bunzip2 kdb-v4.4-2.6.22-rc7-x86_64-1.bz2

echo 'Applying patches'

patch -N -p1 < kdb-v4.4-2.6.22-rc7-common-1
patch -N -p1 < kdb-v4.4-2.6.22-rc7-i386-1
patch -N -p1 < kdb-v4.4-2.6.22-rc7-x86_64-1

rm -f kdb-v4.4-2.6.22-rc7-common-1.bz kdb-v4.4-2.6.22-rc7-i386-1.bz2 kdb-v4.4-2.6.22-rc7-x86_64-1.bz2

make EXTRAVERSION='.19-LASTNAME' -j4

echo 'Done'
