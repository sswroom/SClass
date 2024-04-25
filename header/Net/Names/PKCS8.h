#ifndef _SM_NET_NAMES_PKCS8
#define _SM_NET_NAMES_PKCS8
#include "Net/ASN1Names.h"

namespace Net
{
	namespace Names
	{
		class PKCS8
		{
			public:
			static void PrivateKeyInfo(NN<ASN1Names> names);
			static void PrivateKeyInfoCont(NN<ASN1Names> names);
			static void EncryptedPrivateKeyInfo(NN<ASN1Names> names);
			static void EncryptedPrivateKeyInfoCont(NN<ASN1Names> names);
		};
	}
}
#endif
