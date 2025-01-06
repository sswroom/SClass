#ifndef _SM_SSWR_ORGANWEB_ORGANWEBMAINCONTROLLER
#define _SM_SSWR_ORGANWEB_ORGANWEBMAINCONTROLLER
#include "SSWR/OrganWeb/OrganWebController.h"

namespace SSWR
{
	namespace OrganWeb
	{
		class OrganWebMainController : public OrganWebController
		{
		private:
			static Bool __stdcall SvcGroup(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);
			static Bool __stdcall SvcGroupMod(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);
			static Bool __stdcall SvcSpecies(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);
			static Bool __stdcall SvcSpeciesMod(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);
			static Bool __stdcall SvcList(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);
			static Bool __stdcall SvcPhotoDetail(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);
			static Bool __stdcall SvcSearchInside(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);
			static Bool __stdcall SvcSearchInsideMoreS(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);
			static Bool __stdcall SvcSearchInsideMoreG(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);
			static Bool __stdcall SvcLogout(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);

		public:
			OrganWebMainController(NN<Net::WebServer::MemoryWebSessionManager> sessMgr, NN<OrganWebEnv> env, UInt32 scnSize);
			virtual ~OrganWebMainController();
		};
	}
}
#endif
