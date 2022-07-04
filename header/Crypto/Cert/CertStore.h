#ifndef _SM_CRYPTO_CERT_CERTSTORE
#define _SM_CRYPTO_CERT_CERTSTORE
#include "Crypto/Cert/X509Cert.h"
#include "Data/FastStringMap.h"
#include "Data/ReadingList.h"
#include "IO/PackageFile.h"

namespace Crypto
{
	namespace Cert
	{
		class CertStore : public Data::ReadingList<Crypto::Cert::X509Cert*>
		{
		private:
			Data::FastStringMap<Crypto::Cert::X509Cert*> certMap;
			Text::String *storeName;
		public:
			CertStore(Text::CString name);
			CertStore(Text::String *name);
			virtual ~CertStore();

			CertStore *Clone();
			Bool LoadDir(Text::CString certsDir);
			Bool LoadJavaCACerts(Text::CString jksPath);
			void AddCert(Crypto::Cert::X509Cert *cert);
			void FromPackageFile(IO::PackageFile *pkg);
			Text::String *GetStoreName();

			Crypto::Cert::X509Cert *GetCertByCN(Text::CString commonName);
			virtual UOSInt GetCount() const;
			virtual Crypto::Cert::X509Cert *GetItem(UOSInt index) const;
		};
	}
}
#endif
