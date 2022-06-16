#ifndef _SM_CRYPTO_CERT_CERTSTORE
#define _SM_CRYPTO_CERT_CERTSTORE
#include "Crypto/Cert/X509Cert.h"
#include "Data/FastStringMap.h"
#include "Data/ReadingList.h"

namespace Crypto
{
	namespace Cert
	{
		class CertStore : public Data::ReadingList<Crypto::Cert::X509Cert*>
		{
		private:
			Data::FastStringMap<Crypto::Cert::X509Cert*> certMap;
		public:
			CertStore();
			virtual ~CertStore();

			Bool LoadDir(Text::CString certsDir);
			void AddCert(Crypto::Cert::X509Cert *cert);

			Crypto::Cert::X509Cert *GetCertByCN(Text::CString commonName);
			virtual UOSInt GetCount() const;
			virtual Crypto::Cert::X509Cert *GetItem(UOSInt index) const;
		};
	}
}
#endif
