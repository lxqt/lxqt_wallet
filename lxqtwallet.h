

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <gcrypt.h>

typedef struct lxqt_wallet_struct * lxqt_wallet_t ;


#define KEY_SIZE 44 
#define VALUE_SIZE 512

struct lxqt_key_value{
	char key[ KEY_SIZE ] ;
	char value[ VALUE_SIZE ] ;
	u_int32_t value_size ;
};

const struct lxqt_key_value * lxqt_wallet_read_all_key_values( lxqt_wallet_t ) ;

int lxqt_wallet_create( const char * password,size_t password_length,const char * wallet_name,const char * application_name ) ;

int lxqt_wallet_open( lxqt_wallet_t *,const char * password,size_t password_length,
		      const char * wallet_name,const char * application_name ) ;

char * lxqt_wallet_read_key_value( lxqt_wallet_t,const char * key ) ;

int lxqt_wallet_add_key( lxqt_wallet_t,const char * key,const char * key_value,size_t key_value_length ) ;

int lxqt_wallet_delete_key( lxqt_wallet_t,const char * key ) ;

int lxqt_wallet_delete_wallet( const char * wallet_name,const char * application_name ) ;

int lxqt_wallet_close( lxqt_wallet_t ) ;

int lxqt_wallet_exists( const char * wallet_name,const char * application_name ) ;

void lxqt_wallet_application_wallet_path( char * path_buffer,size_t path_buffer_size,const char * application_name ) ;

int lxqt_wallet_wallet_size( lxqt_wallet_t ) ;

