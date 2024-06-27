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
			typedef Bool (CALLBACKFUNC RequestHandler)(NN<SSWR::Benchmark::BenchmarkWebHandler> me, NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp);
		private:
			Data::FastStringMap<RequestHandler> reqMap;

			static Bool __stdcall UploadReq(NN<SSWR::Benchmark::BenchmarkWebHandler> me, NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp);
			static Bool __stdcall CPUInfoReq(NN<SSWR::Benchmark::BenchmarkWebHandler> me, NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp);

			virtual Bool ProcessRequest(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq);

		public:
			BenchmarkWebHandler();
			virtual ~BenchmarkWebHandler();
		};
	}
}

#endif
