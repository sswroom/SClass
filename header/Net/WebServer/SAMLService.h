#ifndef _SM_NET_WEBSERVER_SAMLSERVICE
#define _SM_NET_WEBSERVER_SAMLSERVICE
#include "Net/WebServer/SAMLHandler.h"
#include "Net/WebServer/WebServiceHandler.h"

namespace Net
{
	namespace WebServer
	{
		class SAMLService : public Net::WebServer::WebServiceHandler
		{
		public:
			typedef void (CALLBACKFUNC SAMLRespFunc)(AnyType userObj, NN<SAMLSSOResponse> resp);
		private:
			NN<SAMLHandler> hdlr;
			SAMLRespFunc respHdlr;
			AnyType respObj;
			Text::EncodingFactory encFact;

			static Bool __stdcall GetLoginFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);
			static Bool __stdcall GetLogoutFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);
			static Bool __stdcall PostSSOFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);
			static Bool __stdcall GetMetadataFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);
		public:
			SAMLService(NN<SAMLHandler> hdlr);
			virtual ~SAMLService();

			void HandleSAMLResponse(SAMLRespFunc hdlr, AnyType userObj);
		};
	}
}
#endif
