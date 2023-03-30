#ifndef _SM_NET_WEBSERVER_GCISNOTIFYHANDLER
#define _SM_NET_WEBSERVER_GCISNOTIFYHANDLER
#include "Net/WebServer/WebServiceHandler.h"

namespace Net
{
	namespace WebServer
	{
		class GCISNotifyHandler : public WebServiceHandler
		{
		private:
			static Bool __stdcall NotifyFunc(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, WebServiceHandler *me);

		public:
			GCISNotifyHandler(Text::CString notifyPath);
			virtual ~GCISNotifyHandler();
		};
	}
}
#endif
