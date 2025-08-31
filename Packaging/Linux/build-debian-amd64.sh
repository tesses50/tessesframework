#!/bin/bash
mkdir -p build
cd build
mkdir build-deb-amd64
cmake -S ../../../ -B build-deb-amd64 -DCMAKE_INSTALL_PREFIX=/usr
cd build-deb-amd64
make -j`nproc`
make install DESTDIR=../tessesframework_1.0.0_amd64
mkdir -p ../tessesframework_1.0.0_amd64/DEBIAN
cp ../../debian/control-amd64 ../tessesframework_1.0.0_amd64/DEBIAN/control
cd ../
dpkg-deb --build tessesframework_1.0.0_amd64