#ifndef _SM_SSWR_ORGANWEB_ORGANWEBBOOKCONTROLLER
#define _SM_SSWR_ORGANWEB_ORGANWEBBOOKCONTROLLER
#include "Data/Comparator.hpp"
#include "SSWR/OrganWeb/OrganWebController.h"

namespace SSWR
{
	namespace OrganWeb
	{
		class OrganWebBookController : public OrganWebController
		{
		private:
			static Bool __stdcall SvcBookView(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);
			static Bool __stdcall SvcBookPhoto(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);
			static Bool __stdcall SvcBookAdd(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);

		public:
			OrganWebBookController(NN<Net::WebServer::MemoryWebSessionManager> sessMgr, NN<OrganWebEnv> env, UInt32 scnSize);
			virtual ~OrganWebBookController();
		};
	}
}
#endif
