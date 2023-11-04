#ifndef _SM_NET_NAMES_PKCS9
#define _SM_NET_NAMES_PKCS9
#include "Net/ASN1Names.h"

namespace Net
{
	namespace Names
	{
		class PKCS9
		{
		public:
			static void AttributeContentType(NotNullPtr<ASN1Names> names);
			static void AttributeMessageDigest(NotNullPtr<ASN1Names> names);
			static void AttributeSigningTime(NotNullPtr<ASN1Names> names);
			static void AttributeSMIMECapabilities(NotNullPtr<ASN1Names> names);
			static void AttributeFriendlyName(NotNullPtr<ASN1Names> names);
			static void AttributeLocalKeyId(NotNullPtr<ASN1Names> names);
			static void SMIMECapabilitiesCont(NotNullPtr<ASN1Names> names);
			static void SMIMECapabilityCont(NotNullPtr<ASN1Names> names);
		};
	}
}
#endif
