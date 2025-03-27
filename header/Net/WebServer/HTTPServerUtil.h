#ifndef _SM_NET_WEBSERVER_HTTPSERVERUTIL
#define _SM_NET_WEBSERVER_HTTPSERVERUTIL
#include "Data/Compress/Deflater.h"
#include "IO/Stream.h"
#include "Net/WebServer/WebResponse.h"
#include "Text/CString.h"

namespace Net
{
	namespace WebServer
	{
		class HTTPServerUtil
		{
		private:
			static Data::Compress::Deflater::CompLevel compLevel;
		public:
			static Bool MIMEToCompress(Text::CStringNN mime);
			static Bool SendContent(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN mime, UInt64 contLeng, NN<IO::Stream> stm);
			static Bool SendContent(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN mime, UInt64 contLeng, UnsafeArray<const UInt8> buff);
			static Bool SendContent(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN mime, Text::CStringNN cont);
			static Bool ResponseFile(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN fileName, OSInt cacheAge);

			static void SetCompLevel(Data::Compress::Deflater::CompLevel compLevel);
			static Bool AllowGZip(Net::BrowserInfo::BrowserType browser, Manage::OSInfo::OSType os);
			static Bool AllowDeflate(Net::BrowserInfo::BrowserType browser, Manage::OSInfo::OSType os);
			static Bool AllowKA(Net::BrowserInfo::BrowserType browser);
		};
	}
}
#endif
