#include "Stdafx.h"
#include "Crypto/Cert/OpenSSLCert.h"
#include "Crypto/Cert/X509FileList.h"
#include "IO/StmData/MemoryDataRef.h"
#include "Parser/FileParser/X509Parser.h"
#include <openssl/ssl.h>

struct Crypto::Cert::OpenSSLCert::ClassData
{
	X509 *x509;
};

Bool Crypto::Cert::OpenSSLCert::FromASN1_TIME(AnyType t, NN<Data::DateTime> dt)
{
	ASN1_TIME *asn1t = (ASN1_TIME *)t.p;
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
	this->clsData = MemAllocNN(ClassData);
	this->clsData->x509 = X509_new();
}

Crypto::Cert::OpenSSLCert::OpenSSLCert(AnyType x509)
{
	this->clsData = MemAllocNN(ClassData);
	this->clsData->x509 = (X509*)x509.p;
}

Crypto::Cert::OpenSSLCert::~OpenSSLCert()
{
	X509_free(this->clsData->x509);
	MemFreeNN(this->clsData);
}

Bool Crypto::Cert::OpenSSLCert::GetNotBefore(NN<Data::DateTime> dt) const
{
	return FromASN1_TIME(X509_get_notBefore(this->clsData->x509), dt);
}

Bool Crypto::Cert::OpenSSLCert::GetNotAfter(NN<Data::DateTime> dt) const
{
	return FromASN1_TIME(X509_get_notAfter(this->clsData->x509), dt);
}

Bool Crypto::Cert::OpenSSLCert::IsSelfSigned() const
{
	X509_NAME *name = X509_get_subject_name(this->clsData->x509);
	X509_NAME *issuer = X509_get_issuer_name(this->clsData->x509);
	return X509_NAME_cmp(name, issuer) == 0;
}

Optional<Crypto::Cert::X509Cert> Crypto::Cert::OpenSSLCert::CreateX509Cert() const
{
	BIO *bio1;
	BIO *bio2;
	UInt8 buff[4096];
	Int32 readSize;
	Optional<Crypto::Cert::X509File> pobjCert = 0;
	NN<Crypto::Cert::X509File> nncert;
	BIO_new_bio_pair(&bio1, 4096, &bio2, 4096);
	PEM_write_bio_X509(bio1, this->clsData->x509);
	readSize = BIO_read(bio2, buff, 4096);
	if (readSize > 0)
	{
		NN<Text::String> fileName = Text::String::New(UTF8STRC("Certificate.crt"));
		pobjCert = Parser::FileParser::X509Parser::ParseBuff(BYTEARR(buff).WithSize((UOSInt)readSize), fileName);
		if (pobjCert.SetTo(nncert))
		{
			if (nncert->GetFileType() == Crypto::Cert::X509File::FileType::Cert)
			{
				NN<Crypto::Cert::X509Cert>::ConvertFrom(nncert)->SetDefaultSourceName();
			}
			else if (nncert->GetFileType() == Crypto::Cert::X509File::FileType::FileList)
			{
				NN<Crypto::Cert::X509FileList>::ConvertFrom(nncert)->SetDefaultSourceName();
			}
		}
		fileName->Release();
	}
	BIO_free(bio1);
	BIO_free(bio2);
	return Optional<Crypto::Cert::X509Cert>::ConvertFrom(pobjCert);
}

void Crypto::Cert::OpenSSLCert::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	/////////////////////////////
	NN<Crypto::Cert::X509Cert> cert;
	if (this->CreateX509Cert().SetTo(cert))
	{
		cert->ToString(sb);
		cert.Delete();
	}
}
