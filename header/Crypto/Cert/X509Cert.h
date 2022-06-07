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
			X509Cert(Text::String *sourceName, const UInt8 *buff, UOSInt buffSize);
			X509Cert(Text::CString sourceName, const UInt8 *buff, UOSInt buffSize);
			virtual ~X509Cert();

			void GetSubjectCN(Text::StringBuilderUTF8 *sb);
			
			virtual FileType GetFileType();
			virtual void ToShortName(Text::StringBuilderUTF8 *sb);

			virtual UOSInt GetCertCount();
			virtual Bool GetCertName(UOSInt index, Text::StringBuilderUTF8 *sb);
			virtual X509Cert *GetNewCert(UOSInt index);

			virtual ASN1Data *Clone();
			virtual X509Cert *CreateX509Cert();
			virtual void ToString(Text::StringBuilderUTF8 *sb);

			Bool GetIssueNames(CertNames *names);
			Bool GetSubjNames(CertNames *names);
			Bool GetExtensions(CertExtensions *ext);
			Crypto::Cert::X509Key *GetNewPublicKey();
			Bool GetKeyId(UInt8 *keyId);
			Bool GetNotBefore(Data::DateTime *dt);
			Bool GetNotAfter(Data::DateTime *dt);
			Bool DomainValid(Text::CString domain);
			Bool IsSelfSigned();

			const UInt8 *GetIssueNamesSeq(UOSInt *dataLen);
			const UInt8 *GetSerialNumber(UOSInt *dataLen);
		};
	}
}
#endif
