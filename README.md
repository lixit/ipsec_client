## Deploy for Windows

windeployqt  vpn_cpp.exe

## Translate

Release Manager
```
# produce .ts file
lupdate iLinkAll.pro

# produce .qm file
lrelese iLinkAll.pro
```

## build Qt statically with OpenSSL on Win10
1. download Qt 5.15.1 Source Code from Qt Maintenance Tool.  
2. install Python, Perl, OpenSSL, jom  
3. make sure they are in Path environment variable.
4. open x64 Native Tools Command Prompt for VS 2019

download OpenSSL from here
https://slproweb.com/products/Win32OpenSSL.html

```
# remove previous config cache
e.g. rm /d/Qt/5.15.1/Src/config.cache

set OPENSSL_DIR=C:\Program Files\OpenSSL-Win64
configure.bat -release -opensource -confirm-license -static -no-pch -optimize-size -opengl desktop -platform win32-msvc  -prefix "D:\Qt\5.15.1\msvc2019_64_static" -skip webengine -nomake tools -nomake tests -nomake examples -openssl-linked OPENSSL_INCDIR="%OPENSSL_DIR%\include" OPENSSL_LIBDIR="%OPENSSL_DIR%\lib\VC\static" OPENSSL_LIBS="-lWs2_32 -lGdi32 -lAdvapi32 -lCrypt32 -lUser32 -llibssl64MDd -llibcrypto64MDd" OPENSSL_LIBS_DEBUG="-llibssl64MDd -llibcrypto64MDd" OPENSSL_LIBS_RELEASE="-llibssl64MD -llibcrypto64MD"  

D:\jom_1_1_3\jom.exe -j8
D:\jom_1_1_3\jom.exe -j8 install
```