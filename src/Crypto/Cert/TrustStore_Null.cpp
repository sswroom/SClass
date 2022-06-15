#include "Stdafx.h"
#include "Crypto/Cert/TrustStore.h"

Crypto::Cert::CertStore *Crypto::Cert::TrustStore::Load()
{
	Crypto::Cert::CertStore *store;
	NEW_CLASS(store, Crypto::Cert::CertStore());
	return store;
}
