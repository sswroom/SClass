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

		protected:
			virtual ~EGaugeHandler();
		public:
			virtual Bool ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq);

			void HandleEGaugeData(DataHandler dataHdlr, void *userObj);
		};
	}
}
#endif
