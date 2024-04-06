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
			NotNullPtr<Text::String> logPath;
			VAMSBTList *btList;

			static Bool __stdcall DevData(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NotNullPtr<WebServiceHandler> me);
			static Bool __stdcall KAData(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NotNullPtr<WebServiceHandler> me);
			static Bool __stdcall LogData(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NotNullPtr<WebServiceHandler> me);
			static Bool __stdcall ListData(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NotNullPtr<WebServiceHandler> me);
			static Bool __stdcall ListItem(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NotNullPtr<WebServiceHandler> me);

		public:
			VAMSBTWebHandler(NotNullPtr<Text::String> logPath, VAMSBTList *btList);
			virtual ~VAMSBTWebHandler();
		};
	}
}

#endif
