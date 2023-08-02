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
			X509Cert(NotNullPtr<Text::String> sourceName, Data::ByteArrayR buff);
			X509Cert(Text::CString sourceName, Data::ByteArrayR buff);
			virtual ~X509Cert();

			Bool GetSubjectCN(NotNullPtr<Text::StringBuilderUTF8> sb) const;
			UTF8Char *GetSubjectCN(UTF8Char *sbuff) const;
			Bool GetIssuerCN(NotNullPtr<Text::StringBuilderUTF8> sb) const;
			
			virtual FileType GetFileType() const;
			virtual void ToShortName(NotNullPtr<Text::StringBuilderUTF8> sb) const;

			virtual UOSInt GetCertCount();
			virtual Bool GetCertName(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb);
			virtual X509Cert *GetNewCert(UOSInt index);
			virtual ValidStatus IsValid(Net::SSLEngine *ssl, Crypto::Cert::CertStore *trustStore) const;

			virtual NotNullPtr<ASN1Data> Clone() const;
			virtual NotNullPtr<X509Cert> CreateX509Cert() const;
			virtual void ToString(NotNullPtr<Text::StringBuilderUTF8> sb) const;

			Bool GetIssuerNames(CertNames *names) const;
			Bool GetSubjNames(CertNames *names) const;
			Bool GetExtensions(CertExtensions *ext) const;
			Crypto::Cert::X509Key *GetNewPublicKey() const;
			Bool GetKeyId(const Data::ByteArray &keyId) const; //20 bytes
			Bool GetNotBefore(Data::DateTime *dt) const;
			Bool GetNotAfter(Data::DateTime *dt) const;
			Bool DomainValid(Text::CString domain) const;
			Bool IsSelfSigned() const;
			UOSInt GetCRLDistributionPoints(Data::ArrayList<Text::CString> *crlDistributionPoints) const;

			const UInt8 *GetIssuerNamesSeq(UOSInt *dataLen) const;
			const UInt8 *GetSerialNumber(UOSInt *dataLen) const;
		};
	}
}
#endif
