#include "Stdafx.h"
#include "Net/OpenSSLClient.h"
#include <openssl/ssl.h>
#include <openssl/err.h>

struct Net::OpenSSLClient::ClassData
{
	SSL *ssl;
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

Net::OpenSSLClient::OpenSSLClient(Net::SocketFactory *sockf, void *ssl, UInt32 *s) : TCPClient(sockf, s)
{
	this->clsData = MemAlloc(ClassData, 1);
	this->clsData->ssl = (SSL*)ssl;
}

Net::OpenSSLClient::~OpenSSLClient()
{
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
			this->currCnt += (UInt32)ret;
			return (UInt32)ret;
		}
		UInt32 err = this->GetLastErrorCode();
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
		int ret = SSL_write(this->clsData->ssl, buff, (int)size);
		if (ret > 0)
		{
			this->currCnt += (UInt32)ret;
			return (UInt32)ret;
		}
		this->flags |= 1;
		return 0;
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
	if (this->s)
	{
		SSL_shutdown(this->clsData->ssl);
	}
	this->Net::TCPClient::Close();
}

Bool Net::OpenSSLClient::Recover()
{
	return false;
}
