
#include "lxqtwallet.h"
#include <string.h>

int main( int argc,char * argv[] )
{
	lxqt_wallet_t wallet ;
	int r = 0 ;
	
	char * e = NULL ;
	
	if( argc  < 2){
		printf ("wrong number of arguments\n" );
		return 1 ;
	}
	if( strcmp( argv[ 1 ],"create" ) == 0 ){
		lxqt_wallet_create( "qqq",3,"wallet_name","application_name" ) ;
	}else{
		r = lxqt_wallet_open( &wallet,"qeq",3,"wallet_name","application_name" ) ;
		
		if( r != 0 ){
			puts( "ee" ) ;
		}else{
			/*lxqt_wallet_add_key( wallet,"rrr","zzz",3 ) ;*/
			
			e = lxqt_wallet_read_key_value( wallet,"rrr" ) ;
			
			if( e ){
				puts( e ) ;
			}
			
			lxqt_wallet_close( wallet ) ;
		}
	}
	
	return r ;
}
