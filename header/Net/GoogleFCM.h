#ifndef _SM_NET_GOOGLEFCM
#define _SM_NET_GOOGLEFCM
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Text/StringBuilderUTF8.h"

namespace Net
{
	class GoogleFCM
	{
	public:
		static Bool SendMessage(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::CString apiKey, Text::CString devToken, Text::CString message, Text::StringBuilderUTF8 *sbResult);
		static Bool SendMessages(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::CString apiKey, Data::ArrayList<Text::String*> *devTokens, Text::CString message, Text::StringBuilderUTF8 *sbResult);
	};
}
#endif
