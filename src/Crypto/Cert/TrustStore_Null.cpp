#include "Stdafx.h"
#include "Crypto/Cert/TrustStore.h"

NN<Crypto::Cert::CertStore> Crypto::Cert::TrustStore::Load()
{
	NN<Crypto::Cert::CertStore> store;
	NEW_CLASSNN(store, Crypto::Cert::CertStore(CSTR("Default Trust Store")));
	return store;
}

NN<Crypto::Cert::CertStore> Crypto::Cert::TrustStore::LoadJavaCA()
{
	NN<Crypto::Cert::CertStore> store;
	NEW_CLASSNN(store, Crypto::Cert::CertStore(CSTR("Java CACerts")));
	return store;
}