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
			static void AddCertificationRequestInfo(NotNullPtr<ASN1Names> names, Text::CStringNN name);
			static void CertificationRequestInfoCont(NotNullPtr<ASN1Names> names);
			static void AttributesCont(NotNullPtr<ASN1Names> names);
			static void CertificationRequest(NotNullPtr<ASN1Names> names);
			static void CertificationRequestCont(NotNullPtr<ASN1Names> names);
		};
	}
}
#endif
