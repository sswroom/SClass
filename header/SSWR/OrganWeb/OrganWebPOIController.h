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
			static Bool __stdcall SvcLogin(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcLogout(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcCateList(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcCate(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcYearList(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcDayList(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcDayDetail(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcBookList(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcBookSelect(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcBookUnselect(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcBookAdd(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcBookDetail(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcPhotoDetail(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcPhotoUpload(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcPhotoName(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcUnfinPeak(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcReload(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcPublicPOI(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcGroupPOI(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcSpeciesPOI(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);
			static Bool __stdcall SvcDayPOI(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, Net::WebServer::WebController *parent);

			void AddGroupPOI(NotNullPtr<Sync::RWMutexUsage> mutUsage, NotNullPtr<Text::JSONBuilder> json, NotNullPtr<GroupInfo> group, Int32 userId, NotNullPtr<Data::ArrayListNN<GroupInfo>> groups, NotNullPtr<Data::ArrayListNN<SpeciesInfo>> speciesList);
			void AddSpeciesPOI(NotNullPtr<Sync::RWMutexUsage> mutUsage, NotNullPtr<Text::JSONBuilder> json, NotNullPtr<SpeciesInfo> species, Int32 userId, Bool publicGroup);
			void AddUserfilePOI(NotNullPtr<Text::JSONBuilder> json, NotNullPtr<SpeciesInfo> species, NotNullPtr<UserFileInfo> file);
			void AddGroups(NotNullPtr<Text::JSONBuilder> json, NotNullPtr<Data::ArrayListNN<GroupInfo>> groups);
			void AddGroup(NotNullPtr<Text::JSONBuilder> json, NotNullPtr<GroupInfo> group);
			void AddSpeciesList(NotNullPtr<Text::JSONBuilder> json, NotNullPtr<Data::ArrayListNN<SpeciesInfo>> speciesList, NotNullPtr<Sync::RWMutexUsage> mutUsage);
			static void AppendUser(NotNullPtr<Text::JSONBuilder> json, NotNullPtr<WebUserInfo> user);
			void AppendSpecies(NotNullPtr<Text::JSONBuilder> json, NotNullPtr<SpeciesInfo> species, NotNullPtr<Sync::RWMutexUsage> mutUsage);
			void AppendDataFiles(NotNullPtr<Text::JSONBuilder> json, NotNullPtr<Data::FastMap<Data::Timestamp, DataFileInfo*>> dataFiles, Int64 startTime, Int64 endTime);
			static Bool ResponseJSON(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, OSInt cacheAge, Text::CStringNN json);
		public:
			OrganWebPOIController(Net::WebServer::MemoryWebSessionManager *sessMgr, OrganWebEnv *env, UInt32 scnSize);
			virtual ~OrganWebPOIController();
		};
	}
}
#endif
