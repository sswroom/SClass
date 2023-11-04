#ifndef _SM_NET_NAMES_PKIX1IMPLICIT88
#define _SM_NET_NAMES_PKIX1IMPLICIT88
#include "Net/ASN1Names.h"

namespace Net
{
	namespace Names
	{
		class PKIX1Implicit88
		{
		public:
			static void AuthorityKeyIdentifier(NotNullPtr<ASN1Names> names);
			static void AuthorityKeyIdentifierCont(NotNullPtr<ASN1Names> names);
			static void SubjectKeyIdentifier(NotNullPtr<ASN1Names> names);
			static void KeyUsage(NotNullPtr<ASN1Names> names);
			static void CertificatePolicies(NotNullPtr<ASN1Names> names);
			static void CertificatePoliciesCont(NotNullPtr<ASN1Names> names);
			static void PolicyInformationCont(NotNullPtr<ASN1Names> names);
			static void PolicyQualifiers(NotNullPtr<ASN1Names> names);
			static void PolicyQualifierInfoCont(NotNullPtr<ASN1Names> names);
			static void GeneralNames(NotNullPtr<ASN1Names> names);
			static void GeneralNameCont(NotNullPtr<ASN1Names> names);
			static void BasicConstraints(NotNullPtr<ASN1Names> names);
			static void BasicConstraintsCont(NotNullPtr<ASN1Names> names);
			static void CRLDistributionPoints(NotNullPtr<ASN1Names> names);
			static void CRLDistributionPointsCont(NotNullPtr<ASN1Names> names);
			static void DistributionPointCont(NotNullPtr<ASN1Names> names);
			static void DistributionPointName(NotNullPtr<ASN1Names> names);
			static void ReasonFlags(NotNullPtr<ASN1Names> names);
			static void ExtKeyUsageSyntax(NotNullPtr<ASN1Names> names);
			static void ExtKeyUsageSyntaxCont(NotNullPtr<ASN1Names> names);
		};
	}
}
#endif
