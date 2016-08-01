lxqt_wallet
===========

This project seeks to give a functionality for secure storage of information that can be presented in key-values pair
like user names-passwords pairs.

Currently the project can store the information in KDE's kwallet,GNOME's secret service or in an internal system that use libgcrypt
as its cryptographic backend.

The internal secure storage system allows the functionality to be provided without dependencies on KDE or gnome libraries.

This project is designed to be used by other projects simply by adding the source folder in the build system and start using it.

The front end is build on Qt/C++ and has an optional dependency on KDE's kwallet and GNOME's libsecret library.

The project allows other Qt based project's to target one API and use it for secure storage of information in different
secure storage systems.

GUI windows produces are the project are:

https://github.com/mhogomchungu/lxqt_wallet/blob/master/images/lxqt_wallet.jpeg

https://github.com/mhogomchungu/lxqt_wallet/blob/master/images/lxqt_wallet1.jpeg

https://github.com/mhogomchungu/lxqt_wallet/blob/master/images/lxqt_wallet2.jpeg
