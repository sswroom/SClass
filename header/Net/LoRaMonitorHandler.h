#ifndef _SM_NET_LORAMONITORHANDLER
#define _SM_NET_LORAMONITORHANDLER
#include "Net/WebServer/WebServiceHandler.h"

namespace Net
{
	class LoRaMonitorCore;
	class LoRaMonitorHandler : public Net::WebServer::WebServiceHandler
	{
	private:
		NN<LoRaMonitorCore> core;

		static Bool __stdcall GetGatewayFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);
		static Bool __stdcall GetGatewayPacketFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);
		static Bool __stdcall UpdateGatewayFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);
		static void AppendGW(NN<LoRaMonitorCore::GWInfo> gw, NN<Text::JSONBuilder> json);
		static void AppendPacket(NN<LoRaMonitorCore::DataPacket> pkt, NN<Text::JSONBuilder> json);
	public:
		LoRaMonitorHandler(NN<LoRaMonitorCore> core, Text::CStringNN webRoot);
		virtual ~LoRaMonitorHandler();
	};
}
#endif