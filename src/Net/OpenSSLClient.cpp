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
	Optional<Data::ArrayListNN<Crypto::Cert::Certificate>> remoteCerts;
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

Net::OpenSSLClient::OpenSSLClient(NN<Net::SocketFactory> sockf, void *ssl, NN<Socket> s) : SSLClient(sockf, s)
{
	this->clsData = MemAlloc(ClassData, 1);
	this->clsData->ssl = (SSL*)ssl;
	this->clsData->remoteCerts = 0;
	this->clsData->shutdown = false;

	STACK_OF(X509) *certs = SSL_get_peer_cert_chain(this->clsData->ssl);
#if OPENSSL_VERSION_NUMBER >= 0x10100010
	if (certs == 0 || sk_X509_num(certs) == 0)
	{
		certs = SSL_get0_verified_chain(this->clsData->ssl);
	}
#endif
	if (certs != 0)
	{
		NN<Data::ArrayListNN<Crypto::Cert::Certificate>> certList;
		NN<Crypto::Cert::OpenSSLCert> cert;
		NEW_CLASSNN(certList, Data::ArrayListNN<Crypto::Cert::Certificate>());
		this->clsData->remoteCerts = certList;
		int i = 0;
		int j = sk_X509_num(certs);
		while (i < j)
		{
			NEW_CLASSNN(cert, Crypto::Cert::OpenSSLCert(sk_X509_value(certs, i)));
			certList->Add(cert);
			i++;
		}
	}
/*	else
	{
		X509 *cert = SSL_get_peer_certificate(this->clsData->ssl);
		if (cert)
		{
			Crypto::Cert::OpenSSLCert *crt;
			NEW_CLASS(crt, Crypto::Cert::OpenSSLCert(cert));
			this->clsData->remoteCerts->Add(crt);
		}
	}*/
}

Net::OpenSSLClient::~OpenSSLClient()
{
	NN<Data::ArrayListNN<Crypto::Cert::Certificate>> certList;
	if (this->clsData->remoteCerts.SetTo(certList))
	{
		UOSInt i = certList->GetCount();
		NN<Crypto::Cert::Certificate> cert;
		while (i-- > 0)
		{
			cert = certList->GetItemNoCheck(i);
			cert.Delete();
		}
		certList.Delete();
		this->clsData->remoteCerts = 0;
	}
	SSL_free(this->clsData->ssl);
	MemFree(this->clsData);
}

UOSInt Net::OpenSSLClient::Read(const Data::ByteArray &buff)
{
	if (this->s.NotNull() && (this->flags & 6) == 0)
	{
		int ret = SSL_read(this->clsData->ssl, buff.Arr().Ptr(), (int)(OSInt)buff.GetSize());
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

UOSInt Net::OpenSSLClient::Write(Data::ByteArrayR buff)
{
	if (this->s.NotNull() && (this->flags & 5) == 0)
	{
		UOSInt totalWrite = 0;
		while (buff.GetSize() > 0)
		{
			int ret = SSL_write(this->clsData->ssl, buff.Arr().Ptr(), (int)buff.GetSize());
			if (ret > 0)
			{
	#if defined(VERBOSE)
				printf("OSSLClient: Write %d bytes\r\n", (UOSInt)ret);
	#endif
				this->currCnt += (UInt32)ret;
				totalWrite += (UInt32)ret;
				buff += ret;
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

void *Net::OpenSSLClient::BeginRead(const Data::ByteArray &buff, Sync::Event *evt)
{
	UOSInt ret = this->Read(buff);
	if (ret)
	{
		evt->Set();
	}
	return (void*)ret;
}

UOSInt Net::OpenSSLClient::EndRead(void *reqData, Bool toWait, OutParam<Bool> incomplete)
{
	incomplete.Set(false);
	return (UOSInt)reqData;
}

void Net::OpenSSLClient::CancelRead(void *reqData)
{

}

void *Net::OpenSSLClient::BeginWrite(Data::ByteArrayR buff, Sync::Event *evt)
{
	UOSInt ret = this->Write(buff);
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
	this->TCPClient::ShutdownSend();
	this->Net::TCPClient::Close();
}

Bool Net::OpenSSLClient::Recover()
{
	return false;
}

void Net::OpenSSLClient::ShutdownSend()
{
	SSL_shutdown(this->clsData->ssl);
}

Optional<Crypto::Cert::Certificate> Net::OpenSSLClient::GetRemoteCert()
{
	NN<Data::ArrayListNN<Crypto::Cert::Certificate>> certs;
	if (this->clsData->remoteCerts.SetTo(certs))
		return certs->GetItem(0);
	else
		return 0;
}

Optional<const Data::ReadingListNN<Crypto::Cert::Certificate>> Net::OpenSSLClient::GetRemoteCerts()
{
	return this->clsData->remoteCerts;
}
