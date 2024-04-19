#include "Stdafx.h"
#include "Crypto/Cert/TrustStore.h"

NotNullPtr<Crypto::Cert::CertStore> Crypto::Cert::TrustStore::Load()
{
	NotNullPtr<Crypto::Cert::CertStore> store;
	NEW_CLASSNN(store, Crypto::Cert::CertStore(CSTR("Default Trust Store")));
	return store;
}

NotNullPtr<Crypto::Cert::CertStore> Crypto::Cert::TrustStore::LoadJavaCA()
{
	NotNullPtr<Crypto::Cert::CertStore> store;
	NEW_CLASSNN(store, Crypto::Cert::CertStore(CSTR("Java CACerts")));
	return store;
}