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
			static void __stdcall AddContentInfo(NN<ASN1Names> names, Text::CStringNN name);
			static void __stdcall ContentInfo(NN<ASN1Names> names);
			static void __stdcall ContentInfoCont(NN<ASN1Names> names);
			static void __stdcall Data(NN<ASN1Names> names);
			static void __stdcall SignedData(NN<ASN1Names> names);
			static void __stdcall SignedDataCont(NN<ASN1Names> names);
			static void __stdcall DigestAlgorithmIdentifiers(NN<ASN1Names> names);
			static void __stdcall CertificateSet(NN<ASN1Names> names);
			static void __stdcall CertificateRevocationLists(NN<ASN1Names> names);
			static void __stdcall SignerInfos(NN<ASN1Names> names);
			static void __stdcall SignerInfoCont(NN<ASN1Names> names);
			static void __stdcall IssuerAndSerialNumberCont(NN<ASN1Names> names);
			static void __stdcall AddDigestInfo(NN<ASN1Names> names, Text::CStringNN name);
			static void __stdcall DigestInfoCont(NN<ASN1Names> names);
			static void __stdcall EnvelopedData(NN<ASN1Names> names);
			static void __stdcall EnvelopedDataCont(NN<ASN1Names> names);
			static void __stdcall OriginatorInfoCont(NN<ASN1Names> names);
			static void __stdcall RecipientInfos(NN<ASN1Names> names);
			static void __stdcall KeyTransportRecipientInfoCont(NN<ASN1Names> names);
			static void __stdcall SignedAndEnvelopedData(NN<ASN1Names> names);
			static void __stdcall SignedAndEnvelopedDataCont(NN<ASN1Names> names);
			static void __stdcall DigestedData(NN<ASN1Names> names);
			static void __stdcall DigestedDataCont(NN<ASN1Names> names);
			static void __stdcall EncryptedData(NN<ASN1Names> names);
			static void __stdcall EncryptedDataCont(NN<ASN1Names> names);
			static void __stdcall EncryptedContentInfoCont(NN<ASN1Names> names);
			static void __stdcall AuthenticatedData(NN<ASN1Names> names);
		};
	}
}
#endif
