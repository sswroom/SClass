#ifndef _SM_NET_ETHERNETWEBHANDLER
#define _SM_NET_ETHERNETWEBHANDLER
#include "Data/FastStringMap.hpp"
#include "Net/EthernetAnalyzer.h"
#include "Net/WebServer/WebStandardHandler.h"

namespace Net
{
	class EthernetWebHandler : public Net::WebServer::WebStandardHandler
	{
	private:
		typedef Bool (CALLBACKFUNC RequestHandler)(NN<EthernetWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);

	private:
		NN<Net::EthernetAnalyzer> analyzer;
		Data::FastStringMap<RequestHandler> reqMap;

		static void AppendHeader(NN<Text::StringBuilderUTF8> sbOut);
		void AppendMenu(NN<Text::StringBuilderUTF8> sbOut);
		static void AppendFooter(NN<Text::StringBuilderUTF8> sbOut);

		static Bool __stdcall DeviceReq(NN<EthernetWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
		static Bool __stdcall IPTransferReq(NN<EthernetWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
		static Bool __stdcall DNSReqv4Req(NN<EthernetWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
		static Bool __stdcall DNSReqv6Req(NN<EthernetWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
		static Bool __stdcall DNSReqOthReq(NN<EthernetWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
		static Bool __stdcall DNSTargetReq(NN<EthernetWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
		static Bool __stdcall DNSClientReq(NN<EthernetWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
		static Bool __stdcall DHCPReq(NN<EthernetWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
		static Bool __stdcall IPLogReq(NN<EthernetWebHandler> me, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);

	private:
		virtual Bool ProcessRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq);

	public:
		EthernetWebHandler(NN<Net::EthernetAnalyzer> analyzer);
		virtual ~EthernetWebHandler();
	};
}
#endif
