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
			static void __stdcall AddCertificationRequestInfo(NN<ASN1Names> names, Text::CStringNN name);
			static void __stdcall CertificationRequestInfoCont(NN<ASN1Names> names);
			static void __stdcall AttributesCont(NN<ASN1Names> names);
			static void __stdcall CertificationRequest(NN<ASN1Names> names);
			static void __stdcall CertificationRequestCont(NN<ASN1Names> names);
		};
	}
}
#endif
