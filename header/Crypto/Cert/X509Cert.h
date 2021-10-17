#ifndef _SM_CRYPTO_CERT_X509CERT
#define _SM_CRYPTO_CERT_X509CERT
#include "Crypto/Cert/Certificate.h"
#include "Crypto/Cert/X509File.h"

namespace Crypto
{
	namespace Cert
	{
		class X509Cert : public Crypto::Cert::X509File, public Crypto::Cert::Certificate
		{
		public:
			X509Cert(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize);
			virtual ~X509Cert();

			void GetSubjectCN(Text::StringBuilderUTF *sb);
			
			virtual FileType GetFileType();
			virtual void ToShortName(Text::StringBuilderUTF *sb);

			virtual ASN1Data *Clone();
			virtual void ToString(Text::StringBuilderUTF *sb);

			Bool GetIssueNames(CertNames *names);
			Bool GetSubjNames(CertNames *names);
			Bool GetExtensions(CertExtensions *ext);
		};
	}
}
#endif
