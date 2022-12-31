#include "Stdafx.h"
#include "Crypto/Cert/OpenSSLCert.h"
#include "Net/OpenSSLClient.h"
#include <openssl/ssl.h>
#include <openssl/err.h>

//#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif
struct Net::OpenSSLClient::ClassData
{
	SSL *ssl;
	Crypto::Cert::OpenSSLCert *remoteCert;
	Bool shutdown;
};

UInt32 Net::OpenSSLClient::GetLastErrorCode()
{
	UInt32 lastError = 0;
	UInt32 thisError;
	while ((thisError = (UInt32)ERR_get_error()) != 0)
	{
		lastError = thisError;
	}
	return lastError;
}

Net::OpenSSLClient::OpenSSLClient(Net::SocketFactory *sockf, void *ssl, Socket *s) : SSLClient(sockf, s)
{
	this->clsData = MemAlloc(ClassData, 1);
	this->clsData->ssl = (SSL*)ssl;
	this->clsData->remoteCert = 0;
	this->clsData->shutdown = false;

//	X509 *cert = SSL_get_peer_certificate(this->clsData->ssl);
	stack_st_X509 *certs = SSL_get_peer_cert_chain(this->clsData->ssl);
	if (certs != 0)
	{
		NEW_CLASS(this->clsData->remoteCert, Crypto::Cert::OpenSSLCert(sk_X509_value(certs, 0)));
	}
}

Net::OpenSSLClient::~OpenSSLClient()
{
	SDEL_CLASS(this->clsData->remoteCert);
	SSL_free(this->clsData->ssl);
	MemFree(this->clsData);
}

UOSInt Net::OpenSSLClient::Read(UInt8 *buff, UOSInt size)
{
	if (s && (this->flags & 6) == 0)
	{
		int ret = SSL_read(this->clsData->ssl, buff, (int)(OSInt)size);
		if (ret > 0)
		{
#if defined(VERBOSE)
			printf("OSSLClient: Read %d bytes\r\n", (UInt32)ret);
#endif
			this->currCnt += (UInt32)ret;
			return (UInt32)ret;
		}
		UInt32 err = this->GetLastErrorCode();
#if defined(VERBOSE)
		printf("OSSLClient: Read error: %x\r\n", err);
#endif
		if (err == SSL_ERROR_ZERO_RETURN)
		{
			return 0;
		}
		this->flags |= 2;
		return 0;
	}
	else
	{
		return 0;
	}
}

UOSInt Net::OpenSSLClient::Write(const UInt8 *buff, UOSInt size)
{
	if (s && (this->flags & 5) == 0)
	{
		UOSInt totalWrite = 0;
		while (size > 0)
		{
			int ret = SSL_write(this->clsData->ssl, buff, (int)size);
			if (ret > 0)
			{
	#if defined(VERBOSE)
				printf("OSSLClient: Write %d bytes\r\n", (UOSInt)ret);
	#endif
				this->currCnt += (UInt32)ret;
				totalWrite += (UInt32)ret;
				buff += ret;
				size -= (UInt32)ret;
			}
			else
			{
				this->flags |= 1;
				break;
			}
		}
		return totalWrite;
	}
	else
	{
		return 0;
	}
}

void *Net::OpenSSLClient::BeginRead(UInt8 *buff, UOSInt size, Sync::Event *evt)
{
	UOSInt ret = this->Read(buff, size);
	if (ret)
	{
		evt->Set();
	}
	return (void*)ret;
}

UOSInt Net::OpenSSLClient::EndRead(void *reqData, Bool toWait, Bool *incomplete)
{
	*incomplete = false;
	return (UOSInt)reqData;
}

void Net::OpenSSLClient::CancelRead(void *reqData)
{

}

void *Net::OpenSSLClient::BeginWrite(const UInt8 *buff, UOSInt size, Sync::Event *evt)
{
	UOSInt ret = this->Write(buff, size);
	if (ret)
	{
		evt->Set();
	}
	return (void*)ret;
}

UOSInt Net::OpenSSLClient::EndWrite(void *reqData, Bool toWait)
{
	return (UOSInt)reqData;
}

void Net::OpenSSLClient::CancelWrite(void *reqData)
{

}

Int32 Net::OpenSSLClient::Flush()
{
	return 0;
}

void Net::OpenSSLClient::Close()
{
	this->ShutdownSend();
	this->Net::TCPClient::Close();
}

Bool Net::OpenSSLClient::Recover()
{
	return false;
}

Crypto::Cert::Certificate *Net::OpenSSLClient::GetRemoteCert()
{
	return this->clsData->remoteCert;
}
