#ifndef _SM_CRYPTO_CERT_WINHTTPCERT
#define _SM_CRYPTO_CERT_WINHTTPCERT
#include "AnyType.h"
#include "Crypto/Cert/Certificate.h"
#include "Crypto/Cert/X509Cert.h"

namespace Crypto
{
	namespace Cert
	{
		class WinHttpCert : public Certificate
		{
		private:
			struct ClassData;

			NN<ClassData> clsData;
		public:
			WinHttpCert(AnyType certInfo);
			virtual ~WinHttpCert();

			Data::Timestamp GetNotBefore() const;
			Data::Timestamp GetNotAfter() const;
			Bool IsSelfSigned() const;

			virtual Optional<Crypto::Cert::X509Cert> CreateX509Cert() const;

			virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
		};
	}
}
#endif
