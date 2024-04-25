#ifndef _SM_NET_NAMES_PKCS12
#define _SM_NET_NAMES_PKCS12
#include "Net/ASN1Names.h"

namespace Net
{
	namespace Names
	{
		class PKCS12
		{
		public:
			static void PFX(NN<ASN1Names> names);
			static void PFXCont(NN<ASN1Names> names);
			static void AddMacData(NN<ASN1Names> names, Text::CStringNN name);
			static void MacDataCont(NN<ASN1Names> names);
			static void AuthenticatedSafeContentInfoCont(NN<ASN1Names> names);
			static void AuthenticatedSafeData(NN<ASN1Names> names);
			static void AuthenticatedSafeEnvelopedData(NN<ASN1Names> names);
			static void AuthenticatedSafeEncryptedData(NN<ASN1Names> names);
			static void AuthenticatedSafe(NN<ASN1Names> names);
			static void AuthSafeContentInfo(NN<ASN1Names> names);
			static void AuthSafeContentInfoCont(NN<ASN1Names> names);
			static void SafeContentsData(NN<ASN1Names> names);
			static void SafeContents(NN<ASN1Names> names);
			static void SafeContentsCont(NN<ASN1Names> names);
			static void SafeBagCont(NN<ASN1Names> names);
			static void CertBag(NN<ASN1Names> names);
			static void CertBagCont(NN<ASN1Names> names);
			static void X509Certificate(NN<ASN1Names> names);
			static void SdsiCertificate(NN<ASN1Names> names);
			static void CRLBag(NN<ASN1Names> names);
			static void CRLBagCont(NN<ASN1Names> names);
			static void X509CRL(NN<ASN1Names> names);
			static void SecretBag(NN<ASN1Names> names);
			static void SecretBagCont(NN<ASN1Names> names);
			static void PKCS12Attributes(NN<ASN1Names> names);
		};
	}
}
#endif
