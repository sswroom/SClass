#ifndef _SM_NET_NAMES_PKCS10
#define _SM_NET_NAMES_PKCS10
#include "Net/ASN1Names.h"

namespace Net
{
	namespace Names
	{
		class PKCS10
		{
		public:
			static void AddCertificationRequestInfo(NN<ASN1Names> names, Text::CStringNN name);
			static void CertificationRequestInfoCont(NN<ASN1Names> names);
			static void AttributesCont(NN<ASN1Names> names);
			static void CertificationRequest(NN<ASN1Names> names);
			static void CertificationRequestCont(NN<ASN1Names> names);
		};
	}
}
#endif
