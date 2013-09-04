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

#include "secret_service.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*
 * This schema is used to store walletName,applicationName,key and key value
 */
static const SecretSchema lxqtValuesSecretSchema = {
	"lxqt.Wallet.Values",SECRET_SCHEMA_NONE,
	{
	    { "string",SECRET_SCHEMA_ATTRIBUTE_STRING },
	    { "string",SECRET_SCHEMA_ATTRIBUTE_STRING },
	    { "string",SECRET_SCHEMA_ATTRIBUTE_STRING },
	    { "NULL",0 },
	}
};

/*
 * This schema is used to store keys in a way they can be retrieved by readAllKeys()
 */
static const SecretSchema lxqtKeysSecretSchema = {
	"lxqt.Wallet.Keys",SECRET_SCHEMA_NONE,
	{
		{ "string",SECRET_SCHEMA_ATTRIBUTE_STRING },
		{ "string",SECRET_SCHEMA_ATTRIBUTE_STRING },
		{ "number", SECRET_SCHEMA_ATTRIBUTE_INTEGER },
		{ "NULL",0 },
	}
};

char * lxqt_secret_service_get_value( const char * key,const char * walletName,const char * applicationName )
{
	return secret_password_lookup_sync( &lxqtValuesSecretSchema,NULL,NULL,
					    "string",walletName,"string",applicationName,"string",key,NULL ) ;
}

gboolean lxqt_secret_service_password_store_sync( const char * key,const char * value,const char * walletName,const char * applicationName )
{
	int i = 0 ;
	char * c ;
	while( 1 ){
		c = secret_password_lookup_sync( &lxqtKeysSecretSchema,NULL,NULL,
						 "string",walletName,"string",applicationName,"number",i,NULL ) ;
		if( c != NULL ){
			i++ ;
			free( c ) ;
		}else{
			secret_password_store_sync( &lxqtKeysSecretSchema,"default","lxqt wallet",key,NULL,NULL,
							   "string",walletName,"string",applicationName,"number",i,NULL ) ;
			break ;
		}
	}

	return secret_password_store_sync( &lxqtValuesSecretSchema,"default","lxqt wallet",value,NULL,NULL,
					   "string",walletName,"string",applicationName,"string",key,NULL ) ;
}

gboolean lxqt_secret_service_clear_sync( const char * key,const char * walletName,const char * applicationName )
{
	int i = 0 ;
	char * c ;

	while( 1 ){
		c = secret_password_lookup_sync( &lxqtKeysSecretSchema,NULL,NULL,
						 "string",walletName,"string",applicationName,"number",i,NULL ) ;
		if( c != NULL ){
			if( strcmp( c,key ) == 0 ){
				secret_password_clear_sync( &lxqtKeysSecretSchema,NULL,NULL,
									   "string",walletName,"string",applicationName,"number",i,NULL ) ;
				free( c  ) ;
				break ;
			}else{
				i++ ;
				free( c ) ;
			}
		}else{
			break ;
		}
	}

	return secret_password_clear_sync( &lxqtValuesSecretSchema,NULL,NULL,
					   "string",walletName,"string",applicationName,"string",key,NULL ) ;
}

char ** lxqt_secret_get_all_keys( const char * walletName,const char * applicationName,int * count )
{
	int i = 0 ;
	char * c  = NULL  ;
	char ** e = NULL ;
	char ** f ;
	*count = 0 ;
	while( 1 ){
		c = secret_password_lookup_sync( &lxqtKeysSecretSchema,NULL,NULL,
						 "string",walletName,"string",applicationName,"number",i,NULL ) ;
		if( c != NULL ){
			f = realloc( e,sizeof( char * ) * ( i + 1 ) ) ;
			if( f != NULL ){
				e = f ;
				e[ i ] =  c ;
				i++ ;
				*count = *count + 1 ;
				c = NULL ;
			}else{
				break ;
			}
		}else{
			break ;
		}
	}

	return e ;
}

int lxqt_secret_service_wallet_size( const char * walletName,const char * applicationName )
{
	int i = 0 ;
	char * c = NULL ;
	while( 1 ){
		c = secret_password_lookup_sync( &lxqtKeysSecretSchema,NULL,NULL,
						 "string",walletName,"string",applicationName,"number",i,NULL ) ;
		if( c != NULL ){
			i++ ;
			free( c ) ;
			c = NULL ;
		}else{
			break ;
		}
	}

	return i ;
}
