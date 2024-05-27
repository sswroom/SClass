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
			X509Cert(NN<Text::String> sourceName, Data::ByteArrayR buff);
			X509Cert(Text::CStringNN sourceName, Data::ByteArrayR buff);
			virtual ~X509Cert();

			Bool GetSubjectCN(NN<Text::StringBuilderUTF8> sb) const;
			UTF8Char *GetSubjectCN(UTF8Char *sbuff) const;
			Bool GetIssuerCN(NN<Text::StringBuilderUTF8> sb) const;
			void SetDefaultSourceName();
			
			virtual FileType GetFileType() const;
			virtual void ToShortName(NN<Text::StringBuilderUTF8> sb) const;

			virtual UOSInt GetCertCount();
			virtual Bool GetCertName(UOSInt index, NN<Text::StringBuilderUTF8> sb);
			virtual Optional<X509Cert> GetNewCert(UOSInt index);
			virtual ValidStatus IsValid(NN<Net::SSLEngine> ssl, Optional<Crypto::Cert::CertStore> trustStore) const;

			virtual NN<ASN1Data> Clone() const;
			virtual Optional<X509Cert> CreateX509Cert() const;
			virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
			virtual NN<Net::ASN1Names> CreateNames() const;

			Bool GetIssuerNames(NN<CertNames> names) const;
			Bool GetSubjNames(NN<CertNames> names) const;
			Bool GetExtensions(NN<CertExtensions> ext) const;
			Optional<Crypto::Cert::X509Key> GetNewPublicKey() const;
			Bool GetKeyId(const Data::ByteArray &keyId) const; //20 bytes
			Bool GetNotBefore(NN<Data::DateTime> dt) const;
			Bool GetNotAfter(NN<Data::DateTime> dt) const;
			Bool DomainValid(Text::CString domain) const;
			Bool IsSelfSigned() const;
			UOSInt GetCRLDistributionPoints(Data::ArrayList<Text::CString> *crlDistributionPoints) const;

			const UInt8 *GetIssuerNamesSeq(OutParam<UOSInt> dataLen) const;
			const UInt8 *GetSerialNumber(OutParam<UOSInt> dataLen) const;
		};
	}
}
#endif
