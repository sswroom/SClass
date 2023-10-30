#ifndef _SM_NET_SSLUTIL
#define _SM_NET_SSLUTIL
#include "Text/CString.h"

namespace Net
{
	class SSLUtil
	{
	private:
		static UInt16 csuites[];
		static UOSInt GenClientHello(UInt8 *buff, Text::CStringNN serverHost);
	public:
		static UOSInt GenSSLClientHello(UInt8 *buff, Text::CStringNN serverHost);
	};
}
#endif