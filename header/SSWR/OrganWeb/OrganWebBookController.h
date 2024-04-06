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
			static Bool __stdcall SvcBookView(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NotNullPtr<Net::WebServer::WebController> parent);
			static Bool __stdcall SvcBookPhoto(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NotNullPtr<Net::WebServer::WebController> parent);
			static Bool __stdcall SvcBookAdd(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NotNullPtr<Net::WebServer::WebController> parent);

		public:
			OrganWebBookController(Net::WebServer::MemoryWebSessionManager *sessMgr, OrganWebEnv *env, UInt32 scnSize);
			virtual ~OrganWebBookController();
		};
	}
}
#endif
