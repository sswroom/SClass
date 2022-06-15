#include "Stdafx.h"
#include "Crypto/Cert/CertStore.h"

Crypto::Cert::CertStore::CertStore()
{
}

Crypto::Cert::CertStore::~CertStore()
{
	UOSInt i = this->certMap.GetCount();
	Crypto::Cert::X509Cert *cert;
	while (i-- > 0)
	{
		cert = this->certMap.GetItem(i);
		DEL_CLASS(cert);
	}
}

void Crypto::Cert::CertStore::AddCert(Crypto::Cert::X509Cert *cert)
{
	Text::StringBuilderUTF8 sb;
	cert->GetSubjectCN(&sb);
	if (sb.GetLength() > 0)
	{
		cert = this->certMap.PutC(sb.ToCString(), cert);
	}
	SDEL_CLASS(cert);
}

Crypto::Cert::X509Cert *Crypto::Cert::CertStore::GetCertByCN(Text::CString commonName)
{
	return this->certMap.GetC(commonName);
}

UOSInt Crypto::Cert::CertStore::GetCount() const
{
	return this->certMap.GetCount();
}

Crypto::Cert::X509Cert *Crypto::Cert::CertStore::GetItem(UOSInt index) const
{
	return this->certMap.GetItem(index);
}

