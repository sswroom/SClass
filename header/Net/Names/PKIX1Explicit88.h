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
			static void AddAttributeTypeAndValue(NN<ASN1Names> names, Text::CStringNN name);
			static void AttributeTypeAndValueCont(NN<ASN1Names> names);
			static void AddName(NN<ASN1Names> names, Text::CStringNN name);
			static void Name(NN<ASN1Names> names);
			static void RDNSequenceCont(NN<ASN1Names> names);
			static void RelativeDistinguishedName(NN<ASN1Names> names);
			static void RelativeDistinguishedNameCont(NN<ASN1Names> names);
			static void Certificate(NN<ASN1Names> names);
			static void CertificateCont(NN<ASN1Names> names);
			static void AddTBSCertificate(NN<ASN1Names> names, Text::CStringNN name);
			static void TBSCertificateCont(NN<ASN1Names> names);
			static void Version(NN<ASN1Names> names);
			static void AddValidity(NN<ASN1Names> names, Text::CStringNN name);
			static void ValidityCont(NN<ASN1Names> names);
			static void AddSubjectPublicKeyInfo(NN<ASN1Names> names, Text::CStringNN name);
			static void SubjectPublicKeyInfoCont(NN<ASN1Names> names);
			static void AddExtensions(NN<ASN1Names> names, Text::CStringNN name);
			static void Extensions(NN<ASN1Names> names);
			static void ExtensionsCont(NN<ASN1Names> names);
			static void ExtensionCont(NN<ASN1Names> names);
			static void CertificateList(NN<ASN1Names> names);
			static void CertificateListCont(NN<ASN1Names> names);
			static void AddTBSCertList(NN<ASN1Names> names, Text::CStringNN name);
			static void TBSCertListCont(NN<ASN1Names> names);
			static void RevokedCertificates(NN<ASN1Names> names);
			static void RevokedCertificateCont(NN<ASN1Names> names);
			static void AddAlgorithmIdentifier(NN<ASN1Names> names, Text::CStringNN name);
			static void AlgorithmIdentifierCont(NN<ASN1Names> names);
		};
	}
}
#endif

