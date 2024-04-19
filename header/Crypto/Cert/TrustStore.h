#ifndef _SM_CRYPTO_CERT_TRUSTSTORE
#define _SM_CRYPTO_CERT_TRUSTSTORE
#include "Crypto/Cert/CertStore.h"

namespace Crypto
{
	namespace Cert
	{
		class TrustStore
		{
		public:
			static NotNullPtr<Crypto::Cert::CertStore> Load();
			static NotNullPtr<Crypto::Cert::CertStore> LoadJavaCA();
		};
	}
}
#endif
