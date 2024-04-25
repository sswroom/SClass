#ifndef _SM_SSWR_ORGANWEB_ORGANWEBBOOKCONTROLLER
#define _SM_SSWR_ORGANWEB_ORGANWEBBOOKCONTROLLER
#include "Data/Comparator.h"
#include "SSWR/OrganWeb/OrganWebController.h"

namespace SSWR
{
	namespace OrganWeb
	{
		class OrganWebBookController : public OrganWebController
		{
		private:
			static Bool __stdcall SvcBookView(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);
			static Bool __stdcall SvcBookPhoto(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);
			static Bool __stdcall SvcBookAdd(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);

		public:
			OrganWebBookController(Net::WebServer::MemoryWebSessionManager *sessMgr, OrganWebEnv *env, UInt32 scnSize);
			virtual ~OrganWebBookController();
		};
	}
}
#endif
