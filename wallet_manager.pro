#-------------------------------------------------
#
# Project created by QtCreator 2013-08-11T17:17:44
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = wallet_manager
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp lxqtwallet.c

INCLUDEPATH += /home/local/KDE4/include /usr/include

LIBS += -lgcrypt

HEADERS += \
    lxqt_wallet_manager.h \
    lxqt_wallet_interface.h
