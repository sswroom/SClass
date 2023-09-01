#ifndef _SM_SSWR_ORGANWEB_ORGANWEBBOOKCONTROLLER
#define _SM_SSWR_ORGANWEB_ORGANWEBBOOKCONTROLLER
#include "Data/Comparator.h"
#include "SSWR/OrganWeb/OrganWebController.h"

namespace SSWR
{
	namespace OrganWeb
	{
		class OrganWebBookController : public OrganWebController, public Data::Comparator<BookInfo*>
		{
		private:
			static Bool __stdcall SvcBookList(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcBook(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcBookView(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcBookPhoto(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcBookAdd(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);

		public:
			OrganWebBookController(Net::WebServer::MemoryWebSessionManager *sessMgr, OrganWebEnv *env, UInt32 scnSize);
			virtual ~OrganWebBookController();
			
			virtual OSInt Compare(BookInfo *a, BookInfo *b) const;
		};
	}
}
#endif
