#ifndef _SM_NET_WEBSERVER_HTTPFORWARDHANDLER
#define _SM_NET_WEBSERVER_HTTPFORWARDHANDLER
#include "Data/ArrayListNN.h"
#include "IO/LogTool.h"
#include "Net/SSLEngine.h"
#include "Net/WebServer/WebStandardHandler.h"
#include "Text/String.h"

namespace Net
{
	namespace WebServer
	{
		class HTTPForwardHandler : public Net::WebServer::WebStandardHandler
		{
		public:
			enum class ForwardType
			{
				Normal,
				Transparent
			};

			typedef void (__stdcall *ReqHandler)(void *userObj, NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp);
		private:
			Data::ArrayListNN<Text::String> forwardAddrs;
			Data::ArrayListNN<Text::String> injHeaders;
			UOSInt nextURL;
			Sync::Mutex mut;
			NotNullPtr<Net::SocketFactory> sockf;
			Net::SSLEngine *ssl;
			ForwardType fwdType;
			ReqHandler reqHdlr;
			void *reqHdlrObj;
			IO::LogTool *log;
			Bool logContent;

			virtual Text::String *GetNextURL(NotNullPtr<Net::WebServer::IWebRequest> req);
		public:
			HTTPForwardHandler(NotNullPtr<Net::SocketFactory> sockf, Net::SSLEngine *ssl, Text::CString forwardURL, ForwardType fwdType);
			virtual ~HTTPForwardHandler();

			virtual Bool ProcessRequest(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq);

			void AddForwardURL(Text::CString url);
			void AddInjectHeader(NotNullPtr<Text::String> header);
			void AddInjectHeader(Text::CString header);
			void SetLog(IO::LogTool *log, Bool logContent);

			void HandleForwardRequest(ReqHandler reqHdlr, void *userObj);
		};
	}
}
#endif
