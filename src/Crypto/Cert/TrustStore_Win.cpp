#include "Stdafx.h"
#include "Crypto/Cert/TrustStore.h"
#include "IO/Path.h"
#include "Manage/EnvironmentVar.h"

#include <windows.h>

Crypto::Cert::CertStore *Crypto::Cert::TrustStore::Load()
{
	Crypto::Cert::CertStore *store;
	NEW_CLASS(store, Crypto::Cert::CertStore(CSTR("Default Trust Store")));
	HCERTSTORE hSystemStore = CertOpenSystemStoreW(0, L"CA"); //L"ROOT"
	if (hSystemStore)
	{
		PCCERT_CONTEXT cert = 0;
		while ((cert = CertEnumCertificatesInStore(hSystemStore, cert)) != 0)
		{
			/////////////////////////////////////////
		}
		CertCloseStore(hSystemStore, 0);
	}
	return store;
}

Crypto::Cert::CertStore *Crypto::Cert::TrustStore::LoadJavaCA()
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Crypto::Cert::CertStore *store;
	NEW_CLASS(store, Crypto::Cert::CertStore(CSTR("Java CACerts")));

	Manage::EnvironmentVar env;
	Text::String *s = env.GetValue(CSTR("JAVA_HOME"));
	if (s)
	{
		sptr = s->ConcatTo(sbuff);
		if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		{
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		sptr = Text::StrConcatC(sptr, UTF8STRC("lib\\security\\cacerts"));
		store->LoadJavaCACerts(CSTRP(sbuff, sptr));
	}
	return store;
}