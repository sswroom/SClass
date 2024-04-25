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

		static Bool __stdcall SendHandler(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> me);
		static Bool __stdcall SendBatchHandler(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> me);
		static Bool __stdcall SubscribeHandler(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> me);
		static Bool __stdcall UnsubscribeHandler(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> me);
		static Bool __stdcall UsersHandler(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> me);
		static Bool __stdcall ListDevicesHandler(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> me);

		void ParseJSONSend(Text::JSONBase *sendJson);
	public:
		PushServerHandler(NN<PushManager> mgr);
		virtual ~PushServerHandler();
	};
}
#endif
