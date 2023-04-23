#ifndef _SM_NET_WEBSERVER_GCISNOTIFYHANDLER
#define _SM_NET_WEBSERVER_GCISNOTIFYHANDLER
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
			typedef void (__stdcall *MailHandler)(void *userObj, Net::NetConnection *cli, Text::MIMEObj::MailMessage *msg);
		private:
			MailHandler hdlr;
			void *hdlrObj;
			IO::LogTool *log;

			static Bool __stdcall NotifyFunc(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, WebServiceHandler *me);
			static Bool __stdcall BatchUplFunc(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, WebServiceHandler *me);

		public:
			GCISNotifyHandler(Text::CString notifyPath, Text::CString batchUplPath, MailHandler hdlr, void *userObj, IO::LogTool *log);
			virtual ~GCISNotifyHandler();
		};
	}
}
#endif
