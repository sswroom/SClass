#include "Stdafx.h"
#include "Crypto/Cert/TrustStore.h"

Crypto::Cert::CertStore *Crypto::Cert::TrustStore::Load()
{
	Crypto::Cert::CertStore *store;
	NEW_CLASS(store, Crypto::Cert::CertStore(CSTR("Default Trust Store")));
	return store;
}

Crypto::Cert::CertStore *Crypto::Cert::TrustStore::LoadJavaCA()
{
	Crypto::Cert::CertStore *store;
	NEW_CLASS(store, Crypto::Cert::CertStore(CSTR("Java CACerts")));
	return store;
}