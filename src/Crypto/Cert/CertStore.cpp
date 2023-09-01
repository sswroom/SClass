#include "Stdafx.h"
#include "Crypto/Cert/CertStore.h"
#include "IO/Path.h"
#include "Parser/FileParser/JKSParser.h"
#include "Parser/FileParser/X509Parser.h"

Crypto::Cert::CertStore::CertStore(Text::CString name)
{
	this->storeName = Text::String::New(name);
}

Crypto::Cert::CertStore::CertStore(NotNullPtr<Text::String> name)
{
	this->storeName = name->Clone();
}

Crypto::Cert::CertStore::~CertStore()
{
	UOSInt i = this->certMap.GetCount();
	Crypto::Cert::X509Cert *cert;
	while (i-- > 0)
	{
		cert = this->certMap.GetItem(i);
		DEL_CLASS(cert);
	}
	this->storeName->Release();
}

NotNullPtr<Crypto::Cert::CertStore> Crypto::Cert::CertStore::Clone() const
{
	NotNullPtr<Crypto::Cert::CertStore> newStore;
	NEW_CLASSNN(newStore, Crypto::Cert::CertStore(this->storeName));
	UOSInt i = 0;
	UOSInt j = this->certMap.GetCount();
	while (i < j)
	{
		newStore->certMap.Put(this->certMap.GetKey(i), (Crypto::Cert::X509Cert*)this->certMap.GetItem(i)->Clone().Ptr());
		i++;
	}
	return newStore;
}

Bool Crypto::Cert::CertStore::LoadDir(Text::CString certsDir)
{
	if (IO::Path::GetPathType(certsDir) != IO::Path::PathType::Directory)
	{
		return false;
	}
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
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
		while ((sptr2 = IO::Path::FindNextFile(sptr, sess, 0, &pt, &fileSize)) != 0)
		{
			if (pt == IO::Path::PathType::File)
			{
				NotNullPtr<IO::ParsedObject> pobj;
				if (pobj.Set(parser.ParseFilePath(CSTRP(sbuff, sptr2))))
				{
					if (pobj->GetParserType() == IO::ParserType::ASN1Data)
					{
						Net::ASN1Data *asn = (Net::ASN1Data*)pobj.Ptr();
						if (asn->GetASN1Type() == Net::ASN1Data::ASN1Type::X509)
						{
							Crypto::Cert::X509File *x509 = (Crypto::Cert::X509File*)asn;
							if (x509->GetFileType() == Crypto::Cert::X509File::FileType::Cert)
							{
								this->AddCert(NotNullPtr<Crypto::Cert::X509Cert>::ConvertFrom(pobj));
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
	IO::PackageFile *pkg = (IO::PackageFile*)parser.ParseFilePath(jksPath);
	if (pkg)
	{
		this->FromPackageFile(pkg);
		DEL_CLASS(pkg);
		return true;
	}
	else
	{
		return false;
	}
}

void Crypto::Cert::CertStore::AddCert(NotNullPtr<Crypto::Cert::X509Cert> cert)
{
	Text::StringBuilderUTF8 sb;
	cert->GetSubjectCN(sb);
	if (sb.GetLength() > 0)
	{
		if (cert.Set(this->certMap.PutC(sb.ToCString(), cert.Ptr())))
		{
			cert.Delete();
		}
	}
	else
	{
		cert.Delete();
	}
}

void Crypto::Cert::CertStore::FromPackageFile(IO::PackageFile *pkg)
{
	UOSInt i = 0;
	UOSInt j = pkg->GetCount();
	while (i < j)
	{
		Bool needRelease;
		IO::ParsedObject *pobj = pkg->GetItemPObj(i, &needRelease);
		if (pobj && pobj->GetParserType() == IO::ParserType::ASN1Data)
		{
			Net::ASN1Data *asn1 = (Net::ASN1Data*)pobj;
			if (asn1->GetASN1Type() == Net::ASN1Data::ASN1Type::X509)
			{
				Crypto::Cert::X509File *x509 = (Crypto::Cert::X509File*)asn1;
				if (x509->GetFileType() == Crypto::Cert::X509File::FileType::Cert)
				{
					this->AddCert(NotNullPtr<Crypto::Cert::X509Cert>::ConvertFrom(x509->Clone()));
				}
			}
		}
		if (needRelease)
		{
			DEL_CLASS(pobj);
		}
		i++;
	}
}

NotNullPtr<Text::String> Crypto::Cert::CertStore::GetStoreName() const
{
	return this->storeName;
}

Crypto::Cert::X509Cert *Crypto::Cert::CertStore::GetCertByCN(Text::CStringNN commonName)
{
	return this->certMap.GetC(commonName);
}

UOSInt Crypto::Cert::CertStore::GetCount() const
{
	return this->certMap.GetCount();
}

Crypto::Cert::X509Cert *Crypto::Cert::CertStore::GetItem(UOSInt index) const
{
	return this->certMap.GetItem(index);
}

