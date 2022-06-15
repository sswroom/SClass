#include "Stdafx.h"
#include "Crypto/Cert/TrustStore.h"
#include "IO/Path.h"
#include "Parser/FileParser/X509Parser.h"

Bool TrustStore_LoadDir(Crypto::Cert::CertStore *store, Text::CString storePath)
{
	if (IO::Path::GetPathType(storePath) != IO::Path::PathType::Directory)
	{
		return false;
	}
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	Parser::FileParser::X509Parser parser;
	sptr = storePath.ConcatTo(sbuff);
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
				IO::ParsedObject *pobj = parser.ParseFilePath(CSTRP(sbuff, sptr2));
				if (pobj)
				{
					if (pobj->GetParserType() == IO::ParserType::ASN1Data)
					{
						Net::ASN1Data *asn = (Net::ASN1Data*)pobj;
						if (asn->GetASN1Type() == Net::ASN1Data::ASN1Type::X509)
						{
							Crypto::Cert::X509File *x509 = (Crypto::Cert::X509File*)asn;
							if (x509->GetFileType() == Crypto::Cert::X509File::FileType::Cert)
							{
								store->AddCert((Crypto::Cert::X509Cert*)x509);
							}
							else
							{
								DEL_CLASS(pobj);
							}
						}
						else
						{
							DEL_CLASS(pobj);
						}
					}
					else
					{
						DEL_CLASS(pobj);
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

Crypto::Cert::CertStore *Crypto::Cert::TrustStore::Load()
{
	Crypto::Cert::CertStore *store;
	NEW_CLASS(store, Crypto::Cert::CertStore());
	if (TrustStore_LoadDir(store, CSTR("/usr/share/ca-certificates/mozilla"))) return store;
	return store;
}
