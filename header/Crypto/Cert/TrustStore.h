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
			static Crypto::Cert::CertStore *Load();
		};
	}
}
#endif
