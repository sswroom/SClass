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

		static Bool __stdcall GetGatewaysFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);
		static void AppendGW(NN<LoRaMonitorCore::GWInfo> gw, NN<Text::JSONBuilder> json);
	public:
		LoRaMonitorHandler(NN<LoRaMonitorCore> core);
		virtual ~LoRaMonitorHandler();
	};
}
#endif