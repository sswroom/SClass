#ifndef _SM_NET_WEBSERVER_HTTPSERVERUTIL
#define _SM_NET_WEBSERVER_HTTPSERVERUTIL
#include "Data/Compress/DeflateStream.h"
#include "IO/Stream.h"
#include "Net/WebServer/IWebResponse.h"
#include "Text/CString.h"

namespace Net
{
	namespace WebServer
	{
		class HTTPServerUtil
		{
		private:
			static Data::Compress::DeflateStream::CompLevel compLevel;
		public:
			static Bool MIMEToCompress(Text::CString mime);
			static Bool SendContent(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CString mime, UInt64 contLeng, NotNullPtr<IO::Stream> stm);
			static Bool SendContent(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CString mime, UInt64 contLeng, const UInt8 *buff);
			static Bool SendContent(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CString mime, Text::CString cont);
			static Bool ResponseFile(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN fileName, OSInt cacheAge);

			static void SetCompLevel(Data::Compress::DeflateStream::CompLevel compLevel);
			static Bool AllowGZip(Net::BrowserInfo::BrowserType browser, Manage::OSInfo::OSType os);
			static Bool AllowDeflate(Net::BrowserInfo::BrowserType browser, Manage::OSInfo::OSType os);
			static Bool AllowKA(Net::BrowserInfo::BrowserType browser);
		};
	}
}
#endif
