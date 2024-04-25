#ifndef _SM_NET_WEBSERVER_HTTPFORWARDHANDLER
#define _SM_NET_WEBSERVER_HTTPFORWARDHANDLER
#include "Data/ArrayListStringNN.h"
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

			typedef void (__stdcall *ReqHandler)(void *userObj, NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp);
		private:
			Data::ArrayListStringNN forwardAddrs;
			Data::ArrayListStringNN injHeaders;
			UOSInt nextURL;
			Sync::Mutex mut;
			NN<Net::SocketFactory> sockf;
			Optional<Net::SSLEngine> ssl;
			Optional<Text::String> forceHost;
			ForwardType fwdType;
			ReqHandler reqHdlr;
			void *reqHdlrObj;
			IO::LogTool *log;
			Bool logContent;

			virtual Optional<Text::String> GetNextURL(NN<Net::WebServer::IWebRequest> req);
		public:
			HTTPForwardHandler(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Text::CStringNN forwardURL, ForwardType fwdType);
			virtual ~HTTPForwardHandler();

			virtual Bool ProcessRequest(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq);

			void AddForwardURL(Text::CString url);
			void AddInjectHeader(NN<Text::String> header);
			void AddInjectHeader(Text::CString header);
			void SetLog(IO::LogTool *log, Bool logContent);
			void SetForceHost(Text::CString forceHost);

			void HandleForwardRequest(ReqHandler reqHdlr, void *userObj);
		};
	}
}
#endif
