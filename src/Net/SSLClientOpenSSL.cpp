#include "Stdafx.h"
#include "Net/SSLClient.h"
#include <openssl/ssl.h>
#include <openssl/err.h>

struct Net::SSLClient::ClassData
{
	SSL *ssl;
};

UInt32 Net::SSLClient::GetLastErrorCode()
{
	UInt32 lastError = 0;
	UInt32 thisError;
	while ((thisError = (UInt32)ERR_get_error()) != 0)
	{
		lastError = thisError;
	}
	return lastError;
}

Net::SSLClient::SSLClient(Net::SocketFactory *sockf, void *ssl, UInt32 *s) : TCPClient(sockf, s)
{
	this->clsData = MemAlloc(ClassData, 1);
	this->clsData->ssl = (SSL*)ssl;
}

Net::SSLClient::~SSLClient()
{
	SSL_free(this->clsData->ssl);
	MemFree(this->clsData);
}

UOSInt Net::SSLClient::Read(UInt8 *buff, UOSInt size)
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

UOSInt Net::SSLClient::Write(const UInt8 *buff, UOSInt size)
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

void *Net::SSLClient::BeginRead(UInt8 *buff, UOSInt size, Sync::Event *evt)
{
	UOSInt ret = this->Read(buff, size);
	if (ret)
	{
		evt->Set();
	}
	return (void*)ret;
}

UOSInt Net::SSLClient::EndRead(void *reqData, Bool toWait, Bool *incomplete)
{
	*incomplete = false;
	return (UOSInt)reqData;
}

void Net::SSLClient::CancelRead(void *reqData)
{

}

void *Net::SSLClient::BeginWrite(const UInt8 *buff, UOSInt size, Sync::Event *evt)
{
	UOSInt ret = this->Write(buff, size);
	if (ret)
	{
		evt->Set();
	}
	return (void*)ret;
}

UOSInt Net::SSLClient::EndWrite(void *reqData, Bool toWait)
{
	return (UOSInt)reqData;
}

void Net::SSLClient::CancelWrite(void *reqData)
{

}

Int32 Net::SSLClient::Flush()
{
	return 0;
}

void Net::SSLClient::Close()
{
	if (this->s)
	{
		SSL_shutdown(this->clsData->ssl);
	}
	this->Net::TCPClient::Close();
}

Bool Net::SSLClient::Recover()
{
	return false;
}
