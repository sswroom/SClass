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
			static void PBEParam(NotNullPtr<ASN1Names> names);
			static void ExtendedValidationCertificates(NotNullPtr<ASN1Names> names); //GoDaddy
			static void AttributeOutlookExpress(NotNullPtr<ASN1Names> names);
		};
	}
}
#endif
