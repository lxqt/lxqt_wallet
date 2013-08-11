
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


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <gcrypt.h>

typedef struct lxqt_wallet_struct * lxqt_wallet_t ;

/*
 * Below structures are exposed to be used with lxqt_wallet_read_all_key_values() function
 */
#define KEY_SIZE 44 
#define VALUE_SIZE 512

struct lxqt_key_value{
	char key[ KEY_SIZE ] ;
	char value[ VALUE_SIZE ] ;
	u_int32_t value_size ;
};

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
	lxqt_wallet_failed_to_create_key_hash
}lxqt_wallet_error;
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
char * lxqt_wallet_read_key_value( lxqt_wallet_t,const char * key ) ;

/*
 * maximum size of the key is 43 bytes
 * maximum size of key value is 511 bytes
 * Larger argument sizes will result in "lxqt_wallet_invalid_argument" error.
 */
lxqt_wallet_error lxqt_wallet_add_key( lxqt_wallet_t,const char * key,const char * key_value,size_t key_value_length ) ;

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
lxqt_wallet_error lxqt_wallet_close( lxqt_wallet_t ) ;

/*
 * Check if a wallet named "wallet_name" of an application named "application_name" exists
 * returns 0 if the wallet does not exist
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

#if 0

examples.

example 1: open a wallet and print all keys and their values

void foo( void )
{
	int j ;
	int k ;
	const struct lxqt_key_value * values ;
	lxqt_wallet_t wallet ;
	const char * password = "rrr" ;
	size_t pass_len = strlen( password ) ;
	int r = lxqt_wallet_open( &wallet,password,pass_len,"wallet_name","application_name" ) ;
	
	if( r == 0 ){
		values = lxqt_wallet_read_all_key_values( wallet ) ;
		j = lxqt_wallet_wallet_size( wallet ) ;
		
		for( k = 0 ; k < j ; k ++ ){
			printf( "key=%s:value=%s\n",values[ k ].key,values[ k ].value ) ;
		}
			
		lxqt_wallet_close( wallet ) ;
	}
}
#endif



