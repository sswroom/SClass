#include "Stdafx.h"
#include "Crypto/Cert/TrustStore.h"

Crypto::Cert::CertStore *Crypto::Cert::TrustStore::Load()
{
	Crypto::Cert::CertStore *store;
	NEW_CLASS(store, Crypto::Cert::CertStore());
	if (store->LoadDir(CSTR("/usr/share/ca-certificates/mozilla"))) return store;
	return store;
}
