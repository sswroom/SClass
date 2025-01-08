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
			static void __stdcall PFX(NN<ASN1Names> names);
			static void __stdcall PFXCont(NN<ASN1Names> names);
			static void __stdcall AddMacData(NN<ASN1Names> names, Text::CStringNN name);
			static void __stdcall MacDataCont(NN<ASN1Names> names);
			static void __stdcall AuthenticatedSafeContentInfoCont(NN<ASN1Names> names);
			static void __stdcall AuthenticatedSafeData(NN<ASN1Names> names);
			static void __stdcall AuthenticatedSafeEnvelopedData(NN<ASN1Names> names);
			static void __stdcall AuthenticatedSafeEncryptedData(NN<ASN1Names> names);
			static void __stdcall AuthenticatedSafe(NN<ASN1Names> names);
			static void __stdcall AuthSafeContentInfo(NN<ASN1Names> names);
			static void __stdcall AuthSafeContentInfoCont(NN<ASN1Names> names);
			static void __stdcall SafeContentsData(NN<ASN1Names> names);
			static void __stdcall SafeContents(NN<ASN1Names> names);
			static void __stdcall SafeContentsCont(NN<ASN1Names> names);
			static void __stdcall SafeBagCont(NN<ASN1Names> names);
			static void __stdcall CertBag(NN<ASN1Names> names);
			static void __stdcall CertBagCont(NN<ASN1Names> names);
			static void __stdcall X509Certificate(NN<ASN1Names> names);
			static void __stdcall SdsiCertificate(NN<ASN1Names> names);
			static void __stdcall CRLBag(NN<ASN1Names> names);
			static void __stdcall CRLBagCont(NN<ASN1Names> names);
			static void __stdcall X509CRL(NN<ASN1Names> names);
			static void __stdcall SecretBag(NN<ASN1Names> names);
			static void __stdcall SecretBagCont(NN<ASN1Names> names);
			static void __stdcall PKCS12Attributes(NN<ASN1Names> names);
		};
	}
}
#endif
