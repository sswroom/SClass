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
			static Bool __stdcall SvcPhotoDown(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcGroup(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcGroupMod(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcSpecies(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcSpeciesMod(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcList(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcPhotoDetail(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcPhotoDetailD(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcPhotoYear(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcPhotoDay(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcPhotoUpload(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcPhotoUpload2(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcPhotoUploadD(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcSearchInside(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcSearchInsideMoreS(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcSearchInsideMoreG(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcLogin(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcLogout(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcReload(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcRestart(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcIndex(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcCate(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcFavicon(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CString subReq, Net::WebServer::WebController *parent);

		public:
			OrganWebMainController(Net::WebServer::MemoryWebSessionManager *sessMgr, OrganWebEnv *env, UInt32 scnSize);
			virtual ~OrganWebMainController();
		};
	}
}
#endif
