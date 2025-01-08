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
			static void __stdcall AttributeContentType(NN<ASN1Names> names);
			static void __stdcall AttributeMessageDigest(NN<ASN1Names> names);
			static void __stdcall AttributeSigningTime(NN<ASN1Names> names);
			static void __stdcall AttributeSMIMECapabilities(NN<ASN1Names> names);
			static void __stdcall AttributeFriendlyName(NN<ASN1Names> names);
			static void __stdcall AttributeLocalKeyId(NN<ASN1Names> names);
			static void __stdcall SMIMECapabilitiesCont(NN<ASN1Names> names);
			static void __stdcall SMIMECapabilityCont(NN<ASN1Names> names);
		};
	}
}
#endif
