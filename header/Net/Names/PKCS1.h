#ifndef _SM_NET_NAMES_PKCS1
#define _SM_NET_NAMES_PKCS1
#include "Net/ASN1Names.h"

namespace Net
{
	namespace Names
	{
		class PKCS1
		{
		public:
			static void RSAPublicKey(NotNullPtr<ASN1Names> names);
			static void RSAPublicKeyCont(NotNullPtr<ASN1Names> names);
			static void RSAPrivateKey(NotNullPtr<ASN1Names> names);
			static void RSAPrivateKeyCont(NotNullPtr<ASN1Names> names);
			static void OtherPrimeInfos(NotNullPtr<ASN1Names> names);
			static void AddDigestInfo(NotNullPtr<ASN1Names> names, Text::CStringNN name);
			static void DigestInfoCont(NotNullPtr<ASN1Names> names);
		};
	}
}
#endif
