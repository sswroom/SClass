#ifndef _SM_NET_WEBBROWSER
#define _SM_NET_WEBBROWSER
#include "Crypto/Hash/IHash.h"
#include "IO/IStreamData.h"
#include "Net/HTTPQueue.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"

namespace Net
{
	class WebBrowser
	{
	private:
		Net::SocketFactory *sockf;
		Net::SSLEngine *ssl;
		const UTF8Char *cacheDir;
		Crypto::Hash::IHash *hash;
		Net::HTTPQueue *queue;

		UTF8Char *GetLocalFileName(UTF8Char *sbuff, const UTF8Char *url, UOSInt urlLen);
	public:
		WebBrowser(Net::SocketFactory *sockf, Net::SSLEngine *ssl, const UTF8Char *cacheDir);
		~WebBrowser();

		IO::IStreamData *GetData(const UTF8Char *url, UOSInt urlLen, Bool forceReload, UTF8Char *contentType);
	};
}
#endif
