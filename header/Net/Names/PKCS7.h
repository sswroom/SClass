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
			static void AddContentInfo(NN<ASN1Names> names, Text::CStringNN name);
			static void ContentInfo(NN<ASN1Names> names);
			static void ContentInfoCont(NN<ASN1Names> names);
			static void Data(NN<ASN1Names> names);
			static void SignedData(NN<ASN1Names> names);
			static void SignedDataCont(NN<ASN1Names> names);
			static void DigestAlgorithmIdentifiers(NN<ASN1Names> names);
			static void CertificateSet(NN<ASN1Names> names);
			static void CertificateRevocationLists(NN<ASN1Names> names);
			static void SignerInfos(NN<ASN1Names> names);
			static void SignerInfoCont(NN<ASN1Names> names);
			static void IssuerAndSerialNumberCont(NN<ASN1Names> names);
			static void AddDigestInfo(NN<ASN1Names> names, Text::CStringNN name);
			static void DigestInfoCont(NN<ASN1Names> names);
			static void EnvelopedData(NN<ASN1Names> names);
			static void EnvelopedDataCont(NN<ASN1Names> names);
			static void OriginatorInfoCont(NN<ASN1Names> names);
			static void RecipientInfos(NN<ASN1Names> names);
			static void KeyTransportRecipientInfoCont(NN<ASN1Names> names);
			static void SignedAndEnvelopedData(NN<ASN1Names> names);
			static void SignedAndEnvelopedDataCont(NN<ASN1Names> names);
			static void DigestedData(NN<ASN1Names> names);
			static void DigestedDataCont(NN<ASN1Names> names);
			static void EncryptedData(NN<ASN1Names> names);
			static void EncryptedDataCont(NN<ASN1Names> names);
			static void EncryptedContentInfoCont(NN<ASN1Names> names);
			static void AuthenticatedData(NN<ASN1Names> names);
		};
	}
}
#endif
