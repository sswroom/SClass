#include "Stdafx.h"
#include "Crypto/Cert/X509Cert.h"
#include "Crypto/Cert/X509PrivKey.h"
#include "Data/DateTime.h"
#include "IO/StmData/MemoryDataRef.h"
#include "Net/OpenSSLClient.h"
#include "Net/OpenSSLCore.h"
#include "Net/OpenSSLEngine.h"
#include "Parser/FileParser/X509Parser.h"
#include "Text/MyString.h"
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/err.h>
#include <openssl/ec.h>

#define SHOW_DEBUG
#ifdef SHOW_DEBUG
#if defined(DEBUGCON)
#include <stdio.h>
#include <syslog.h>
#define printf(fmt, ...) {Char sbuff[512]; sprintf(sbuff, fmt, __VA_ARGS__); syslog(LOG_DEBUG, sbuff);}
#else
#include <stdio.h>
#endif
#endif

#if OPENSSL_VERSION_NUMBER < 0x10100000L
#define TLS_method() TLSv1_method()
#endif
struct Net::OpenSSLEngine::ClassData
{
	SSL_CTX *ctx;
	Crypto::Cert::X509File *cliCert;
	Crypto::Cert::X509File *cliKey;
	Data::FastStringMap<Bool> *alpnSupports;
};

Net::SSLClient *Net::OpenSSLEngine::CreateServerConn(Socket *s)
{
	SSL *ssl = SSL_new(this->clsData->ctx);
	this->sockf->SetRecvTimeout(s, 2000);
	this->sockf->SetNoDelay(s, true);
	SSL_set_fd(ssl, this->sockf->SocketGetFD(s));
	int ret;
	if ((ret = SSL_accept(ssl)) <= 0)
	{
#ifdef SHOW_DEBUG
		int code = SSL_get_error(ssl, ret);
		printf("SSL_accept: ret = %d, Error code = %d, %s\r\n", ret, code, ERR_error_string(ERR_get_error(), 0));
#endif
		SSL_free(ssl);
		this->sockf->DestroySocket(s);
		return 0;
	}
	else
	{
		this->sockf->SetRecvTimeout(s, 120000);
		Net::SSLClient *cli;
		NEW_CLASS(cli, OpenSSLClient(this->sockf, ssl, s));
		return cli;
	}
}

Net::SSLClient *Net::OpenSSLEngine::CreateClientConn(void *sslObj, Socket *s, Text::CString hostName, ErrorType *err)
{
	SSL *ssl = (SSL*)sslObj;
	this->sockf->SetNoDelay(s, true);
	this->sockf->SetRecvTimeout(s, 2000);
	SSL_set_fd(ssl, this->sockf->SocketGetFD(s));
	SSL_set_tlsext_host_name(ssl, hostName.v);
	int ret;
	if ((ret = SSL_connect(ssl)) <= 0)
	{
		this->sockf->DestroySocket(s);
#ifdef SHOW_DEBUG
		int code = SSL_get_error(ssl, ret);
		printf("SSL_connect: ret = %d, Error code = %d\r\n", ret, code);
#endif
		SSL_free(ssl);
		if (err)
			*err = ErrorType::InitSession;
		return 0;
	}
	if (!this->skipCertCheck)
	{
		stack_st_X509 *certs = SSL_get_peer_cert_chain(ssl);
		if (certs == 0)
		{
			this->sockf->DestroySocket(s);
			SSL_free(ssl);
			if (err)
				*err = ErrorType::CertNotFound;
			return 0;
		}
		X509 *cert = sk_X509_value(certs, 0);
		UInt8 certBuff[4096];
		UInt8 *certPtr = certBuff;
		Int32 certLen = i2d_X509(cert, &certPtr);
		if (certLen <= 0)
		{
			this->sockf->DestroySocket(s);
			SSL_free(ssl);
			if (err)
				*err = ErrorType::CertNotFound;
			return 0;
		}
		Crypto::Cert::X509Cert *svrCert;
		NEW_CLASS(svrCert, Crypto::Cert::X509Cert(hostName, Data::ByteArrayR(certBuff, (UInt32)certLen)));
		Data::DateTime dt;
		Int64 currTime;
		dt.SetCurrTimeUTC();
		currTime = dt.ToTicks();
		if (!svrCert->GetNotBefore(&dt) || currTime < dt.ToTicks())
		{
			DEL_CLASS(svrCert);
			this->sockf->DestroySocket(s);
			SSL_free(ssl);
			if (err)
				*err = ErrorType::InvalidPeriod;
			return 0;
		}
		if (!svrCert->GetNotAfter(&dt) || currTime > dt.ToTicks())
		{
			DEL_CLASS(svrCert);
			this->sockf->DestroySocket(s);
			SSL_free(ssl);
			if (err)
				*err = ErrorType::InvalidPeriod;
			return 0;
		}
		if (!svrCert->DomainValid(hostName))
		{
			DEL_CLASS(svrCert);
			this->sockf->DestroySocket(s);
			SSL_free(ssl);
			if (err)
				*err = ErrorType::InvalidName;
			return 0;
		}
		if (svrCert->IsSelfSigned())
		{
			DEL_CLASS(svrCert);
			this->sockf->DestroySocket(s);
			SSL_free(ssl);
			if (err)
				*err = ErrorType::SelfSign;
			return 0;
		}
		DEL_CLASS(svrCert);
	}
	this->sockf->SetRecvTimeout(s, 120000);
	Net::SSLClient *cli;
	NEW_CLASS(cli, OpenSSLClient(this->sockf, ssl, s));
	return cli;
}

Bool Net::OpenSSLEngine::SetRSAPadding(void *ctx, Crypto::Encrypt::RSACipher::Padding rsaPadding)
{
	Int32 ipadding;
	switch (rsaPadding)
	{
	case Crypto::Encrypt::RSACipher::Padding::PKCS1:
		ipadding = RSA_PKCS1_PADDING;
		break;
	case Crypto::Encrypt::RSACipher::Padding::NoPadding:
		ipadding = RSA_NO_PADDING;
		break;
	case Crypto::Encrypt::RSACipher::Padding::PKCS1_OAEP:
		ipadding = RSA_PKCS1_OAEP_PADDING;
		break;
	case Crypto::Encrypt::RSACipher::Padding::X931:
		ipadding = RSA_X931_PADDING;
		break;
	case Crypto::Encrypt::RSACipher::Padding::PKCS1_PSS:
		ipadding = RSA_PKCS1_PSS_PADDING;
		break;
	case Crypto::Encrypt::RSACipher::Padding::PKCS1_WithTLS:
#if defined(RSA_PKCS1_WITH_TLS_PADDING)
		ipadding = RSA_PKCS1_WITH_TLS_PADDING;
#else
		ipadding = 7;
#endif
		break;
	default:
		return false;

	}
	return EVP_PKEY_CTX_set_rsa_padding((EVP_PKEY_CTX*)ctx, ipadding) > 0;
}

Net::OpenSSLEngine::OpenSSLEngine(Net::SocketFactory *sockf, Method method) : Net::SSLEngine(sockf)
{
	Net::OpenSSLCore::Init();
	const SSL_METHOD *m = 0;
	switch (method)
	{
	case Method::SSLV3:
#ifdef OPENSSL_NO_SSL3_METHOD
		m = SSLv23_method();
#else
		m = SSLv3_method();
#endif
		break;
	case Method::SSLV23:
		m = SSLv23_method();
		break;
	case Method::Default:
	case Method::TLS:
		m = TLS_method();
		break;
	case Method::TLSV1:
#if defined(OPENSSL_NO_TLS1_METHOD) || defined(OSSL_DEPRECATEDIN_1_1_0)
		m = TLS_method();
#else
		m = TLSv1_method();
#endif
		break;
	case Method::TLSV1_1:
#if defined(OPENSSL_NO_TLS1_1_METHOD) || defined(OSSL_DEPRECATEDIN_1_1_0)
		m = TLS_method();
#else
		m = TLSv1_1_method();
#endif
		break;
	case Method::TLSV1_2:
#if defined(OPENSSL_NO_TLS1_2_METHOD) || defined(OSSL_DEPRECATEDIN_1_1_0)
		m = TLS_method();
#else
		m = TLSv1_2_method();
#endif
		break;
	case Method::DTLS:
		m = DTLS_method();
		break;
	case Method::DTLSV1:
#if defined(OPENSSL_NO_DTLS1_METHOD) || defined(OSSL_DEPRECATEDIN_1_1_0)
		m = DTLS_method();
#else
		m = DTLSv1_method();
#endif
		break;
	case Method::DTLSV1_2:
#if defined(OPENSSL_NO_DTLS1_2_METHOD) || defined(OSSL_DEPRECATEDIN_1_1_0)
		m = DTLS_method();
#else
		m = DTLSv1_2_method();
#endif
		break;
	}
	this->clsData = MemAlloc(ClassData, 1);
	this->clsData->ctx = SSL_CTX_new(m);
	this->clsData->cliCert = 0;
	this->clsData->cliKey = 0;
	this->clsData->alpnSupports = 0;
	if (this->clsData->ctx)
	{
		SSL_CTX_set_options(this->clsData->ctx, SSL_OP_NO_TICKET);
	}
	this->skipCertCheck = false;
}

Net::OpenSSLEngine::~OpenSSLEngine()
{
	if (this->clsData->ctx)
	{
		SSL_CTX_free(this->clsData->ctx);
	}
	SDEL_CLASS(this->clsData->cliCert);
	SDEL_CLASS(this->clsData->cliKey);
	SDEL_CLASS(this->clsData->alpnSupports);
	MemFree(this->clsData);
	Net::OpenSSLCore::Deinit();
}

Bool Net::OpenSSLEngine::IsError()
{
	return this->clsData->ctx == 0;
}

Bool Net::OpenSSLEngine::ServerSetCertsASN1(Crypto::Cert::X509Cert *certASN1, Crypto::Cert::X509File *keyASN1, Crypto::Cert::X509Cert *caCert)
{
	if (this->clsData->ctx == 0)
	{
		return false;
	}
	
	if (certASN1 != 0 && keyASN1 != 0)
	{
		SSL_CTX_set_ecdh_auto(this->clsData->ctx, 1);
		if (SSL_CTX_use_certificate_ASN1(this->clsData->ctx, (int)certASN1->GetASN1BuffSize(), certASN1->GetASN1Buff()) <= 0)
		{
			return false;
		}
		if (caCert)
		{
			const UInt8 *asn1 = caCert->GetASN1Buff();
			X509 *x509 = d2i_X509(0, &asn1, (long)caCert->GetASN1BuffSize());
			if (x509 == 0)
			{
				return false;
			}
			SSL_CTX_add_extra_chain_cert(this->clsData->ctx, x509);
		}

		if (keyASN1->GetFileType() == Crypto::Cert::X509File::FileType::PrivateKey)
		{
			if (SSL_CTX_use_PrivateKey_ASN1(EVP_PKEY_RSA, this->clsData->ctx, keyASN1->GetASN1Buff(), (long)keyASN1->GetASN1BuffSize()) <= 0)
			{
				return false;
			}
			return true;
		}
		else if (keyASN1->GetFileType() == Crypto::Cert::X509File::FileType::Key && ((Crypto::Cert::X509Key*)keyASN1)->IsPrivateKey())
		{
			Crypto::Cert::X509PrivKey *privKey = Crypto::Cert::X509PrivKey::CreateFromKey((Crypto::Cert::X509Key*)keyASN1);
			if (privKey)
			{
				if (SSL_CTX_use_PrivateKey_ASN1(EVP_PKEY_RSA, this->clsData->ctx, privKey->GetASN1Buff(), (long)privKey->GetASN1BuffSize()) <= 0)
				{
					DEL_CLASS(privKey);
					return false;
				}
				DEL_CLASS(privKey);
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else if (certASN1 != 0 && certASN1->GetFileType() == Crypto::Cert::X509File::FileType::Cert && keyASN1 != 0 && keyASN1->GetFileType() == Crypto::Cert::X509File::FileType::Key)
	{
		SSL_CTX_set_ecdh_auto(this->clsData->ctx, 1);
		if (SSL_CTX_use_certificate_ASN1(this->clsData->ctx, (int)certASN1->GetASN1BuffSize(), certASN1->GetASN1Buff()) <= 0)
		{
			return false;
		}
		Crypto::Cert::X509PrivKey *privKey = Crypto::Cert::X509PrivKey::CreateFromKey((Crypto::Cert::X509Key*)keyASN1);
		if (SSL_CTX_use_PrivateKey_ASN1(EVP_PKEY_RSA, this->clsData->ctx, privKey->GetASN1Buff(), (long)privKey->GetASN1BuffSize()) <= 0)
		{
			DEL_CLASS(privKey);
			return false;
		}
		DEL_CLASS(privKey);
		return true;
	}
	return false;
}

static int OpenSSLEngine_verify_cb(int preverify_ok, X509_STORE_CTX *x509_ctx)
{
	return 1;
}

Bool Net::OpenSSLEngine::ServerSetRequireClientCert(ClientCertType cliCert)
{
	if (this->clsData->ctx == 0)
	{
		return false;
	}
	int mode;
	switch (cliCert)
	{
	case ClientCertType::Optional:
		mode = SSL_VERIFY_PEER;
		break;
	case ClientCertType::MustExist:
		mode = SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT;
		break;
	case ClientCertType::None:
	default:
		mode = SSL_VERIFY_NONE;
		break;
	}
	SSL_CTX_set_verify(this->clsData->ctx, mode, OpenSSLEngine_verify_cb);
	return true;
}

Bool Net::OpenSSLEngine::ServerSetClientCA(Text::CString clientCA)
{
	if (this->clsData->ctx == 0)
	{
		return false;
	}
	STACK_OF(X509_NAME) *names = sk_X509_NAME_new_null();
	X509_NAME *name = X509_NAME_new();
	if (!X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, clientCA.v, (int)clientCA.leng, -1, 0))
	{
		X509_NAME_free(name);
		sk_X509_NAME_free(names);
		return false;
	}
	sk_X509_NAME_push(names, name);
	SSL_CTX_set_client_CA_list(this->clsData->ctx, names);
	return true;
}

int OpenSSLEngine_alpn_select_cb(SSL *ssl, const unsigned char **out, unsigned char *outlen, const unsigned char *in, unsigned int inlen, void *arg)
{
	Net::OpenSSLEngine::ClassData *clsData = (Net::OpenSSLEngine::ClassData *)arg;
	while (true)
	{
		if ((UOSInt)in[0] + 1 > inlen)
			return SSL_TLSEXT_ERR_NOACK;
		if (clsData->alpnSupports->GetC(Text::CString(in + 1, in[0])))
		{
			*out = in + 1;
			*outlen = in[0];
			return SSL_TLSEXT_ERR_OK;
		}
		inlen -= in[0] + 1;
		in += in[0] + 1;
	}
}

int OpenSSLEngine_next_proto_select_cb(SSL *s, unsigned char **out, unsigned char *outlen, const unsigned char *in, unsigned int inlen, void *arg)
{
	Net::OpenSSLEngine::ClassData *clsData = (Net::OpenSSLEngine::ClassData *)arg;
	while (true)
	{
		if ((UOSInt)in[0] + 1 > inlen)
			return SSL_TLSEXT_ERR_NOACK;
		if (clsData->alpnSupports->GetC(Text::CString(in + 1, in[0])))
		{
			*out = (unsigned char*)in + 1;
			*outlen = in[0];
			return SSL_TLSEXT_ERR_OK;
		}
		inlen -= in[0] + 1;
		in += in[0] + 1;
	}
}

Bool Net::OpenSSLEngine::ServerAddALPNSupport(Text::CString proto)
{
#if OPENSSL_VERSION_NUMBER >= 0x10002000L
	if (this->clsData->ctx == 0)
	{
		return false;
	}
	if (this->clsData->alpnSupports == 0)
	{
		NEW_CLASS(this->clsData->alpnSupports, Data::FastStringMap<Bool>());
		SSL_CTX_set_alpn_select_cb(this->clsData->ctx, OpenSSLEngine_alpn_select_cb, this->clsData);
		SSL_CTX_set_next_proto_select_cb(this->clsData->ctx, OpenSSLEngine_next_proto_select_cb, this->clsData);
	}
	this->clsData->alpnSupports->PutC(proto, true);
	return true;
#else
	return false;
#endif
}


Bool Net::OpenSSLEngine::ClientSetCertASN1(Crypto::Cert::X509Cert *certASN1, Crypto::Cert::X509File *keyASN1)
{
	SDEL_CLASS(this->clsData->cliCert);
	SDEL_CLASS(this->clsData->cliKey);
	if (certASN1)
	{
		this->clsData->cliCert = (Crypto::Cert::X509File*)certASN1->Clone();
	}
	if (keyASN1)
	{
		this->clsData->cliKey = (Crypto::Cert::X509File*)keyASN1->Clone();
	}
	return true;
}

void Net::OpenSSLEngine::ClientSetSkipCertCheck(Bool skipCertCheck)
{
	this->skipCertCheck = skipCertCheck;
}


Net::SSLClient *Net::OpenSSLEngine::ClientConnect(Text::CString hostName, UInt16 port, ErrorType *err, Data::Duration timeout)
{
	Net::SocketUtil::AddressInfo addr[1];
	UOSInt addrCnt = this->sockf->DNSResolveIPs(hostName, addr, 1);
	if (addrCnt == 0)
	{
		if (err)
			*err = ErrorType::HostnameNotResolved;
		return 0;
	}
	SSL *ssl = SSL_new(this->clsData->ctx);
	if (ssl == 0)
	{
		if (err)
			*err = ErrorType::OutOfMemory;
		return 0;
	}
	if (this->clsData->cliCert)
	{
		SSL_use_certificate_ASN1(ssl, this->clsData->cliCert->GetASN1Buff(), (int)(OSInt)this->clsData->cliCert->GetASN1BuffSize());
	}
	if (this->clsData->cliKey)
	{
		SSL_use_PrivateKey_ASN1(EVP_PKEY_RSA, ssl, this->clsData->cliKey->GetASN1Buff(), (int)(OSInt)this->clsData->cliKey->GetASN1BuffSize());
	}
	Socket *s;
	UOSInt addrInd = 0;
	while (addrInd < addrCnt)
	{
		if (addr[addrInd].addrType == Net::AddrType::IPv4)
		{
			s = this->sockf->CreateTCPSocketv4();
			if (s == 0)
			{
				SSL_free(ssl);
				if (err)
					*err = ErrorType::OutOfMemory;
				return 0;
			}
			if (this->sockf->Connect(s, &addr[addrInd], port, timeout))
			{
				return CreateClientConn(ssl, s, hostName, err);
			}
			this->sockf->DestroySocket(s);
		}
		else if (addr[addrInd].addrType == Net::AddrType::IPv6)
		{
			s = this->sockf->CreateTCPSocketv6();
			if (s == 0)
			{
				SSL_free(ssl);
				if (err)
					*err = ErrorType::OutOfMemory;
				return 0;
			}
			if (this->sockf->Connect(s, &addr[addrInd], port, timeout))
			{
				return CreateClientConn(ssl, s, hostName, err);
			}
			this->sockf->DestroySocket(s);
		}
		addrInd++;
	}

	SSL_free(ssl);
	if (err)
		*err = ErrorType::CannotConnect;
	return 0;
}

Net::SSLClient *Net::OpenSSLEngine::ClientInit(Socket *s, Text::CString hostName, ErrorType *err)
{
	SSL *ssl = SSL_new(this->clsData->ctx);
	if (ssl == 0)
	{
		if (err)
			*err = ErrorType::OutOfMemory;
		return 0;
	}
	if (this->clsData->cliCert)
	{
		SSL_use_certificate_ASN1(ssl, this->clsData->cliCert->GetASN1Buff(), (int)(OSInt)this->clsData->cliCert->GetASN1BuffSize());
	}
	if (this->clsData->cliKey)
	{
		SSL_use_PrivateKey_ASN1(EVP_PKEY_RSA, ssl, this->clsData->cliKey->GetASN1Buff(), (int)(OSInt)this->clsData->cliKey->GetASN1BuffSize());
	}
	return CreateClientConn(ssl, s, hostName, err);
}

UTF8Char *Net::OpenSSLEngine::GetErrorDetail(UTF8Char *sbuff)
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

Bool Net::OpenSSLEngine::GenerateCert(Text::CString country, Text::CString company, Text::CString commonName, Crypto::Cert::X509Cert **certASN1, Crypto::Cert::X509File **keyASN1)
{
	if (certASN1 == 0 || keyASN1 == 0)
	{
		return false;
	}

	Bool succ = false;
	EVP_PKEY *pkey;
#if defined(OSSL_DEPRECATEDIN_3_0)
	pkey = EVP_RSA_gen(2048);
	if (pkey)
	{
#else
	BIGNUM *bn = BN_new();
	BN_set_word(bn, RSA_F4);
	RSA *rsa = RSA_new();
	if (RSA_generate_key_ex(rsa, 2048, bn, 0) > 0)
	{
		pkey = EVP_PKEY_new();
		EVP_PKEY_assign(pkey, EVP_PKEY_RSA, rsa);
#endif
		X509 *cert = X509_new();
		ASN1_INTEGER_set(X509_get_serialNumber(cert), 1);

		X509_gmtime_adj(X509_get_notBefore(cert), 0);
		X509_gmtime_adj(X509_get_notAfter(cert), 365 * 24 * 3600);

		X509_set_pubkey(cert, pkey);

		X509_name_st *name = X509_get_subject_name(cert);
		X509_NAME_add_entry_by_txt(name, "C",  MBSTRING_ASC, country.v, (int)country.leng, -1, 0);
		X509_NAME_add_entry_by_txt(name, "O",  MBSTRING_ASC, company.v, (int)company.leng, -1, 0);
		X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, commonName.v, (int)commonName.leng, -1, 0);

		X509_set_issuer_name(cert, name);
		X509_sign(cert, pkey, EVP_sha256());

		BIO *bio1;
		BIO *bio2;
		UInt8 buff[4096];
		Crypto::Cert::X509File *pobjKey = 0;
		Crypto::Cert::X509Cert *pobjCert = 0;

		BIO_new_bio_pair(&bio1, 4096, &bio2, 4096);
		PEM_write_bio_PrivateKey(bio1, pkey, nullptr, nullptr, 0, nullptr, nullptr);
		int readSize = BIO_read(bio2, buff, 4096);
		if (readSize > 0)
		{
			NotNullPtr<Text::String> fileName = Text::String::New(UTF8STRC("Certificate.key"));
			pobjKey = Parser::FileParser::X509Parser::ParseBuff(BYTEARR(buff).SubArray(0, (UInt32)readSize), fileName);
			fileName->Release();
		}
		PEM_write_bio_X509(bio1, cert);
		readSize = BIO_read(bio2, buff, 4096);
		if (readSize > 0)
		{
			NotNullPtr<Text::String> fileName = Text::String::New(UTF8STRC("Certificate.crt"));
			pobjCert = (Crypto::Cert::X509Cert*)Parser::FileParser::X509Parser::ParseBuff(BYTEARR(buff).SubArray(0, (UInt32)readSize), fileName);
			fileName->Release();
		}
		BIO_free(bio1);
		BIO_free(bio2);
		X509_free(cert);
		EVP_PKEY_free(pkey);

		if (pobjCert && pobjKey)
		{
			succ = true;
			pobjCert->SetSourceName(CSTR("cert.crt"));
			pobjKey->SetSourceName(CSTR("RSAKey.key"));
			*certASN1 = pobjCert;
			*keyASN1 = pobjKey;
		}
		else
		{
			SDEL_CLASS(pobjCert);
			SDEL_CLASS(pobjKey);
		}
	}
#if !defined(OSSL_DEPRECATEDIN_3_0)
	else
	{
		RSA_free(rsa);
	}
	BN_free(bn);
#endif
	return succ;
}

Crypto::Cert::X509Key *Net::OpenSSLEngine::GenerateRSAKey()
{
#if !defined(OSSL_DEPRECATEDIN_3_0)
	BIGNUM *bn = BN_new();
	BN_set_word(bn, RSA_F4);
	RSA *rsa = RSA_new();
	if (RSA_generate_key_ex(rsa, 2048, bn, 0) > 0)
	{
		BIO *bio1;
		BIO *bio2;
		UInt8 buff[4096];
		Crypto::Cert::X509File *pobjKey = 0;

		BIO_new_bio_pair(&bio1, 4096, &bio2, 4096);
		PEM_write_bio_RSAPrivateKey(bio1, rsa, nullptr, nullptr, 0, nullptr, nullptr);
		int readSize = BIO_read(bio2, buff, 4096);
		if (readSize > 0)
		{
			Text::String *fileName = Text::String::New(UTF8STRC("RSAKey.key"));
			pobjKey = Parser::FileParser::X509Parser::ParseBuff(BYTEARR(buff).SubArray(0, (UOSInt)readSize), fileName);
			fileName->Release();
		}
		BIO_free(bio1);
		BIO_free(bio2);

		RSA_free(rsa);
		BN_free(bn);
		return (Crypto::Cert::X509Key*)pobjKey;
	}
	RSA_free(rsa);
	BN_free(bn);
	return 0;
#else
	EVP_PKEY *pkey;
	pkey = EVP_RSA_gen(2048);
	if (pkey)
	{
		BIO *bio1;
		BIO *bio2;
		UInt8 buff[4096];
		Crypto::Cert::X509File *pobjKey = 0;

		BIO_new_bio_pair(&bio1, 4096, &bio2, 4096);
		PEM_write_bio_PrivateKey(bio1, pkey, nullptr, nullptr, 0, nullptr, nullptr);
		int readSize = BIO_read(bio2, buff, 4096);
		if (readSize > 0)
		{
			NotNullPtr<Text::String> fileName = Text::String::New(UTF8STRC("Certificate.key"));
			pobjKey = Parser::FileParser::X509Parser::ParseBuff(BYTEARR(buff).SubArray(0, (UInt32)readSize), fileName);
			if (pobjKey && pobjKey->GetFileType() == Crypto::Cert::X509File::FileType::PrivateKey)
			{
				Crypto::Cert::X509PrivKey *privKey = (Crypto::Cert::X509PrivKey*)pobjKey;
				pobjKey = privKey->CreateKey();
				DEL_CLASS(privKey);
			}
			fileName->Release();
		}
		BIO_free(bio1);
		BIO_free(bio2);
		EVP_PKEY_free(pkey);
		return (Crypto::Cert::X509Key*)pobjKey;
	}
	return 0;
#endif
}

int OpenSSLEngine_GetCurveName(Crypto::Cert::X509File::ECName ecName)
{
	switch (ecName)
	{
	case Crypto::Cert::X509File::ECName::secp256r1:
		return NID_X9_62_prime256v1;
	case Crypto::Cert::X509File::ECName::secp384r1:
		return NID_secp384r1;
	case Crypto::Cert::X509File::ECName::secp521r1:
		return NID_secp521r1;
	case Crypto::Cert::X509File::ECName::Unknown:
	default:
		return 0;
	}
}
EVP_PKEY *OpenSSLEngine_LoadKey(Crypto::Cert::X509Key *key, Bool privateKeyOnly)
{
	EVP_PKEY *pkey = 0;
	if (key->GetKeyType() == Crypto::Cert::X509File::KeyType::RSA)
	{
		const UInt8 *keyPtr = key->GetASN1Buff();
		pkey = d2i_PrivateKey(EVP_PKEY_RSA, 0, &keyPtr, (long)key->GetASN1BuffSize());
#ifdef SHOW_DEBUG
		if (pkey == 0)
		{
			printf("d2i_PrivateKey: error = %s\r\n", ERR_error_string(ERR_get_error(), 0));
		}
#endif
	}
	else if (key->GetKeyType() == Crypto::Cert::X509File::KeyType::ECDSA)
	{
		const UInt8 *keyPtr = key->GetASN1Buff();
		pkey = d2i_PrivateKey(EVP_PKEY_EC, 0, &keyPtr, (long)key->GetASN1BuffSize());
#ifdef SHOW_DEBUG
		if (pkey == 0)
		{
			printf("d2i_PrivateKey: error = %s\r\n", ERR_error_string(ERR_get_error(), 0));
		}
#endif
	}
	else if (privateKeyOnly)
	{
		return 0;
	}
	else if (key->GetKeyType() == Crypto::Cert::X509File::KeyType::ECPublic)
	{
		Text::StringBuilderUTF8 sb;
		key->ToASN1String(&sb);
		printf("%s\r\n", sb.ToString());
		Crypto::Cert::X509File::ECName ecName = key->GetECName();
		UOSInt keyLen;
		const UInt8 *keyPtr = key->GetECPublic(&keyLen);
		if (ecName == Crypto::Cert::X509File::ECName::Unknown)
		{
#ifdef SHOW_DEBUG
			printf("Unknown curve name\r\n");
#endif
			return 0;
		}
		else if (keyPtr == 0)
		{
#ifdef SHOW_DEBUG
			printf("EC public key not found\r\n");
#endif
			return 0;			
		}
		EC_GROUP *group = EC_GROUP_new_by_curve_name(OpenSSLEngine_GetCurveName(ecName));
		EC_POINT *point = EC_POINT_new(group);
		if (EC_POINT_oct2point(group, point, keyPtr, (size_t)keyLen, 0) == 0)
		{
#ifdef SHOW_DEBUG
			printf("EC_POINT_oct2point: error = %s\r\n", ERR_error_string(ERR_get_error(), 0));
#endif
			EC_POINT_free(point);
			EC_GROUP_free(group);
			return 0;
		}
		EC_KEY *eck = EC_KEY_new_by_curve_name(OpenSSLEngine_GetCurveName(ecName));
		if (EC_KEY_set_public_key(eck, point) == 0)
		{
#ifdef SHOW_DEBUG
			printf("EC_KEY_set_public_key: error = %s\r\n", ERR_error_string(ERR_get_error(), 0));
#endif
			EC_KEY_free(eck);
			EC_POINT_free(point);
			EC_GROUP_free(group);
			return 0;
		}
		EC_POINT_free(point);
		EC_GROUP_free(group);

		pkey = EVP_PKEY_new();
		int ret = EVP_PKEY_assign_EC_KEY(pkey, eck);
		if (ret == 0)
		{
#ifdef SHOW_DEBUG
			printf("EVP_PKEY_assign_EC_KEY: error = %s\r\n", ERR_error_string(ERR_get_error(), 0));
#endif
			EVP_PKEY_free(pkey);
			EC_KEY_free(eck);
			pkey = 0;
		}
	}
	else if (key->GetKeyType() == Crypto::Cert::X509File::KeyType::RSAPublic)
	{
		const UInt8 *keyPtr = key->GetASN1Buff();
		pkey = d2i_PublicKey(EVP_PKEY_RSA, 0, &keyPtr, (long)key->GetASN1BuffSize());
#ifdef SHOW_DEBUG
		if (pkey == 0)
		{
			printf("d2i_PublicKey: error = %s\r\n", ERR_error_string(ERR_get_error(), 0));
		}
#endif
	}
	else
	{
		return 0;
	}
	return pkey;
}

const EVP_MD *OpenSSLEngine_GetHash(Crypto::Hash::HashType hashType)
{
	if (hashType == Crypto::Hash::HashType::SHA256)
	{
		return EVP_sha256();
	}
	else if (hashType == Crypto::Hash::HashType::SHA384)
	{
		return EVP_sha384();
	}
	else if (hashType == Crypto::Hash::HashType::SHA512)
	{
		return EVP_sha512();
	}
	else if (hashType == Crypto::Hash::HashType::SHA224)
	{
		return EVP_sha224();
	}
	else if (hashType == Crypto::Hash::HashType::SHA1)
	{
		return EVP_sha1();
	}
	else if (hashType == Crypto::Hash::HashType::MD5)
	{
		return EVP_md5();
	}
	else
	{
		return 0;
	}
}
Bool Net::OpenSSLEngine::Signature(Crypto::Cert::X509Key *key, Crypto::Hash::HashType hashType, const UInt8 *payload, UOSInt payloadLen, UInt8 *signData, UOSInt *signLen)
{
	const EVP_MD *htype = OpenSSLEngine_GetHash(hashType);
	if (htype == 0)
	{
		return false;
	}
	EVP_PKEY *pkey = OpenSSLEngine_LoadKey(key, true);
	if (pkey == 0)
	{
		return false;
	}
	EVP_MD_CTX *emc = EVP_MD_CTX_create();
    if (emc == 0)
	{
		EVP_PKEY_free(pkey);
		return false;
    }
	unsigned int len;
    if (!EVP_SignInit_ex(emc, htype, NULL))
	{
		EVP_MD_CTX_destroy(emc);
		EVP_PKEY_free(pkey);
		return false;
    }
    if (!EVP_SignUpdate(emc, payload, payloadLen))
	{
		EVP_MD_CTX_destroy(emc);
		EVP_PKEY_free(pkey);
		return false;
    }
    if (!EVP_SignFinal(emc, signData, &len, pkey)) {
		EVP_MD_CTX_destroy(emc);
		EVP_PKEY_free(pkey);
		return false;
    }
    *signLen = len;
	EVP_MD_CTX_destroy(emc);
	EVP_PKEY_free(pkey);
	return true;
}

Bool Net::OpenSSLEngine::SignatureVerify(Crypto::Cert::X509Key *key, Crypto::Hash::HashType hashType, const UInt8 *payload, UOSInt payloadLen, const UInt8 *signData, UOSInt signLen)
{
	const EVP_MD *htype = OpenSSLEngine_GetHash(hashType);
	if (htype == 0)
	{
		return false;
	}
	EVP_PKEY *pkey = OpenSSLEngine_LoadKey(key, false);
	if (pkey == 0)
	{
		return false;
	}
	EVP_MD_CTX *emc = EVP_MD_CTX_create();
    if (emc == 0)
	{
		EVP_PKEY_free(pkey);
		return false;
    }
    if (!EVP_VerifyInit(emc, htype))
	{
		EVP_MD_CTX_destroy(emc);
		EVP_PKEY_free(pkey);
		return false;
    }
    if (!EVP_VerifyUpdate(emc, payload, payloadLen))
	{
		EVP_MD_CTX_destroy(emc);
		EVP_PKEY_free(pkey);
		return false;
    }
	Bool succ = false;
	int res = EVP_VerifyFinal(emc, signData, (UInt32)signLen, pkey);
//	printf("res = %d, %s\r\n", res, ERR_error_string(ERR_get_error(), 0));
    if (res < 0) {
		EVP_MD_CTX_destroy(emc);
		EVP_PKEY_free(pkey);
		return false;
    }
	succ = (res == 1);
	EVP_MD_CTX_destroy(emc);
	EVP_PKEY_free(pkey);
	return succ;
}

UOSInt Net::OpenSSLEngine::Encrypt(Crypto::Cert::X509Key *key, UInt8 *encData, const UInt8 *payload, UOSInt payloadLen, Crypto::Encrypt::RSACipher::Padding rsaPadding)
{
	EVP_PKEY *pkey = OpenSSLEngine_LoadKey(key, false);
	if (pkey == 0)
	{
		return 0;
	}
	EVP_PKEY_CTX *ctx;
	ctx = EVP_PKEY_CTX_new(pkey, 0);
	if (!ctx)
	{
		EVP_PKEY_free(pkey);
		return 0;
	}
	if (EVP_PKEY_encrypt_init(ctx) <= 0)
	{
		EVP_PKEY_CTX_free(ctx);
		EVP_PKEY_free(pkey);
		return 0;
	}
	if (key->GetKeyType() == Crypto::Cert::X509File::KeyType::RSA || key->GetKeyType() == Crypto::Cert::X509File::KeyType::RSAPublic)
	{
		if (!SetRSAPadding(ctx, rsaPadding))
		{
			EVP_PKEY_CTX_free(ctx);
			EVP_PKEY_free(pkey);
			return 0;
		}
	}
	size_t outlen = 512;
	int ret = EVP_PKEY_encrypt(ctx, encData, &outlen, payload, payloadLen);
	if (ret <= 0)
	{
//		printf("EVP_PKEY_encrypt returns %d\r\n", ret);
//		ERR_print_errors_fp(stdout);
		EVP_PKEY_CTX_free(ctx);
		EVP_PKEY_free(pkey);
		return 0;
	}
	EVP_PKEY_CTX_free(ctx);
	EVP_PKEY_free(pkey);
	return (UOSInt)outlen;
}

UOSInt Net::OpenSSLEngine::Decrypt(Crypto::Cert::X509Key *key, UInt8 *decData, const UInt8 *payload, UOSInt payloadLen, Crypto::Encrypt::RSACipher::Padding rsaPadding)
{
	if (key->GetKeyType() == Crypto::Cert::X509File::KeyType::RSAPublic)
	{
		return RSAPublicDecrypt(key, decData, payload, payloadLen, rsaPadding);
	}
	EVP_PKEY *pkey = OpenSSLEngine_LoadKey(key, false);
	if (pkey == 0)
	{
		return 0;
	}
	EVP_PKEY_CTX *ctx;
	ctx = EVP_PKEY_CTX_new(pkey, 0);
	if (!ctx)
	{
		EVP_PKEY_free(pkey);
		return 0;
	}
	if (EVP_PKEY_decrypt_init(ctx) <= 0)
	{
		EVP_PKEY_CTX_free(ctx);
		EVP_PKEY_free(pkey);
		return 0;
	}
	if (key->GetKeyType() == Crypto::Cert::X509File::KeyType::RSA || key->GetKeyType() == Crypto::Cert::X509File::KeyType::RSAPublic)
	{
		if (!SetRSAPadding(ctx, rsaPadding))
		{
			EVP_PKEY_CTX_free(ctx);
			EVP_PKEY_free(pkey);
			return 0;
		}
	}
	size_t outlen = 512;
	int ret = EVP_PKEY_decrypt(ctx, decData, &outlen, payload, payloadLen);
	if (ret <= 0)
	{
		printf("EVP_PKEY_decrypt returns %d\r\n", ret);
		ERR_print_errors_fp(stdout);

		EVP_PKEY_CTX_free(ctx);
		EVP_PKEY_free(pkey);
		return 0;
	}
	EVP_PKEY_CTX_free(ctx);
	EVP_PKEY_free(pkey);
	return (UOSInt)outlen;
}

UOSInt Net::OpenSSLEngine::RSAPublicDecrypt(Crypto::Cert::X509Key *key, UInt8 *decData, const UInt8 *payload, UOSInt payloadLen, Crypto::Encrypt::RSACipher::Padding rsaPadding)
{
	EVP_PKEY *pkey = OpenSSLEngine_LoadKey(key, false);
	if (pkey == 0)
	{
		return 0;
	}
	EVP_PKEY_CTX *ctx;
	ctx = EVP_PKEY_CTX_new(pkey, 0);
	if (!ctx)
	{
		EVP_PKEY_free(pkey);
		return 0;
	}
	if (EVP_PKEY_verify_recover_init(ctx) <= 0)
	{
		EVP_PKEY_CTX_free(ctx);
		EVP_PKEY_free(pkey);
		return 0;
	}
	if (!SetRSAPadding(ctx, rsaPadding))
	{
		EVP_PKEY_CTX_free(ctx);
		EVP_PKEY_free(pkey);
		return 0;
	}
	size_t outlen = 512;
	int ret = EVP_PKEY_verify_recover(ctx, decData, &outlen, payload, payloadLen);
	if (ret <= 0)
	{
		printf("EVP_PKEY_verify_recover returns %d\r\n", ret);
		ERR_print_errors_fp(stdout);

		EVP_PKEY_CTX_free(ctx);
		EVP_PKEY_free(pkey);
		return 0;
	}
	EVP_PKEY_CTX_free(ctx);
	EVP_PKEY_free(pkey);
	return (UOSInt)outlen;
}
