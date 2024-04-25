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
			static void AuthorityInfoAccessSyntax(NN<ASN1Names> names);
			static void AuthorityInfoAccessSyntaxCont(NN<ASN1Names> names);
			static void AccessDescriptionCont(NN<ASN1Names> names);
		};
	}
}
#endif
