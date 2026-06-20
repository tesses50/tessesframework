if [ $# -lt 2 ]
then
    echo "Not enough args, pass nfpm arch and debian tripple as argument"
    echo "This script requires our docker container: git.tesses.org/tesses50/linux-*:latest"
else
    mkdir -p build
    cd build
    cmake-cross -S .. -B . -DTESSESFRAMEWORK_NFPM_ARCH=$1 -DCMAKE_INSTALL_LIBDIR=lib/$2 -DCMAKE_BUILD_TYPE=Release
    make -j`nproc`
    make install DESTDIR=debian
    cd debian
    nfpm package -f ../nfpm.yaml -p deb
    curl --user tesses50:$PACKAGE_AND_BREW \
     --upload-file *.deb \
     https://git.tesses.org/api/packages/tesses50/debian/pool/universal/main/upload
    #debian/pool/bionic
    cd ..
    if [ "$1" = "386" ] || [ "$1" = "arm7" ]
    then
    cmake-cross -S .. -B . -DTESSESFRAMEWORK_NFPM_ARCH=$1 -DCMAKE_INSTALL_LIBDIR=lib32
    else
    cmake-cross -S .. -B . -DTESSESFRAMEWORK_NFPM_ARCH=$1 -DCMAKE_INSTALL_LIBDIR=lib64
    fi
    make install DESTDIR=fedora
    cd fedora
    nfpm package -f ../nfpm.yaml -p rpm
    curl --user tesses50:$PACKAGE_AND_BREW \
     --upload-file *.rpm \
     https://git.tesses.org/api/packages/tesses50/rpm/upload
    cd ..
    cmake-cross -S .. -B . -DTESSESFRAMEWORK_NFPM_ARCH=$1 -DCMAKE_INSTALL_LIBDIR=lib
    make install DESTDIR=arch
    cd arch
    nfpm package -f ../nfpm.yaml -p archlinux
    curl --user tesses50:$PACKAGE_AND_BREW \
     --upload-file *.pkg.tar.zst \
     https://git.tesses.org/api/packages/tesses50/arch/core
    mkdir -p ../../artifacts
    tar czf ../../artifacts/tessesframework-$VERSION\-$1\.tar.gz -C usr/ .
fi