#ifndef _SM_SSWR_BENCHMARK_BENCHMARKWEBHANDLER
#define _SM_SSWR_BENCHMARK_BENCHMARKWEBHANDLER
#include "Data/FastStringMap.h"
#include "Net/WebServer/MemoryWebSessionManager.h"
#include "Net/WebServer/WebStandardHandler.h"

namespace SSWR
{
	namespace Benchmark
	{
		class BenchmarkWebHandler : public Net::WebServer::WebStandardHandler
		{
		private:
			typedef Bool (__stdcall *RequestHandler)(SSWR::Benchmark::BenchmarkWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp);
		private:
			Data::FastStringMap<RequestHandler> *reqMap;

			static Bool __stdcall UploadReq(SSWR::Benchmark::BenchmarkWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp);
			static Bool __stdcall CPUInfoReq(SSWR::Benchmark::BenchmarkWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp);

			virtual Bool ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq);

		public:
			BenchmarkWebHandler();
			virtual ~BenchmarkWebHandler();
		};
	}
}

#endif
