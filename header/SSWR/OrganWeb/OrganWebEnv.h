#ifndef _SM_SSWR_ORGANWEB_ORGANWEBENV
#define _SM_SSWR_ORGANWEB_ORGANWEBENV
#include "Data/ArrayListDbl.h"
#include "Data/FastMap.h"
#include "Data/StringUTF8Map.h"
#include "DB/DBTool.h"
#include "IO/ConfigFile.h"
#include "IO/LogTool.h"
#include "IO/MemoryStream.h"
#include "Map/OSM/OSMCacheHandler.h"
#include "Media/ColorManager.h"
#include "Net/SocketFactory.h"
#include "Net/WebServer/HTTPDirectoryHandler.h"
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
			DB::DBTool *db;
			Text::String *imageDir;
			Text::String *cacheDir;
			Text::String *dataDir;
			Int32 unorganizedGroupId;
			UInt32 scnSize;
			Text::String *reloadPwd;
			Net::SocketFactory *sockf;
			Net::SSLEngine *ssl;
			IO::LogTool *log;
			Net::WebServer::WebListener *listener;
			Net::WebServer::WebListener *sslListener;
			Sync::Mutex parserMut;
			Parser::FullParserList parsers;
			BookInfo *selectedBook;

			Media::ColorManager colorMgr;
			Media::ColorManagerSess *colorSess;
			Media::DrawEngine *eng;
			Map::OSM::OSMCacheHandler *osmHdlr;
			Net::WebServer::HTTPDirectoryHandler *mapDirHdlr;
			SSWR::OrganWeb::OrganWebHandler *webHdlr;

			Data::FastStringMap<CategoryInfo*> cateSMap;
			Data::FastMap<Int32, CategoryInfo*> cateMap;

			Sync::RWMutex dataMut;
			Data::FastMap<Int32, SpeciesInfo*> spMap;
			Data::FastStringMap<SpeciesInfo*> spNameMap;
			Data::FastMap<Int32, GroupInfo*> groupMap;
			Data::FastMap<Int32, BookInfo*> bookMap;
			Data::FastMap<Int32, WebUserInfo*> userMap;
			Data::FastStringMap<WebUserInfo*> userNameMap;
			Data::FastMap<Int32, UserFileInfo*> userFileMap;
			Data::FastMap<UInt32, IO::ConfigFile*> langMap;
			Data::FastMap<Int32, LocationInfo*> locMap;
			Data::FastMap<Int32, DataFileInfo*> dataFileMap;
			Text::Locale locale;

			void LoadLangs();
			void LoadCategory();
			void LoadSpecies();
			void LoadGroups();
			void LoadBooks();
			void LoadUsers(Sync::RWMutexUsage *mutUsage);
			void LoadLocations();
			void FreeSpecies();
			void FreeGroups();
			void FreeGroup(GroupInfo *group);
			void FreeBooks();
			void FreeUsers();
			void ClearUsers();
		public:
			OrganWebEnv(Net::SocketFactory *sockf, Net::SSLEngine *ssl, IO::LogTool *log, DB::DBTool *db, Text::String *imageDir, UInt16 port, UInt16 sslPort, Text::String *cacheDir, Text::String *dataDir, UInt32 scnSize, Text::String *reloadPwd, Int32 unorganizedGroupId, Media::DrawEngine *eng, Text::CString osmCachePath);
			~OrganWebEnv();

			Bool IsError();

			void Reload();
			void Restart();

			IO::ParsedObject *ParseFileType(IO::StreamData *fd, IO::ParserType targetType);
			Bool HasReloadPwd() const;
			Bool ReloadPwdMatches(Text::String *pwd) const;
			Text::String *GetCacheDir() const;
			Text::String *GetDataDir() const;
			Media::ColorManagerSess *GetColorSess() const;
			Media::DrawEngine *GetDrawEngine() const;

			void CalcGroupCount(Sync::RWMutexUsage *mutUsage, GroupInfo *group);
			void GetGroupSpecies(Sync::RWMutexUsage *mutUsage, GroupInfo *group, Data::DataMap<Text::String*, SpeciesInfo*> *spMap, WebUserInfo *user);
			void SearchInGroup(Sync::RWMutexUsage *mutUsage, GroupInfo *group, const UTF8Char *searchStr, UOSInt searchStrLen, Data::ArrayListDbl *speciesIndice, Data::ArrayList<SpeciesInfo*> *speciesObjs, Data::ArrayListDbl *groupIndice, Data::ArrayList<GroupInfo*> *groupObjs, WebUserInfo *user);
			Bool GroupIsAdmin(GroupInfo *group);
			UTF8Char *PasswordEnc(UTF8Char *buff, Text::CString pwd);

			BookInfo *BookGet(Sync::RWMutexUsage *mutUsage, Int32 id);
			BookInfo *BookGetSelected(Sync::RWMutexUsage *mutUsage);
			void BookSelect(BookInfo *book);
			UTF8Char *BookGetPath(UTF8Char *sbuff, Int32 bookId);
			void BookGetDateMap(Sync::RWMutexUsage *mutUsage, Data::FastMap<Int64, BookInfo*> *bookMap);
			Bool BookFileExist(BookInfo *book);
			Bool BookSetPhoto(Sync::RWMutexUsage *mutUsage, Int32 bookId, Int32 userfileId);
			BookInfo *BookAdd(Sync::RWMutexUsage *mutUsage, Text::String *title, Text::String *author, Text::String *press, Data::Timestamp pubDate, Text::String *url);
			Bool BookAddSpecies(Sync::RWMutexUsage *mutUsage, Int32 speciesId, Text::String *bookspecies, Bool allowDuplicate);

			Bool UserGPSGetPos(Sync::RWMutexUsage *mutUsage, Int32 userId, const Data::Timestamp &t, Double *lat, Double *lon);
			WebUserInfo *UserGet(Sync::RWMutexUsage *mutUsage, Int32 id);
			WebUserInfo *UserGetByName(Sync::RWMutexUsage *mutUsage, Text::String *name);

			SpeciesInfo *SpeciesGet(Sync::RWMutexUsage *mutUsage, Int32 id);
			SpeciesInfo *SpeciesGetByName(Sync::RWMutexUsage *mutUsage, Text::String *sname);
			Int32 SpeciesAdd(Sync::RWMutexUsage *mutUsage, Text::CString engName, Text::CString chiName, Text::CString sciName, Int32 groupId, Text::CString description, Text::CString dirName, Text::CString idKey, Int32 cateId);
			Bool SpeciesUpdateDefPhoto(Sync::RWMutexUsage *mutUsage, Int32 speciesId);
			Bool SpeciesSetPhotoId(Sync::RWMutexUsage *mutUsage, Int32 speciesId, Int32 photoId);
			Bool SpeciesSetFlags(Sync::RWMutexUsage *mutUsage, Int32 speciesId, SpeciesFlags flags);
			Bool SpeciesMove(Sync::RWMutexUsage *mutUsage, Int32 speciesId, Int32 groupId, Int32 cateId);
			Bool SpeciesModify(Sync::RWMutexUsage *mutUsage, Int32 speciesId, Text::CString engName, Text::CString chiName, Text::CString sciName, Text::CString description, Text::CString dirName);
			Bool SpeciesDelete(Sync::RWMutexUsage *mutUsage, Int32 speciesId);
			Bool SpeciesMerge(Sync::RWMutexUsage *mutUsage, Int32 srcSpeciesId, Int32 destSpeciesId, Int32 cateId);
			UserFileInfo *UserfileGetCheck(Sync::RWMutexUsage *mutUsage, Int32 userfileId, Int32 speciesId, Int32 cateId, WebUserInfo *currUser, UTF8Char **filePathOut);
			UserFileInfo *UserfileGet(Sync::RWMutexUsage *mutUsage, Int32 id);
			UTF8Char *UserfileGetPath(UTF8Char *sbuff, const UserFileInfo *userfile);
			Int32 UserfileAdd(Sync::RWMutexUsage *mutUsage, Int32 userId, Int32 spId, Text::CString fileName, const UInt8 *fileCont, UOSInt fileSize, Bool mustHaveCamera);
			Bool UserfileMove(Sync::RWMutexUsage *mutUsage, Int32 userfileId, Int32 speciesId, Int32 cateId);
			Bool UserfileUpdateDesc(Sync::RWMutexUsage *mutUsage, Int32 userfileId, Text::CString descr);
			Bool UserfileUpdateRotType(Sync::RWMutexUsage *mutUsage, Int32 userfileId, Int32 rotType);
			Bool SpeciesBookIsExist(Sync::RWMutexUsage *mutUsage, Text::CString speciesName, Text::StringBuilderUTF8 *bookNameOut);
			void UserFilePrevUpdated(Sync::RWMutexUsage *mutUsage, UserFileInfo *userFile);
			void WebFilePrevUpdated(Sync::RWMutexUsage *mutUsage, WebFileInfo *userFile);

			GroupInfo *GroupGet(Sync::RWMutexUsage *mutUsage, Int32 id);
			Int32 GroupAdd(Sync::RWMutexUsage *mutUsage, Text::CString engName, Text::CString chiName, Int32 parentId, Text::CString descr, Int32 groupTypeId, Int32 cateId, GroupFlags flags);
			Bool GroupModify(Sync::RWMutexUsage *mutUsage, Int32 id, Text::CString engName, Text::CString chiName, Text::CString descr, Int32 groupTypeId, GroupFlags flags);
			Bool GroupDelete(Sync::RWMutexUsage *mutUsage, Int32 id);
			Bool GroupMove(Sync::RWMutexUsage *mutUsage, Int32 groupId, Int32 destGroupId, Int32 cateId);
			Bool GroupAddCounts(Sync::RWMutexUsage *mutUsage, Int32 groupId, UOSInt totalCount, UOSInt photoCount, UOSInt myPhotoCount);
			Bool GroupSetPhotoSpecies(Sync::RWMutexUsage *mutUsage, Int32 groupId, Int32 photoSpeciesId);
			Bool GroupSetPhotoGroup(Sync::RWMutexUsage *mutUsage, Int32 groupId, Int32 photoGroupId);
			Bool GroupIsPublic(Sync::RWMutexUsage *mutUsage, Int32 groupId);

			CategoryInfo *CateGet(Sync::RWMutexUsage *mutUsage, Int32 id);
			CategoryInfo *CateGetByName(Sync::RWMutexUsage *mutUsage, Text::String *name);
			Data::ReadingList<CategoryInfo*> *CateGetList(Sync::RWMutexUsage *mutUsage);

			IO::ConfigFile *LangGet(Net::WebServer::IWebRequest *req);
		};
	}
}
#endif
