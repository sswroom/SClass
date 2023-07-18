#ifndef _SM_NET_WEBBROWSER
#define _SM_NET_WEBBROWSER
#include "Crypto/Hash/CRC32RIEEE.h"
#include "IO/StreamData.h"
#include "Net/HTTPQueue.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Text/CString.h"
#include "Text/String.h"

namespace Net
{
	class WebBrowser
	{
	private:
		NotNullPtr<Net::SocketFactory> sockf;
		Net::SSLEngine *ssl;
		NotNullPtr<Text::String> cacheDir;
		Crypto::Hash::CRC32RIEEE hash;
		Net::HTTPQueue queue;

		UTF8Char *GetLocalFileName(UTF8Char *sbuff, const UTF8Char *url, UOSInt urlLen);
	public:
		WebBrowser(NotNullPtr<Net::SocketFactory> sockf, Net::SSLEngine *ssl, Text::CString cacheDir);
		~WebBrowser();

		IO::StreamData *GetData(Text::CString url, Bool forceReload, UTF8Char *contentType);
	};
}
#endif
