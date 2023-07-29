#ifndef _SM_CRYPTO_CERT_WINHTTPCERT
#define _SM_CRYPTO_CERT_WINHTTPCERT
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

			ClassData *clsData;
		public:
			WinHttpCert(void *certInfo);
			virtual ~WinHttpCert();

			Bool GetNotBefore(Data::DateTime *dt) const;
			Bool GetNotAfter(Data::DateTime *dt) const;
			Bool IsSelfSigned() const;

			virtual Crypto::Cert::X509Cert *CreateX509Cert() const;

			virtual void ToString(NotNullPtr<Text::StringBuilderUTF8> sb) const;
		};
	}
}
#endif
