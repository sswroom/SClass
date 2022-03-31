#ifndef _SM_SSWR_VAMS_VAMSBTWEBHANDLER
#define _SM_SSWR_VAMS_VAMSBTWEBHANDLER
#include "Net/WebServer/WebServiceHandler.h"
#include "SSWR/VAMS/VAMSBTList.h"
#include "Text/String.h"

namespace SSWR
{
	namespace VAMS
	{
		class VAMSBTWebHandler : public Net::WebServer::WebServiceHandler
		{
		private:
			Text::String *logPath;
			VAMSBTList *btList;

			static Bool __stdcall DevData(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, WebServiceHandler *me);
			static Bool __stdcall KAData(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, WebServiceHandler *me);
			static Bool __stdcall LogData(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, WebServiceHandler *me);
			static Bool __stdcall ListData(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, WebServiceHandler *me);

		public:
			VAMSBTWebHandler(Text::String *logPath, VAMSBTList *btList);
			virtual ~VAMSBTWebHandler();
		};
	}
}

#endif
