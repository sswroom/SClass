#ifndef _SM_CRYPTO_CERT_X509CRL
#define _SM_CRYPTO_CERT_X509CRL
#include "Crypto/Cert/X509File.h"

namespace Crypto
{
	namespace Cert
	{
		class X509CRL : public Crypto::Cert::X509File
		{
		public:
			X509CRL(Text::String *sourceName, const UInt8 *buff, UOSInt buffSize);
			X509CRL(Text::CString sourceName, const UInt8 *buff, UOSInt buffSize);
			virtual ~X509CRL();

			virtual FileType GetFileType();
			virtual void ToShortName(Text::StringBuilderUTF8 *sb);
			virtual ValidStatus IsValid(Net::SSLEngine *ssl, Crypto::Cert::CertStore *trustStore);
			
			virtual ASN1Data *Clone();
			virtual void ToString(Text::StringBuilderUTF8 *sb);

			Bool HasVersion();
			Bool GetIssuerCN(Text::StringBuilderUTF8 *sb);
			Bool GetThisUpdate(Data::DateTime *dt);
			Bool GetNextUpdate(Data::DateTime *dt);
			Bool IsRevoked(Crypto::Cert::X509Cert *cert);
		};
	}
}
#endif