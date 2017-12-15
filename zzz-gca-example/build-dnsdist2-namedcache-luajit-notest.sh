#!/bin/bash -e
echo ""
echo "from:  http://dnsdist.org/download/"
echo ""
echo "clone from:  git clone https://github.com/PowerDNS/pdns.git"
echo "--or from our copy--"
echo "https://github.com/GlobalCyberAlliance/pdns.git"
echo ""

echo "cd ../pdns/dnsdistdist"
echo ""
cd ../pdns/dnsdistdist
echo ""
echo "autoreconf -i"
echo ""
autoreconf -i
echo ""
echo "NOTE: configure with libsodium enabled to allow cache test to succeed - Seth - Global Cyber Alliance"
echo "NOTE: configure also with named cache enabled to allow cache test to succeed - Seth - Global Cyber Alliance - 9/27/2017"
echo "NOTE: configure also with luajit - 11/13/2017"
echo "./configure --enable-libsodium --enable-namedcache --with-luajit"
./configure --enable-libsodium --enable-namedcache --with-luajit
echo ""
echo "do a \"make clean\" incase this is not the first time through"
echo ""
make clean
echo ""
echo "now do a make"
echo ""
echo "make"
echo ""
make
echo ""
echo "finished"


