#include "Stdafx.h"
#include "Crypto/Cert/OpenSSLCert.h"
#include "IO/StmData/MemoryData.h"
#include "Parser/FileParser/X509Parser.h"
#include <openssl/ssl.h>

struct Crypto::Cert::OpenSSLCert::ClassData
{
	X509 *x509;
};

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

Bool Crypto::Cert::OpenSSLCert::GetNotBefore(Data::DateTime *dt)
{
	ASN1_TIME *notBefore = X509_get_notBefore(this->clsData->x509);
	tm tm;
	ASN1_TIME_to_tm(notBefore, &tm);
	dt->SetValue((UInt16)(tm.tm_year + 1900), tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, 0, (Int8)(tm.tm_gmtoff / 60 / 15));
	return true;
}

Bool Crypto::Cert::OpenSSLCert::GetNotAfter(Data::DateTime *dt)
{
	ASN1_TIME *notBefore = X509_get_notAfter(this->clsData->x509);
	tm tm;
	ASN1_TIME_to_tm(notBefore, &tm);
	dt->SetValue((UInt16)(tm.tm_year + 1900), tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, 0, (Int8)(tm.tm_gmtoff / 60 / 15));
	return true;
}

Bool Crypto::Cert::OpenSSLCert::IsSelfSigned()
{
	X509_NAME *name = X509_get_subject_name(this->clsData->x509);
	X509_NAME *issuer = X509_get_issuer_name(this->clsData->x509);
	return X509_NAME_cmp(name, issuer) == 0;
}

Crypto::Cert::X509Cert *Crypto::Cert::OpenSSLCert::CreateX509Cert()
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
		pobjCert = (Crypto::Cert::X509File*)parser.ParseFile(mdata, 0, IO::ParsedObject::PT_ASN1_DATA);
		DEL_CLASS(mdata);
	}
	BIO_free(bio1);
	BIO_free(bio2);
	return (Crypto::Cert::X509Cert*)pobjCert;
}

void Crypto::Cert::OpenSSLCert::ToString(Text::StringBuilderUTF *sb)
{
	/////////////////////////////
	Crypto::Cert::X509Cert *cert = this->CreateX509Cert();
	if (cert)
	{
		cert->ToString(sb);
		DEL_CLASS(cert);
	}
}
