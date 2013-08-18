/*
 * copyright: 2013
 * name : mhogo mchungu
 * email: mhogomchungu@gmail.com
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

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

#define LXQT_WALLET_KEY_SIZE 76

struct lxqt_wallet_struct{
	char * application_name ;
	char * wallet_name ;
	char key[ PASSWORD_SIZE ] ;
	char * wallet_data ;
	size_t wallet_data_size ;
	size_t wallet_data_entry_count ;
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
	if( wallet == NULL ){
		return -1 ;
	}else{
		return wallet->wallet_data_size ;
	}
}

int lxqt_wallet_wallet_entry_count( lxqt_wallet_t wallet )
{
	if( wallet == NULL ){
		return -1 ;
	}else{
		return wallet->wallet_data_entry_count ;
	}
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
		unlink( path ) ;
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

lxqt_wallet_error lxqt_wallet_change_wallet_password( lxqt_wallet_t wallet,const char * new_key,size_t new_key_size )
{
	char key[ PASSWORD_SIZE ] ;
	gcry_error_t r ;

	if( wallet == NULL || new_key == NULL ){
		return lxqt_wallet_invalid_argument ;
	}else{
		r = _create_key( key,new_key,new_key_size ) ;
		if( r != GPG_ERR_NO_ERROR ){
			return lxqt_wallet_failed_to_create_key_hash ;
		}else{
			memcpy( wallet->key,key,PASSWORD_SIZE ) ;
			wallet->wallet_modified = 1 ;
			return lxqt_wallet_no_error ;
		}
	}
}

lxqt_wallet_error lxqt_wallet_open( lxqt_wallet_t * wallet,const char * password,size_t password_length,
		      const char * wallet_name,const char * application_name )
{
	struct stat st ;
	size_t len ;

	gcry_error_t r ;
	gcry_cipher_hd_t gcry_cipher_handle ;

	char iv[ IV_SIZE ] ;
	char path[ PATH_MAX ] ;
	char magic_string[ MAGIC_STRING_BUFFER_SIZE ] ;
	char version_buffer[ VERSION_SIZE + 1 ] ;
	char load_header[ BLOCK_SIZE ] ;

	char * e ;

	int fd;

	struct lxqt_wallet_struct * w ;

	if( wallet_name == NULL || application_name == NULL || wallet == NULL ){
		return lxqt_wallet_invalid_argument ;
	}

	gcry_check_version( NULL ) ;

	gcry_control( GCRYCTL_INITIALIZATION_FINISHED,0 ) ;

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
			*wallet = w ;
			return _open_exit( lxqt_wallet_no_error,NULL,gcry_cipher_handle ) ;
		}else{
			fd = open( path,O_RDONLY ) ;
			if( fd != -1 ){
				lseek( fd,IV_SIZE + MAGIC_STRING_BUFFER_SIZE,SEEK_SET ) ;
				read( fd,load_header,BLOCK_SIZE ) ;

				gcry_cipher_decrypt( gcry_cipher_handle,load_header,BLOCK_SIZE,NULL,0 ) ;

				memcpy( &w->wallet_data_size,load_header,sizeof( u_int32_t ) ) ;

				memcpy( &w->wallet_data_entry_count,load_header + sizeof( u_int32_t ),sizeof( u_int32_t ) ) ;

				len = st.st_size - ( IV_SIZE + MAGIC_STRING_BUFFER_SIZE + BLOCK_SIZE ) ;

				e = malloc( len ) ;
				if( e != NULL ){
					read( fd,e,len ) ;
					gcry_cipher_decrypt( gcry_cipher_handle,e,len,NULL,0 ) ;
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

void lxqt_wallet_read_key_value( lxqt_wallet_t wallet,const char * key,void ** value,size_t * value_size )
{
	const char * e ;
	const char * z ;
	char * r ;
	
	size_t k = 0 ;
	size_t i = 0 ;
	
	u_int32_t key_len ;
	u_int32_t key_value_len ;
	u_int32_t key_len_1 ;
	
	if( key == NULL || wallet == NULL || value_size == NULL ){
		;
	}else{
		e = wallet->wallet_data ;
		z = e ;
		k = wallet->wallet_data_size ;
		key_len_1 = strlen( key ) ;
		
		while( i < k ){
			memcpy( &key_len,e,sizeof( u_int32_t ) ) ;
			memcpy( &key_value_len,e + sizeof( u_int32_t ) + key_len,sizeof( u_int32_t ) ) ;
			
			if( strncmp( key,e + sizeof( u_int32_t ),key_len_1 ) == 0 ){
				r = malloc( key_value_len + 1 ) ;
				if( r != NULL ){
					memcpy( r,e + sizeof( u_int32_t ) + key_len + sizeof( u_int32_t ),key_value_len ) ;
					*( r + key_value_len ) = '\0' ;
					*value = r ;
					*value_size = key_value_len ;
					break ;
				}
			}else{
				i = i + sizeof( u_int32_t ) + key_len + sizeof( u_int32_t ) + key_value_len ;
				e = z + i ;
			}
		}
	}
}

lxqt_wallet_error lxqt_wallet_add_key( lxqt_wallet_t wallet,const char * key,
				       const void * value,u_int32_t key_value_length )
{
	char * e ;
	char * f ;
	size_t len ;
	u_int32_t key_len ;
	
	if( key == NULL || wallet == NULL ){
		return lxqt_wallet_invalid_argument ;
	}else{
		key_len = strlen( key ) ;
		if( key_len == 0 ){
			return lxqt_wallet_invalid_argument ;
		}else{
			if( value == NULL || key_value_length == 0 ){
				key_value_length = 0 ;
				value = "" ;
			}
			
			len = sizeof( u_int32_t ) + key_len + sizeof( u_int32_t ) + key_value_length ;
			f = realloc( wallet->wallet_data,wallet->wallet_data_size + len ) ;

			if( f != NULL ){
				
				e = f + wallet->wallet_data_size ;
				
				memcpy( e,&key_len,sizeof( u_int32_t ) ) ;
				memcpy( e + sizeof( u_int32_t ),key,key_len ) ;
				memcpy( e + sizeof( u_int32_t ) + key_len,&key_value_length,sizeof( u_int32_t ) ) ;
				memcpy( e + sizeof( u_int32_t ) + key_len + sizeof( u_int32_t ),value,key_value_length ) ;
				
				wallet->wallet_data_size += len ;
				wallet->wallet_modified = 1 ;
				wallet->wallet_data = f ;
				wallet->wallet_data_entry_count++ ;
				
				return lxqt_wallet_no_error ;
			}else{
				return lxqt_wallet_failed_to_allocate_memory ;
			}
		}
	}
}

lxqt_wallet_key_values_t * lxqt_wallet_read_all_keys( lxqt_wallet_t wallet )
{
	const char * e ;
	const char * z ;
	
	size_t k = 0 ;
	size_t i = 0 ;
	size_t q = 0 ;
	
	u_int32_t key_len ;
	u_int32_t key_value_len ;
	
	lxqt_wallet_key_values_t * entries ;
	
	if( wallet == NULL ){
		return NULL ;
	}else{
		entries = malloc( sizeof( lxqt_wallet_key_values_t ) * wallet->wallet_data_entry_count );
		if( entries == NULL ){
			return NULL ;
		}else{
			e = wallet->wallet_data ;
			z = e ;
			k = wallet->wallet_data_entry_count ;
			
			memset( entries,'\0',sizeof( lxqt_wallet_key_values_t ) * wallet->wallet_data_entry_count ) ;
			
			while( q < k ){
				memcpy( &key_len,e,sizeof( u_int32_t ) ) ;
				memcpy( &key_value_len,e + sizeof( u_int32_t ) + key_len,sizeof( u_int32_t ) ) ;
				
				entries[ q ].key = malloc( key_len + 1 ) ;
				memcpy( entries[ q ].key,e + sizeof( u_int32_t ),key_len ) ;
				entries[ q ].key[ key_len ] = '\0' ;
				entries[ q ].key_size = key_len ;
								
				i = i + sizeof( u_int32_t ) + key_len + sizeof( u_int32_t ) + key_value_len ;
				e = z + i ;
				q++ ;
			}
			
			return entries ;
		}
	}
}

lxqt_wallet_key_values_t * lxqt_wallet_read_all_key_values( lxqt_wallet_t wallet )
{
	const char * e ;
	const char * z ;
	
	size_t k = 0 ;
	size_t i = 0 ;
	size_t q = 0 ;
	
	u_int32_t key_len ;
	u_int32_t key_value_len ;
	
	lxqt_wallet_key_values_t * entries ;
	
	if( wallet == NULL ){
		return NULL ;
	}else{
		entries = malloc( sizeof( lxqt_wallet_key_values_t ) * wallet->wallet_data_entry_count );
		if( entries == NULL ){
			return NULL ;
		}else{
			e = wallet->wallet_data ;
			z = e ;
			k = wallet->wallet_data_entry_count ;
			
			while( q < k ){
				memcpy( &key_len,e,sizeof( u_int32_t ) ) ;
				memcpy( &key_value_len,e + sizeof( u_int32_t ) + key_len,sizeof( u_int32_t ) ) ;
				
				entries[ q ].key = malloc( key_len + 1 ) ;
				memcpy( entries[ q ].key,e + sizeof( u_int32_t ),key_len ) ;
				entries[ q ].key[ key_len ] = '\0' ;
				entries[ q ].key_size = key_len ;
				
				entries[ q ].key_value = malloc( key_value_len + 1 ) ;
				memcpy( entries[ q ].key_value,e + sizeof( u_int32_t ) + key_len + sizeof( u_int32_t ),key_value_len ) ;
				entries[ q ].key_value[ key_value_len ] = '\0' ;
				entries[ q ].key_value_size = key_value_len ;
				
				i = i + sizeof( u_int32_t ) + key_len + sizeof( u_int32_t ) + key_value_len ;
				e = z + i ;
				q++ ;
			}
			
			return entries ;
		}
	}
}

lxqt_wallet_error lxqt_wallet_delete_key( lxqt_wallet_t wallet,const char * key )
{
	char * e ;
	char * z ;
	
	size_t k = 0 ;
	size_t i = 0 ;
	
	u_int32_t key_len ;
	u_int32_t key_value_len ;
	u_int32_t key_len_1 ;
	
	size_t block_size ;
	
	if( key == NULL || wallet == NULL ){
		return lxqt_wallet_invalid_argument ;
	}else{
		e = wallet->wallet_data ;
		z = e ;
		k = wallet->wallet_data_size ;
		key_len_1 = strlen( key ) ;
		
		while( i < k ){
			
			memcpy( &key_len,e,sizeof( u_int32_t ) ) ;
			memcpy( &key_value_len,e + sizeof( u_int32_t ) + key_len,sizeof( u_int32_t ) ) ;
			
			if( strncmp( key,e + sizeof( u_int32_t ),key_len_1 ) == 0 ){
				if( wallet->wallet_data_entry_count == 1 ){
					free( wallet->wallet_data ) ;
					wallet->wallet_data_size = 0 ;
					wallet->wallet_modified = 1 ;
					wallet->wallet_data = NULL ;
					wallet->wallet_data_entry_count = 0 ;
				}else{
					block_size = key_len + sizeof( u_int32_t ) + key_value_len + sizeof( u_int32_t ) ;
				
					memmove( e,e + block_size,wallet->wallet_data_size - ( i + block_size ) ) ;
				
					wallet->wallet_data_size -= block_size ;
					wallet->wallet_modified = 1 ;
					wallet->wallet_data_entry_count-- ;
				}
				
				break ;
			}else{
				i = i + sizeof( u_int32_t ) + key_len + sizeof( u_int32_t ) + key_value_len ;
				e = z + i ;
			}
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
		memset( wallet->wallet_data,'\0',wallet->wallet_data_size ) ;
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
	char load_header[ BLOCK_SIZE ] ;

	lxqt_wallet_t wallet ;

	size_t k ;
	char * e ;
	
	gcry_error_t r ;

	if( w == NULL || *w == NULL ){
		return lxqt_wallet_invalid_argument ;
	}

	gcry_control( GCRYCTL_INITIALIZATION_FINISHED,0 ) ;

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

	memcpy( load_header,&wallet->wallet_data_size,sizeof( u_int32_t ) ) ;
	memcpy( load_header + sizeof( u_int32_t ),&wallet->wallet_data_entry_count,sizeof( u_int32_t ) ) ;

	r = gcry_cipher_encrypt( gcry_cipher_handle,load_header,BLOCK_SIZE,NULL,0 ) ;

	write( fd,load_header,BLOCK_SIZE ) ;

	k = wallet->wallet_data_size + BLOCK_SIZE ;

	while( k % 16 != 0 ){
		k++ ;
	}
	
	e = realloc( wallet->wallet_data,k ) ;

	if( e != NULL ){
		wallet->wallet_data = e ;
		r = gcry_cipher_encrypt( gcry_cipher_handle,wallet->wallet_data,k,NULL,0 ) ;
		if( r != GPG_ERR_NO_ERROR ){
			unlink( path_1 ) ;
			close( fd ) ;
			return _close_exit( lxqt_wallet_gcry_cipher_encrypt_failed,w,gcry_cipher_handle ) ;
		}else{
			write( fd,wallet->wallet_data,k ) ;
			close( fd ) ;
			rename( path_1,path ) ;
			return _close_exit( lxqt_wallet_no_error,w,gcry_cipher_handle ) ;
		}
	}else{
		return _close_exit( lxqt_wallet_failed_to_allocate_memory,w,gcry_cipher_handle ) ;
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

	gcry_error_t r = gcry_md_open( &md,GCRY_MD_SHA256,GCRY_MD_FLAG_SECURE ) ;

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
