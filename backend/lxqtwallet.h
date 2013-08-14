
/***********************************************************************************
 *   Copyright 2013 by mhogomchungu <mhogomchungu@gmail.com>		           *
 *                                                                                 *
 *                                                                                 *
 *   This library is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public                    *
 *   License as published by the Free Software Foundation; either                  *
 *   version 2.1 of the License, or (at your option) any later version.            *
 *                                                                                 *
 *   This library is distributed in the hope that it will be useful,               *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of                *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU             *
 *   Lesser General Public License for more details.                               *
 *                                                                                 *
 *   You should have received a copy of the GNU Lesser General Public              *
 *   License along with this library.  If not, see <http://www.gnu.org/licenses/>. *
 ***********************************************************************************/

#ifndef LXQTWALLET_H
#define LXQTWALLET_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <gcrypt.h>

typedef struct lxqt_wallet_struct * lxqt_wallet_t ;

/*
 * Below structures are exposed to be used with lxqt_wallet_read_all_key_values() function
 * The values can be changed to anything as long as the size of the resulting structure
 * is divisible by 16
 *
 * Stored values are NULL terminated and hence the maximum allowed characters is one less of
 * the size of the variable.
 */
#define LXQT_WALLET_KEY_SIZE 512
#define LXQT_WALLET_VALUE_SIZE 1036

/*
 * error values
 */
typedef enum{
	lxqt_wallet_no_error = 0,
	lxqt_wallet_wrong_password,
	lxqt_wallet_wallet_exists,
	lxqt_wallet_gcry_cipher_open_failed,
	lxqt_wallet_gcry_cipher_setkey_failed,
	lxqt_wallet_gcry_cipher_setiv_failed,
	lxqt_wallet_gcry_cipher_encrypt_failed,
	lxqt_wallet_gcry_cipher_decrypt_failed,
	lxqt_wallet_failed_to_open_file,
	lxqt_wallet_failed_to_allocate_memory,
	lxqt_wallet_invalid_argument,
	lxqt_wallet_incompatible_wallet,
	lxqt_wallet_failed_to_create_key_hash,
	lxqt_wallet_libgcrypt_version_mismatch
}lxqt_wallet_error;


struct lxqt_key_value{
	char key[ LXQT_WALLET_KEY_SIZE ] ;
	char value[ LXQT_WALLET_VALUE_SIZE ] ;
	u_int32_t value_size ;
};

/*
 * maximum values for key and key_value are shown and explained above.
 * Larger argument sizes will result in "lxqt_wallet_invalid_argument" error.
 */
lxqt_wallet_error lxqt_wallet_add_key( lxqt_wallet_t,const char * key,const void * key_value,size_t key_value_length ) ;

/*
 * open "wallet_name" wallet of application "application_name" using a password of size password_length.
 *
 * The rest of the API except lxqt_wallet_create() are undefined if this function returns a non zero number
 */
lxqt_wallet_error lxqt_wallet_open( lxqt_wallet_t *,const char * password,size_t password_length,
		      const char * wallet_name,const char * application_name ) ;

/*
 * create a new wallet named "wallet_name" owned by application "application_name" using a password "password" of size "password_length".
 */
lxqt_wallet_error lxqt_wallet_create( const char * password,size_t password_length,const char * wallet_name,const char * application_name ) ;

/*
 * returns a value of a key.
 * caller of this function is responsible for the returned buffer and should free() it when done with it
 * NULL is returned if the key could not be found
 */
void lxqt_wallet_read_key_value( lxqt_wallet_t,const char * key,void ** value,size_t * value_size ) ;

/*
 * delete a key.
 */
lxqt_wallet_error lxqt_wallet_delete_key( lxqt_wallet_t,const char * key ) ;

/*
 *
 * delete a wallet named "wallet_name" of an application named "application_name" exists
 */
lxqt_wallet_error lxqt_wallet_delete_wallet( const char * wallet_name,const char * application_name ) ;

/*
 * close a wallet handled.
 */
lxqt_wallet_error lxqt_wallet_close( lxqt_wallet_t * ) ;

/*
 * Check if a wallet named "wallet_name" of an application named "application_name" exists
 * returns 0 if the wallet exist
 */
int lxqt_wallet_exists( const char * wallet_name,const char * application_name ) ;

/*
 * returns a path to where the wallet file is stored.
 * on return path_buffer will contain something like "/home/$USER/.local/application_name/wallets"
 */
void lxqt_wallet_application_wallet_path( char * path_buffer,size_t path_buffer_size,const char * application_name ) ;

/*
 * returns number of keys in the wallet
 */
int lxqt_wallet_wallet_size( lxqt_wallet_t ) ;

/*
 * get a list of all key-values in the wallet to make it easy to iterate over them.
 * see example below for an example use case
 */
const struct lxqt_key_value * lxqt_wallet_read_all_key_values( lxqt_wallet_t ) ;

/*
 * search for key and if found,delete it and then put key1 and its value at the deleted place.
 * key1 will just be added if key was not found
 */
void lxqt_wallet_replace_key( lxqt_wallet_t,const char * key,const char * key1,const void * value,size_t value_size ) ;

/*
 * chance the wallet password
 */
lxqt_wallet_error lxqt_wallet_change_wallet_password( lxqt_wallet_t,const char * new_password,size_t new_password_size ) ;

/*
 * below is a complete program that tests the library functionality,it may be useful as a
 * source of how the use the API
 */
#if 0

#include "lxqtwallet.h"
#include <string.h>

/*
 * This source file shows how the library can be used
 */

static const char * wallet_name    = "wallet_name" ;
static const char * application_name = "application_name" ;

#define stringsAreEqual( x,y ) strcmp( x,y ) == 0

int main( int argc,char * argv[] )
{
	lxqt_wallet_t wallet ;
	lxqt_wallet_error r = lxqt_wallet_no_error ;

	char * e = NULL ;
	const char * f ;
	const char * z ;
	const char * command ;
	const struct lxqt_key_value * values ;
	int j ;
	int k ;
	size_t q ;

	if( argc  < 2 ){
		printf( "wrong number of arguments\n" ) ;
		return lxqt_wallet_invalid_argument ;
	}

	command = argv[ 1 ] ;

	if( stringsAreEqual( command,"create" ) ){
		/*
		 * create a new wallet
		 * additional arguments: password
		 * eg ./wallet create xxx
		 */
		if( argc < 3 ){
			r = lxqt_wallet_invalid_argument ;
		}else{
			f = argv[ 2 ] ;
			r = lxqt_wallet_create( f,strlen( f ),wallet_name,application_name ) ;
		}
	}else if( stringsAreEqual( command,"add" ) ){
		/*
		 * add an entry to the wallet
		 * additional arguments: password key key_value
		 * eg ./wallet add xxx alicia abc
		 */
		if( argc < 3 ){
			r = lxqt_wallet_invalid_argument ;
		}else{
			f = argv[ 2 ] ;
			r = lxqt_wallet_open( &wallet,f,strlen( f ),wallet_name,application_name ) ;
		}

		if( r != lxqt_wallet_no_error ){
			if( r == lxqt_wallet_wrong_password ){
				puts( "wrong password" ) ;
			}else{
				puts( "general error has occured" ) ;
			}
		}else{
			f = argv[ 3 ] ;
			z = argv[ 4 ] ;
			r = lxqt_wallet_add_key( wallet,f,z,strlen( z ) ) ;

			lxqt_wallet_close( &wallet ) ;
		}
	}else if( stringsAreEqual( command,"read" ) ){
		/*
		 * read an value in a wallet through its key
		 * additional arguments: password key key_value
		 * eg ./wallet read xxx alicia
		 */
		if( argc < 3 ){
			r = lxqt_wallet_invalid_argument ;
		}else{
			f = argv[ 2 ] ;
			r = lxqt_wallet_open( &wallet,f,strlen( f ),wallet_name,application_name ) ;
		}

		if( r != lxqt_wallet_no_error ){
			if( r == lxqt_wallet_wrong_password ){
				puts( "wrong password" ) ;
			}else{
				puts( "general error has occured" ) ;
			}
		}else{
			f = argv[ 3 ] ;
			e = NULL ;
			lxqt_wallet_read_key_value( wallet,f,&e,&q ) ;

			if( e ){
				printf( "key=%s:value=%s\n",f,e ) ;
				free( e ) ;
			}else{
				printf( "key=%s:value=(NULL)\n",f ) ;
			}

			lxqt_wallet_close( &wallet ) ;
		}
	}else if( stringsAreEqual( command,"print" ) ){
		/*
		 * print all entries in the wallet
		 * additional arguments: password
		 * eg ./wallet print xxx alicia
		 */
		if( argc < 3 ){
			r = lxqt_wallet_invalid_argument ;
		}else{
			f = argv[ 2 ] ;
			r = lxqt_wallet_open( &wallet,f,strlen( f ),wallet_name,application_name ) ;
		}

		if( r == lxqt_wallet_no_error ){
			values = lxqt_wallet_read_all_key_values( wallet ) ;
			j = lxqt_wallet_wallet_size( wallet ) ;

			for( k = 0 ; k < j ; k ++ ){
				printf( "key=%s: value=%s\n",values[ k ].key,values[ k ].value ) ;
			}

			lxqt_wallet_close( &wallet ) ;
		}else{
			if( r == lxqt_wallet_wrong_password ){
				puts( "wrong password" ) ;
			}else{
				puts( "general error has occured" ) ;
			}
		}
	}else if( stringsAreEqual( command,"delete" ) ){
		/*
		 * delete a key from a wallet
		 * additional arguments: password key
		 * eg ./wallet delete xxx alicia
		 */
		if( argc < 3 ){
			r = lxqt_wallet_invalid_argument ;
		}else{
			f = argv[ 2 ] ;
			r = lxqt_wallet_open( &wallet,f,strlen( f ),wallet_name,application_name ) ;
		}

		if( r == lxqt_wallet_no_error ){
			f = argv[ 3 ] ;
			r = lxqt_wallet_delete_key( wallet,f ) ;
			lxqt_wallet_close( &wallet ) ;
		}else{
			if( r == lxqt_wallet_wrong_password ){
				puts( "wrong password" ) ;
			}else{
				puts( "general error has occured" ) ;
			}
		}
	}

	return r ;
}

#endif

#ifdef __cplusplus
}
#endif

#endif
