#ifndef _SM_SSWR_SDNSPROXY_SDNSPROXYWEBHANDLER
#define _SM_SSWR_SDNSPROXY_SDNSPROXYWEBHANDLER
#include "IO/CyclicLogBuffer.h"
#include "IO/LogTool.h"
#include "Net/DNSProxy.h"
#include "Net/WebServer/WebStandardHandler.h"
#include "Sync/Mutex.h"
#include "Text/StringBuilderUTF8.h"

namespace SSWR
{
	namespace SDNSProxy
	{
		class SDNSProxyCore;

		class SDNSProxyWebHandler : public Net::WebServer::WebStandardHandler
		{
		private:
			typedef Bool (__stdcall *RequestHandler)(SSWR::SDNSProxy::SDNSProxyWebHandler *me, NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp);
		private:
			Net::DNSProxy *proxy;
			IO::LogTool *log;
			Data::FastStringMap<RequestHandler> *reqMap;
			NotNullPtr<IO::CyclicLogBuffer> logBuff;
			SDNSProxyCore *core;

			static Bool __stdcall StatusReq(SSWR::SDNSProxy::SDNSProxyWebHandler *me, NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp);
			static Bool __stdcall ReqV4Req(SSWR::SDNSProxy::SDNSProxyWebHandler *me, NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp);
			static Bool __stdcall ReqV6Req(SSWR::SDNSProxy::SDNSProxyWebHandler *me, NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp);
			static Bool __stdcall ReqOthReq(SSWR::SDNSProxy::SDNSProxyWebHandler *me, NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp);
			static Bool __stdcall TargetReq(SSWR::SDNSProxy::SDNSProxyWebHandler *me, NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp);
			static Bool __stdcall BlacklistReq(SSWR::SDNSProxy::SDNSProxyWebHandler *me, NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp);
			static Bool __stdcall LogReq(SSWR::SDNSProxy::SDNSProxyWebHandler *me, NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp);
			static Bool __stdcall ClientReq(SSWR::SDNSProxy::SDNSProxyWebHandler *me, NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp);
			static Bool __stdcall ReqPerMinReq(SSWR::SDNSProxy::SDNSProxyWebHandler *me, NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp);

			static void AppendHeader(NotNullPtr<Text::StringBuilderUTF8> sbOut);
			static void AppendMenu(NotNullPtr<Text::StringBuilderUTF8> sbOut);
			static void AppendFooter(NotNullPtr<Text::StringBuilderUTF8> sbOut);

			virtual Bool ProcessRequest(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq);

		public:
			SDNSProxyWebHandler(Net::DNSProxy *proxy, IO::LogTool *log, SDNSProxyCore *core);
			virtual ~SDNSProxyWebHandler();

			virtual void Release();

		};
	}
}

#endif
