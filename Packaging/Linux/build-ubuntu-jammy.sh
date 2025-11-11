#!/bin/bash
source ./version.sh
mkdir -p build/jammy
cd build/jammy
mkdir build-amd64
cmake -S ../../../../ -B build-amd64 -DCMAKE_INSTALL_PREFIX=/usr -DTESSESFRAMEWORK_VENDERCERTCHAIN=OFF -DTESSESFRAMEWORK_FETCHCONTENT=OFF
cd build-amd64
make -j`nproc`
make install DESTDIR=../tessesframework_$DEB_VERSION\_amd64
mkdir -p ../tessesframework_$DEB_VERSION\_amd64/DEBIAN
bash ../../../make-control.sh ../tessesframework_$DEB_VERSION\_amd64/DEBIAN/control amd64
cd ../
dpkg-deb --build tessesframework_$DEB_VERSION\_amd64

# ARM64
mkdir build-arm64
cmake -S ../../../../ -B build-arm64 -DCMAKE_INSTALL_PREFIX=/usr -DTESSESFRAMEWORK_VENDERCERTCHAIN=OFF -DTESSESFRAMEWORK_FETCHCONTENT=OFF -DCMAKE_TOOLCHAIN_FILE=/opt/toolchains/arm64.cmake
cd build-arm64
make -j`nproc`
make install DESTDIR=../tessesframework_$DEB_VERSION\_arm64
mkdir -p ../tessesframework_$DEB_VERSION\_arm64/DEBIAN
bash ../../../make-control.sh ../tessesframework_$DEB_VERSION\_arm64/DEBIAN/control arm64
cd ../
dpkg-deb --build tessesframework_$DEB_VERSION\_arm64

# RISCV64
mkdir build-riscv64
cmake -S ../../../../ -B build-riscv64 -DCMAKE_INSTALL_PREFIX=/usr -DTESSESFRAMEWORK_VENDERCERTCHAIN=OFF -DTESSESFRAMEWORK_FETCHCONTENT=OFF -DCMAKE_TOOLCHAIN_FILE=/opt/toolchains/riscv64.cmake
cd build-riscv64
make -j`nproc`
make install DESTDIR=../tessesframework_$DEB_VERSION\_riscv64
mkdir -p ../tessesframework_$DEB_VERSION\_riscv64/DEBIAN
bash ../../../make-control.sh ../tessesframework_$DEB_VERSION\_riscv64/DEBIAN/control riscv64
cd ../
dpkg-deb --build tessesframework_$DEB_VERSION\_riscv64

# ARMHF
mkdir build-armhf
cmake -S ../../../../ -B build-armhf -DCMAKE_INSTALL_PREFIX=/usr -DTESSESFRAMEWORK_VENDERCERTCHAIN=OFF -DTESSESFRAMEWORK_FETCHCONTENT=OFF -DCMAKE_TOOLCHAIN_FILE=/opt/toolchains/armhf.cmake
cd build-armhf
make -j`nproc`
make install DESTDIR=../tessesframework_$DEB_VERSION\_armhf
mkdir -p ../tessesframework_$DEB_VERSION\_armhf/DEBIAN
bash ../../../make-control.sh ../tessesframework_$DEB_VERSION\_armhf/DEBIAN/control armhf
cd ../
dpkg-deb --build tessesframework_$DEB_VERSION\_armhf