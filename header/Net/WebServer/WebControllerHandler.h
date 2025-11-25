#ifndef _SM_NET_WEBSERVER_WEBCONTROLLERHANDLER
#define _SM_NET_WEBSERVER_WEBCONTROLLERHANDLER
#include "Data/ArrayListNN.hpp"
#include "Net/WebServer/HTTPDirectoryHandler.h"
#include "Net/WebServer/WebController.h"

namespace Net
{
	namespace WebServer
	{
		class WebControllerHandler : public Net::WebServer::HTTPDirectoryHandler
		{
		private:
			Data::ArrayListNN<WebController> ctrlList;

		protected:
			virtual Bool ProcessRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq);
		public:
			WebControllerHandler();
			WebControllerHandler(Text::CStringNN rootDir);
			virtual ~WebControllerHandler();

			void AddController(NN<WebController> ctrl);
		};
	}
}
#endif
