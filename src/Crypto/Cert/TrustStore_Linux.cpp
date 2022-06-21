#include "Stdafx.h"
#include "Crypto/Cert/TrustStore.h"
#include "IO/Path.h"
#include "Parser/FileParser/JKSParser.h"

Crypto::Cert::CertStore *Crypto::Cert::TrustStore::Load()
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Crypto::Cert::CertStore *store;
	NEW_CLASS(store, Crypto::Cert::CertStore(CSTR("Default Trust Store")));
	
	sptr = IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("trustcerts"));
	store->LoadDir(CSTRP(sbuff, sptr));

	if (store->LoadDir(CSTR("/usr/share/ca-certificates/mozilla"))) return store;
	return store;
}

Crypto::Cert::CertStore *Crypto::Cert::TrustStore::LoadJavaCA()
{
//	UTF8Char sbuff[512];
//	UTF8Char *sptr;
	Crypto::Cert::CertStore *store;
	NEW_CLASS(store, Crypto::Cert::CertStore(CSTR("Java CACerts")));
	
//	sptr = IO::Path::GetProcessFileName(sbuff);
//	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("trustcerts"));
//	store->LoadDir(CSTRP(sbuff, sptr));

	Parser::FileParser::JKSParser parser;
	IO::PackageFile *pkg = (IO::PackageFile*)parser.ParseFilePath(CSTR("/etc/ssl/certs/java/cacerts"));
	if (pkg)
	{
		store->FromPackageFile(pkg);
		DEL_CLASS(pkg);
	}
	return store;
}
