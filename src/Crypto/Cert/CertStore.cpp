#include "Stdafx.h"
#include "Crypto/Cert/CertStore.h"
#include "IO/Path.h"
#include "Parser/FileParser/JKSParser.h"
#include "Parser/FileParser/X509Parser.h"

Crypto::Cert::CertStore::CertStore(Text::CStringNN name)
{
	this->storeName = Text::String::New(name);
}

Crypto::Cert::CertStore::CertStore(NN<Text::String> name)
{
	this->storeName = name->Clone();
}

Crypto::Cert::CertStore::~CertStore()
{
	UOSInt i = this->certMap.GetCount();
	NN<Crypto::Cert::X509Cert> cert;
	while (i-- > 0)
	{
		cert = this->certMap.GetItemNoCheck(i);
		cert.Delete();
	}
	this->storeName->Release();
}

NN<Crypto::Cert::CertStore> Crypto::Cert::CertStore::Clone() const
{
	NN<Crypto::Cert::CertStore> newStore;
	NEW_CLASSNN(newStore, Crypto::Cert::CertStore(this->storeName));
	UOSInt i = 0;
	UOSInt j = this->certMap.GetCount();
	while (i < j)
	{
		newStore->certMap.Put(this->certMap.GetKey(i), NN<Crypto::Cert::X509Cert>::ConvertFrom(this->certMap.GetItemNoCheck(i)->Clone()));
		i++;
	}
	return newStore;
}

Bool Crypto::Cert::CertStore::LoadDir(Text::CStringNN certsDir)
{
	if (IO::Path::GetPathType(certsDir) != IO::Path::PathType::Directory)
	{
		return false;
	}
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	Parser::FileParser::X509Parser parser;
	sptr = certsDir.ConcatTo(sbuff);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	IO::Path::FindFileSession *sess;
	IO::Path::PathType pt;
	UInt64 fileSize;
	sess = IO::Path::FindFile(CSTRP(sbuff, sptr2));
	if (sess)
	{
		while (IO::Path::FindNextFile(sptr, sess, 0, &pt, &fileSize).SetTo(sptr2))
		{
			if (pt == IO::Path::PathType::File)
			{
				NN<IO::ParsedObject> pobj;
				if (parser.ParseFilePath(CSTRP(sbuff, sptr2)).SetTo(pobj))
				{
					if (pobj->GetParserType() == IO::ParserType::ASN1Data)
					{
						Net::ASN1Data *asn = (Net::ASN1Data*)pobj.Ptr();
						if (asn->GetASN1Type() == Net::ASN1Data::ASN1Type::X509)
						{
							Crypto::Cert::X509File *x509 = (Crypto::Cert::X509File*)asn;
							if (x509->GetFileType() == Crypto::Cert::X509File::FileType::Cert)
							{
								this->AddCert(NN<Crypto::Cert::X509Cert>::ConvertFrom(pobj));
							}
							else
							{
								pobj.Delete();
							}
						}
						else
						{
							pobj.Delete();
						}
					}
					else
					{
						pobj.Delete();
					}
				}
			}
		}
		IO::Path::FindFileClose(sess);
		return true;
	}
	else
	{
		return false;
	}
}

Bool Crypto::Cert::CertStore::LoadJavaCACerts(Text::CStringNN jksPath)
{
	Parser::FileParser::JKSParser parser;
	NN<IO::PackageFile> pkg;
	if (Optional<IO::PackageFile>::ConvertFrom(parser.ParseFilePath(jksPath)).SetTo(pkg))
	{
		this->FromPackageFile(pkg);
		pkg.Delete();
		return true;
	}
	else
	{
		return false;
	}
}

void Crypto::Cert::CertStore::AddCert(NN<Crypto::Cert::X509Cert> cert)
{
	Text::StringBuilderUTF8 sb;
	cert->GetSubjectCN(sb);
	if (sb.GetLength() > 0)
	{
		if (this->certMap.PutC(sb.ToCString(), cert).SetTo(cert))
		{
			cert.Delete();
		}
	}
	else
	{
		cert.Delete();
	}
}

void Crypto::Cert::CertStore::FromPackageFile(NN<IO::PackageFile> pkg)
{
	UOSInt i = 0;
	UOSInt j = pkg->GetCount();
	while (i < j)
	{
		Bool needRelease;
		NN<IO::ParsedObject> pobj;
		if (pkg->GetItemPObj(i, needRelease).SetTo(pobj))
		{
			if (pobj->GetParserType() == IO::ParserType::ASN1Data)
			{
				NN<Net::ASN1Data> asn1 = NN<Net::ASN1Data>::ConvertFrom(pobj);
				if (asn1->GetASN1Type() == Net::ASN1Data::ASN1Type::X509)
				{
					NN<Crypto::Cert::X509File> x509 = NN<Crypto::Cert::X509File>::ConvertFrom(asn1);
					if (x509->GetFileType() == Crypto::Cert::X509File::FileType::Cert)
					{
						this->AddCert(NN<Crypto::Cert::X509Cert>::ConvertFrom(x509->Clone()));
					}
				}
			}
			if (needRelease)
			{
				pobj.Delete();
			}
		}
		i++;
	}
}

NN<Text::String> Crypto::Cert::CertStore::GetStoreName() const
{
	return this->storeName;
}

Optional<Crypto::Cert::X509Cert> Crypto::Cert::CertStore::GetCertByCN(Text::CStringNN commonName)
{
	return this->certMap.GetC(commonName);
}

UOSInt Crypto::Cert::CertStore::GetCount() const
{
	return this->certMap.GetCount();
}

Optional<Crypto::Cert::X509Cert> Crypto::Cert::CertStore::GetItem(UOSInt index) const
{
	return this->certMap.GetItem(index);
}

NN<Crypto::Cert::X509Cert> Crypto::Cert::CertStore::GetItemNoCheck(UOSInt index) const
{
	return this->certMap.GetItemNoCheck(index);
}
