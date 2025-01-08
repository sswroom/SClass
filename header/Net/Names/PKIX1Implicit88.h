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
			static void __stdcall AuthorityKeyIdentifier(NN<ASN1Names> names);
			static void __stdcall AuthorityKeyIdentifierCont(NN<ASN1Names> names);
			static void __stdcall SubjectKeyIdentifier(NN<ASN1Names> names);
			static void __stdcall KeyUsage(NN<ASN1Names> names);
			static void __stdcall CertificatePolicies(NN<ASN1Names> names);
			static void __stdcall CertificatePoliciesCont(NN<ASN1Names> names);
			static void __stdcall PolicyInformationCont(NN<ASN1Names> names);
			static void __stdcall PolicyQualifiers(NN<ASN1Names> names);
			static void __stdcall PolicyQualifierInfoCont(NN<ASN1Names> names);
			static void __stdcall GeneralNames(NN<ASN1Names> names);
			static void __stdcall GeneralNameCont(NN<ASN1Names> names);
			static void __stdcall BasicConstraints(NN<ASN1Names> names);
			static void __stdcall BasicConstraintsCont(NN<ASN1Names> names);
			static void __stdcall CRLDistributionPoints(NN<ASN1Names> names);
			static void __stdcall CRLDistributionPointsCont(NN<ASN1Names> names);
			static void __stdcall DistributionPointCont(NN<ASN1Names> names);
			static void __stdcall DistributionPointName(NN<ASN1Names> names);
			static void __stdcall ReasonFlags(NN<ASN1Names> names);
			static void __stdcall ExtKeyUsageSyntax(NN<ASN1Names> names);
			static void __stdcall ExtKeyUsageSyntaxCont(NN<ASN1Names> names);
		};
	}
}
#endif
