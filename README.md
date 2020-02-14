<h1 align="center">
  <img src="https://github.com/TheWanderingCoel/Trojan-Qt5/blob/master/icons/trojan-qt5.png?raw=true" alt="Trojan-Qt5" width="200">
  <br>
  Trojan-Qt5
  <br>
</h1>

A cross-platform trojan GUI client

[![Build status](https://travis-ci.com/TheWanderingCoel/Trojan-Qt5.svg?branch=master)](https://travis-ci.com/TheWanderingCoel/Trojan-Qt5)
[![Build status](https://ci.appveyor.com/api/projects/status/shjhg9mlvc3c74ek?svg=true)](https://ci.appveyor.com/project/CoelWu/trojan-qt5)
[![HitCount](http://hits.dwyl.io/TheWanderingCoel/Trojan-Qt5.svg)](http://hits.dwyl.io/TheWanderingCoel/Trojan-Qt5)
[![Download status](https://img.shields.io/github/downloads/TheWanderingCoel/Trojan-Qt5/total.svg)](https://github.com/TheWanderingCoel/Trojan-Qt5/releases)
[![License](https://img.shields.io/badge/license-GPL%20V3-blue.svg?longCache=true)](https://www.gnu.org/licenses/gpl-3.0.en.html)

## Install

You can download from [release](https://github.com/TheWanderingCoel/Trojan-Qt5/releases) page

## Compiling

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
sudo apt-get install libgl-dev git build-essential g++ python-dev autotools-dev libicu-dev libbz2-dev checkinstall zlib1g-dev -y;
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

## Special Thanks

This project is based on:

- [libQtShadowsocks](https://github.com/shadowsocks/libQtShadowsocks) LGPL-3.0
- [Shadowsocks-qt](https://github.com/trojan-gfw/trojan-qt) LGPL-3.0
- [trojan](https://github.com/trojan-gfw/trojan) GPL-3.0
- [trojan-qt](https://github.com/trojan-gfw/trojan-qt) GPL-3.0
- [Privoxy](https://www.privoxy.org) GPL-2.0
- [sysproxy](https://github.com/Noisyfox/sysproxy/) Apache-2.0