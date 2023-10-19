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
			static Bool __stdcall SvcLang(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcLoginInfo(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcCateList(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcYearList(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcBookList(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcBookSelect(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcBookUnselect(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcBookAdd(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcBookDetail(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcPhotoUpload(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcReload(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcPublicPOI(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcGroupPOI(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcSpeciesPOI(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcDayPOI(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);

			void AddGroupPOI(NotNullPtr<Sync::RWMutexUsage> mutUsage, NotNullPtr<Text::JSONBuilder> json, NotNullPtr<GroupInfo> group, Int32 userId, NotNullPtr<Data::ArrayListNN<GroupInfo>> groups, NotNullPtr<Data::ArrayListNN<SpeciesInfo>> speciesList);
			void AddSpeciesPOI(NotNullPtr<Sync::RWMutexUsage> mutUsage, NotNullPtr<Text::JSONBuilder> json, NotNullPtr<SpeciesInfo> species, Int32 userId, Bool publicGroup);
			void AddUserfilePOI(NotNullPtr<Text::JSONBuilder> json, NotNullPtr<SpeciesInfo> species, NotNullPtr<UserFileInfo> file);
			void AddGroups(NotNullPtr<Text::JSONBuilder> json, NotNullPtr<Data::ArrayListNN<GroupInfo>> groups);
			void AddSpeciesList(NotNullPtr<Text::JSONBuilder> json, NotNullPtr<Data::ArrayListNN<SpeciesInfo>> speciesList);
			static void AppendUser(NotNullPtr<Text::JSONBuilder> json, NotNullPtr<WebUserInfo> user);
			void AppendSpecies(NotNullPtr<Text::JSONBuilder> json, NotNullPtr<SpeciesInfo> species);
			static Bool ResponseJSON(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, OSInt cacheAge, Text::CStringNN json);
		public:
			OrganWebPOIController(Net::WebServer::MemoryWebSessionManager *sessMgr, OrganWebEnv *env, UInt32 scnSize);
			virtual ~OrganWebPOIController();
		};
	}
}
#endif
