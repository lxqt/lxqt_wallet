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
	}else if( stringsAreEqual( command,"change" ) ){
		/*
		 * replace wallet key 
		 * additional arguments: old_password new_password
		 * eg ./wallet replace xxx zzz
		 */
		if( argc < 3 ){
			r = lxqt_wallet_invalid_argument ;
		}else{
			f = argv[ 2 ] ;
			r = lxqt_wallet_open( &wallet,f,strlen( f ),wallet_name,application_name ) ;
		}
		
		if( r == lxqt_wallet_no_error ){
			f = argv[ 3 ] ;
			r = lxqt_wallet_change_wallet_key( wallet,f,strlen( f ) ) ;
			lxqt_wallet_close( &wallet ) ;
		}else{
			if( r == lxqt_wallet_wrong_password ){
				puts( "wrong password" ) ;
			}else{
				puts( "general error has occured" ) ;
			}
		}
	}else if( stringsAreEqual( command,"replace" ) ){
		/*
		 * delete a key and put another in its slot 
		 * additional arguments: wallet_key old_key new_key new_key_value
		 * eg ./wallet replace xxx rrr ttt ccc
		 */
		if( argc < 3 ){
			r = lxqt_wallet_invalid_argument ;
		}else{
			f = argv[ 2 ] ;
			r = lxqt_wallet_open( &wallet,f,strlen( f ),wallet_name,application_name ) ;
		}
			
		if( r == lxqt_wallet_no_error ){
			lxqt_wallet_replace_key( wallet,argv[ 3 ],argv[ 4 ],argv[ 5 ],strlen( argv[ 5 ] ) ) ;
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
