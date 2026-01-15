#ifndef _SM_NET_WEBSERVER_EGAUGEHANDLER
#define _SM_NET_WEBSERVER_EGAUGEHANDLER
#include "AnyType.h"
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
			typedef void (CALLBACKFUNC DataHandler)(AnyType userObj, UnsafeArray<const UInt8> data, UIntOS dataSize);
		private:
			AnyType dataHdlrObj;
			DataHandler dataHdlr;
		public:
			EGaugeHandler();
			virtual ~EGaugeHandler();

			virtual Bool ProcessRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq);

			void HandleEGaugeData(DataHandler dataHdlr, AnyType userObj);
		};
	}
}
#endif
