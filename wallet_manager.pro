#-------------------------------------------------
#
# Project created by QtCreator 2013-08-11T17:17:44
#
#-------------------------------------------------

QT       += core gui

TARGET = wallet_manager


TEMPLATE = app


SOURCES += main.cpp lxqtwallet.c \
    lxqt_wallet_interface.cpp \
    lxqt_internal_wallet.cpp \
    password_dialog.cpp \
    mainwindow.cpp \
    lxqt_kwallet.cpp \
    open_wallet_thread.cpp \
    changepassworddialog.cpp

INCLUDEPATH += /home/local/KDE4/include /usr/include

LIBS += -lgcrypt -lkwalletbackend -L/home/local/KDE4/lib

HEADERS += \
    lxqt_wallet_interface.h \
    lxqt_internal_wallet.h \
    password_dialog.h \
    mainwindow.h \
    lxqt_kwallet.h \
    open_wallet_thread.h \
    changepassworddialog.h

FORMS += \
    password_dialog.ui \
    mainwindow.ui \
    changepassworddialog.ui
