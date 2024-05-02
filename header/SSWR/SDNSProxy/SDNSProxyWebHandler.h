#ifndef _SM_SSWR_SDNSPROXY_SDNSPROXYWEBHANDLER
#define _SM_SSWR_SDNSPROXY_SDNSPROXYWEBHANDLER
#include "Data/FastStringMap.h"
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
			typedef Bool (__stdcall *RequestHandler)(SSWR::SDNSProxy::SDNSProxyWebHandler *me, NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp);
		private:
			NN<Net::DNSProxy> proxy;
			NN<IO::LogTool> log;
			Data::FastStringMap<RequestHandler> reqMap;
			NN<IO::CyclicLogBuffer> logBuff;
			SDNSProxyCore *core;

			static Bool __stdcall StatusReq(SSWR::SDNSProxy::SDNSProxyWebHandler *me, NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp);
			static Bool __stdcall ReqV4Req(SSWR::SDNSProxy::SDNSProxyWebHandler *me, NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp);
			static Bool __stdcall ReqV6Req(SSWR::SDNSProxy::SDNSProxyWebHandler *me, NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp);
			static Bool __stdcall ReqOthReq(SSWR::SDNSProxy::SDNSProxyWebHandler *me, NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp);
			static Bool __stdcall TargetReq(SSWR::SDNSProxy::SDNSProxyWebHandler *me, NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp);
			static Bool __stdcall BlacklistReq(SSWR::SDNSProxy::SDNSProxyWebHandler *me, NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp);
			static Bool __stdcall LogReq(SSWR::SDNSProxy::SDNSProxyWebHandler *me, NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp);
			static Bool __stdcall ClientReq(SSWR::SDNSProxy::SDNSProxyWebHandler *me, NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp);
			static Bool __stdcall ReqPerMinReq(SSWR::SDNSProxy::SDNSProxyWebHandler *me, NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp);

			static void AppendHeader(NN<Text::StringBuilderUTF8> sbOut);
			static void AppendMenu(NN<Text::StringBuilderUTF8> sbOut);
			static void AppendFooter(NN<Text::StringBuilderUTF8> sbOut);

			virtual Bool ProcessRequest(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq);

		public:
			SDNSProxyWebHandler(NN<Net::DNSProxy> proxy, NN<IO::LogTool> log, SDNSProxyCore *core);
			virtual ~SDNSProxyWebHandler();

			virtual void Release();

		};
	}
}

#endif
