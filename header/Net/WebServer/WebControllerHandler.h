#ifndef _SM_NET_WEBSERVER_WEBCONTROLLERHANDLER
#define _SM_NET_WEBSERVER_WEBCONTROLLERHANDLER
#include "Data/ArrayList.h"
#include "Net/WebServer/HTTPDirectoryHandler.h"
#include "Net/WebServer/WebController.h"

namespace Net
{
	namespace WebServer
	{
		class WebControllerHandler : public Net::WebServer::HTTPDirectoryHandler
		{
		private:
			Data::ArrayList<WebController *> ctrlList;

		protected:
			virtual ~WebControllerHandler();
			virtual Bool ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq);
		public:
			WebControllerHandler();
			WebControllerHandler(Text::CString rootDir);

			void AddController(WebController *ctrl);
		};
	}
}
#endif