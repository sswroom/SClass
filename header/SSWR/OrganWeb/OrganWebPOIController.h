#ifndef _SM_SSWR_ORGANWEB_ORGANWEBPOICONTROLLER
#define _SM_SSWR_ORGANWEB_ORGANWEBPOICONTROLLER
#include "SSWR/OrganWeb/OrganWebController.h"
#include "Text/JSONBuilder.h"

namespace SSWR
{
	namespace OrganWeb
	{
		class OrganWebPOIController : public OrganWebController
		{
		private:
			static Bool __stdcall SvcPhotoUpload(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcPublicPOI(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcGroupPOI(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcSpeciesPOI(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcDayPOI(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);

			void AddGroupPOI(NotNullPtr<Sync::RWMutexUsage> mutUsage, NotNullPtr<Text::JSONBuilder> json, NotNullPtr<GroupInfo> group, Int32 userId, NotNullPtr<Data::ArrayListNN<GroupInfo>> groups, NotNullPtr<Data::ArrayListNN<SpeciesInfo>> speciesList);
			void AddSpeciesPOI(NotNullPtr<Sync::RWMutexUsage> mutUsage, NotNullPtr<Text::JSONBuilder> json, NotNullPtr<SpeciesInfo> species, Int32 userId, Bool publicGroup);
			void AddUserfilePOI(NotNullPtr<Text::JSONBuilder> json, NotNullPtr<SpeciesInfo> species, NotNullPtr<UserFileInfo> file);
			void AddGroups(NotNullPtr<Text::JSONBuilder> json, NotNullPtr<Data::ArrayListNN<GroupInfo>> groups);
			void AddSpeciesList(NotNullPtr<Text::JSONBuilder> json, NotNullPtr<Data::ArrayListNN<SpeciesInfo>> speciesList);
			Bool ResponseJSON(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN json);
		public:
			OrganWebPOIController(Net::WebServer::MemoryWebSessionManager *sessMgr, OrganWebEnv *env, UInt32 scnSize);
			virtual ~OrganWebPOIController();
		};
	}
}
#endif
