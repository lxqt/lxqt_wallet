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
 * Its a bit tricky to use stable part of libsecret API to add our secrets the way that will agree with out API that is consistent with
 * libsecret backend,kwallet backend and the internal back end.
 *
 * The biggest problem is how to get information you do not know about in the wallet.For our usecase,how to get a list of all keys
 * in the wallet.
 *
 * To work around this problem,a schema "walletSize" is used to track the number entries in the wallet.Another schema "keyID" is
 * used to track keys in the wallet.When an entry is added to the wallet,a key is added to this schema and the key is reffered
 * by a number.Since a key with a lower number maybe deleted,each time a key is to be added,the key will be added with the lowest available number.
 *
 * This is a bit ugly and slow but it seem to work.
 */

/*
 * This schema is used to store walletName,applicationName,key and key value
 */
static const SecretSchema keyValue = {
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
 * The "number" property holds the ID of the key.When a new key is added,a lowest number is first searched
 * and used when found.
 *
 */
static const SecretSchema keyID = {
	"lxqt.Wallet.Keys",SECRET_SCHEMA_NONE,
	{
		{ "string",SECRET_SCHEMA_ATTRIBUTE_STRING },
		{ "string",SECRET_SCHEMA_ATTRIBUTE_STRING },
		{ "number", SECRET_SCHEMA_ATTRIBUTE_INTEGER },
		{ "NULL",0 },
	}
};

/*
 * This schema is used to store the number of entries in the wallet
 */
static const SecretSchema walletSize = {
	"lxqt.Wallet.Keys.Number",SECRET_SCHEMA_NONE,
	{
		{ "string",SECRET_SCHEMA_ATTRIBUTE_STRING },
		{ "string",SECRET_SCHEMA_ATTRIBUTE_STRING },
		{ "string",SECRET_SCHEMA_ATTRIBUTE_STRING },
		{ "NULL",0 },
	}
};

#define BUFFER_SIZE 32

static int _number_of_entries_in_the_wallet( const char * walletName,const char * applicationName )
{
	int i ;
	char * c = secret_password_lookup_sync( &walletSize,NULL,NULL,
					 "string",walletName,"string",applicationName,"string","lxqt_wallet_size",NULL ) ;
	if( c == NULL ){
		return 0 ;
	}else{
		i = atoi( c ) ;
		free( c ) ;
		return i ;
	}
}

char * lxqt_secret_service_get_value( const char * key,const char * walletName,const char * applicationName )
{
	return secret_password_lookup_sync( &keyValue,NULL,NULL,
					    "string",walletName,"string",applicationName,"string",key,NULL ) ;
}

gboolean lxqt_secret_service_password_store_sync( const char * key,const char * value,const char * walletName,const char * applicationName )
{
	int i = 0 ;
	char * c ;
	char d[ BUFFER_SIZE ] ;
	int j ;

	c = secret_password_lookup_sync( &walletSize,NULL,NULL,
					 "string",walletName,"string",applicationName,"string","lxqt_wallet_size",NULL ) ;

	if( c == NULL ){
		secret_password_store_sync( &walletSize,"default","lxqt wallet","1",NULL,NULL,
					    "string",walletName,"string",applicationName,"string","lxqt_wallet_size",NULL ) ;

		secret_password_store_sync( &keyID,"default","lxqt wallet",key,NULL,NULL,
					    "string",walletName,"string",applicationName,"number",0,NULL ) ;
	}else{
		snprintf( d,BUFFER_SIZE,"%d",atoi( c ) + 1 ) ;
		free( c ) ;

		secret_password_store_sync( &walletSize,"default","lxqt wallet",d,NULL,NULL,
					    "string",walletName,"string",applicationName,"string","lxqt_wallet_size",NULL ) ;

		j = _number_of_entries_in_the_wallet( walletName,applicationName ) ;
		while( i < j ){
			c = secret_password_lookup_sync( &keyID,NULL,NULL,
							 "string",walletName,"string",applicationName,"number",i,NULL ) ;
			if( c == NULL ){
				snprintf( d,BUFFER_SIZE,"%d",i ) ;
				secret_password_store_sync( &keyID,"default","lxqt wallet",key,NULL,NULL,
							    "string",walletName,"string",applicationName,"number",i,NULL ) ;
				break ;
			}else{
				i++ ;
				free( c ) ;
			}
		}
	}

	return secret_password_store_sync( &keyValue,"default","lxqt wallet",value,NULL,NULL,
					   "string",walletName,"string",applicationName,"string",key,NULL ) ;
}

gboolean lxqt_secret_service_clear_sync( const char * key,const char * walletName,const char * applicationName )
{
	int i = 0 ;
	int k = 0 ;
	char * c ;
	char d[ BUFFER_SIZE ] ;
	int j = _number_of_entries_in_the_wallet( walletName,applicationName ) ;

	while( i <= j ){
		c = secret_password_lookup_sync( &keyID,NULL,NULL,
						 "string",walletName,"string",applicationName,"number",k,NULL ) ;
		if( c != NULL ){
			if( strcmp( c,key ) == 0 ){
				secret_password_clear_sync( &keyID,NULL,NULL,
									   "string",walletName,"string",applicationName,"number",i,NULL ) ;
				free( c ) ;

				c = secret_password_lookup_sync( &walletSize,NULL,NULL,
								   "string",walletName,"string",applicationName,"string","lxqt_wallet_size",NULL ) ;

				snprintf( d,BUFFER_SIZE,"%d",atoi( c ) - 1 ) ;
				free( c ) ;
				secret_password_store_sync( &walletSize,"default","lxqt wallet",d,NULL,NULL,
							    "string",walletName,"string",applicationName,"string","lxqt_wallet_size",NULL ) ;

				break ;
			}else{
				i++ ;
				k++ ;
				free( c ) ;
			}
		}else{
			k++ ;
		}
	}

	return secret_password_clear_sync( &keyValue,NULL,NULL,
					   "string",walletName,"string",applicationName,"string",key,NULL ) ;
}

char ** lxqt_secret_get_all_keys( const char * walletName,const char * applicationName,int * count )
{
	int k = 0 ;
	int i = 0 ;
	int j = _number_of_entries_in_the_wallet( walletName,applicationName ) ;
	char * c  = NULL  ;
	char ** e = NULL ;
	char ** f ;
	*count = 0 ;
	while( i < j ){
		c = secret_password_lookup_sync( &keyID,NULL,NULL,
						 "string",walletName,"string",applicationName,"number",k,NULL ) ;
		if( c != NULL ){
			f = realloc( e,sizeof( char * ) * ( i + 1 ) ) ;
			if( f != NULL ){
				e = f ;
				e[ i ] =  c ;
				*count = *count + 1 ;
			}
			i++ ;
			k++ ;
		}else{
			k++;
		}
	}

	return e ;
}

int lxqt_secret_service_wallet_size( const char * walletName,const char * applicationName )
{
	return _number_of_entries_in_the_wallet( walletName,applicationName ) ;
}
