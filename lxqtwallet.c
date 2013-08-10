
#include "lxqtwallet.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>

#define DEBUG 1
#define IV_SIZE 16 
#define MAGIC_STRING "lxqtwallet"
#define MAGIN_STRING_SIZE 16
#define KEY_SIZE 16

struct lxqt_wallet_struct{
	char * application_name ;
	char * wallet_name ;
	char * wallet_data ;
	size_t wallet_data_size ;
	gcry_cipher_hd_t gcry_cipher_hd ;
};

static char * _wallet_full_path( char * path_buffer,size_t path_buffer_size,const char * wallet_name,const char * application_name ) ;

static void _create_application_wallet_path( const char * application_name ) ;

static void _create_key( char output_key[ KEY_SIZE ],const char * input_key,size_t input_key_length ) ;

static int _get_iv_from_wallet_header( char iv[ IV_SIZE ],const char * wallet_name,const char * application_name ) ;

static int _get_magic_string_from_header( char magic_string[ MAGIN_STRING_SIZE ],const char * wallet_path ) ;

static void _get_iv( char iv[ IV_SIZE ] )
{
	int fd ;
	fd = open( "/dev/urandom",O_RDONLY ) ;
	read( fd,iv,IV_SIZE ) ;
	close( fd ) ;
}

char ** lxqt_wallet_read_all_key_values( lxqt_wallet_t wallet ) 
{
	if( wallet ){;}
	return NULL ;
}

int lxqt_wallet_create( const char * password,size_t password_length,const char * wallet_name,const char * application_name )
{
	int fd ;
	char path[ PATH_MAX ] ;
	char iv[ IV_SIZE ] ;
	char magic_string[ MAGIN_STRING_SIZE ] ;
	char key[ KEY_SIZE ] ;
	
	gcry_error_t r ;
	
	gcry_cipher_hd_t gcry_cipher_handle ;
		
	if( lxqt_wallet_exists( wallet_name,application_name ) ){
		puts( "ERROR: wallet exists" ) ;
		return 1 ;
	}else{
		;
	}
	
	gcry_check_version( NULL ) ;
	gcry_control( GCRYCTL_INITIALIZATION_FINISHED,0 ) ;
	
	r = gcry_cipher_open( &gcry_cipher_handle,GCRY_CIPHER_AES128,GCRY_CIPHER_MODE_CBC,0 ) ;
	
	if( r != GPG_ERR_NO_ERROR ){
		fprintf( stderr,"gcry_cipher_open: %s/%s\n",gcry_strsource( r ),gcry_strerror( r ) ) ;
		return 1 ;
	}
	
	_create_key( key,password,password_length ) ;
	r = gcry_cipher_setkey( gcry_cipher_handle,key,KEY_SIZE ) ;
	
	if( r != GPG_ERR_NO_ERROR ){
		fprintf( stderr,"gcry_cipher_setkey: %s/%s\n",gcry_strsource( r ),gcry_strerror( r ) ) ;
		gcry_cipher_close( gcry_cipher_handle ) ;
		return 1 ;
	}
	
	_get_iv( iv ) ;
	
	r = gcry_cipher_setiv( gcry_cipher_handle,iv,IV_SIZE ) ;
	
	if( r != GPG_ERR_NO_ERROR ){
		fprintf( stderr,"gcry_cipher_setiv: %s/%s\n",gcry_strsource( r ),gcry_strerror( r ) ) ;
		gcry_cipher_close( gcry_cipher_handle ) ;
		return 1 ;
	}
	
	_create_application_wallet_path( application_name ) ;
	
	_wallet_full_path( path,PATH_MAX,wallet_name,application_name ) ;
	
	fd = open( path,O_WRONLY|O_CREAT,0600 ) ;
	
	if( fd == -1 ){
		gcry_cipher_close( gcry_cipher_handle ) ;
		return 1 ;
	}
	
	write( fd,iv,IV_SIZE ) ;
	
	strncpy( magic_string,MAGIC_STRING,MAGIN_STRING_SIZE ) ;
	
	r = gcry_cipher_encrypt( gcry_cipher_handle,magic_string,MAGIN_STRING_SIZE,NULL,0 ) ;
	gcry_cipher_close( gcry_cipher_handle ) ;
	
	if( r != GPG_ERR_NO_ERROR ){
		close( fd ) ;
		fprintf( stderr,"gcry_cipher_encrypt: %s/%s\n",gcry_strsource (r),gcry_strerror (r));
		return 1 ;
	}else{
		write( fd,magic_string,MAGIN_STRING_SIZE ) ;
		close( fd ) ;
		return 0 ;
	}
}

/*
 * output:
 * 0 : success
 * 1 : malloc failed
 * 2 : failed to read iv
 * 3 : wrong password
 * 4 : grypt error
 */
int lxqt_wallet_open( lxqt_wallet_t * wallet,const char * password,size_t password_length,
		      const char * wallet_name,const char * application_name )
{
	size_t len ;
	gcry_error_t r ;
	gcry_cipher_hd_t gcry_cipher_handle ;
	char iv[ IV_SIZE ] ;
	char path[ PATH_MAX ] ;
	char magic_string[ MAGIN_STRING_SIZE ] ;
	char magic_string_1[ MAGIN_STRING_SIZE ] ;
	char key[ KEY_SIZE ] ;
	
	struct lxqt_wallet_struct * w ;
	
	if( wallet_name == NULL || application_name == NULL || wallet == NULL ){
		return 1 ;
	}
	
	w = malloc( sizeof( struct lxqt_wallet_struct ) ) ;
	
	if( w == NULL ){
		return 1 ;
	}
	
	memset( w,'\0',sizeof( struct lxqt_wallet_struct ) ) ;
	
	len = strlen( wallet_name ) ;
	w->wallet_name = malloc( sizeof( char ) * ( len + 1 ) ) ;
	if( w->wallet_name == NULL ){
		free( w ) ;
		return 1 ;
	}else{
		memcpy( w->wallet_name,wallet_name,len + 1 ) ;
	}
	
	len = strlen( application_name ) ;
	w->application_name = malloc( sizeof( char ) * ( len + 1 ) ) ;
	if( w->application_name == NULL ){
		free( w->wallet_name ) ;
		free( w ) ;
		return 1 ;
	}else{
		memcpy( w->application_name,application_name,len + 1 ) ;
	}
	
	gcry_check_version( NULL ) ;
	gcry_control( GCRYCTL_INITIALIZATION_FINISHED,0 ) ;
	
	r = gcry_cipher_open( &w->gcry_cipher_hd,GCRY_CIPHER_AES128,GCRY_CIPHER_MODE_CBC,0 ) ;

	if( r != GPG_ERR_NO_ERROR ){
		fprintf( stderr,"gcry_cipher_open: %s/%s\n",gcry_strsource( r ),gcry_strerror( r ) ) ;
		free( w ) ;
		return 4 ;
	}
	
	gcry_cipher_handle = w->gcry_cipher_hd ;
	
	_create_key( key,password,password_length ) ;
	
	r = gcry_cipher_setkey( gcry_cipher_handle,key,KEY_SIZE ) ;
	
	if( r != GPG_ERR_NO_ERROR ){
		fprintf( stderr,"gcry_cipher_setkey: %s/%s\n",gcry_strsource( r ),gcry_strerror( r ) ) ;
		gcry_cipher_close( gcry_cipher_handle ) ;
		free( w->wallet_name ) ;
		free( w->application_name ) ;
		free( w ) ;
		return 4 ;
	}
	
	if( _get_iv_from_wallet_header( iv,wallet_name,application_name ) ){
		;
	}else{
		fprintf( stderr,"failed to read iv from wallet header" ) ;
		return 2 ;
	}
		
	r = gcry_cipher_setiv( gcry_cipher_handle,iv,IV_SIZE ) ;
	
	if( r != GPG_ERR_NO_ERROR ){
		fprintf( stderr,"gcry_cipher_setiv: %s/%s\n",gcry_strsource (r),gcry_strerror( r ) ) ;
		gcry_cipher_close( gcry_cipher_handle ) ;
		free( w->wallet_name ) ;
		free( w->application_name ) ;
		free( w ) ;
		return 4 ;
	}
	
	_wallet_full_path( path,PATH_MAX,wallet_name,application_name ) ;
	
	if( _get_magic_string_from_header( magic_string,path ) ){
		;
	}else{
		gcry_cipher_close( gcry_cipher_handle ) ;
		free( w->wallet_name ) ;
		free( w->application_name ) ;
		free( w ) ;
		fprintf( stderr,"failed to read magic string from wallet header" ) ;
		return 2 ;
	}
	
	r =  gcry_cipher_decrypt( gcry_cipher_handle,magic_string,MAGIN_STRING_SIZE,NULL,0 ) ;
	
	if( r != GPG_ERR_NO_ERROR ){
		fprintf( stderr,"gcry_cipher_decrypt: %s/%s\n",gcry_strsource( r ),gcry_strerror( r ) ) ;
		gcry_cipher_close( gcry_cipher_handle ) ;
		free( w->wallet_name ) ;
		free( w->application_name ) ;
		free( w ) ;
		return 4 ;
	}
	
	strncpy( magic_string_1,MAGIC_STRING,MAGIN_STRING_SIZE ) ;
	
	if( memcmp( magic_string,magic_string_1,MAGIN_STRING_SIZE ) == 0 ){
		printf( "correct password,magic string is: %s\n",magic_string ) ;
		/*
		 * TODO:the wallet is open,dump content of the wallet to memory
		 */
		return 0 ;
	}else{
		gcry_cipher_close( gcry_cipher_handle ) ;
		free( w->wallet_name ) ;
		free( w->application_name ) ;
		free( w ) ;
		fprintf( stderr,"wrong password\n" ) ;
		return 3 ;
	}
}

char * lxqt_wallet_read_key_value( lxqt_wallet_t wallet,const char * key )
{
	if( wallet ){;}
	if( key ){;}
	return NULL ;
}

int lxqt_wallet_add_key( lxqt_wallet_t wallet,const char * key,const char * key_value,size_t key_value_length ) 
{
	if( wallet ){;}
	if( key ){;}
	if( key_value ){;}
	if( key_value_length ){;}
	return 0 ;
}

int lxqt_wallet_delete_key( lxqt_wallet_t wallet,const char * key )
{
	if( wallet ){;}
	if( key ){;}
	return 0 ;
}

int lxqt_wallet_delete_wallet( const char * wallet_name,const char * application_name ) 
{
	if( wallet_name ){;}
	if( application_name ){;}
	return 0 ;
}

void lxqt_wallet_close( lxqt_wallet_t wallet ) 
{
	gcry_cipher_close( wallet->gcry_cipher_hd ) ;
	free( wallet->wallet_data ) ;
	free( wallet->wallet_name ) ;
	free( wallet->application_name ) ;
	free( wallet ) ;
}

int lxqt_wallet_exists( const char * wallet_name,const char * application_name ) 
{
	char path[ PATH_MAX ] ;
	struct stat st ;
	_wallet_full_path( path,PATH_MAX,wallet_name,application_name ) ;
	return stat( path,&st ) == 0 ;
}

void lxqt_wallet_application_wallet_path( char * path,size_t path_buffer_size,const char * application_name )
{
	struct passwd * pass = getpwuid( getuid() ) ;
	
	char path_1[ PATH_MAX ] ;
	
	snprintf( path,path_buffer_size,"%s/.config",pass->pw_dir ) ;
	
	snprintf( path_1,path_buffer_size,"%s/%s",path,application_name )  ;
	
	snprintf( path,path_buffer_size,"%s/wallets",path_1 ) ;
}

char * _wallet_full_path( char * path_buffer,size_t path_buffer_size,const char * wallet_name,const char * application_name )
{
	struct passwd * pass = getpwuid( getuid() ) ;
	snprintf( path_buffer,path_buffer_size,"%s/.config/%s/wallets/%s",pass->pw_dir,application_name,wallet_name ) ;
	return path_buffer ;
}

static void _create_application_wallet_path( const char * application_name )
{
	struct passwd * pass = getpwuid( getuid() ) ;
	
	char path_1[ PATH_MAX ] ;
	char path_2[ PATH_MAX ] ;
	
	snprintf( path_1,PATH_MAX,"%s/.config",pass->pw_dir ) ;
	mkdir( path_1,0755 ) ;
	
	snprintf( path_2,PATH_MAX,"%s/%s",path_1,application_name )  ;
	mkdir( path_2,0755 ) ;
	
	snprintf( path_1,PATH_MAX,"%s/wallets",path_2 ) ;
	mkdir( path_1,0755 ) ;
}

static void _create_key( char output_key[ KEY_SIZE ],const char * input_key,size_t input_key_length )
{
	if( input_key_length ){;}
	strncpy( output_key,input_key,KEY_SIZE ) ;
}

static int _get_iv_from_wallet_header( char iv[ IV_SIZE ],const char * wallet_name,const char * application_name ) 
{
	int fd ;
	char path[ PATH_MAX ] ;
	if( lxqt_wallet_exists( wallet_name,application_name ) ){
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

int _get_magic_string_from_header( char magic_string[ MAGIN_STRING_SIZE ],const char * wallet_path ) 
{
	int fd = open( wallet_path,O_RDONLY ) ;
	if( fd == -1 ){
		return 0 ;
	}else{
		lseek( fd,IV_SIZE,SEEK_SET ) ;
		read( fd,magic_string,MAGIN_STRING_SIZE ) ;
		close( fd ) ;
		return 1 ;
	}
}

