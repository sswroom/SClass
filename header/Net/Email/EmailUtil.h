#ifndef _SM_NET_EMAIL_EMAILUTIL
#define _SM_NET_EMAIL_EMAILUTIL

namespace Net
{
	namespace Email
	{
		class EmailUtil
		{
		public:
			static Bool AddressValid(UnsafeArray<const UTF8Char> addr);
		};
	}
}
#endif
