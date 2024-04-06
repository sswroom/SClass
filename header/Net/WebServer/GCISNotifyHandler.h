#ifndef _SM_NET_WEBSERVER_GCISNOTIFYHANDLER
#define _SM_NET_WEBSERVER_GCISNOTIFYHANDLER
#include "AnyType.h" 
#include "IO/LogTool.h"
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
			typedef void (__stdcall *MailHandler)(AnyType userObj, NotNullPtr<Net::NetConnection> cli, NotNullPtr<const Text::MIMEObj::MailMessage> msg);
		private:
			MailHandler hdlr;
			AnyType hdlrObj;
			NotNullPtr<IO::LogTool> log;

			static Bool __stdcall NotifyFunc(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NotNullPtr<WebServiceHandler> me);
			static Bool __stdcall BatchUplFunc(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NotNullPtr<WebServiceHandler> me);

		public:
			GCISNotifyHandler(Text::CStringNN notifyPath, Text::CStringNN batchUplPath, MailHandler hdlr, AnyType userObj, NotNullPtr<IO::LogTool> log);
			virtual ~GCISNotifyHandler();
		};
	}
}
#endif
