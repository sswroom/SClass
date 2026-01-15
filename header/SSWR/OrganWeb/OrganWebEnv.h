#ifndef _SM_SSWR_ORGANWEB_ORGANWEBENV
#define _SM_SSWR_ORGANWEB_ORGANWEBENV
#include "Data/ArrayListDbl.h"
#include "Data/FastMapNN.hpp"
#include "Data/StringUTF8Map.hpp"
#include "DB/DBTool.h"
#include "IO/ConfigFile.h"
#include "IO/LogTool.h"
#include "IO/MemoryStream.h"
#include "Map/GPSTrack.h"
#include "Map/OSM/OSMCacheHandler.h"
#include "Media/ColorManager.h"
#include "Net/TCPClientFactory.h"
#include "Net/WebServer/NodeModuleHandler.h"
#include "Net/WebServer/WebListener.h"
#include "Parser/FullParserList.h"
#include "SSWR/OrganWeb/OrganWebCommon.h"
#include "SSWR/OrganWeb/OrganWebHandler.h"
#include "Sync/RWMutex.h"
#include "Sync/RWMutexUsage.h"
#include "Text/Locale.h"
#include "Text/String.h"

namespace SSWR
{
	namespace OrganWeb
	{
		class OrganWebEnv
		{
		private:
			Optional<DB::DBTool> db;
			NN<Text::String> imageDir;
			Optional<Text::String> cacheDir;
			NN<Text::String> dataDir;
			Int32 unorganizedGroupId;
			UInt32 scnSize;
			Optional<Text::String> reloadPwd;
			NN<Net::TCPClientFactory> clif;
			Optional<Net::SSLEngine> ssl;
			NN<IO::LogTool> log;
			Optional<Net::WebServer::WebListener> listener;
			Optional<Net::WebServer::WebListener> sslListener;
			Sync::Mutex parserMut;
			Parser::FullParserList parsers;
			Optional<BookInfo> selectedBook;

			Media::ColorManager colorMgr;
			NN<Media::ColorManagerSess> colorSess;
			NN<Media::DrawEngine> eng;
			NN<Map::OSM::OSMCacheHandler> osmHdlr;
			NN<Net::WebServer::NodeModuleHandler> nodeHdlr;
			Optional<SSWR::OrganWeb::OrganWebHandler> webHdlr;

			Optional<Map::GPSTrack> gpsTrk;
			Int32 gpsUserId;
			Data::Timestamp gpsStartTime;
			Data::Timestamp gpsEndTime;

			Data::FastStringMapNN<CategoryInfo> cateSMap;
			Data::FastMapNN<Int32, CategoryInfo> cateMap;

			Sync::RWMutex dataMut;
			Data::FastMapNN<Int32, SpeciesInfo> spMap;
			Data::FastStringMapNN<SpeciesInfo> spNameMap;
			Data::FastMapNN<Int32, GroupInfo> groupMap;
			Data::FastMapNN<Int32, BookInfo> bookMap;
			Data::FastMapNN<Int32, WebUserInfo> userMap;
			Data::FastStringMapNN<WebUserInfo> userNameMap;
			Data::FastMapNN<Int32, UserFileInfo> userFileMap;
			Data::FastMapNN<UInt32, IO::ConfigFile> langMap;
			Data::FastMapNN<Int32, LocationInfo> locMap;
			Data::FastMapNN<Int32, DataFileInfo> dataFileMap;
			Text::Locale locale;

			void LoadLangs();
			void LoadCategory();
			void LoadSpecies();
			void LoadGroups();
			void LoadBooks();
			void LoadUsers(NN<Sync::RWMutexUsage> mutUsage);
			void LoadLocations();
			void FreeSpecies();
			void FreeGroups();
			void FreeGroup(NN<GroupInfo> group);
			void FreeBooks();
			void FreeUsers();
			void ClearUsers();
		public:
			OrganWebEnv(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, NN<IO::LogTool> log, Optional<DB::DBTool> db, NN<Text::String> imageDir, UInt16 port, UInt16 sslPort, Optional<Text::String> cacheDir, NN<Text::String> dataDir, UInt32 scnSize, Optional<Text::String> reloadPwd, Int32 unorganizedGroupId, NN<Media::DrawEngine> eng, Text::CStringNN osmCachePath);
			~OrganWebEnv();

			Bool IsError();

			void Reload();
			void Restart();

			Optional<IO::ParsedObject> ParseFileType(NN<IO::StreamData> fd, IO::ParserType targetType);
			Bool HasReloadPwd() const;
			Bool ReloadPwdMatches(NN<Text::String> pwd) const;
			Optional<Text::String> GetCacheDir() const;
			NN<Text::String> GetDataDir() const;
			NN<Media::ColorManagerSess> GetColorSess() const;
			NN<Media::DrawEngine> GetDrawEngine() const;
			void SetUpgradeInsecureURL(Text::CStringNN upgradeInsecureURL);

			void CalcGroupCount(NN<Sync::RWMutexUsage> mutUsage, NN<GroupInfo> group);
			void GetGroupSpecies(NN<Sync::RWMutexUsage> mutUsage, NN<GroupInfo> group, NN<Data::DataMapNN<Optional<Text::String>, SpeciesInfo>> spMap, Optional<WebUserInfo> user);
			void SearchInGroup(NN<Sync::RWMutexUsage> mutUsage, NN<GroupInfo> group, UnsafeArray<const UTF8Char> searchStr, UIntOS searchStrLen, NN<Data::ArrayListDbl> speciesIndice, NN<Data::ArrayListNN<SpeciesInfo>> speciesObjs, NN<Data::ArrayListDbl> groupIndice, NN<Data::ArrayListNN<GroupInfo>> groupObjs, Optional<WebUserInfo> user);
			Bool GroupIsAdmin(NN<GroupInfo> group);
			UnsafeArray<UTF8Char> PasswordEnc(UnsafeArray<UTF8Char> buff, Text::CStringNN pwd);

			Optional<BookInfo> BookGet(NN<Sync::RWMutexUsage> mutUsage, Int32 id);
			Optional<BookInfo> BookGetSelected(NN<Sync::RWMutexUsage> mutUsage);
			void BookSelect(Optional<BookInfo> book);
			UnsafeArray<UTF8Char> BookGetPath(UnsafeArray<UTF8Char> sbuff, Int32 bookId);
			void BookGetList(NN<Sync::RWMutexUsage> mutUsage, NN<Data::ArrayListNN<BookInfo>> bookList);
			Bool BookFileExist(NN<BookInfo> book);
			Bool BookSetPhoto(NN<Sync::RWMutexUsage> mutUsage, Int32 bookId, Int32 userfileId);
			Optional<BookInfo> BookAdd(NN<Sync::RWMutexUsage> mutUsage, NN<Text::String> title, NN<Text::String> author, NN<Text::String> press, Data::Timestamp pubDate, NN<Text::String> url);
			Bool BookAddSpecies(NN<Sync::RWMutexUsage> mutUsage, Int32 speciesId, NN<Text::String> bookspecies, Bool allowDuplicate);

			Bool UserGPSGetPos(NN<Sync::RWMutexUsage> mutUsage, Int32 userId, const Data::Timestamp &t, OutParam<Double> lat, OutParam<Double> lon);
			Optional<WebUserInfo> UserGet(NN<Sync::RWMutexUsage> mutUsage, Int32 id);
			Optional<WebUserInfo> UserGetByName(NN<Sync::RWMutexUsage> mutUsage, NN<Text::String> name);

			Optional<SpeciesInfo> SpeciesGet(NN<Sync::RWMutexUsage> mutUsage, Int32 id);
			Optional<SpeciesInfo> SpeciesGetByName(NN<Sync::RWMutexUsage> mutUsage, NN<Text::String> sname);
			Int32 SpeciesAdd(NN<Sync::RWMutexUsage> mutUsage, Text::CStringNN engName, Text::CStringNN chiName, Text::CStringNN sciName, Int32 groupId, Text::CStringNN description, Text::CStringNN dirName, Text::CStringNN idKey, Int32 cateId);
			Bool SpeciesUpdateDefPhoto(NN<Sync::RWMutexUsage> mutUsage, Int32 speciesId);
			Bool SpeciesSetPhotoId(NN<Sync::RWMutexUsage> mutUsage, Int32 speciesId, Int32 photoId);
			Bool SpeciesSetPhotoWId(NN<Sync::RWMutexUsage> mutUsage, Int32 speciesId, Int32 photoWId, Bool removePhotoId);
			Bool SpeciesSetFlags(NN<Sync::RWMutexUsage> mutUsage, Int32 speciesId, SpeciesFlags flags);
			Bool SpeciesMove(NN<Sync::RWMutexUsage> mutUsage, Int32 speciesId, Int32 groupId, Int32 cateId);
			Bool SpeciesModify(NN<Sync::RWMutexUsage> mutUsage, Int32 speciesId, Text::CStringNN engName, Text::CStringNN chiName, Text::CStringNN sciName, Text::CStringNN description, Text::CStringNN dirName);
			Bool SpeciesDelete(NN<Sync::RWMutexUsage> mutUsage, Int32 speciesId);
			Bool SpeciesMerge(NN<Sync::RWMutexUsage> mutUsage, Int32 srcSpeciesId, Int32 destSpeciesId, Int32 cateId);
			Bool SpeciesAddWebfile(NN<Sync::RWMutexUsage> mutUsage, Int32 speciesId, Text::CStringNN imgURL, Text::CStringNN sourceURL, Text::CStringNN location);
			Optional<UserFileInfo> UserfileGetCheck(NN<Sync::RWMutexUsage> mutUsage, Int32 userfileId, Int32 speciesId, Int32 cateId, Optional<WebUserInfo> currUser, InOutParam<UnsafeArray<UTF8Char>> filePathOut);
			Optional<UserFileInfo> UserfileGet(NN<Sync::RWMutexUsage> mutUsage, Int32 id);
			UnsafeArray<UTF8Char> UserfileGetPath(UnsafeArray<UTF8Char> sbuff, NN<const UserFileInfo> userfile);
			Int32 UserfileAdd(NN<Sync::RWMutexUsage> mutUsage, Int32 userId, Int32 spId, Text::CStringNN fileName, UnsafeArray<const UInt8> fileCont, UIntOS fileSize, Bool mustHaveCamera, Optional<Text::String> location);
			Bool UserfileMove(NN<Sync::RWMutexUsage> mutUsage, Int32 userfileId, Int32 speciesId, Int32 cateId);
			Bool UserfileUpdateDesc(NN<Sync::RWMutexUsage> mutUsage, Int32 userfileId, Text::CString descr);
			Bool UserfileUpdateRotType(NN<Sync::RWMutexUsage> mutUsage, Int32 userfileId, Int32 rotType);
			Bool UserfileUpdatePos(NN<Sync::RWMutexUsage> mutUsage, Int32 userfileId, Data::Timestamp captureTime, Double lat, Double lon, LocType locType);
			Bool SpeciesBookIsExist(NN<Sync::RWMutexUsage> mutUsage, Text::CStringNN speciesName, NN<Text::StringBuilderUTF8> bookNameOut);
			void UserFilePrevUpdated(NN<Sync::RWMutexUsage> mutUsage, NN<UserFileInfo> userFile);
			void WebFilePrevUpdated(NN<Sync::RWMutexUsage> mutUsage, NN<WebFileInfo> userFile);
			Bool GPSFileAdd(NN<Sync::RWMutexUsage> mutUsage, Int32 webuserId, Text::CStringNN fileName, Data::Timestamp startTime, Data::Timestamp endTime, UnsafeArray<const UInt8> fileCont, UIntOS fileSize, NN<Map::GPSTrack> gpsTrk, OutParam<Text::CString> errMsg);

			Bool DataFileAdd(NN<Sync::RWMutexUsage> mutUsage, Int32 webuserId, Text::CStringNN fileName, Data::Timestamp startTime, Data::Timestamp endTime, DataFileType fileType, UnsafeArray<const UInt8> fileCont, UIntOS fileSize, OutParam<Text::CString> errMsg);
			Optional<IO::ParsedObject> DataFileParse(NN<DataFileInfo> dataFile);
			Optional<DataFileInfo> DataFileGet(NN<Sync::RWMutexUsage> mutUsage, Int32 datafileId);

			Optional<GroupInfo> GroupGet(NN<Sync::RWMutexUsage> mutUsage, Int32 id);
			Int32 GroupAdd(NN<Sync::RWMutexUsage> mutUsage, Text::CStringNN engName, Text::CStringNN chiName, Int32 parentId, Text::CStringNN descr, Int32 groupTypeId, Int32 cateId, GroupFlags flags);
			Bool GroupModify(NN<Sync::RWMutexUsage> mutUsage, Int32 id, Text::CStringNN engName, Text::CStringNN chiName, Text::CStringNN descr, Int32 groupTypeId, GroupFlags flags);
			Bool GroupDelete(NN<Sync::RWMutexUsage> mutUsage, Int32 id);
			Bool GroupMove(NN<Sync::RWMutexUsage> mutUsage, Int32 groupId, Int32 destGroupId, Int32 cateId);
			Bool GroupAddCounts(NN<Sync::RWMutexUsage> mutUsage, Int32 groupId, UIntOS totalCount, UIntOS photoCount, UIntOS myPhotoCount);
			Bool GroupSetPhotoSpecies(NN<Sync::RWMutexUsage> mutUsage, Int32 groupId, Int32 photoSpeciesId);
			Bool GroupSetPhotoGroup(NN<Sync::RWMutexUsage> mutUsage, Int32 groupId, Int32 photoGroupId);
			Bool GroupIsPublic(NN<Sync::RWMutexUsage> mutUsage, Int32 groupId);

			Optional<CategoryInfo> CateGet(NN<Sync::RWMutexUsage> mutUsage, Int32 id);
			Optional<CategoryInfo> CateGetByName(NN<Sync::RWMutexUsage> mutUsage, NN<Text::String> name);
			NN<Data::ReadingListNN<CategoryInfo>> CateGetList(NN<Sync::RWMutexUsage> mutUsage);

			UIntOS PeakGetUnfin(NN<Sync::RWMutexUsage> mutUsage, NN<Data::ArrayListNN<PeakInfo>> peaks);
			Bool PeakUpdateStatus(NN<Sync::RWMutexUsage> mutUsage, Int32 id, Int32 status);
			void PeakFreeAll(NN<Data::ArrayListNN<PeakInfo>> peaks);

			Optional<IO::ConfigFile> LangGet(NN<Net::WebServer::WebRequest> req);
		};
	}
}
#endif
