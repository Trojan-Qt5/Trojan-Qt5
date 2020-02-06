#
# Copyright (C) 2015-2016 Symeon Huang <hzwhuang@gmail.com>
# Copyright (C) 2019-2020 TheWanderingCoel <thewanderingcoel@protonmail.com>
#
# Trojan-Qt5 is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published
# by the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# Trojan-Qt5 is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Trojan-Qt5; see the file LICENSE. If not, see
# <http://www.gnu.org/licenses/>.
#

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

ICON = $$PWD/icons/trojan-qt5.icns

TARGET = trojan-qt5

CONFIG += c++11
CONFIG += sdk_no_version_check
CONFIG += lrelease
CONFIG += embed_translations

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
#DEFINES += QT_DEPRECATED_WARNINGS

# Define App Version
DEFINES += "APP_VERSION=\"\\\"0.0.1\\\"\""

# Trojan
#DEFINES += ENABLE_MYSQL
DEFINES += TCP_FASTOPEN
#DEFINES += TCP_FASTOPEN_CONNECT
DEFINES += ENABLE_SSL_KEYLOG
#DEFINES += ENABLE_NAT
DEFINES += ENABLE_TLS13_CIPHERSUITES
#DEFINES += ENABLE_REUSE_PORT

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32 {
    INCLUDEPATH += $$PWD\src\trojan\src
    INCLUDEPATH += C:\Libraries\ZBar\include
    INCLUDEPATH += C:\Libraries\boost_1_71_0\include\boost-1_71
    INCLUDEPATH += C:\Libraries\OpenSSL-Win64\include
    INCLUDEPATH += C:\Libraries\QREncode\include
    LIBS += -LC:\Libraries\ZBar\lib -llibzbar-0
    LIBS += -LC:\Libraries\OpenSSL-Win64\lib -llibcrypto -llibssl
    LIBS += -LC:\Libraries\boost_1_71_0\lib -lboost_system
    LIBS += -LC:\Libraries\QREncode\lib -lqrcodelib
    LIBS += -lwsock32 -lws2_32
    LIBS += -lCrypt32
    DEFINES += WIN32_LEAN_AND_MEAN
}

mac {
    INCLUDEPATH += $$PWD/src/trojan/src
    INCLUDEPATH += /usr/local/opt/zbar/include
    INCLUDEPATH += /usr/local/opt/qrencode/include
    INCLUDEPATH += /usr/local/opt/openssl@1.1/include
    INCLUDEPATH += /usr/local/opt/boost/include
    LIBS += -L/usr/local/opt/zbar/lib -lzbar
    LIBS += -L/usr/local/opt/qrencode/lib -lqrencode
    LIBS += -L/usr/local/opt/openssl@1.1/lib -lcrypto -lssl
    LIBS += -L/usr/local/opt/boost/lib -lboost_system
    LIBS += -framework Security -framework Cocoa
}

unix:!mac {
    INCLUDEPATH += $$PWD/src/trojan/src
    INCLUDEPATH += /usr/local/zbar/include
    INCLUDEPATH += /usr/local/qrencode/include
    INCLUDEPATH += /usr/local/openssl/include
    INCLUDEPATH += /usr/local/boost/include
    LIBS += -L/usr/local/zbar/lib -lzbar
    LIBS += -L/usr/local/qrencode/lib -lqrencode
    LIBS += -L/usr/local/openssl/lib -lcrypto -lssl
    LIBS += -L/usr/local/boost/lib -lboost_system

    isEmpty(PREFIX) {
        PREFIX = /usr/local
    }

    target.path = $$PREFIX/bin
    shortcutfiles.files = src/trojan-qt5.desktop
    shortcutfiles.path = $$PREFIX/share/applications/
    data.files += icons/trojan-qt5.png
    data.path = $$PREFIX/share/hicolor/512x512/trojan-qt5.png

    INSTALLS += shortcutfiles
    INSTALLS += data
}

!isEmpty(target.path): INSTALLS += target

SOURCES += \
    src/servicethread.cpp \
    src/addresstester.cpp \
    src/confighelper.cpp \
    src/connection.cpp \
    src/connectionitem.cpp \
    src/connectiontablemodel.cpp \
    src/editdialog.cpp \
    src/ip4validator.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/portvalidator.cpp \
    src/qrcodecapturer.cpp \
    src/qrwidget.cpp \
    src/settingsdialog.cpp \
    src/sharedialog.cpp \
    src/tqprofile.cpp \
    src/statusnotifier.cpp \
    src/trojanvalidator.cpp \
    src/urihelper.cpp \
    src/uriinputdialog.cpp \
    src/trojan/src/core/authenticator.cpp \
    src/trojan/src/core/config.cpp \
    src/trojan/src/core/log.cpp \
    src/trojan/src/core/service.cpp \
    src/trojan/src/core/version.cpp \
    src/trojan/src/proto/socks5address.cpp \
    src/trojan/src/proto/trojanrequest.cpp \
    src/trojan/src/proto/udppacket.cpp \
    src/trojan/src/session/clientsession.cpp \
    src/trojan/src/session/forwardsession.cpp \
    src/trojan/src/session/natsession.cpp \
    src/trojan/src/session/serversession.cpp \
    src/trojan/src/session/session.cpp \
    src/trojan/src/session/udpforwardsession.cpp \
    src/trojan/src/ssl/ssldefaults.cpp \
    src/trojan/src/ssl/sslsession.cpp


HEADERS += \
    src/servicethread.h \
    src/addresstester.h \
    src/confighelper.h \
    src/connection.h \
    src/connectionitem.h \
    src/connectiontablemodel.h \
    src/editdialog.h \
    src/ip4validator.h \
    src/mainwindow.h \
    src/portvalidator.h \
    src/qrcodecapturer.h \
    src/qrwidget.h \
    src/settingsdialog.h \
    src/sharedialog.h \
    src/tqprofile.h \
    src/statusnotifier.h \
    src/trojanvalidator.h \
    src/urihelper.h \
    src/uriinputdialog.h \
    src/trojan/src/core/authenticator.h \
    src/trojan/src/core/config.h \
    src/trojan/src/core/log.h \
    src/trojan/src/core/service.h \
    src/trojan/src/core/version.h \
    src/trojan/src/proto/socks5address.h \
    src/trojan/src/proto/trojanrequest.h \
    src/trojan/src/proto/udppacket.h \
    src/trojan/src/session/clientsession.h \
    src/trojan/src/session/forwardsession.h \
    src/trojan/src/session/natsession.h \
    src/trojan/src/session/serversession.h \
    src/trojan/src/session/session.h \
    src/trojan/src/session/udpforwardsession.h \
    src/trojan/src/ssl/ssldefaults.h \
    src/trojan/src/ssl/sslsession.h

FORMS += \
    ui/editdialog.ui \
    ui/mainwindow.ui \
    ui/settingsdialog.ui \
    ui/sharedialog.ui \
    ui/uriinputdialog.ui

EXTRA_TRANSLATIONS += \
    i18n/trojan-qt5_zh_CN.ts \
    i18n/trojan-qt5_zh_TW.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin

RESOURCES += \
    icons.qrc
