#ifndef _SM_NET_WEBSERVER_EGAUGEHANDLER
#define _SM_NET_WEBSERVER_EGAUGEHANDLER
#include "Parser/ParserList.h"
#include "Net/WebServer/WebStandardHandler.h"
#include "Sync/Mutex.h"

namespace Net
{
	namespace WebServer
	{
		class EGaugeHandler : public Net::WebServer::WebStandardHandler
		{
		public:
			typedef void (__stdcall *DataHandler)(void *userObj, const UInt8 *data, UOSInt dataSize);
		private:
			void *dataHdlrObj;
			DataHandler dataHdlr;
		public:
			EGaugeHandler();
			virtual ~EGaugeHandler();

			virtual Bool ProcessRequest(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq);

			void HandleEGaugeData(DataHandler dataHdlr, void *userObj);
		};
	}
}
#endif
