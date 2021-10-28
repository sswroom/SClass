#ifndef _SM_CRYPTO_CERT_X509PKCS12
#define _SM_CRYPTO_CERT_X509PKCS12
#include "Crypto/Cert/X509File.h"

namespace Crypto
{
	namespace Cert
	{
		class X509PKCS12 : public Crypto::Cert::X509File
		{
		public:
			X509PKCS12(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize);
			virtual ~X509PKCS12();

			virtual FileType GetFileType();
			virtual void ToShortName(Text::StringBuilderUTF *sb);
			
			virtual UOSInt GetCertCount();
			virtual Bool GetCertName(UOSInt index, Text::StringBuilderUTF *sb);
			virtual X509Cert *NewCert(UOSInt index);

			virtual ASN1Data *Clone();
			virtual void ToString(Text::StringBuilderUTF *sb);
		};
	}
}
#endif
