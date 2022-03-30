#ifndef _SM_NET_WEBSERVER_HTTPSERVERUTIL
#define _SM_NET_WEBSERVER_HTTPSERVERUTIL
#include "IO/Stream.h"
#include "Net/WebServer/IWebResponse.h"
#include "Text/CString.h"

namespace Net
{
	namespace WebServer
	{
		class HTTPServerUtil
		{
		public:
			static Bool MIMEToCompress(Text::CString mime);
			static Bool SendContent(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString mime, UInt64 contLeng, IO::Stream *stm);
			static Bool SendContent(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString mime, UInt64 contLeng, const UInt8 *buff);
			static Bool ResponseFile(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString fileName, OSInt cacheAge);
		};
	}
}
#endif
