#include "Stdafx.h"
#include "Crypto/Cert/TrustStore.h"
#include "IO/Path.h"
#include "Manage/EnvironmentVar.h"

#include <windows.h>

void TrustStore_LoadStore(Crypto::Cert::CertStore *store, WChar *storeName)
{
	HCERTSTORE hSystemStore = CertOpenSystemStoreW(0, storeName);
	if (hSystemStore)
	{
		PCCERT_CONTEXT cert = 0;
		while ((cert = CertEnumCertificatesInStore(hSystemStore, cert)) != 0)
		{
			Crypto::Cert::X509Cert *x509Cert;
			NEW_CLASS(x509Cert, Crypto::Cert::X509Cert(CSTR("temp.crt"), cert->pbCertEncoded, cert->cbCertEncoded));
			store->AddCert(x509Cert);
		}
		CertCloseStore(hSystemStore, 0);
	}
}

Crypto::Cert::CertStore *Crypto::Cert::TrustStore::Load()
{
	Crypto::Cert::CertStore *store;
	NEW_CLASS(store, Crypto::Cert::CertStore(CSTR("Default Trust Store")));
	TrustStore_LoadStore(store, L"CA");
	TrustStore_LoadStore(store, L"ROOT");
	return store;
}

Crypto::Cert::CertStore *Crypto::Cert::TrustStore::LoadJavaCA()
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Crypto::Cert::CertStore *store;
	NEW_CLASS(store, Crypto::Cert::CertStore(CSTR("Java CACerts")));

	Manage::EnvironmentVar env;
	const UTF8Char *csptr = env.GetValue(CSTR("JAVA_HOME"));
	if (csptr)
	{
		sptr = Text::StrConcat(sbuff, csptr);
		if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		{
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		sptr = Text::StrConcatC(sptr, UTF8STRC("lib\\security\\cacerts"));
		store->LoadJavaCACerts(CSTRP(sbuff, sptr));
	}
	return store;
}