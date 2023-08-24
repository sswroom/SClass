#include "Stdafx.h"
#include "Net/OpenSSLCore.h"
#include "Sync/Interlocked.h"
#include <openssl/ssl.h>

Int32 Net::OpenSSLCore::useCnt = 0;

void Net::OpenSSLCore::Init()
{
	if (Sync::Interlocked::IncrementI32(useCnt) <= 1)
	{
		SSL_load_error_strings();	
    	OpenSSL_add_ssl_algorithms();
	}
}

void Net::OpenSSLCore::Deinit()
{
	if (Sync::Interlocked::DecrementI32(useCnt) <= 0)
	{
		EVP_cleanup();
	}
}
