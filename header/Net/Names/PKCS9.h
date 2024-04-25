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
			static void AttributeContentType(NN<ASN1Names> names);
			static void AttributeMessageDigest(NN<ASN1Names> names);
			static void AttributeSigningTime(NN<ASN1Names> names);
			static void AttributeSMIMECapabilities(NN<ASN1Names> names);
			static void AttributeFriendlyName(NN<ASN1Names> names);
			static void AttributeLocalKeyId(NN<ASN1Names> names);
			static void SMIMECapabilitiesCont(NN<ASN1Names> names);
			static void SMIMECapabilityCont(NN<ASN1Names> names);
		};
	}
}
#endif
