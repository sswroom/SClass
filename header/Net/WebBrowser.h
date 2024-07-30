#ifndef _SM_NET_WEBBROWSER
#define _SM_NET_WEBBROWSER
#include "Crypto/Hash/CRC32RIEEE.h"
#include "IO/StreamData.h"
#include "Net/HTTPQueue.h"
#include "Net/SSLEngine.h"
#include "Net/TCPClientFactory.h"
#include "Text/CString.h"
#include "Text/String.h"

namespace Net
{
	class WebBrowser
	{
	private:
		NN<Net::TCPClientFactory> clif;
		Optional<Net::SSLEngine> ssl;
		NN<Text::String> cacheDir;
		Crypto::Hash::CRC32RIEEE hash;
		Net::HTTPQueue queue;

		UnsafeArrayOpt<UTF8Char> GetLocalFileName(UnsafeArray<UTF8Char> sbuff, UnsafeArray<const UTF8Char> url, UOSInt urlLen);
	public:
		WebBrowser(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN cacheDir);
		~WebBrowser();

		IO::StreamData *GetData(Text::CStringNN url, Bool forceReload, UnsafeArrayOpt<UTF8Char> contentType);
	};
}
#endif
