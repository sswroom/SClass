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
			typedef Bool (CALLBACKFUNC RequestHandler)(NN<SSWR::SDNSProxy::SDNSProxyWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
		private:
			NN<Net::DNSProxy> proxy;
			NN<IO::LogTool> log;
			Data::FastStringMap<RequestHandler> reqMap;
			NN<IO::CyclicLogBuffer> logBuff;
			SDNSProxyCore *core;

			static Bool __stdcall StatusReq(NN<SSWR::SDNSProxy::SDNSProxyWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
			static Bool __stdcall ReqV4Req(NN<SSWR::SDNSProxy::SDNSProxyWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
			static Bool __stdcall ReqV6Req(NN<SSWR::SDNSProxy::SDNSProxyWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
			static Bool __stdcall ReqOthReq(NN<SSWR::SDNSProxy::SDNSProxyWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
			static Bool __stdcall TargetReq(NN<SSWR::SDNSProxy::SDNSProxyWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
			static Bool __stdcall BlacklistReq(NN<SSWR::SDNSProxy::SDNSProxyWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
			static Bool __stdcall LogReq(NN<SSWR::SDNSProxy::SDNSProxyWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
			static Bool __stdcall ClientReq(NN<SSWR::SDNSProxy::SDNSProxyWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
			static Bool __stdcall ReqPerMinReq(NN<SSWR::SDNSProxy::SDNSProxyWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);

			static void AppendHeader(NN<Text::StringBuilderUTF8> sbOut);
			static void AppendMenu(NN<Text::StringBuilderUTF8> sbOut);
			static void AppendFooter(NN<Text::StringBuilderUTF8> sbOut);

			virtual Bool ProcessRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq);

		public:
			SDNSProxyWebHandler(NN<Net::DNSProxy> proxy, NN<IO::LogTool> log, SDNSProxyCore *core);
			virtual ~SDNSProxyWebHandler();

			virtual void Release();

		};
	}
}

#endif
