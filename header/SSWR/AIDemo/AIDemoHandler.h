#ifndef _SM_SSWR_AIDEMO_AIDEMOHANDLER
#define _SM_SSWR_AIDEMO_AIDEMOHANDLER
#include "Net/WebServer/WebServiceHandler.h"

namespace SSWR
{
	namespace AIDemo
	{
		class AIDemoCore;
		class AIDemoHandler : public Net::WebServer::WebServiceHandler
		{
		private:
			NN<IO::LogTool> log;
			NN<AIDemoCore> core;

			static Bool __stdcall BotFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);

		public:
			AIDemoHandler(Text::CStringNN rootDir, NN<IO::LogTool> log, NN<AIDemoCore> core);
			virtual ~AIDemoHandler();
		};
	}
}
#endif
