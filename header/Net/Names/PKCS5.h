#ifndef _SM_NET_NAMES_PKCS5
#define _SM_NET_NAMES_PKCS5
#include "Net/ASN1Names.h"

namespace Net
{
	namespace Names
	{
		class PKCS5
		{
		public:
			static void __stdcall PBKDF2Params(NN<ASN1Names> names);
			static void __stdcall PBEParameter(NN<ASN1Names> names);
			static void __stdcall PBES2Params(NN<ASN1Names> names);
			static void __stdcall PBMAC1Params(NN<ASN1Names> names);
			static void __stdcall RC2CBCParam(NN<ASN1Names> names);
			static void __stdcall RC5CBCParams(NN<ASN1Names> names);
		};
	}
}
#endif
