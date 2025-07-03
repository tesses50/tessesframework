Tesses Framework
================

## If you are on onedev
We are switching to [gitea](https://git.tesseslanguage.com/tesses50/tesses-framework)

## To Install
- Install [mbedtls](https://github.com/Mbed-TLS/mbedtls) (use sudo apt install libmbedtls-dev on debian) for TessesFramework
- Follow the commands bellow

## Run these commands to install tesses-framework
```bash
git clone https://git.tesseslanguage.com/tesses-framework
cd tesses-framework
mkdir build 
cd build
cmake -S .. -B .
make
sudo make install
```

## What libraries have I embeded from other people
- [HodwardHinnart's date](https://github.com/HowardHinnant/date) (in folder src/HodwardHinnart_date, modified tz.cpp to point to correct header)

## Dependencies
- [mbedtls](https://github.com/Mbed-TLS/mbedtls)

## Code used from other peoples projects
- [Multipart parser (rewriten in C++ was C# in dejuric/simplehttp)](https://github.com/dajuric/simple-http)

## Resources used from others
- [ca-certificate.crt (from ubuntu)](ca-certificate.crt)
- [TanoheSans-Regular.ttf (license OFL)](font/License-OFL.txt)