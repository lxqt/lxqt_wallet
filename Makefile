

all:
	gcc -D_FILE_OFFSET_BITS=64 -Wall -Wextra -pedantic -c -o lxqtwallet.o lxqtwallet.c

	gcc -D_FILE_OFFSET_BITS=64 -Wall -Wextra -pedantic -shared -Wl,-soname,liblxqtwallet.so.1.0.0 -o liblxqtwallet.so.1.0.0 lxqtwallet.o 

	ln -sf liblxqtwallet.so.1.0.0 liblxqtwallet.so
	
	gcc -D_FILE_OFFSET_BITS=64 -Wall -Wextra -pedantic -o wallet wallet.c -L. -llxqtwallet -lgcrypt 