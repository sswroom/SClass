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
		PushManager *mgr;

		static Bool __stdcall SendHandler(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, WebServiceHandler *me);
		static Bool __stdcall SendBatchHandler(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, WebServiceHandler *me);
		static Bool __stdcall SubscribeHandler(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, WebServiceHandler *me);
		static Bool __stdcall UnsubscribeHandler(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, WebServiceHandler *me);
		static Bool __stdcall UsersHandler(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, WebServiceHandler *me);
		static Bool __stdcall ListDevicesHandler(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, WebServiceHandler *me);

		void ParseJSONSend(Text::JSONBase *sendJson);
	public:
		PushServerHandler(PushManager *mgr);
		virtual ~PushServerHandler();
	};
}
#endif
