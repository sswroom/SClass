#ifndef _SM_CRYPTO_CERT_CERTSTORE
#define _SM_CRYPTO_CERT_CERTSTORE
#include "Crypto/Cert/X509Cert.h"
#include "Data/FastStringMapNN.h"
#include "Data/ReadingList.h"
#include "IO/PackageFile.h"

namespace Crypto
{
	namespace Cert
	{
		class CertStore : public Data::ReadingListNN<Crypto::Cert::X509Cert>
		{
		private:
			Data::FastStringMapNN<Crypto::Cert::X509Cert> certMap;
			NN<Text::String> storeName;
		public:
			CertStore(Text::CStringNN name);
			CertStore(NN<Text::String> name);
			virtual ~CertStore();

			NN<CertStore> Clone() const;
			Bool LoadDir(Text::CStringNN certsDir);
			Bool LoadJavaCACerts(Text::CStringNN jksPath);
			void AddCert(NN<Crypto::Cert::X509Cert> cert);
			void FromPackageFile(NN<IO::PackageFile> pkg);
			NN<Text::String> GetStoreName() const;

			Optional<Crypto::Cert::X509Cert> GetCertByCN(Text::CStringNN commonName);
			virtual UOSInt GetCount() const;
			virtual Optional<Crypto::Cert::X509Cert> GetItem(UOSInt index) const;
			virtual NN<Crypto::Cert::X509Cert> GetItemNoCheck(UOSInt index) const;
		};
	}
}
#endif
