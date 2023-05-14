#ifndef _SM_SSWR_ORGANWEB_ORGANWEBBOOKCONTROLLER
#define _SM_SSWR_ORGANWEB_ORGANWEBBOOKCONTROLLER
#include "SSWR/OrganWeb/OrganWebController.h"

namespace SSWR
{
	namespace OrganWeb
	{
		class OrganWebBookController : public OrganWebController
		{
		private:
			static Bool __stdcall SvcBookList(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcBook(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcBookView(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent);

		public:
			OrganWebBookController(Net::WebServer::MemoryWebSessionManager *sessMgr, OrganWebEnv *env, UInt32 scnSize);
			virtual ~OrganWebBookController();
		};
	}
}
#endif
