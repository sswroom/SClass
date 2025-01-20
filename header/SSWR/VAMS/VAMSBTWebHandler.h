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
			NN<Text::String> logPath;
			VAMSBTList *btList;

			static Bool __stdcall DevData(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> me);
			static Bool __stdcall KAData(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> me);
			static Bool __stdcall LogData(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> me);
			static Bool __stdcall ListData(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> me);
			static Bool __stdcall ListItem(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> me);

		public:
			VAMSBTWebHandler(NN<Text::String> logPath, VAMSBTList *btList);
			virtual ~VAMSBTWebHandler();
		};
	}
}

#endif
