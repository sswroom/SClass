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
			static void PFX(NotNullPtr<ASN1Names> names);
			static void PFXCont(NotNullPtr<ASN1Names> names);
			static void AddMacData(NotNullPtr<ASN1Names> names, Text::CStringNN name);
			static void MacDataCont(NotNullPtr<ASN1Names> names);
			static void AuthenticatedSafeContentInfoCont(NotNullPtr<ASN1Names> names);
			static void AuthenticatedSafeData(NotNullPtr<ASN1Names> names);
			static void AuthenticatedSafeEnvelopedData(NotNullPtr<ASN1Names> names);
			static void AuthenticatedSafeEncryptedData(NotNullPtr<ASN1Names> names);
			static void AuthenticatedSafe(NotNullPtr<ASN1Names> names);
			static void AuthSafeContentInfo(NotNullPtr<ASN1Names> names);
			static void AuthSafeContentInfoCont(NotNullPtr<ASN1Names> names);
			static void SafeContentsData(NotNullPtr<ASN1Names> names);
			static void SafeContents(NotNullPtr<ASN1Names> names);
			static void SafeContentsCont(NotNullPtr<ASN1Names> names);
			static void SafeBagCont(NotNullPtr<ASN1Names> names);
			static void CertBag(NotNullPtr<ASN1Names> names);
			static void CertBagCont(NotNullPtr<ASN1Names> names);
			static void X509Certificate(NotNullPtr<ASN1Names> names);
			static void SdsiCertificate(NotNullPtr<ASN1Names> names);
			static void CRLBag(NotNullPtr<ASN1Names> names);
			static void CRLBagCont(NotNullPtr<ASN1Names> names);
			static void X509CRL(NotNullPtr<ASN1Names> names);
			static void SecretBag(NotNullPtr<ASN1Names> names);
			static void SecretBagCont(NotNullPtr<ASN1Names> names);
			static void PKCS12Attributes(NotNullPtr<ASN1Names> names);
		};
	}
}
#endif
