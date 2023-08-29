#ifndef _SM_SSWR_ORGANWEB_ORGANWEBPOICONTROLLER
#define _SM_SSWR_ORGANWEB_ORGANWEBPOICONTROLLER
#include "SSWR/OrganWeb/OrganWebController.h"

namespace SSWR
{
	namespace OrganWeb
	{
		class OrganWebPOIController : public OrganWebController
		{
		private:
			static Bool __stdcall SvcPublicPOI(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcGroupPOI(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcSpeciesPOI(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcDayPOI(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CString subReq, Net::WebServer::WebController *parent);

			void AddGroupPOI(NotNullPtr<Sync::RWMutexUsage> mutUsage, NotNullPtr<Text::StringBuilderUTF8> sb, GroupInfo *group, Int32 userId);
			void AddSpeciesPOI(NotNullPtr<Sync::RWMutexUsage> mutUsage, NotNullPtr<Text::StringBuilderUTF8> sb, SpeciesInfo *species, Int32 userId, Bool publicGroup);
			void AddUserfilePOI(NotNullPtr<Text::StringBuilderUTF8> sb, SpeciesInfo *species, UserFileInfo *file);

		public:
			OrganWebPOIController(Net::WebServer::MemoryWebSessionManager *sessMgr, OrganWebEnv *env, UInt32 scnSize);
			virtual ~OrganWebPOIController();
		};
	}
}
#endif
