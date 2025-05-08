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
			typedef void (CALLBACKFUNC SAMLStrFunc)(AnyType userObj, Text::CStringNN msg);
			typedef Bool (CALLBACKFUNC SAMLLoginFunc)(AnyType userObj, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, NN<const SAMLMessage> msg);
		private:
			NN<SAMLHandler> hdlr;
			SAMLStrFunc rawRespHdlr;
			AnyType rawRespObj;
			SAMLLoginFunc loginHdlr;
			AnyType loginObj;

			static Bool __stdcall GetLoginFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);
			static Bool __stdcall GetLogoutFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);
			static Bool __stdcall PostSSOFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);
			static Bool __stdcall GetMetadataFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);
		public:
			SAMLService(NN<SAMLHandler> hdlr);
			virtual ~SAMLService();

			void HandleRAWSAMLResponse(SAMLStrFunc hdlr, AnyType userObj);
			void HandleLoginRequest(SAMLLoginFunc hdlr, AnyType userObj);
		};
	}
}
#endif
