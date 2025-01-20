#ifndef _SM_NET_PUSHSERVERHANDLER
#define _SM_NET_PUSHSERVERHANDLER
#include "Net/PushManager.h"
#include "Net/WebServer/WebServiceHandler.h"
#include "Text/JSON.h"

namespace Net
{
	class PushServerHandler : public Net::WebServer::WebServiceHandler
	{
	private:
		NN<PushManager> mgr;

		static Bool __stdcall SendHandler(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> me);
		static Bool __stdcall SendBatchHandler(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> me);
		static Bool __stdcall SubscribeHandler(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> me);
		static Bool __stdcall UnsubscribeHandler(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> me);
		static Bool __stdcall UsersHandler(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> me);
		static Bool __stdcall ListDevicesHandler(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> me);

		void ParseJSONSend(NN<Text::JSONBase> sendJson);
	public:
		PushServerHandler(NN<PushManager> mgr);
		virtual ~PushServerHandler();
	};
}
#endif
