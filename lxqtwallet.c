
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

#include "lxqtwallet.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>

/*
 * below string MUST BE 5 bytes long
 */
#define VERSION "1.0.0"
#define VERSION_SIZE 5
/*
 * below string MUST BE 11 bytes long
 */
#define MAGIC_STRING "lxqt_wallet"
#define MAGIC_STRING_SIZE 11
#define MAGIC_STRING_BUFFER_SIZE 16
#define PASSWORD_SIZE 16
#define BLOCK_SIZE 16
#define IV_SIZE 16

struct lxqt_wallet_struct{
	char * application_name ;
	char * wallet_name ;
	char key[ PASSWORD_SIZE ] ;
	struct lxqt_key_value * wallet_data ;
	size_t wallet_data_size ;
	int wallet_modified ;
};

static char * _wallet_full_path( char * path_buffer,size_t path_buffer_size,const char * wallet_name,const char * application_name ) ;

static void _create_application_wallet_path( const char * application_name ) ;

static gcry_error_t _create_key( char output_key[ PASSWORD_SIZE ],const char * input_key,size_t input_key_length ) ;

static int _get_iv_from_wallet_header( char iv[ IV_SIZE ],const char * wallet_name,const char * application_name ) ;

static int _get_magic_string_from_header( char magic_string[ MAGIC_STRING_BUFFER_SIZE ],const char * wallet_path ) ;

static void _get_iv( char iv[ IV_SIZE ] ) ;

static void _create_magic_string_header( char magic_string[ MAGIC_STRING_BUFFER_SIZE ] ) ;

static int _wallet_is_not_compatible( char version_buffer[ VERSION_SIZE + 1 ] ) ;

int lxqt_wallet_wallet_size( lxqt_wallet_t wallet )
{
	return wallet->wallet_data_size ;
}

const struct lxqt_key_value * lxqt_wallet_read_all_key_values( lxqt_wallet_t wallet )
{
	return wallet->wallet_data ;
}

lxqt_wallet_error lxqt_wallet_create( const char * password,size_t password_length,
				      const char * wallet_name,const char * application_name )
{
	int fd ;
	char path[ PATH_MAX ] ;
	char iv[ IV_SIZE ] ;
	char magic_string[ MAGIC_STRING_BUFFER_SIZE ] ;
	char key[ PASSWORD_SIZE ] ;

	gcry_error_t r ;

	gcry_cipher_hd_t gcry_cipher_handle ;

	if( password == NULL || wallet_name == NULL || application_name == NULL ){
		return lxqt_wallet_invalid_argument ;
	}
	if( lxqt_wallet_exists( wallet_name,application_name ) == 0 ){
		return lxqt_wallet_wallet_exists ;
	}else{
		;
	}

	gcry_check_version( NULL ) ;
	gcry_control( GCRYCTL_INITIALIZATION_FINISHED,0 ) ;

	r = gcry_cipher_open( &gcry_cipher_handle,GCRY_CIPHER_AES128,GCRY_CIPHER_MODE_CBC,0 ) ;

	if( r != GPG_ERR_NO_ERROR ){
		return lxqt_wallet_gcry_cipher_open_failed ;
	}

	r = _create_key( key,password,password_length ) ;

	if( r != GPG_ERR_NO_ERROR ){
		gcry_cipher_close( gcry_cipher_handle ) ;
		return lxqt_wallet_failed_to_create_key_hash ;
	}

	r = gcry_cipher_setkey( gcry_cipher_handle,key,PASSWORD_SIZE ) ;

	if( r != GPG_ERR_NO_ERROR ){
		gcry_cipher_close( gcry_cipher_handle ) ;
		return lxqt_wallet_gcry_cipher_setkey_failed ;
	}

	_get_iv( iv ) ;

	r = gcry_cipher_setiv( gcry_cipher_handle,iv,IV_SIZE ) ;

	if( r != GPG_ERR_NO_ERROR ){
		gcry_cipher_close( gcry_cipher_handle ) ;
		return lxqt_wallet_gcry_cipher_setiv_failed ;
	}

	_create_application_wallet_path( application_name ) ;

	_wallet_full_path( path,PATH_MAX,wallet_name,application_name ) ;

	fd = open( path,O_WRONLY|O_CREAT,0600 ) ;

	if( fd == -1 ){
		gcry_cipher_close( gcry_cipher_handle ) ;
		return lxqt_wallet_failed_to_open_file ;
	}

	write( fd,iv,IV_SIZE ) ;

	_create_magic_string_header( magic_string ) ;

	r = gcry_cipher_encrypt( gcry_cipher_handle,magic_string,MAGIC_STRING_BUFFER_SIZE,NULL,0 ) ;
	gcry_cipher_close( gcry_cipher_handle ) ;

	if( r != GPG_ERR_NO_ERROR ){
		close( fd ) ;
		return lxqt_wallet_gcry_cipher_encrypt_failed ;
	}else{
		write( fd,magic_string,MAGIC_STRING_BUFFER_SIZE ) ;
		close( fd ) ;
		return lxqt_wallet_no_error ;
	}
}

static lxqt_wallet_error _open_exit( lxqt_wallet_error st,
				     struct lxqt_wallet_struct * w,gcry_cipher_hd_t gcry_cipher_handle )
{
	if( gcry_cipher_handle != 0 ){
		gcry_cipher_close( gcry_cipher_handle ) ;
	}
	if( w != NULL ){
		if( w->wallet_name != NULL ){
			free( w->wallet_name ) ;
		}
		if( w->application_name != NULL ){
			free( w->application_name ) ;
		}
		free( w ) ;
	}
	return st ;
}

lxqt_wallet_error lxqt_wallet_open( lxqt_wallet_t * wallet,const char * password,size_t password_length,
		      const char * wallet_name,const char * application_name )
{
	struct stat st ;
	size_t len ;
	size_t wallet_size ;

	gcry_error_t r ;
	gcry_cipher_hd_t gcry_cipher_handle ;

	char iv[ IV_SIZE ] ;
	char path[ PATH_MAX ] ;
	char magic_string[ MAGIC_STRING_BUFFER_SIZE ] ;
	char version_buffer[ VERSION_SIZE + 1 ] ;

	struct lxqt_key_value * e ;

	int fd;

	struct lxqt_wallet_struct * w ;

	if( wallet_name == NULL || application_name == NULL || wallet == NULL ){
		return lxqt_wallet_invalid_argument ;
	}

	*wallet = NULL ;

	w = malloc( sizeof( struct lxqt_wallet_struct ) ) ;

	if( w == NULL ){
		return lxqt_wallet_failed_to_allocate_memory ;
	}

	memset( w,'\0',sizeof( struct lxqt_wallet_struct ) ) ;

	len = strlen( wallet_name ) ;

	w->wallet_name = malloc( sizeof( char ) * ( len + 1 ) ) ;

	if( w->wallet_name == NULL ){
		return _open_exit( lxqt_wallet_failed_to_allocate_memory,w,0 ) ;
	}else{
		memcpy( w->wallet_name,wallet_name,len + 1 ) ;
	}

	len = strlen( application_name ) ;

	w->application_name = malloc( sizeof( char ) * ( len + 1 ) ) ;

	if( w->application_name == NULL ){
		return _open_exit( lxqt_wallet_failed_to_allocate_memory,w,0 ) ;
	}else{
		memcpy( w->application_name,application_name,len + 1 ) ;
	}

	gcry_check_version( NULL ) ;
	gcry_control( GCRYCTL_INITIALIZATION_FINISHED,0 ) ;

	r = gcry_cipher_open( &gcry_cipher_handle,GCRY_CIPHER_AES128,GCRY_CIPHER_MODE_CBC,0 ) ;

	if( r != GPG_ERR_NO_ERROR ){
		return _open_exit( lxqt_wallet_gcry_cipher_open_failed,w,0 ) ;
	}

	r = _create_key( w->key,password,password_length ) ;

	if( r != GPG_ERR_NO_ERROR ){
		return _open_exit( lxqt_wallet_failed_to_create_key_hash,w,gcry_cipher_handle ) ;
	}

	r = gcry_cipher_setkey( gcry_cipher_handle,w->key,PASSWORD_SIZE ) ;

	if( r != GPG_ERR_NO_ERROR ){
		return _open_exit( lxqt_wallet_gcry_cipher_setkey_failed,w,gcry_cipher_handle ) ;
	}

	if( _get_iv_from_wallet_header( iv,wallet_name,application_name ) ){
		;
	}else{
		return _open_exit( lxqt_wallet_failed_to_open_file,w,gcry_cipher_handle ) ;
	}

	r = gcry_cipher_setiv( gcry_cipher_handle,iv,IV_SIZE ) ;

	if( r != GPG_ERR_NO_ERROR ){
		return _open_exit( lxqt_wallet_gcry_cipher_setiv_failed,w,gcry_cipher_handle ) ;
	}

	_wallet_full_path( path,PATH_MAX,wallet_name,application_name ) ;

	if( _get_magic_string_from_header( magic_string,path ) ){
		;
	}else{
		return _open_exit( lxqt_wallet_failed_to_open_file,w,gcry_cipher_handle ) ;
	}

	r =  gcry_cipher_decrypt( gcry_cipher_handle,magic_string,MAGIC_STRING_BUFFER_SIZE,NULL,0 ) ;

	if( r != GPG_ERR_NO_ERROR ){
		return _open_exit( lxqt_wallet_gcry_cipher_decrypt_failed,w,gcry_cipher_handle ) ;
	}

	if( memcmp( magic_string,MAGIC_STRING,MAGIC_STRING_SIZE ) == 0 ){

		/*
		 * correct password was given and wallet is opened
		 */

		memcpy( version_buffer,magic_string + MAGIC_STRING_SIZE,VERSION_SIZE ) ;
		version_buffer[ VERSION_SIZE ] = '\0' ;
		if( _wallet_is_not_compatible( version_buffer ) ){
			return _open_exit( lxqt_wallet_incompatible_wallet,w,gcry_cipher_handle ) ;
		}

		stat( path,&st ) ;
		if( st.st_size <= IV_SIZE + MAGIC_STRING_BUFFER_SIZE ){
			/*
			 * wallet is empty
			 */
			w->wallet_data_size = 0 ;
			*wallet = w ;
			return _open_exit( lxqt_wallet_no_error,NULL,gcry_cipher_handle ) ;
		}else{
			fd = open( path,O_RDONLY ) ;
			if( fd != -1 ){
				wallet_size = st.st_size - ( IV_SIZE + MAGIC_STRING_BUFFER_SIZE ) ;
				w->wallet_data_size = wallet_size / sizeof( struct lxqt_key_value ) ;
				e = malloc( wallet_size ) ;
				if( e != NULL ){
					lseek( fd,IV_SIZE + MAGIC_STRING_BUFFER_SIZE,SEEK_SET ) ;
					read( fd,e,wallet_size ) ;
					gcry_cipher_decrypt( gcry_cipher_handle,e,wallet_size,NULL,0 ) ;
					w->wallet_data = e ;
					*wallet = w ;
					close( fd ) ;
					return _open_exit( lxqt_wallet_no_error,NULL,gcry_cipher_handle ) ;
				}else{
					close( fd ) ;
					return _open_exit( lxqt_wallet_failed_to_allocate_memory,w,gcry_cipher_handle ) ;
				}
			}else{
				return _open_exit( lxqt_wallet_failed_to_open_file,w,gcry_cipher_handle ) ;
			}
		}
	}else{
		return _open_exit( lxqt_wallet_wrong_password,w,gcry_cipher_handle ) ;
	}
}

void lxqt_wallet_read_key_value( lxqt_wallet_t wallet,const char * key,char ** value,size_t * value_size )
{
	struct lxqt_key_value * i = wallet->wallet_data ;
	struct lxqt_key_value * k ;
	struct lxqt_key_value * j = i + wallet->wallet_data_size ;

	if( key == NULL || wallet == NULL || value_size == NULL ){
		;
	}else{
		while( i != j ){
			k = i ;
			i++ ;
			if( strcmp( key,k->key ) == 0 ){
				*value = malloc( k->value_size ) ;
				memcpy( *value,k->value,k->value_size ) ;
				*value_size = k->value_size - 1 ;
			}
		}
	}
}

lxqt_wallet_error lxqt_wallet_add_key( lxqt_wallet_t wallet,const char * key,
				       const char * value,size_t key_value_length )
{
	struct lxqt_key_value * key_value ;
	struct lxqt_key_value * key_value_1 ;

	size_t len ;

	if( key == NULL || value == NULL || wallet == NULL ){
		return lxqt_wallet_invalid_argument ;
	}else{
		len = strlen( key ) ;
		if( len >= LXQT_WALLET_KEY_SIZE || len == 0 ||
			key_value_length >= LXQT_WALLET_VALUE_SIZE || key_value_length == 0 ){
			return lxqt_wallet_invalid_argument ;
		}else{
			len = sizeof( struct lxqt_key_value ) * ( wallet->wallet_data_size + 1 ) ;
			key_value = realloc( wallet->wallet_data,len ) ;
			if( key_value != NULL ){
				key_value_1 = key_value + wallet->wallet_data_size ;
				strncpy( ( char *)&key_value_1->key,key,LXQT_WALLET_KEY_SIZE ) ;
				memcpy( &key_value_1->value,value,key_value_length ) ;
				key_value_1->value[ key_value_length ] = '\0' ;
				key_value_1->value_size = key_value_length + 1 ;
				wallet->wallet_data = key_value ;
				wallet->wallet_data_size++ ;
				wallet->wallet_modified = 1 ;
				return lxqt_wallet_no_error ;
			}else{
				return lxqt_wallet_failed_to_allocate_memory ;
			}
		}
	}
}

lxqt_wallet_error lxqt_wallet_delete_key( lxqt_wallet_t wallet,const char * key )
{
	struct lxqt_key_value * k  ;

	int i ;
	int j ;

	size_t e ;

	if( key == NULL || wallet == NULL ){
		return lxqt_wallet_invalid_argument ;
	}

	k = wallet->wallet_data ;
	j = wallet->wallet_data_size ;

	for( i = 0 ; i < j ; i++ ){
		if( strcmp( key,k[ i ].key ) == 0 ){
			if( j == 1 ){
				free( wallet->wallet_data ) ;
				wallet->wallet_data = NULL ;
				wallet->wallet_data_size = 0 ;
				wallet->wallet_modified = 1 ;
			}else{
				memmove( k + i,k + i + 1,( j - 1 - i ) * sizeof( struct lxqt_key_value ) ) ;
				wallet->wallet_data_size-- ;
				e = wallet->wallet_data_size * sizeof( struct lxqt_key_value ) ;
				wallet->wallet_data = realloc( wallet->wallet_data,e ) ;
				wallet->wallet_modified = 1 ;
			}
			break ;
		}
	}

	return lxqt_wallet_no_error ;
}

lxqt_wallet_error lxqt_wallet_delete_wallet( const char * wallet_name,const char * application_name )
{
	char path[ PATH_MAX ] ;
	_wallet_full_path( path,PATH_MAX,wallet_name,application_name ) ;
	unlink( path ) ;
	return lxqt_wallet_no_error ;
}

static lxqt_wallet_error _close_exit( lxqt_wallet_error err,lxqt_wallet_t * w,gcry_cipher_hd_t gcry_cipher_handle )
{
	lxqt_wallet_t wallet = *w ;
	*w = NULL ;
	if( gcry_cipher_handle != 0 ){
		gcry_cipher_close( gcry_cipher_handle ) ;
	}
	if( wallet->wallet_data_size > 0 ){
		free( wallet->wallet_data ) ;
	}
	free( wallet->wallet_name ) ;
	free( wallet->application_name ) ;
	free( wallet ) ;
	return err ;
}

lxqt_wallet_error lxqt_wallet_close( lxqt_wallet_t * w )
{
	gcry_cipher_hd_t gcry_cipher_handle ;
	int fd ;
	char iv[ IV_SIZE ] ;
	char path[ PATH_MAX ] ;
	char path_1[ PATH_MAX ] ;
	char magic_string[ MAGIC_STRING_BUFFER_SIZE ] ;

	lxqt_wallet_t wallet ;
	size_t e ;

	gcry_error_t r ;

	if( w == NULL || *w == NULL ){
		return lxqt_wallet_invalid_argument ;
	}

	wallet = *w ;

	if( wallet->wallet_modified == 0 ){
		return _close_exit( lxqt_wallet_no_error,w,0 ) ;
	}

	r = gcry_cipher_open( &gcry_cipher_handle,GCRY_CIPHER_AES128,GCRY_CIPHER_MODE_CBC,0 ) ;

	if( r != GPG_ERR_NO_ERROR ){
		return _close_exit( lxqt_wallet_gcry_cipher_open_failed,w,0 ) ;
	}

	r = gcry_cipher_setkey( gcry_cipher_handle,wallet->key,PASSWORD_SIZE ) ;

	if( r != GPG_ERR_NO_ERROR ){
		return _close_exit( lxqt_wallet_gcry_cipher_setkey_failed,w,gcry_cipher_handle ) ;
	}

	_get_iv( iv ) ;

	r = gcry_cipher_setiv( gcry_cipher_handle,iv,IV_SIZE ) ;

	if( r != GPG_ERR_NO_ERROR ){
		return _close_exit( lxqt_wallet_gcry_cipher_setiv_failed,w,gcry_cipher_handle ) ;
	}

	_wallet_full_path( path,PATH_MAX,wallet->wallet_name,wallet->application_name ) ;

	snprintf( path_1,PATH_MAX,"%s.tmp",path ) ;

	fd = open( path_1,O_WRONLY|O_CREAT,0600 ) ;

	if( fd == -1 ){
		return _close_exit( lxqt_wallet_gcry_cipher_open_failed,w,gcry_cipher_handle ) ;
	}

	write( fd,iv,IV_SIZE ) ;

	_create_magic_string_header( magic_string ) ;

	r = gcry_cipher_encrypt( gcry_cipher_handle,magic_string,MAGIC_STRING_BUFFER_SIZE,NULL,0 ) ;

	if( r != GPG_ERR_NO_ERROR ){
		unlink( path_1 ) ;
		close( fd ) ;
		return _close_exit( lxqt_wallet_gcry_cipher_encrypt_failed,w,gcry_cipher_handle ) ;
	}

	write( fd,magic_string,MAGIC_STRING_BUFFER_SIZE ) ;

	e = wallet->wallet_data_size * sizeof( struct lxqt_key_value ) ;
	r = gcry_cipher_encrypt( gcry_cipher_handle,wallet->wallet_data,e,NULL,0 ) ;

	if( r != GPG_ERR_NO_ERROR ){
		unlink( path_1 ) ;
		close( fd ) ;
		return _close_exit( lxqt_wallet_gcry_cipher_encrypt_failed,w,gcry_cipher_handle ) ;
	}else{
		write( fd,wallet->wallet_data,wallet->wallet_data_size * sizeof( struct lxqt_key_value ) ) ;
		close( fd ) ;
		rename( path_1,path ) ;
		return _close_exit( lxqt_wallet_no_error,w,gcry_cipher_handle ) ;
	}
}

int lxqt_wallet_exists( const char * wallet_name,const char * application_name )
{
	struct stat st ;
	char path[ PATH_MAX ] ;
	if( wallet_name == NULL || application_name == NULL ){
		return lxqt_wallet_invalid_argument ;
	}else{
		_wallet_full_path( path,PATH_MAX,wallet_name,application_name ) ;
		return stat( path,&st ) ;
	}
}

void lxqt_wallet_application_wallet_path( char * path,size_t path_buffer_size,const char * application_name )
{
	struct passwd * pass = getpwuid( getuid() ) ;
	snprintf( path,path_buffer_size,"%s/.config/lxqt/wallets/%s/",pass->pw_dir,application_name ) ;
}

char * _wallet_full_path( char * path_buffer,size_t path_buffer_size,const char * wallet_name,const char * application_name )
{
	char path_1[ PATH_MAX ] ;
	lxqt_wallet_application_wallet_path( path_1,PATH_MAX,application_name ) ;
	snprintf( path_buffer,path_buffer_size,"%s/%s.lwt",path_1,wallet_name ) ;
	return path_buffer ;
}

static void _create_application_wallet_path( const char * application_name )
{
	char path[ PATH_MAX ] ;
	char * e ;

	lxqt_wallet_application_wallet_path( path,PATH_MAX,application_name ) ;

	for( e = path + 1 ; *e != '\0' ; e++ ){
		if( *e == '/' ){
			*e = '\0' ;
			mkdir( path,0755 ) ;
			*e = '/' ;
		}
	}
}

static gcry_error_t _create_key( char output_key[ PASSWORD_SIZE ],const char * input_key,size_t input_key_length )
{
	gcry_md_hd_t md ;
	unsigned char * digest ;

	gcry_error_t r = gcry_md_open( &md,GCRY_MD_SHA1,GCRY_MD_FLAG_SECURE ) ;

	if( r == GPG_ERR_NO_ERROR ){
		gcry_md_write( md,input_key,input_key_length ) ;
		gcry_md_final( md ) ;
		digest = gcry_md_read( md,0 ) ;
		if( digest == NULL ){
			r = !GPG_ERR_NO_ERROR ;
		}else{
			memcpy( output_key,digest,PASSWORD_SIZE ) ;
		}
		gcry_md_close( md ) ;
	}else{
		;
	}

	return r ;
}

static int _get_iv_from_wallet_header( char iv[ IV_SIZE ],const char * wallet_name,const char * application_name )
{
	int fd ;
	char path[ PATH_MAX ] ;
	if( lxqt_wallet_exists( wallet_name,application_name ) == 0 ){
		_wallet_full_path( path,PATH_MAX,wallet_name,application_name ) ;
		fd = open( path,O_RDONLY ) ;
		if( fd == -1 ){
			return 0 ;
		}else{
			read( fd,iv,IV_SIZE ) ;
			return 1 ;
		}
	}else{
		return 0 ;
	}
}

int _get_magic_string_from_header( char magic_string[ MAGIC_STRING_BUFFER_SIZE ],const char * wallet_path )
{
	int fd = open( wallet_path,O_RDONLY ) ;
	if( fd == -1 ){
		return 0 ;
	}else{
		lseek( fd,IV_SIZE,SEEK_SET ) ;
		read( fd,magic_string,MAGIC_STRING_BUFFER_SIZE ) ;
		close( fd ) ;
		return 1 ;
	}
}

static void _get_iv( char iv[ IV_SIZE ] )
{
	int fd ;
	fd = open( "/dev/urandom",O_RDONLY ) ;
	read( fd,iv,IV_SIZE ) ;
	close( fd ) ;
}

static void _create_magic_string_header( char magic_string[ MAGIC_STRING_BUFFER_SIZE ] )
{
	/*
	 * write 11 bytes of magic string
	 */
	memcpy( magic_string,MAGIC_STRING,MAGIC_STRING_SIZE ) ;
	/*
	 * write version information in the remaining 5 bytes of the 16 byte buffer
	 */
	memcpy( magic_string + MAGIC_STRING_SIZE,VERSION,VERSION_SIZE ) ;
}

static int _wallet_is_not_compatible( char version_buffer[ VERSION_SIZE + 1 ] )
{
	if( version_buffer ){;}
	return 0 ;
}
