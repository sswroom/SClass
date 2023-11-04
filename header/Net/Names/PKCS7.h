#ifndef _SM_NET_NAMES_PKCS7
#define _SM_NET_NAMES_PKCS7
#include "Net/ASN1Names.h"

namespace Net
{
	namespace Names
	{
		class PKCS7
		{
		public:
			static void AddContentInfo(NotNullPtr<ASN1Names> names, Text::CStringNN name);
			static void ContentInfo(NotNullPtr<ASN1Names> names);
			static void ContentInfoCont(NotNullPtr<ASN1Names> names);
			static void Data(NotNullPtr<ASN1Names> names);
			static void SignedData(NotNullPtr<ASN1Names> names);
			static void SignedDataCont(NotNullPtr<ASN1Names> names);
			static void DigestAlgorithmIdentifiers(NotNullPtr<ASN1Names> names);
			static void CertificateSet(NotNullPtr<ASN1Names> names);
			static void CertificateRevocationLists(NotNullPtr<ASN1Names> names);
			static void SignerInfos(NotNullPtr<ASN1Names> names);
			static void SignerInfoCont(NotNullPtr<ASN1Names> names);
			static void IssuerAndSerialNumberCont(NotNullPtr<ASN1Names> names);
			static void AddDigestInfo(NotNullPtr<ASN1Names> names, Text::CStringNN name);
			static void DigestInfoCont(NotNullPtr<ASN1Names> names);
			static void EnvelopedData(NotNullPtr<ASN1Names> names);
			static void EnvelopedDataCont(NotNullPtr<ASN1Names> names);
			static void OriginatorInfoCont(NotNullPtr<ASN1Names> names);
			static void RecipientInfos(NotNullPtr<ASN1Names> names);
			static void KeyTransportRecipientInfoCont(NotNullPtr<ASN1Names> names);
			static void SignedAndEnvelopedData(NotNullPtr<ASN1Names> names);
			static void SignedAndEnvelopedDataCont(NotNullPtr<ASN1Names> names);
			static void DigestedData(NotNullPtr<ASN1Names> names);
			static void DigestedDataCont(NotNullPtr<ASN1Names> names);
			static void EncryptedData(NotNullPtr<ASN1Names> names);
			static void EncryptedDataCont(NotNullPtr<ASN1Names> names);
			static void EncryptedContentInfoCont(NotNullPtr<ASN1Names> names);
			static void AuthenticatedData(NotNullPtr<ASN1Names> names);
		};
	}
}
#endif
