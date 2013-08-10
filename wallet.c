
#include "lxqtwallet.h"
#include <string.h>

int main( int argc,char * argv[] )
{
	lxqt_wallet_t handle ;
	int r = 0 ;
	char path[ 122 ] ;
	
	if( argc  < 2){
		printf ("wrong number of arguments\n" );
		return 1 ;
	}
	if( strcmp( argv[ 1 ],"create" ) == 0 ){
		lxqt_wallet_create( "qqq",3,"wallet_name","application_name" ) ;
	}else{
		r = lxqt_wallet_open( &handle,"qqeq",3,"wallet_name","application_name" ) ;
	}
	
	lxqt_wallet_application_wallet_path( path,122,"application_name" ) ;
	
	puts( path ) ;
	return r ;
}
