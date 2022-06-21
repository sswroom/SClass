#include "Stdafx.h"
#include "Crypto/Cert/TrustStore.h"
#include "IO/Path.h"

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
	
	if (store->LoadJavaCACerts(CSTR("/etc/ssl/certs/java/cacerts"))) return store;
	if (store->LoadJavaCACerts(CSTR("/etc/pki/ca-trust/extracted/java/cacerts"))) return store;
	return store;
}
