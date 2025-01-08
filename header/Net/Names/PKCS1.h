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
			static void __stdcall RSAPublicKey(NN<ASN1Names> names);
			static void __stdcall RSAPublicKeyCont(NN<ASN1Names> names);
			static void __stdcall RSAPrivateKey(NN<ASN1Names> names);
			static void __stdcall RSAPrivateKeyCont(NN<ASN1Names> names);
			static void __stdcall OtherPrimeInfos(NN<ASN1Names> names);
			static void __stdcall AddDigestInfo(NN<ASN1Names> names, Text::CStringNN name);
			static void __stdcall DigestInfoCont(NN<ASN1Names> names);
		};
	}
}
#endif
