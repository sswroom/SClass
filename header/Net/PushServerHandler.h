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
		NotNullPtr<PushManager> mgr;

		static Bool __stdcall SendHandler(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, WebServiceHandler *me);
		static Bool __stdcall SendBatchHandler(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, WebServiceHandler *me);
		static Bool __stdcall SubscribeHandler(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, WebServiceHandler *me);
		static Bool __stdcall UnsubscribeHandler(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, WebServiceHandler *me);
		static Bool __stdcall UsersHandler(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, WebServiceHandler *me);
		static Bool __stdcall ListDevicesHandler(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, WebServiceHandler *me);

		void ParseJSONSend(Text::JSONBase *sendJson);
	public:
		PushServerHandler(NotNullPtr<PushManager> mgr);
		virtual ~PushServerHandler();
	};
}
#endif
