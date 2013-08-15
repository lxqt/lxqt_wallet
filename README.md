lxqt_wallet
===========

This projects seeks to create a secure storage of sensitive information that can be represented in key-pair values
without creating dependencies on kde with its kwallet secure storage facilities or gnome with its gnome-keyring secure storage facilities.

The project is divided into two parts.

The back end is written in C,has a dependency only on gcrypt and is designed to be used simply by dropping it in the middle
of a build tree and start using it.

The front end to the project is in Qt/C++.
The front end is plugin based and has a default plugin of the backend discussed above and optional additional plugins in kwallet and
a yet to be developed gnome-keyring.

This plugin architecture allows users of the library to target only one API and securely store information in different wallet systems.

The front end and its backend counter part are build specifically to be easily intergrated in other projects simply by dropping the folder
in the middle of the source tree and start using it.

This project is designed with small time independent developers who want to store their user's sensitive information but do not want to
tie themselves to kde or gnome.
