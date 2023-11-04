#ifndef _SM_NET_NAMES_RFC2459
#define _SM_NET_NAMES_RFC2459
#include "Net/ASN1Names.h"

namespace Net
{
	namespace Names
	{
		class RFC2459
		{
		public:
			static void AuthorityInfoAccessSyntax(NotNullPtr<ASN1Names> names);
			static void AuthorityInfoAccessSyntaxCont(NotNullPtr<ASN1Names> names);
			static void AccessDescriptionCont(NotNullPtr<ASN1Names> names);
		};
	}
}
#endif
