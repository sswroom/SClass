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
			typedef void (CALLBACKFUNC MailHandler)(AnyType userObj, NN<Net::NetConnection> cli, NN<const Text::MIMEObj::MailMessage> msg);
		private:
			MailHandler hdlr;
			AnyType hdlrObj;
			NN<IO::LogTool> log;

			static Bool __stdcall NotifyFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> me);
			static Bool __stdcall BatchUplFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> me);

		public:
			GCISNotifyHandler(Text::CStringNN notifyPath, Text::CStringNN batchUplPath, MailHandler hdlr, AnyType userObj, NN<IO::LogTool> log);
			virtual ~GCISNotifyHandler();
		};
	}
}
#endif
