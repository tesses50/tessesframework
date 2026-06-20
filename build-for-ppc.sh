
mkdir -p build
pushd build
cmake-cross -S .. -B . -DCMAKE_INSTALL_LIBDIR=lib -DCMAKE_BUILD_TYPE=Release
make -j`nproc`
mkdir -p /home/build/tf
chown build:build -R /home/build
make install DESTDIR=/home/build/tf/src
cp ppc-pkgbuild /home/build/tf/PKGBUILD
chmod 644 /home/build/tf/PKGBUILD
chown build:build -R /home/build/tf
pushd /home/build/tf
su build -c 'CARCH=powerpc makepkg -d'
curl --user tesses50:$PACKAGE_AND_BREW \
     --upload-file *.pkg.tar.zst \
     https://git.tesses.org/api/packages/tesses50/arch/core
popd
popd
mkdir -p artifacts
tar czf artifacts/tessesframework-$VERSION\-powerpc.tar.gz -C /home/build/tf/src/usr .