#ifndef _SM_NET_WEBSERVER_GCISNOTIFYHANDLER
#define _SM_NET_WEBSERVER_GCISNOTIFYHANDLER
#include "Net/NetConnection.h"
#include "Net/WebServer/WebServiceHandler.h"
#include "Text/MIMEObj/MailMessage.h"

namespace Net
{
	namespace WebServer
	{
		class GCISNotifyHandler : public WebServiceHandler
		{
		public:
			typedef void (__stdcall *MailHandler)(void *userObj, Net::NetConnection *cli, Text::MIMEObj::MailMessage *msg);
		private:
			MailHandler hdlr;
			void *hdlrObj;

			static Bool __stdcall NotifyFunc(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, WebServiceHandler *me);

		public:
			GCISNotifyHandler(Text::CString notifyPath, MailHandler hdlr, void *userObj);
			virtual ~GCISNotifyHandler();
		};
	}
}
#endif
