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
			static void PrivateKeyInfo(NotNullPtr<ASN1Names> names);
			static void PrivateKeyInfoCont(NotNullPtr<ASN1Names> names);
			static void EncryptedPrivateKeyInfo(NotNullPtr<ASN1Names> names);
			static void EncryptedPrivateKeyInfoCont(NotNullPtr<ASN1Names> names);
		};
	}
}
#endif
