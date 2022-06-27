#include "Stdafx.h"
#include "Crypto/Cert/OpenSSLCert.h"
#include "IO/StmData/MemoryData.h"
#include "Parser/FileParser/X509Parser.h"
#include <openssl/ssl.h>

struct Crypto::Cert::OpenSSLCert::ClassData
{
	X509 *x509;
};

Bool Crypto::Cert::OpenSSLCert::FromASN1_TIME(void *t, Data::DateTime *dt)
{
	ASN1_TIME *asn1t = (ASN1_TIME *)t;
	UTF8Char *sptr;
	UInt16 year;
	sptr = asn1t->data;
	if (asn1t->type == V_ASN1_UTCTIME)
	{
		year = (UInt16)(2000 + (sptr[0] - 0x30) * 10 + (sptr[1] - 0x30));
		sptr += 2;
	}
	else if (asn1t->type == V_ASN1_GENERALIZEDTIME)
	{
		year = (UInt16)((sptr[0] - 0x30) * 1000 + (sptr[1] - 0x30) * 100 + (sptr[2] - 0x30) * 10 + (sptr[3] - 0x30));
		sptr += 4;
	}
	else
	{
		return false;
	}
	dt->SetValue(year, (sptr[0] - 0x30) * 10 + (sptr[1] - 0x30), (sptr[2] - 0x30) * 10 + (sptr[3] - 0x30), (sptr[4] - 0x30) * 10 + (sptr[5] - 0x30), (sptr[6] - 0x30) * 10 + (sptr[7] - 0x30), (sptr[8] - 0x30) * 10 + (sptr[9] - 0x30), 0, 0);
	return true;
}

Crypto::Cert::OpenSSLCert::OpenSSLCert()
{
	this->clsData = MemAlloc(ClassData, 1);
	this->clsData->x509 = X509_new();
}

Crypto::Cert::OpenSSLCert::OpenSSLCert(void *x509)
{
	this->clsData = MemAlloc(ClassData, 1);
	this->clsData->x509 = (X509*)x509;
}

Crypto::Cert::OpenSSLCert::~OpenSSLCert()
{
	X509_free(this->clsData->x509);
	MemFree(this->clsData);
}

Bool Crypto::Cert::OpenSSLCert::GetNotBefore(Data::DateTime *dt) const
{
	return FromASN1_TIME(X509_get_notBefore(this->clsData->x509), dt);
}

Bool Crypto::Cert::OpenSSLCert::GetNotAfter(Data::DateTime *dt) const
{
	return FromASN1_TIME(X509_get_notAfter(this->clsData->x509), dt);
}

Bool Crypto::Cert::OpenSSLCert::IsSelfSigned() const
{
	X509_NAME *name = X509_get_subject_name(this->clsData->x509);
	X509_NAME *issuer = X509_get_issuer_name(this->clsData->x509);
	return X509_NAME_cmp(name, issuer) == 0;
}

Crypto::Cert::X509Cert *Crypto::Cert::OpenSSLCert::CreateX509Cert() const
{
	BIO *bio1;
	BIO *bio2;
	UInt8 buff[4096];
	Int32 readSize;
	IO::StmData::MemoryData *mdata;
	Crypto::Cert::X509File *pobjCert = 0;
	BIO_new_bio_pair(&bio1, 4096, &bio2, 4096);
	PEM_write_bio_X509(bio1, this->clsData->x509);
	readSize = BIO_read(bio2, buff, 4096);
	if (readSize > 0)
	{
		Parser::FileParser::X509Parser parser;
		NEW_CLASS(mdata, IO::StmData::MemoryData(buff, (UInt32)readSize));
		pobjCert = (Crypto::Cert::X509File*)parser.ParseFile(mdata, 0, IO::ParserType::ASN1Data);
		DEL_CLASS(mdata);
	}
	BIO_free(bio1);
	BIO_free(bio2);
	return (Crypto::Cert::X509Cert*)pobjCert;
}

void Crypto::Cert::OpenSSLCert::ToString(Text::StringBuilderUTF8 *sb) const
{
	/////////////////////////////
	Crypto::Cert::X509Cert *cert = this->CreateX509Cert();
	if (cert)
	{
		cert->ToString(sb);
		DEL_CLASS(cert);
	}
}
