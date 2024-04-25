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
			static void RSAPublicKey(NN<ASN1Names> names);
			static void RSAPublicKeyCont(NN<ASN1Names> names);
			static void RSAPrivateKey(NN<ASN1Names> names);
			static void RSAPrivateKeyCont(NN<ASN1Names> names);
			static void OtherPrimeInfos(NN<ASN1Names> names);
			static void AddDigestInfo(NN<ASN1Names> names, Text::CStringNN name);
			static void DigestInfoCont(NN<ASN1Names> names);
		};
	}
}
#endif
