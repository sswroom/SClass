#include "Stdafx.h"
#include "Crypto/Cert/X509Cert.h"
#include "Crypto/Cert/X509PrivKey.h"
#include "Data/DateTime.h"
#include "Data/FastStringMap.h"
#include "IO/StmData/MemoryDataRef.h"
#include "Net/ASN1PDUBuilder.h"
#include "Net/OpenSSLClient.h"
#include "Net/OpenSSLCore.h"
#include "Net/OpenSSLEngine.h"
#include "Parser/FileParser/X509Parser.h"
#include "Text/MyString.h"
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/err.h>
#include <openssl/ec.h>

//#define SHOW_DEBUG
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

Optional<Net::SSLClient> Net::OpenSSLEngine::CreateServerConn(NN<Socket> s)
{
	SSL *ssl = SSL_new(this->clsData->ctx);
	this->clif->GetSocketFactory()->SetRecvTimeout(s, 2000);
	this->clif->GetSocketFactory()->SetNoDelay(s, true);
	SSL_set_fd(ssl, (int)this->clif->GetSocketFactory()->SocketGetFD(s));
	int ret;
	if ((ret = SSL_accept(ssl)) <= 0)
	{
#ifdef SHOW_DEBUG
		int code = SSL_get_error(ssl, ret);
		printf("SSL_accept: ret = %d, Error code = %d, %s\r\n", ret, code, ERR_error_string(ERR_get_error(), 0));
#endif
		SSL_free(ssl);
		this->clif->GetSocketFactory()->DestroySocket(s);
		return 0;
	}
	else
	{
		this->clif->GetSocketFactory()->SetRecvTimeout(s, 120000);
		Net::SSLClient *cli;
		NEW_CLASS(cli, OpenSSLClient(this->clif->GetSocketFactory(), ssl, s));
		return cli;
	}
}

Optional<Net::SSLClient> Net::OpenSSLEngine::CreateClientConn(void *sslObj, NN<Socket> s, Text::CStringNN hostName, OptOut<ErrorType> err)
{
	SSL *ssl = (SSL*)sslObj;
	this->clif->GetSocketFactory()->SetNoDelay(s, true);
	this->clif->GetSocketFactory()->SetRecvTimeout(s, 2000);
	SSL_set_fd(ssl, (int)this->clif->GetSocketFactory()->SocketGetFD(s));
	SSL_set_tlsext_host_name(ssl, hostName.v.Ptr());
	int ret;
	if ((ret = SSL_connect(ssl)) <= 0)
	{
		this->clif->GetSocketFactory()->DestroySocket(s);
#ifdef SHOW_DEBUG
		int code = SSL_get_error(ssl, ret);
		printf("SSL_connect: ret = %d, Error code = %d\r\n", ret, code);
#endif
		SSL_free(ssl);
		err.Set(ErrorType::InitSession);
		return 0;
	}
	if (!this->skipCertCheck)
	{
		stack_st_X509 *certs = SSL_get_peer_cert_chain(ssl);
		if (certs == 0)
		{
			this->clif->GetSocketFactory()->DestroySocket(s);
			SSL_free(ssl);
			err.Set(ErrorType::CertNotFound);
			return 0;
		}
		X509 *cert = sk_X509_value(certs, 0);
		UInt8 certBuff[4096];
		UInt8 *certPtr = certBuff;
		Int32 certLen = i2d_X509(cert, &certPtr);
		if (certLen <= 0)
		{
			this->clif->GetSocketFactory()->DestroySocket(s);
			SSL_free(ssl);
			err.Set(ErrorType::CertNotFound);
			return 0;
		}
		Crypto::Cert::X509Cert *svrCert;
		NEW_CLASS(svrCert, Crypto::Cert::X509Cert(hostName, Data::ByteArrayR(certBuff, (UInt32)certLen)));
		Data::DateTime dt;
		Int64 currTime;
		dt.SetCurrTimeUTC();
		currTime = dt.ToTicks();
		if (!svrCert->GetNotBefore(dt) || currTime < dt.ToTicks())
		{
			DEL_CLASS(svrCert);
			this->clif->GetSocketFactory()->DestroySocket(s);
			SSL_free(ssl);
			err.Set(ErrorType::InvalidPeriod);
			return 0;
		}
		if (!svrCert->GetNotAfter(dt) || currTime > dt.ToTicks())
		{
			DEL_CLASS(svrCert);
			this->clif->GetSocketFactory()->DestroySocket(s);
			SSL_free(ssl);
			err.Set(ErrorType::InvalidPeriod);
			return 0;
		}
		if (!svrCert->DomainValid(hostName))
		{
			DEL_CLASS(svrCert);
			this->clif->GetSocketFactory()->DestroySocket(s);
			SSL_free(ssl);
			err.Set(ErrorType::InvalidName);
			return 0;
		}
		if (svrCert->IsSelfSigned())
		{
			DEL_CLASS(svrCert);
			this->clif->GetSocketFactory()->DestroySocket(s);
			SSL_free(ssl);
			err.Set(ErrorType::SelfSign);
			return 0;
		}
		DEL_CLASS(svrCert);
	}
	this->clif->GetSocketFactory()->SetRecvTimeout(s, 120000);
	Net::SSLClient *cli;
	NEW_CLASS(cli, OpenSSLClient(this->clif->GetSocketFactory(), ssl, s));
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

Net::OpenSSLEngine::OpenSSLEngine(NN<Net::TCPClientFactory> clif, Method method) : Net::SSLEngine(clif)
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
#if OPENSSL_VERSION_NUMBER >= 0x10002000
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
#else
	case Method::DTLSV1_2:
	case Method::DTLSV1:
	case Method::DTLS:
		m = TLS_method();
		break;
#endif
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

Bool Net::OpenSSLEngine::ServerSetCertsASN1(NN<Crypto::Cert::X509Cert> certASN1, NN<Crypto::Cert::X509File> keyASN1, NN<Data::ArrayListNN<Crypto::Cert::X509Cert>> caCerts)
{
	if (this->clsData->ctx == 0)
	{
		return false;
	}
	
#if OPENSSL_VERSION_NUMBER >= 0x10002000
	SSL_CTX_set_ecdh_auto(this->clsData->ctx, 1);
#endif
	if (SSL_CTX_use_certificate_ASN1(this->clsData->ctx, (int)certASN1->GetASN1BuffSize(), certASN1->GetASN1Buff().Ptr()) <= 0)
	{
		return false;
	}
	Data::ArrayIterator<NN<Crypto::Cert::X509Cert>> it = caCerts->Iterator();
	while (it.HasNext())
	{
		NN<Crypto::Cert::X509Cert> caCert = it.Next();
		const UInt8 *asn1 = caCert->GetASN1Buff().Ptr();
		X509 *x509 = d2i_X509(0, &asn1, (long)caCert->GetASN1BuffSize());
		if (x509 == 0)
		{
			return false;
		}
		SSL_CTX_add_extra_chain_cert(this->clsData->ctx, x509);
	}

	if (keyASN1->GetFileType() == Crypto::Cert::X509File::FileType::PrivateKey)
	{
		NN<Crypto::Cert::X509PrivKey> pkey = NN<Crypto::Cert::X509PrivKey>::ConvertFrom(keyASN1);
		Crypto::Cert::X509File::KeyType keyType = pkey->GetKeyType();
		if (keyType == Crypto::Cert::X509File::KeyType::ECDSA)
		{
			if (SSL_CTX_use_PrivateKey_ASN1(EVP_PKEY_EC, this->clsData->ctx, keyASN1->GetASN1Buff().Ptr(), (long)keyASN1->GetASN1BuffSize()) <= 0)
			{
				return false;
			}
		}
		else
		{
			if (SSL_CTX_use_PrivateKey_ASN1(EVP_PKEY_RSA, this->clsData->ctx, keyASN1->GetASN1Buff().Ptr(), (long)keyASN1->GetASN1BuffSize()) <= 0)
			{
				return false;
			}
		}
		return true;
	}
	else if (keyASN1->GetFileType() == Crypto::Cert::X509File::FileType::Key && NN<Crypto::Cert::X509Key>::ConvertFrom(keyASN1)->IsPrivateKey())
	{
		NN<Crypto::Cert::X509PrivKey> privKey;
		if (Crypto::Cert::X509PrivKey::CreateFromKey(NN<Crypto::Cert::X509Key>::ConvertFrom(keyASN1)).SetTo(privKey))
		{
			Crypto::Cert::X509File::KeyType keyType = privKey->GetKeyType();
			if (keyType == Crypto::Cert::X509File::KeyType::ECDSA)
			{
				if (SSL_CTX_use_PrivateKey_ASN1(EVP_PKEY_EC, this->clsData->ctx, privKey->GetASN1Buff().Ptr(), (long)privKey->GetASN1BuffSize()) <= 0)
				{
					privKey.Delete();
					return false;
				}
			}
			else
			{
				if (SSL_CTX_use_PrivateKey_ASN1(EVP_PKEY_RSA, this->clsData->ctx, privKey->GetASN1Buff().Ptr(), (long)privKey->GetASN1BuffSize()) <= 0)
				{
					privKey.Delete();
					return false;
				}
			}
			privKey.Delete();
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

Bool Net::OpenSSLEngine::ServerSetClientCA(Text::CStringNN clientCA)
{
	if (this->clsData->ctx == 0)
	{
		return false;
	}
	STACK_OF(X509_NAME) *names = sk_X509_NAME_new_null();
	X509_NAME *name = X509_NAME_new();
	if (!X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, clientCA.v.Ptr(), (int)clientCA.leng, -1, 0))
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
		if (inlen == 0 || (UOSInt)in[0] + 1 > inlen)
			return SSL_TLSEXT_ERR_NOACK;
		if (clsData->alpnSupports->GetC(Text::CStringNN(in + 1, in[0])))
		{
			*out = in + 1;
			*outlen = in[0];
			return SSL_TLSEXT_ERR_OK;
		}
		inlen -= (unsigned int)in[0] + 1;
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
		if (clsData->alpnSupports->GetC(Text::CStringNN(in + 1, in[0])))
		{
			*out = (unsigned char*)in + 1;
			*outlen = in[0];
			return SSL_TLSEXT_ERR_OK;
		}
		inlen -= (unsigned int)in[0] + 1;
		in += in[0] + 1;
	}
}

Bool Net::OpenSSLEngine::ServerAddALPNSupport(Text::CStringNN proto)
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


Bool Net::OpenSSLEngine::ClientSetCertASN1(NN<Crypto::Cert::X509Cert> certASN1, NN<Crypto::Cert::X509File> keyASN1)
{
	SDEL_CLASS(this->clsData->cliCert);
	SDEL_CLASS(this->clsData->cliKey);
	this->clsData->cliCert = (Crypto::Cert::X509File*)certASN1->Clone().Ptr();
	this->clsData->cliKey = (Crypto::Cert::X509File*)keyASN1->Clone().Ptr();
	return true;
}

void Net::OpenSSLEngine::ClientSetSkipCertCheck(Bool skipCertCheck)
{
	this->skipCertCheck = skipCertCheck;
}


Optional<Net::SSLClient> Net::OpenSSLEngine::ClientConnect(Text::CStringNN hostName, UInt16 port, OptOut<ErrorType> err, Data::Duration timeout)
{
	Net::SocketUtil::AddressInfo addr[5];
	UOSInt addrCnt = this->clif->GetSocketFactory()->DNSResolveIPs(hostName, Data::DataArray<SocketUtil::AddressInfo>(addr, 5));
	if (addrCnt == 0)
	{
		err.Set(ErrorType::HostnameNotResolved);
		return 0;
	}
	SSL *ssl = SSL_new(this->clsData->ctx);
	if (ssl == 0)
	{
		err.Set(ErrorType::OutOfMemory);
		return 0;
	}
	if (this->clsData->cliCert)
	{
		SSL_use_certificate_ASN1(ssl, this->clsData->cliCert->GetASN1Buff().Ptr(), (int)(OSInt)this->clsData->cliCert->GetASN1BuffSize());
	}
	if (this->clsData->cliKey)
	{
		SSL_use_PrivateKey_ASN1(EVP_PKEY_RSA, ssl, this->clsData->cliKey->GetASN1Buff().Ptr(), (int)(OSInt)this->clsData->cliKey->GetASN1BuffSize());
	}
	NN<Socket> s;
	NN<Net::TCPClient> cli;
	UOSInt addrInd = 0;
	while (addrInd < addrCnt)
	{
		cli = this->clif->Create(addr[addrInd], port, timeout);
		if (cli->IsConnectError())
		{
			cli.Delete();
		}
		else if (cli->RemoveSocket().SetTo(s))
		{
			cli.Delete();
			return CreateClientConn(ssl, s, hostName, err);
		}
		else
		{
			cli.Delete();
		}
		addrInd++;
	}

	SSL_free(ssl);
	err.Set(ErrorType::CannotConnect);
	return 0;
}

Optional<Net::SSLClient> Net::OpenSSLEngine::ClientInit(NN<Socket> s, Text::CStringNN hostName, OptOut<ErrorType> err)
{
	SSL *ssl = SSL_new(this->clsData->ctx);
	if (ssl == 0)
	{
		err.Set(ErrorType::OutOfMemory);
		return 0;
	}
	if (this->clsData->cliCert)
	{
		SSL_use_certificate_ASN1(ssl, this->clsData->cliCert->GetASN1Buff().Ptr(), (int)(OSInt)this->clsData->cliCert->GetASN1BuffSize());
	}
	if (this->clsData->cliKey)
	{
		SSL_use_PrivateKey_ASN1(EVP_PKEY_RSA, ssl, this->clsData->cliKey->GetASN1Buff().Ptr(), (int)(OSInt)this->clsData->cliKey->GetASN1BuffSize());
	}
	return CreateClientConn(ssl, s, hostName, err);
}

UnsafeArray<UTF8Char> Net::OpenSSLEngine::GetErrorDetail(UnsafeArray<UTF8Char> sbuff)
{
	UInt32 err = (UInt32)ERR_get_error();
	if (err == 0)
	{
		*sbuff = 0;
		return sbuff;
	}
	ERR_error_string(err, (char*)sbuff.Ptr());
	return &sbuff[Text::StrCharCnt(sbuff)];
}

Bool Net::OpenSSLEngine::GenerateCert(Text::CString country, Text::CString company, Text::CStringNN commonName, OutParam<NN<Crypto::Cert::X509Cert>> certASN1, OutParam<NN<Crypto::Cert::X509File>> keyASN1, UOSInt keyLength)
{
	Bool succ = false;
	EVP_PKEY *pkey;
#if defined(OSSL_DEPRECATEDIN_3_0)
	pkey = EVP_RSA_gen(keyLength);
	if (pkey)
	{
#else
	BIGNUM *bn = BN_new();
	BN_set_word(bn, RSA_F4);
	RSA *rsa = RSA_new();
	if (RSA_generate_key_ex(rsa, keyLength, bn, 0) > 0)
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
		X509_NAME_add_entry_by_txt(name, "C",  MBSTRING_ASC, country.v.Ptr(), (int)country.leng, -1, 0);
		X509_NAME_add_entry_by_txt(name, "O",  MBSTRING_ASC, company.v.Ptr(), (int)company.leng, -1, 0);
		X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, commonName.v.Ptr(), (int)commonName.leng, -1, 0);

		X509_set_issuer_name(cert, name);
		X509_sign(cert, pkey, EVP_sha256());

		BIO *bio1;
		BIO *bio2;
		UInt8 buff[8192];
		Optional<Crypto::Cert::X509File> pobjKey = 0;
		Optional<Crypto::Cert::X509Cert> pobjCert = 0;

		BIO_new_bio_pair(&bio1, 8192, &bio2, 8192);
		PEM_write_bio_PrivateKey(bio1, pkey, nullptr, nullptr, 0, nullptr, nullptr);
		int readSize = BIO_read(bio2, buff, 8192);
		if (readSize > 0)
		{
			NN<Text::String> fileName = Text::String::New(UTF8STRC("Certificate.key"));
			pobjKey = Parser::FileParser::X509Parser::ParseBuff(BYTEARR(buff).SubArray(0, (UInt32)readSize), fileName);
			fileName->Release();
		}
		PEM_write_bio_X509(bio1, cert);
		readSize = BIO_read(bio2, buff, 8192);
		if (readSize > 0)
		{
			NN<Text::String> fileName = Text::String::New(UTF8STRC("Certificate.crt"));
			pobjCert = Optional<Crypto::Cert::X509Cert>::ConvertFrom(Parser::FileParser::X509Parser::ParseBuff(BYTEARR(buff).SubArray(0, (UInt32)readSize), fileName));
			fileName->Release();
		}
		BIO_free(bio1);
		BIO_free(bio2);
		X509_free(cert);
		EVP_PKEY_free(pkey);

		NN<Crypto::Cert::X509File> nnKey;
		NN<Crypto::Cert::X509Cert> nnCert;
		if (pobjCert.SetTo(nnCert) && pobjKey.SetTo(nnKey))
		{
			succ = true;
			nnCert->SetSourceName(CSTR("cert.crt"));
			nnKey->SetSourceName(CSTR("RSAKey.key"));
			certASN1.Set(nnCert);
			keyASN1.Set(nnKey);
		}
		else
		{
			pobjCert.Delete();
			pobjKey.Delete();
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

Optional<Crypto::Cert::X509Key> Net::OpenSSLEngine::GenerateRSAKey(UOSInt keyLength)
{
#if !defined(OSSL_DEPRECATEDIN_3_0)
	BIGNUM *bn = BN_new();
	BN_set_word(bn, RSA_F4);
	RSA *rsa = RSA_new();
	if (RSA_generate_key_ex(rsa, keyLength, bn, 0) > 0)
	{
		BIO *bio1;
		BIO *bio2;
		UInt8 buff[8192];
		Optional<Crypto::Cert::X509File> pobjKey = 0;

		BIO_new_bio_pair(&bio1, 8192, &bio2, 8192);
		PEM_write_bio_RSAPrivateKey(bio1, rsa, nullptr, nullptr, 0, nullptr, nullptr);
		int readSize = BIO_read(bio2, buff, 8192);
		if (readSize > 0)
		{
			NN<Text::String> fileName = Text::String::New(UTF8STRC("RSAKey.key"));
			pobjKey = Parser::FileParser::X509Parser::ParseBuff(BYTEARR(buff).SubArray(0, (UOSInt)readSize), fileName);
			fileName->Release();
		}
		BIO_free(bio1);
		BIO_free(bio2);

		RSA_free(rsa);
		BN_free(bn);
		return Optional<Crypto::Cert::X509Key>::ConvertFrom(pobjKey);
	}
	RSA_free(rsa);
	BN_free(bn);
	return 0;
#else
	EVP_PKEY *pkey;
	pkey = EVP_RSA_gen(keyLength);
	if (pkey)
	{
		BIO *bio1;
		BIO *bio2;
		UInt8 buff[8192];
		Optional<Crypto::Cert::X509File> pobjKey = 0;
		NN<Crypto::Cert::X509File> nnpobjKey;

		BIO_new_bio_pair(&bio1, 8192, &bio2, 8192);
		PEM_write_bio_PrivateKey(bio1, pkey, nullptr, nullptr, 0, nullptr, nullptr);
		int readSize = BIO_read(bio2, buff, 8192);
		if (readSize > 0)
		{
			NN<Text::String> fileName = Text::String::New(UTF8STRC("Certificate.key"));
			pobjKey = Parser::FileParser::X509Parser::ParseBuff(BYTEARR(buff).SubArray(0, (UInt32)readSize), fileName);
			if (pobjKey.SetTo(nnpobjKey) && nnpobjKey->GetFileType() == Crypto::Cert::X509File::FileType::PrivateKey)
			{
				NN<Crypto::Cert::X509PrivKey> privKey = NN<Crypto::Cert::X509PrivKey>::ConvertFrom(nnpobjKey);
				pobjKey = privKey->CreateKey();
				privKey.Delete();
			}
			fileName->Release();
		}
		BIO_free(bio1);
		BIO_free(bio2);
		EVP_PKEY_free(pkey);
		return Optional<Crypto::Cert::X509Key>::ConvertFrom(pobjKey);
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
EVP_PKEY *OpenSSLEngine_LoadKey(NN<Crypto::Cert::X509Key> key, Bool privateKeyOnly)
{
	EVP_PKEY *pkey = 0;
	if (key->GetKeyType() == Crypto::Cert::X509File::KeyType::RSA)
	{
		const UInt8 *keyPtr = key->GetASN1Buff().Ptr();
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
		const UInt8 *keyPtr = key->GetASN1Buff().Ptr();
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
		key->ToASN1String(sb);
		printf("%s\r\n", sb.ToPtr());
		Crypto::Cert::X509File::ECName ecName = key->GetECName();
		UOSInt keyLen;
		UnsafeArray<const UInt8> keyPtr;
		if (!key->GetECPublic(keyLen).SetTo(keyPtr))
		{
#ifdef SHOW_DEBUG
			printf("EC public key not found\r\n");
#endif
			return 0;			
		}
		else if (ecName == Crypto::Cert::X509File::ECName::Unknown)
		{
#ifdef SHOW_DEBUG
			printf("Unknown curve name\r\n");
#endif
			return 0;
		}
		EC_GROUP *group = EC_GROUP_new_by_curve_name(OpenSSLEngine_GetCurveName(ecName));
		EC_POINT *point = EC_POINT_new(group);
		if (EC_POINT_oct2point(group, point, keyPtr.Ptr(), (size_t)keyLen, 0) == 0)
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
		const UInt8 *keyPtr = key->GetASN1Buff().Ptr();
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

Optional<Crypto::Cert::X509Key> Net::OpenSSLEngine::GenerateECDSAKey(Crypto::Cert::X509File::ECName name)
{
	EVP_PKEY *pkey = 0;
	switch (name)
	{
	case Crypto::Cert::X509File::ECName::secp256r1:
		pkey = EVP_EC_gen("P-256");
		break;
	case Crypto::Cert::X509File::ECName::secp384r1:
		pkey = EVP_EC_gen("P-384");
		break;
	case Crypto::Cert::X509File::ECName::secp521r1:
		pkey = EVP_EC_gen("P-521");
		break;
	case Crypto::Cert::X509File::ECName::Unknown:
		break;
	}
	if (pkey)
	{
		BIO *bio1;
		BIO *bio2;
		UInt8 buff[8192];
		Optional<Crypto::Cert::X509File> pobjKey = 0;
		NN<Crypto::Cert::X509File> nnpobjKey;

		BIO_new_bio_pair(&bio1, 8192, &bio2, 8192);
		PEM_write_bio_PrivateKey(bio1, pkey, nullptr, nullptr, 0, nullptr, nullptr);
		int readSize = BIO_read(bio2, buff, 8192);
		if (readSize > 0)
		{
			NN<Text::String> fileName = Text::String::New(UTF8STRC("Certificate.key"));
			pobjKey = Parser::FileParser::X509Parser::ParseBuff(BYTEARR(buff).SubArray(0, (UInt32)readSize), fileName);
			if (pobjKey.SetTo(nnpobjKey) && nnpobjKey->GetFileType() == Crypto::Cert::X509File::FileType::PrivateKey)
			{
				NN<Crypto::Cert::X509PrivKey> privKey = NN<Crypto::Cert::X509PrivKey>::ConvertFrom(nnpobjKey);
				NN<Crypto::Cert::X509Key> tempKey;
				UnsafeArray<const UInt8> privBuff;
				UnsafeArray<const UInt8> pubBuff;
				UOSInt privSize;
				UOSInt pubSize;
				if (privKey->CreateKey().SetTo(tempKey) && tempKey->GetECPublic(pubSize).SetTo(pubBuff) && tempKey->GetECPrivate(privSize).SetTo(privBuff))
				{
					Net::ASN1PDUBuilder asn1;
					asn1.BeginSequence();
						asn1.AppendInt32(1);
						asn1.AppendOctetString(privBuff, privSize);
						asn1.BeginContentSpecific(0);
							if (name == Crypto::Cert::X509File::ECName::secp256r1)
								asn1.AppendOIDString(CSTR("1.2.840.10045.3.1.7"));
							else if (name == Crypto::Cert::X509File::ECName::secp384r1)
								asn1.AppendOIDString(CSTR("1.3.132.0.34"));
							else if (name == Crypto::Cert::X509File::ECName::secp521r1)
								asn1.AppendOIDString(CSTR("1.3.132.0.35"));
						asn1.EndLevel();
						asn1.BeginContentSpecific(1);
							asn1.AppendBitString(0, Data::ByteArrayR(pubBuff, pubSize));
						asn1.EndLevel();
					asn1.EndLevel();
					tempKey.Delete();
					privKey.Delete();
					NEW_CLASSOPT(pobjKey, Crypto::Cert::X509Key(CSTR("ECDSA.key"), asn1.GetArray(), Crypto::Cert::X509File::KeyType::ECDSA));
				}
			}
			fileName->Release();
		}
		BIO_free(bio1);
		BIO_free(bio2);
		EVP_PKEY_free(pkey);
		return Optional<Crypto::Cert::X509Key>::ConvertFrom(pobjKey);
	}
	return 0;
}

Bool Net::OpenSSLEngine::Signature(NN<Crypto::Cert::X509Key> key, Crypto::Hash::HashType hashType, Data::ByteArrayR payload, UnsafeArray<UInt8> signData, OutParam<UOSInt> signLen)
{
	const EVP_MD *htype = OpenSSLEngine_GetHash(hashType);
	if (htype == 0)
	{
#if defined(SHOW_DEBUG)
		printf("OpenSSLEngine.Signature: Error in getting hash type\r\n");
#endif
		return false;
	}
	EVP_PKEY *pkey = OpenSSLEngine_LoadKey(key, true);
	if (pkey == 0)
	{
#if defined(SHOW_DEBUG)
		printf("OpenSSLEngine.Signature: Error in Loading key\r\n");
#endif
		return false;
	}
	EVP_MD_CTX *emc = EVP_MD_CTX_create();
    if (emc == 0)
	{
#if defined(SHOW_DEBUG)
		printf("OpenSSLEngine.Signature: Error in creating context\r\n");
#endif
		EVP_PKEY_free(pkey);
		return false;
    }
	unsigned int len;
    if (!EVP_SignInit_ex(emc, htype, NULL))
	{
#if defined(SHOW_DEBUG)
		printf("OpenSSLEngine.Signature: Error in EVP_SignInit_ex\r\n");
#endif
		EVP_MD_CTX_destroy(emc);
		EVP_PKEY_free(pkey);
		return false;
    }
    if (!EVP_SignUpdate(emc, payload.Arr().Ptr(), payload.GetSize()))
	{
#if defined(SHOW_DEBUG)
		printf("OpenSSLEngine.Signature: Error in EVP_SignUpdate\r\n");
#endif
		EVP_MD_CTX_destroy(emc);
		EVP_PKEY_free(pkey);
		return false;
    }
    if (!EVP_SignFinal(emc, signData.Ptr(), &len, pkey)) {
#if defined(SHOW_DEBUG)
		printf("OpenSSLEngine.Signature: Error in EVP_SignFinal\r\n");
#endif
		EVP_MD_CTX_destroy(emc);
		EVP_PKEY_free(pkey);
		return false;
    }
    signLen.Set(len);
	EVP_MD_CTX_destroy(emc);
	EVP_PKEY_free(pkey);
	return true;
}

Bool Net::OpenSSLEngine::SignatureVerify(NN<Crypto::Cert::X509Key> key, Crypto::Hash::HashType hashType, Data::ByteArrayR payload, Data::ByteArrayR signData)
{
	const EVP_MD *htype = OpenSSLEngine_GetHash(hashType);
	if (htype == 0)
	{
#if defined(SHOW_DEBUG)
		printf("OpenSSLEngine.SignatureVerify: Error in getting hash type\r\n");
#endif
		return false;
	}
	EVP_PKEY *pkey = OpenSSLEngine_LoadKey(key, false);
	if (pkey == 0)
	{
#if defined(SHOW_DEBUG)
		printf("OpenSSLEngine.SignatureVerify: Error in OpenSSLEngine_LoadKey\r\n");
#endif
		return false;
	}
	EVP_MD_CTX *emc = EVP_MD_CTX_create();
    if (emc == 0)
	{
#if defined(SHOW_DEBUG)
		printf("OpenSSLEngine.SignatureVerify: Error in EVP_MD_CTX_create\r\n");
#endif
		EVP_PKEY_free(pkey);
		return false;
    }
    if (!EVP_VerifyInit(emc, htype))
	{
#if defined(SHOW_DEBUG)
		printf("OpenSSLEngine.SignatureVerify: Error in EVP_VerifyInit\r\n");
#endif
		EVP_MD_CTX_destroy(emc);
		EVP_PKEY_free(pkey);
		return false;
    }
    if (!EVP_VerifyUpdate(emc, payload.Arr().Ptr(), payload.GetSize()))
	{
#if defined(SHOW_DEBUG)
		printf("OpenSSLEngine.SignatureVerify: Error in EVP_VerifyUpdate\r\n");
#endif
		EVP_MD_CTX_destroy(emc);
		EVP_PKEY_free(pkey);
		return false;
    }
	Bool succ = false;
	int res = EVP_VerifyFinal(emc, signData.Arr().Ptr(), (UInt32)signData.GetSize(), pkey);
    if (res < 0) {
#if defined(SHOW_DEBUG)
		printf("OpenSSLEngine.SignatureVerify: Error in EVP_VerifyFinal: %d, %s\r\n", res, ERR_error_string(ERR_get_error(), 0));
		Text::StringBuilderUTF8 sb;
		sb.AppendHexBuff(signData, ' ', Text::LineBreakType::CRLF);
		printf("OpenSSLEngine.SignatureVerify: %s\r\n", sb.v.Ptr());
#endif
		EVP_MD_CTX_destroy(emc);
		EVP_PKEY_free(pkey);
		return false;
    }
	succ = (res == 1);
	EVP_MD_CTX_destroy(emc);
	EVP_PKEY_free(pkey);
	return succ;
}

UOSInt Net::OpenSSLEngine::Encrypt(NN<Crypto::Cert::X509Key> key, UnsafeArray<UInt8> encData, Data::ByteArrayR payload, Crypto::Encrypt::RSACipher::Padding rsaPadding)
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
	int ret = EVP_PKEY_encrypt(ctx, encData.Ptr(), &outlen, payload.Arr().Ptr(), payload.GetSize());
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

UOSInt Net::OpenSSLEngine::Decrypt(NN<Crypto::Cert::X509Key> key, UnsafeArray<UInt8> decData, Data::ByteArrayR  payload, Crypto::Encrypt::RSACipher::Padding rsaPadding)
{
	if (key->GetKeyType() == Crypto::Cert::X509File::KeyType::RSAPublic)
	{
		return RSAPublicDecrypt(key, decData, payload, rsaPadding);
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
	int ret = EVP_PKEY_decrypt(ctx, decData.Ptr(), &outlen, payload.Arr().Ptr(), payload.GetSize());
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

UOSInt Net::OpenSSLEngine::RSAPublicDecrypt(NN<Crypto::Cert::X509Key> key, UnsafeArray<UInt8> decData, Data::ByteArrayR payload, Crypto::Encrypt::RSACipher::Padding rsaPadding)
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
	int ret = EVP_PKEY_verify_recover(ctx, decData.Ptr(), &outlen, payload.Arr().Ptr(), payload.GetSize());
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
