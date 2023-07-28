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
			static Bool __stdcall SvcPublicPOI(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcGroupPOI(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcSpeciesPOI(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcDayPOI(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebController *parent);

			void AddGroupPOI(NotNullPtr<Sync::RWMutexUsage> mutUsage, Text::StringBuilderUTF8 *sb, GroupInfo *group, Int32 userId);
			void AddSpeciesPOI(NotNullPtr<Sync::RWMutexUsage> mutUsage, Text::StringBuilderUTF8 *sb, SpeciesInfo *species, Int32 userId, Bool publicGroup);
			void AddUserfilePOI(Text::StringBuilderUTF8 *sb, SpeciesInfo *species, UserFileInfo *file);

		public:
			OrganWebPOIController(Net::WebServer::MemoryWebSessionManager *sessMgr, OrganWebEnv *env, UInt32 scnSize);
			virtual ~OrganWebPOIController();
		};
	}
}
#endif
