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

static const SecretSchema lxqtSecretSchema = {
	"lxqt.Wallet",SECRET_SCHEMA_NONE,
	{
	    { "string",SECRET_SCHEMA_ATTRIBUTE_STRING },
	    { "string",SECRET_SCHEMA_ATTRIBUTE_STRING },
	    { "string",SECRET_SCHEMA_ATTRIBUTE_STRING },
	    { "NULL",0 },
	}
};

char * lxqt_secret_service_get_value( const char * key,const char * walletName,const char * applicationName )
{
	return secret_password_lookup_sync( &lxqtSecretSchema,NULL,NULL,
					    "string",walletName,"string",applicationName,"string",key,NULL ) ;
}

gboolean lxqt_secret_service_password_store_sync( const char * key,const char * value,const char * walletName,const char * applicationName )
{
	return secret_password_store_sync( &lxqtSecretSchema,"default","lxqt wallet",value,NULL,NULL,
					   "string",walletName,"string",applicationName,"string",key,NULL ) ;
}

gboolean lxqt_secret_service_clear_sync( const char * key,const char * walletName,const char * applicationName )
{
	return secret_password_clear_sync( &lxqtSecretSchema,NULL,NULL,
					   "string",walletName,"string",applicationName,"string",key,NULL ) ;
}
