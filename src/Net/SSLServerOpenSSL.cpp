#include "Stdafx.h"
#include "Net/OpenSSLCore.h"
#include "Net/SSLClient.h"
#include "Net/SSLServer.h"
#include "Text/MyString.h"
#include <openssl/ssl.h>
#include <openssl/err.h>

struct Net::SSLServer::ClassData
{
	SSL_CTX *ctx;
};

Net::SSLServer::SSLServer(Net::SocketFactory *sockf, Method method, const UTF8Char *certFile, const UTF8Char *keyFile)
{
	this->sockf = sockf;
	Net::OpenSSLCore::Init();
	const SSL_METHOD *m = 0;
	switch (method)
	{
	case M_SSLV3:
#ifdef OPENSSL_NO_SSL3_METHOD
		m = SSLv23_method();
#else
		m = SSLv3_method();
#endif
		break;
	case M_SSLV23:
		m = SSLv23_method();
		break;
	case M_TLS:
		m = TLS_method();
		break;
	case M_TLSV1:
#ifdef OPENSSL_NO_TLS1_METHOD
		m = TLS_method();
#else
		m = TLSv1_method();
#endif
		break;
	case M_TLSV1_1:
#ifdef OPENSSL_NO_TLS1_1_METHOD
		m = TLS_method();
#else
		m = TLSv1_1_method();
#endif
		break;
	case M_TLSV1_2:
#ifdef OPENSSL_NO_TLS1_2_METHOD
		m = TLS_method();
#else
		m = TLSv1_2_method();
#endif
		break;
	case M_DTLS:
		m = DTLS_method();
		break;
	case M_DTLSV1:
#ifdef OPENSSL_NO_DTLS1_METHOD
		m = DTLS_method();
#else
		m = DTLSv1_method();
#endif
		break;
	case M_DTLSV1_2:
#ifdef OPENSSL_NO_DTLS1_2_METHOD
		m = DTLS_method();
#else
		m = DTLSv1_2_method();
#endif
		break;
	}
	this->clsData = MemAlloc(ClassData, 1);
	this->clsData->ctx = SSL_CTX_new(m);
	if (this->clsData->ctx)
	{
		SSL_CTX_set_ecdh_auto(this->clsData->ctx, 1);
		if (SSL_CTX_use_certificate_file(this->clsData->ctx, (const Char*)certFile, SSL_FILETYPE_PEM) <= 0)
		{
			SSL_CTX_free(this->clsData->ctx);
			this->clsData->ctx = 0;
			return;
		}

		if (SSL_CTX_use_PrivateKey_file(this->clsData->ctx, (const Char*)keyFile, SSL_FILETYPE_PEM) <= 0 )
		{
			SSL_CTX_free(this->clsData->ctx);
			this->clsData->ctx = 0;
			return;
		}
	}
}

Net::SSLServer::~SSLServer()
{
	if (this->clsData->ctx)
	{
		SSL_CTX_free(this->clsData->ctx);
	}
	MemFree(this->clsData);
	Net::OpenSSLCore::Deinit();
}

Bool Net::SSLServer::IsError()
{
	return this->clsData->ctx == 0;
}

UTF8Char *Net::SSLServer::GetErrorDetail(UTF8Char *sbuff)
{
	UInt32 err = (UInt32)ERR_get_error();
	if (err == 0)
	{
		*sbuff = 0;
		return sbuff;
	}
	ERR_error_string(err, (char*)sbuff);
	return &sbuff[Text::StrCharCnt(sbuff)];
}

Net::TCPClient *Net::SSLServer::CreateClient(UInt32 *s)
{
	SSL *ssl = SSL_new(this->clsData->ctx);
	SSL_set_fd(ssl, this->sockf->SocketGetFD(s));
	if (SSL_accept(ssl) <= 0)
	{
		SSL_free(ssl);
		return 0;
	}
	else
	{
		Net::TCPClient *cli;
		NEW_CLASS(cli, SSLClient(this->sockf, ssl, s));
		return cli;
	}
}
