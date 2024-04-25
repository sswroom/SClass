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
			static void AuthorityKeyIdentifier(NN<ASN1Names> names);
			static void AuthorityKeyIdentifierCont(NN<ASN1Names> names);
			static void SubjectKeyIdentifier(NN<ASN1Names> names);
			static void KeyUsage(NN<ASN1Names> names);
			static void CertificatePolicies(NN<ASN1Names> names);
			static void CertificatePoliciesCont(NN<ASN1Names> names);
			static void PolicyInformationCont(NN<ASN1Names> names);
			static void PolicyQualifiers(NN<ASN1Names> names);
			static void PolicyQualifierInfoCont(NN<ASN1Names> names);
			static void GeneralNames(NN<ASN1Names> names);
			static void GeneralNameCont(NN<ASN1Names> names);
			static void BasicConstraints(NN<ASN1Names> names);
			static void BasicConstraintsCont(NN<ASN1Names> names);
			static void CRLDistributionPoints(NN<ASN1Names> names);
			static void CRLDistributionPointsCont(NN<ASN1Names> names);
			static void DistributionPointCont(NN<ASN1Names> names);
			static void DistributionPointName(NN<ASN1Names> names);
			static void ReasonFlags(NN<ASN1Names> names);
			static void ExtKeyUsageSyntax(NN<ASN1Names> names);
			static void ExtKeyUsageSyntaxCont(NN<ASN1Names> names);
		};
	}
}
#endif
