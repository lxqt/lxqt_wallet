
#include "lxqtwallet.h"
#include <string.h>

int main( int argc,char * argv[] )
{
	lxqt_wallet_t wallet ;
	int r = 0 ;
	
	char * e = NULL ;
	const char * f ;
	const char * z ;
	
	const struct lxqt_key_value * values ;
	int j ;
	int k ;
	if( argc  < 2 ){
		printf ("wrong number of arguments\n" );
		return 1 ;
	}
	if( strcmp( argv[ 1 ],"create" ) == 0 ){
		if( argc < 3 ){
			r = 10 ;
		}else{
			f = argv[ 2 ] ;
			r = lxqt_wallet_create( f,strlen( f ),"wallet_name","application_name" ) ;
		}
	}else if( strcmp( argv[ 1 ],"add" ) == 0 ){
		if( argc < 3 ){
			r = 20 ;
		}else{
			f = argv[ 2 ] ;
			r = lxqt_wallet_open( &wallet,f,strlen( f ),"wallet_name","application_name" ) ;
		}
				
		if( r != 0 ){
			puts( "ee" ) ;
		}else{
			f = argv[ 3 ] ;
			z = argv[ 4 ] ;
			r = lxqt_wallet_add_key( wallet,f,z,strlen( z ) ) ; 
			
			lxqt_wallet_close( wallet ) ;
		}
	}else if( strcmp( argv[ 1 ],"read" ) == 0 ){
		if( argc < 3 ){
			r = 20 ;
		}else{
			f = argv[ 2 ] ;
			r = lxqt_wallet_open( &wallet,f,strlen( f ),"wallet_name","application_name" ) ;
		}
		
		if( r != 0 ){
			puts( "ee" ) ;
		}else{
			f = argv[ 3 ] ;
			e = lxqt_wallet_read_key_value( wallet,f ) ;
			
			if( e ){
				printf( "key=%s:value=%s\n",f,e ) ;
				free( e ) ;
			}else{
				printf( "key=%s:value=(NULL)\n",f ) ;
			}
			
			lxqt_wallet_close( wallet ) ;
		}
	}else if( strcmp( argv[ 1 ],"print" ) == 0 ){
		if( argc < 3 ){
			r = 20 ;
		}else{
			f = argv[ 2 ] ;
			r = lxqt_wallet_open( &wallet,f,strlen( f ),"wallet_name","application_name" ) ;
		}

		if( r == 0 ){
			values = lxqt_wallet_read_all_key_values( wallet ) ;
			j = lxqt_wallet_wallet_size( wallet ) ;
			
			for( k = 0 ; k < j ; k ++ ){
				printf( "key=%s:value=%s\n",values[ k ].key,values[ k ].value ) ;
			}
		}
	}
	
	return r ;
}
