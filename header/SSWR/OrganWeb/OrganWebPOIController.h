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
			static Bool __stdcall SvcLang(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);
			static Bool __stdcall SvcLoginInfo(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);
			static Bool __stdcall SvcLogin(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);
			static Bool __stdcall SvcLogout(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);
			static Bool __stdcall SvcCateList(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);
			static Bool __stdcall SvcCate(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);
			static Bool __stdcall SvcYearList(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);
			static Bool __stdcall SvcDayList(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);
			static Bool __stdcall SvcDayDetail(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);
			static Bool __stdcall SvcBookList(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);
			static Bool __stdcall SvcBookSelect(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);
			static Bool __stdcall SvcBookUnselect(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);
			static Bool __stdcall SvcBookAdd(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);
			static Bool __stdcall SvcBookDetail(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);
			static Bool __stdcall SvcSpecies(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);
			static Bool __stdcall SvcPhotoDetail(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);
			static Bool __stdcall SvcPhotoUpload(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);
			static Bool __stdcall SvcPhotoName(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);
			static Bool __stdcall SvcPhotoPos(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);
			static Bool __stdcall SvcUnfinPeak(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);
			static Bool __stdcall SvcUpdatePeak(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);
			static Bool __stdcall SvcReload(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);
			static Bool __stdcall SvcPublicPOI(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);
			static Bool __stdcall SvcGroupPOI(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);
			static Bool __stdcall SvcSpeciesPOI(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);
			static Bool __stdcall SvcDayPOI(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);
			static Bool __stdcall SvcDatafilePOI(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<Net::WebServer::WebController> parent);

			void AddGroupPOI(NN<Sync::RWMutexUsage> mutUsage, NN<Text::JSONBuilder> json, NN<GroupInfo> group, Int32 userId, NN<Data::ArrayListNN<GroupInfo>> groups, NN<Data::ArrayListNN<SpeciesInfo>> speciesList);
			void AddSpeciesPOI(NN<Sync::RWMutexUsage> mutUsage, NN<Text::JSONBuilder> json, NN<SpeciesInfo> species, Int32 userId, Bool publicGroup);
			void AddUserfilePOI(NN<Text::JSONBuilder> json, NN<SpeciesInfo> species, NN<UserFileInfo> file);
			void AddGroups(NN<Text::JSONBuilder> json, NN<Data::ArrayListNN<GroupInfo>> groups);
			void AddGroup(NN<Text::JSONBuilder> json, NN<GroupInfo> group);
			void AddSpeciesList(NN<Text::JSONBuilder> json, NN<Data::ArrayListNN<SpeciesInfo>> speciesList, NN<Sync::RWMutexUsage> mutUsage);
			static void AppendUser(NN<Text::JSONBuilder> json, NN<WebUserInfo> user);
			void AppendSpecies(NN<Text::JSONBuilder> json, NN<SpeciesInfo> species, NN<Sync::RWMutexUsage> mutUsage);
			void AppendDataFiles(NN<Text::JSONBuilder> json, NN<Data::FastMapNN<Data::Timestamp, DataFileInfo>> dataFiles, Int64 startTime, Int64 endTime, Bool includeCont);
			void AppendLocator(NN<Text::JSONBuilder> json, NN<Sync::RWMutexUsage> mutUsage, NN<GroupInfo> group, NN<CategoryInfo> cate);
			static Bool ResponseJSON(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, OSInt cacheAge, Text::CStringNN json);
		public:
			OrganWebPOIController(NN<Net::WebServer::MemoryWebSessionManager> sessMgr, NN<OrganWebEnv> env, UInt32 scnSize);
			virtual ~OrganWebPOIController();
		};
	}
}
#endif
