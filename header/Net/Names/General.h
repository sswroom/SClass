#ifndef _SM_NET_NAMES_GENERAL
#define _SM_NET_NAMES_GENERAL
#include "Net/ASN1Names.h"

namespace Net
{
	namespace Names
	{
		class General
		{
		public:
			static void __stdcall PBEParam(NN<ASN1Names> names);
			static void __stdcall ExtendedValidationCertificates(NN<ASN1Names> names); //GoDaddy
			static void __stdcall AttributeOutlookExpress(NN<ASN1Names> names);
		};
	}
}
#endif
