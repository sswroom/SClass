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
			static void AddAttributeTypeAndValue(NotNullPtr<ASN1Names> names, Text::CStringNN name);
			static void AttributeTypeAndValueCont(NotNullPtr<ASN1Names> names);
			static void AddName(NotNullPtr<ASN1Names> names, Text::CStringNN name);
			static void Name(NotNullPtr<ASN1Names> names);
			static void RDNSequenceCont(NotNullPtr<ASN1Names> names);
			static void RelativeDistinguishedName(NotNullPtr<ASN1Names> names);
			static void RelativeDistinguishedNameCont(NotNullPtr<ASN1Names> names);
			static void Certificate(NotNullPtr<ASN1Names> names);
			static void CertificateCont(NotNullPtr<ASN1Names> names);
			static void AddTBSCertificate(NotNullPtr<ASN1Names> names, Text::CStringNN name);
			static void TBSCertificateCont(NotNullPtr<ASN1Names> names);
			static void Version(NotNullPtr<ASN1Names> names);
			static void AddValidity(NotNullPtr<ASN1Names> names, Text::CStringNN name);
			static void ValidityCont(NotNullPtr<ASN1Names> names);
			static void AddSubjectPublicKeyInfo(NotNullPtr<ASN1Names> names, Text::CStringNN name);
			static void SubjectPublicKeyInfoCont(NotNullPtr<ASN1Names> names);
			static void AddExtensions(NotNullPtr<ASN1Names> names, Text::CStringNN name);
			static void Extensions(NotNullPtr<ASN1Names> names);
			static void ExtensionsCont(NotNullPtr<ASN1Names> names);
			static void ExtensionCont(NotNullPtr<ASN1Names> names);
			static void CertificateList(NotNullPtr<ASN1Names> names);
			static void CertificateListCont(NotNullPtr<ASN1Names> names);
			static void AddTBSCertList(NotNullPtr<ASN1Names> names, Text::CStringNN name);
			static void TBSCertListCont(NotNullPtr<ASN1Names> names);
			static void RevokedCertificates(NotNullPtr<ASN1Names> names);
			static void RevokedCertificateCont(NotNullPtr<ASN1Names> names);
			static void AddAlgorithmIdentifier(NotNullPtr<ASN1Names> names, Text::CStringNN name);
			static void AlgorithmIdentifierCont(NotNullPtr<ASN1Names> names);
		};
	}
}
#endif

