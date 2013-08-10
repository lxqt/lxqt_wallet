

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <gcrypt.h>

/*
 * lxqt_wallet_t is an opaque handle and all wallet operations are done through the handle.
 * 
 */
typedef struct lxqt_wallet_struct * lxqt_wallet_t ;

int lxqt_wallet_create( const char * password,size_t password_length,const char * wallet_name,const char * application_name ) ;

int lxqt_wallet_open( lxqt_wallet_t *,const char * password,size_t password_length,
		      const char * wallet_name,const char * application_name ) ;

char * lxqt_wallet_read_password( lxqt_wallet_t,const char * key ) ;

char ** lxqt_wallet_read_all_passwords( lxqt_wallet_t ) ;

int lxqt_wallet_t_write_password( lxqt_wallet_t,const char * key,const char * password,size_t password_length ) ;

void lxqt_wallet_close( lxqt_wallet_t ) ;

int lxqt_wallet_exists( const char * wallet_name,const char * application_name ) ;

void lxqt_wallet_application_wallet_path( char * path_buffer,size_t path_buffer_size,const char * application_name ) ;
