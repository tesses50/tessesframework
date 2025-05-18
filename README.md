Tesses Framework
================

## To Install
- Install [mbedtls](https://github.com/Mbed-TLS/mbedtls) (use sudo apt install libmbedtls-dev on debian) for TessesFramework
- Follow the commands bellow

## Run these commands to install tesses-framework
```bash
git clone https://onedev.site.tesses.net/tesses-framework
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