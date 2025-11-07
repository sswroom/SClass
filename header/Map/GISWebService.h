#ifndef _SM_MAP_GISWEBSERVICE
#define _SM_MAP_GISWEBSERVICE
#include "Net/WebServer/WebResponse.h"

namespace Map
{
	class GISWebService
	{
	public:
		struct GISWorkspace
		{
			NN<Text::String> name;
			NN<Text::String> uri;
		};

		virtual ~GISWebService() {};

		virtual Bool ResponseException(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN exceptionCode, Text::CStringNN locator, Text::CStringNN exceptionText) = 0;
		virtual void AddRespHeaders(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp) = 0;
	};
}
#endif
