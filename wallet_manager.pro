#-------------------------------------------------
#
# Project created by QtCreator 2013-08-11T17:17:44
#
#-------------------------------------------------

QT       += core gui

TARGET = wallet_manager

TEMPLATE = app

INCLUDEPATH += /home/local/KDE4/include /usr/include /home/ink/src/build-lxde-wallet /home/ink/src/wallet_manager

LIBS += -lgcrypt -lkwalletbackend -L/home/local/KDE4/lib

OTHER_FILES += \
    frontend/README \
    frontend/CMakeLists.txt \
    backend/README \
    backend/CMakeLists.txt \
    test/README

HEADERS += \
    frontend/password_dialog.h \
    frontend/open_wallet_thread.h \
    frontend/lxqt_wallet_interface.h \
    frontend/lxqt_kwallet.h \
    frontend/lxqt_internal_wallet.h \
    frontend/changepassworddialog.h \
    backend/lxqtwallet.h \
    /home/ink/src/build-lxde-wallet/mainwindow.h

SOURCES += \
    frontend/password_dialog.cpp \
    frontend/open_wallet_thread.cpp \
    frontend/lxqt_wallet_interface.cpp \
    frontend/lxqt_kwallet.cpp \
    frontend/lxqt_internal_wallet.cpp \
    frontend/changepassworddialog.cpp \
    backend/lxqtwallet.c \
    /home/ink/src/build-lxde-wallet/mainwindow.cpp \
    /home/ink/src/build-lxde-wallet/main.cpp

FORMS += \
    frontend/password_dialog.ui \
    frontend/changepassworddialog.ui \
    /home/ink/src/build-lxde-wallet/mainwindow.ui

