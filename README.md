Tesses Framework
================

## Features
- DateTime, TimeSpan
- VFS (polymorphic filesystems) (Mountable is kind of broken)
- BitTorrent file generation, bencoding
- Http Client/Server
- Json
- Simple SMTP client
- Environment variables (cross platform), user special folders (also cross platform, using sago/platform-folders)
- Sqlite3 (embedded into this library)
- Process API (for windows/mac/linux/probably bsd)
- Polymorphic byte streams (for files/network/memory)
- C/C++ header generator for embedding files
- A console api which works on windows/mac/linux (even before windows 10, tested on 8.1 and works), with password entry, readline, partial vt100 emulation for win32, colors, raw/cooked modes and terminal list (for somewhat primitive interactive tuis, it does scroll though, pages)
- Thread pool and threading
- Basic arg parser
- Uuids
- Lazy initializable type
- Unicode support, (I need to do better validation, I know, only really used for windows and parsing json)
- EventLoop
- Basic crytography using mbedtls
- a LCG Random number generator (do not use for crypto, use it for non security things like number guessing games and stuff, where rand would be ok)

## Supported Platforms
- Linux
- Windows (with mingw and static linked for sure)
- MacOS
- FreeBSD (including GhostBSD)
- NetBSD
- Wii (using libogc2, may be broken)
- Switch (using libnx, may be broken)
- Android (kinda, uses legacy and hardcoded /sdcard api unless you use TF_AllowPortable(std::string))

> NOTE: if platform folders is disabled tessesframework uses the path /TF_User for special folders (except for android which it uses /sdcard)

## To Install
- Install [mbedtls](https://github.com/Mbed-TLS/mbedtls) (use sudo apt install libmbedtls-dev on debian) for TessesFramework
- Follow the commands bellow

## Run these commands to install tesses-framework
```bash
git clone https://git.tesses.org/tesses50/tessesframework
cd tessesframework
mkdir build 
cd build
cmake -S .. -B .
make
sudo make install
```

## What libraries have I embeded from other people
- [HodwardHinnart's date](https://github.com/HowardHinnant/date) (in folder src/HodwardHinnart_date, modified tz.cpp to point to correct header)
- [PlatformFolders](https://github.com/sago007/PlatformFolders) (in folder src/Platform/sago, modified to conditionally disable for GEKKO, NX etc)
- [Sqlite 3](https://www.sqlite.org/) (in folder src/Serialization/sqlite, modified for GEKKO, NX etc, uses the amalgamation)

## Dependencies
- [mbedtls](https://github.com/Mbed-TLS/mbedtls)

## Code used from other peoples projects
- [Multipart parser (rewriten in C++ was C# in dejuric/simplehttp)](https://github.com/dajuric/simple-http)

## Resources used from others
- [ca-certificate.crt (from ubuntu)](ca-certificate.crt)
