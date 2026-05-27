export BUILDDIR=builds/x86-windows-gnu
mkdir -p $BUILDDIR
cmake -S ../.. -B $BUILDDIR --toolchain $PWD/x86.cmake -DCMAKE_BUILD_TYPE=Release -DTESSESFRAMEWORK_INSTALL_DEVELOPMENT=OFF -DTESSESFRAMEWORK_ENABLE_EXAMPLES=OFF -DTESSESFRAMEWORK_STATIC=ON -DCMAKE_EXE_LINKER_FLAGS="-static-libgcc -static-libstdc++ -static -Wl,--strip-all" -DCMAKE_POSITION_INDEPENDENT_CODE=ON -GNinja
cmake --build $BUILDDIR || exit 1
cmake --install $BUILDDIR --prefix $BUILDDIR/out
env -C $BUILDDIR/out zip -r ../../../../../artifacts/tessesframework-tools-x86-windows-gnu.zip bin

export BUILDDIR=builds/x86_64-windows-gnu
mkdir -p $BUILDDIR
cmake -S ../.. -B $BUILDDIR --toolchain $PWD/x86_64.cmake -DCMAKE_BUILD_TYPE=Release -DTESSESFRAMEWORK_INSTALL_DEVELOPMENT=OFF -DTESSESFRAMEWORK_ENABLE_EXAMPLES=OFF -DTESSESFRAMEWORK_STATIC=ON -DCMAKE_EXE_LINKER_FLAGS="-static-libgcc -static-libstdc++ -static -Wl,--strip-all" -DCMAKE_POSITION_INDEPENDENT_CODE=ON -GNinja
cmake --build $BUILDDIR || exit 1
cmake --install $BUILDDIR --prefix $BUILDDIR/out
env -C $BUILDDIR/out zip -r ../../../../../artifacts/tessesframework-tools-x86_64-windows-gnu.zip bin