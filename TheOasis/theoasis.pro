QT -= gui
QT += sql
QT += core

CONFIG += c++17 console
CONFIG -= app_bundle

DEFINES += ZMQ_STATIC
LIBS += -L$$PWD/../lib -lzmq -lnzmqt -lws2_32 -lIphlpapi
INCLUDEPATH += $$PWD/../include

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
		player.cpp \
		oasis.cpp \
		dbmanager.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
		player.h \
		oasis.h \
		dbmanager.h
