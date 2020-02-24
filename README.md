<h1 align="center">
  <img src="https://github.com/TheWanderingCoel/Trojan-Qt5/blob/master/resources/icons/trojan-qt5.png?raw=true" alt="Trojan-Qt5" width="200">
  <br>
  Trojan-Qt5
  <br>
</h1>

A cross-platform Trojan GUI client

[![Build status](https://travis-ci.com/TheWanderingCoel/Trojan-Qt5.svg?branch=master)](https://travis-ci.com/TheWanderingCoel/Trojan-Qt5)
[![Build status](https://ci.appveyor.com/api/projects/status/shjhg9mlvc3c74ek?svg=true)](https://ci.appveyor.com/project/CoelWu/trojan-qt5)
[![HitCount](http://hits.dwyl.io/TheWanderingCoel/Trojan-Qt5.svg)](http://hits.dwyl.io/TheWanderingCoel/Trojan-Qt5)
![GitHub Releases](https://img.shields.io/github/downloads/TheWanderingCoel/Trojan-Qt5/latest/total?style=flat-square&logo=github)
![GitHub All Releases](https://img.shields.io/github/downloads/TheWanderingCoel/Trojan-Qt5/total?label=downloads-total&logo=github&style=flat-square)
[![License](https://img.shields.io/badge/license-GPL%20V3-blue.svg?longCache=true)](https://www.gnu.org/licenses/gpl-3.0.en.html)

## Install

You can download from [release](https://github.com/TheWanderingCoel/Trojan-Qt5/releases) page

## Compiling

### 0.Requirement
- Windows, macOS, Linux
- c++ compiler(cl, clang, gcc)
- Qt 5.13.0 +
- QHttpServer(follow the instruction below to install)
```
git clone https://github.com/qt-labs/qthttpserver.git
cd qthttpserver
qmake
make -j$(nproc) // nmake on Windows
make install // nmake install on Windows
```

### 1.Windows
- 1. Initialize the Environment
```
C:\"Program Files (x86)"\"Microsoft Visual Studio"\2019\Community\VC\Auxiliary\Build\vcvarsall.bat x86
```
- 2. Clone Libraries
```
git clone https://github.com/TheWanderingCoel/Trojan-Qt5-Libraries.git C:\TQLibraries
```
- 3. Install Boost Library Manually
```
curl -Lo boost_1_72_0-msvc-14.2-32.exe https://sourceforge.net/projects/boost/files/boost-binaries/1.72.0/boost_1_72_0-msvc-14.2-32.exe/download
powershell ".\\boost_1_72_0-msvc-14.2-32.exe /SILENT /SP- /SUPPRESSMSGBOXES /DIR='C:\TQLibraries\boost_1_72_0'"
```
- 4. Run Build
```
mkdir build && cd build
qmake ..
nmake
```

### 2.macOS
- 1. Install HomeBrew
```
ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```
- 2. Install Dependencies
```
brew install zbar qrencode boost openssl@1.1 zlib pcre
```
- 3. Run Build
```
mkdir build && cd build
qmake ..
make -j$(nproc)
```

### 3.Linux

- 1. Install Dependencies
```
sudo apt-get install libgl-dev git build-essential g++ python-dev autotools-dev libicu-dev libbz2-dev checkinstall zlib1g-dev -y
```
- 2. Compile Dependencies
```
sudo bash scripts/linux_compile.sh
```
- 3. Configure Privoxy
```
cd src/privoxy
autoheader && autoconf && ./configure
cd ../..
```
- 4. Run Build
```
mkdir build && cd build
qmake ..
make -j$(nproc)
```

## Frequent Ask Question (FAQ)

### 1. What is the relation of values in profile editor to trojan config?
![Profile Editor](https://i.imgur.com/xA58JNG.png)

### 2. Junkware detected, 360 detected as a virus?
- v0.0.1 [VirusTotal](https://www.virustotal.com/gui/file/247faa5d67592af7583a7ebd53654383d25e258de329ee145f7d8abbf2ba7034/detection)  
- v0.0.2 [VirusTotal](https://www.virustotal.com/gui/file/4f73d88689b13d46f087bccb6ed7da3935917e1980e80e8c92de64e081b6a537/detection)
- Also do not forget to the the md5 checksum and sha1 checksum
- If you want to see aboard, do not use any 360 software!

### 3. How to check md5 checksum?
#### 1. Windows
```
certutil -hashfile Trojan-Qt5-Windows.zip MD5
```
#### 2. macOS
```
md5 -r Trojan-Qt5-macOS.dmg
```
#### 3. Linux
```
md5sum Trojan-Qt5-Linux.AppImage
```

### 4. How to check SHA1 checksum?
#### 1. Windows
```
certutil -hashfile Trojan-Qt5-Windows.zip SHA1
```
#### 2. macOS
```
shasum Trojan-Qt5-macOS.dmg
```
#### 3. Linux
```
sha1sum Trojan-Qt5-Linux.AppImage
```

## Warning
Note: Trojan-Qt5 can ONLY be used for learning related technologies such as Qt/C++/Linux/CI/automation and use within the scope permitted by law. Any individual or group MAY NOT use Trojan-Qt5 for any violation of relevant laws and regulations.

Any attempt to download of any branch or distribution of Trojan-Qt5 constitutes your agreement that the author of the project will not be liable for any legal liability arising from your breach of the above guidelines.

## Special Thanks

This project is based on:

- [libQtShadowsocks](https://github.com/shadowsocks/libQtShadowsocks) LGPL-3.0
- [Shadowsocks-qt](https://github.com/trojan-gfw/trojan-qt) LGPL-3.0
- [trojan](https://github.com/trojan-gfw/trojan) GPL-3.0
- [trojan-qt](https://github.com/trojan-gfw/trojan-qt) GPL-3.0
- [Privoxy](https://www.privoxy.org) GPL-2.0
- [sysproxy](https://github.com/Noisyfox/sysproxy/) Apache-2.0

Thanks for @eejworks 's Fantastic UI Design(working in progress)