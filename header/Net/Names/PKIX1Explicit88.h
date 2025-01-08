#ifndef _SM_NET_NAMES_PKIX1EXPLICIT88
#define _SM_NET_NAMES_PKIX1EXPLICIT88
#include "Net/ASN1Names.h"

namespace Net
{
	namespace Names
	{
		class PKIX1Explicit88
		{
		public:
			static void __stdcall AddAttributeTypeAndValue(NN<ASN1Names> names, Text::CStringNN name);
			static void __stdcall AttributeTypeAndValueCont(NN<ASN1Names> names);
			static void __stdcall AddName(NN<ASN1Names> names, Text::CStringNN name);
			static void __stdcall Name(NN<ASN1Names> names);
			static void __stdcall RDNSequenceCont(NN<ASN1Names> names);
			static void __stdcall RelativeDistinguishedName(NN<ASN1Names> names);
			static void __stdcall RelativeDistinguishedNameCont(NN<ASN1Names> names);
			static void __stdcall Certificate(NN<ASN1Names> names);
			static void __stdcall CertificateCont(NN<ASN1Names> names);
			static void __stdcall AddTBSCertificate(NN<ASN1Names> names, Text::CStringNN name);
			static void __stdcall TBSCertificateCont(NN<ASN1Names> names);
			static void __stdcall Version(NN<ASN1Names> names);
			static void __stdcall AddValidity(NN<ASN1Names> names, Text::CStringNN name);
			static void __stdcall ValidityCont(NN<ASN1Names> names);
			static void __stdcall AddSubjectPublicKeyInfo(NN<ASN1Names> names, Text::CStringNN name);
			static void __stdcall SubjectPublicKeyInfoCont(NN<ASN1Names> names);
			static void __stdcall AddExtensions(NN<ASN1Names> names, Text::CStringNN name);
			static void __stdcall Extensions(NN<ASN1Names> names);
			static void __stdcall ExtensionsCont(NN<ASN1Names> names);
			static void __stdcall ExtensionCont(NN<ASN1Names> names);
			static void __stdcall CertificateList(NN<ASN1Names> names);
			static void __stdcall CertificateListCont(NN<ASN1Names> names);
			static void __stdcall AddTBSCertList(NN<ASN1Names> names, Text::CStringNN name);
			static void __stdcall TBSCertListCont(NN<ASN1Names> names);
			static void __stdcall RevokedCertificates(NN<ASN1Names> names);
			static void __stdcall RevokedCertificateCont(NN<ASN1Names> names);
			static void __stdcall AddAlgorithmIdentifier(NN<ASN1Names> names, Text::CStringNN name);
			static void __stdcall AlgorithmIdentifierCont(NN<ASN1Names> names);
		};
	}
}
#endif

