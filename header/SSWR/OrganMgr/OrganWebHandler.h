#ifndef _SM_SSWR_ORGANMGR_ORGANWEBHANDLER
#define _SM_SSWR_ORGANMGR_ORGANWEBHANDLER
#include "Data/ArrayListDbl.h"
#include "Data/Comparator.h"
#include "Data/FastMap.h"
#include "Data/RandomOS.h"
#include "Data/StringUTF8Map.h"
#include "DB/DBTool.h"
#include "IO/ConfigFile.h"
#include "IO/LogTool.h"
#include "IO/MemoryStream.h"
#include "Map/OSM/OSMCacheHandler.h"
#include "Media/LRGBLimiter.h"
#include "Media/CS/CSConverter.h"
#include "Media/Resizer/LanczosResizerLR_C32.h"
#include "Net/SocketFactory.h"
#include "Net/WebServer/HTTPDirectoryHandler.h"
#include "Net/WebServer/MemoryWebSessionManager.h"
#include "Net/WebServer/WebListener.h"
#include "Net/WebServer/WebServiceHandler.h"
#include "Parser/FullParserList.h"
#include "Sync/RWMutex.h"
#include "Text/Locale.h"
#include "Text/String.h"

namespace SSWR
{
	namespace OrganMgr
	{
		class OrganWebHandler : public Net::WebServer::WebServiceHandler
		{
		private:
			typedef enum
			{
				POT_UNKNOWN,
				POT_USERFILE,
				POT_GROUP,
				POT_SPECIES
			} PickObjType;

			typedef enum
			{
				GF_NONE = 0,
				GF_ADMIN_ONLY = 1
			} GroupFlags;

			typedef enum
			{
				SF_NONE = 0,
				SF_HAS_MYPHOTO = 1,
				SF_HAS_WEBPHOTO = 8
			} SpeciesFlags;

			typedef struct
			{
				Int32 bookId;
				Int32 speciesId;
				Text::String *dispName;
			} BookSpInfo;

			class BookInfo
			{
			public:
				Int32 id;
				Text::String *title;
				Text::String *author;
				Text::String *press;
				Int64 publishDate;
				Text::String *url;

				Data::ArrayList<BookSpInfo*> species;
			};

			typedef struct
			{
				Int32 id;
				Int32 parentId;
				Text::String *cname;
				Text::String *ename;
				Double lat;
				Double lon;
				Int32 cateId;
				Int32 locType;
			} LocationInfo;

			typedef struct
			{
				Int64 fromDate;
				Int64 toDate;
				Int32 locId;
				Int32 cateId;
			} TripInfo;
			
			typedef struct
			{
				Int32 id;
				Int32 fileType;
				Text::String *oriFileName;
				Int64 fileTimeTicks;
				Double lat;
				Double lon;
				Int32 webuserId;
				Int32 speciesId;
				Int64 captureTimeTicks;
				Text::String *dataFileName;
				UInt32 crcVal;
				Int32 rotType;
				Int32 prevUpdated;
				Double cropLeft;
				Double cropTop;
				Double cropRight;
				Double cropBottom;
				Text::String *descript;
				Text::String *location;
			} UserFileInfo;

			typedef struct
			{
				Int32 id;
				Int32 crcVal;
				Text::String *imgUrl;
				Text::String *srcUrl;
				Text::String *location;
				Int32 prevUpdated;
				Double cropLeft;
				Double cropTop;
				Double cropRight;
				Double cropBottom;
			} WebFileInfo;

			typedef struct
			{
				Int32 id;
				Text::String *userName;
				Text::String *pwd;
				Text::String *watermark;
				Int32 userType;
				Data::ArrayListInt64 userFileIndex;
				Data::ArrayList<UserFileInfo*> userFileObj;
				Data::FastMap<Int32, Data::FastMap<Int64, TripInfo*>*> tripCates;
				Int32 unorganSpId;
			} WebUserInfo;

			class SpeciesInfo
			{
			public:
				Int32 speciesId;
				Text::String *engName;
				Text::String *chiName;
				Text::String *sciName;
				UInt32 sciNameHash;
				Int32 groupId;
				Text::String *descript;
				Text::String *dirName;
				Text::String *photo;
				Text::String *idKey;
				Int32 cateId;
				SpeciesFlags flags;
				Int32 photoId;
				Int32 photoWId;
				Text::String *poiImg;

				Data::ArrayList<BookSpInfo*> books;
				Data::ArrayList<UserFileInfo*> files;
				Data::FastMap<Int32, WebFileInfo*> wfiles;
			};

			class GroupInfo
			{
			public:
				Int32 id;
				Int32 groupType;
				Text::String *engName;
				Text::String *chiName;
				Text::String *descript;
				Int32 parentId;
				Int32 photoGroup;
				Int32 photoSpecies;
				Text::String *idKey;
				Int32 cateId;
				GroupFlags flags;

				UOSInt photoCount;
				UOSInt myPhotoCount;
				UOSInt totalCount;
				SpeciesInfo *photoSpObj;
				Data::ArrayList<SpeciesInfo*> species;
				Data::ArrayList<GroupInfo *> groups;
			};

			typedef struct
			{
				Int32 id;
				Text::String *chiName;
				Text::String *engName;
			} GroupTypeInfo;

			struct CategoryInfo
			{
				Int32 cateId;
				Text::String *chiName;
				Text::String *dirName;
				Text::String *srcDir;
				Int32 flags;
				Data::FastMap<Int32, GroupTypeInfo *> groupTypes;
				Data::ArrayList<GroupInfo*> groups;
			};

			typedef struct
			{
				UInt32 scnWidth;
				Bool isMobile;
				WebUserInfo *user;
				Data::ArrayListInt32 *pickObjs;
				PickObjType pickObjType;
			} RequestEnv;

			class SpeciesSciNameComparator : public Data::Comparator<SpeciesInfo*>
			{
			public:
				virtual ~SpeciesSciNameComparator();
				virtual OSInt Compare(SpeciesInfo *a, SpeciesInfo *b) const;
			};

			class UserFileTimeComparator : public Data::Comparator<UserFileInfo*>
			{
			public:
				virtual ~UserFileTimeComparator();
				virtual OSInt Compare(UserFileInfo *a, UserFileInfo *b) const;
			};
		private:
			Data::RandomOS random;
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
			Sync::Mutex resizerMut;
			Media::Resizer::LanczosResizerLR_C32 *resizerLR;
			Sync::Mutex csconvMut;
			Media::CS::CSConverter *csconv;
			UInt32 csconvFCC;
			UInt32 csconvBpp;
			Media::PixelFormat csconvPF;
			Media::ColorProfile csconvColor;
			Media::LRGBLimiter lrgbLimiter;

			Media::ColorManager colorMgr;
			Media::ColorManagerSess *colorSess;
			Media::DrawEngine *eng;
			Net::WebServer::MemoryWebSessionManager *sessMgr;
			Map::OSM::OSMCacheHandler *osmHdlr;
			Net::WebServer::HTTPDirectoryHandler *mapDirHdlr;

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
			Text::Locale locale;

			void LoadLangs();
			void LoadCategory();
			void LoadSpecies();
			void LoadGroups();
			void LoadBooks();
			void LoadUsers();
			void LoadLocations();
			void FreeSpecies();
			void FreeGroups();
			void FreeGroup(GroupInfo *group);
			void FreeBooks();
			void FreeUsers();
			void ClearUsers();
			void UserFilePrevUpdated(UserFileInfo *userFile);
			void WebFilePrevUpdated(WebFileInfo *userFile);

			void CalcGroupCount(GroupInfo *group);
			void GetGroupSpecies(GroupInfo *group, Data::DataMap<Text::String*, SpeciesInfo*> *spMap, WebUserInfo *user);
			void SearchInGroup(GroupInfo *group, const UTF8Char *searchStr, UOSInt searchStrLen, Data::ArrayListDbl *speciesIndice, Data::ArrayList<SpeciesInfo*> *speciesObjs, Data::ArrayListDbl *groupIndice, Data::ArrayList<GroupInfo*> *groupObjs, WebUserInfo *user);
			Bool GroupIsAdmin(GroupInfo *group);
			UTF8Char *PasswordEnc(UTF8Char *buff, Text::CString pwd);
			Bool BookFileExist(BookInfo *book);
			Bool UserGPSGetPos(Int32 userId, const Data::Timestamp &t, Double *lat, Double *lon);

			//LockWrite dataMut before calling
			Int32 SpeciesAdd(Text::CString engName, Text::CString chiName, Text::CString sciName, Int32 groupId, Text::CString description, Text::CString dirName, Text::CString idKey, Int32 cateId);
			Bool SpeciesSetPhotoId(Int32 speciesId, Int32 photoId);
			Bool SpeciesSetFlags(Int32 speciesId, SpeciesFlags flags);
			Bool SpeciesMove(Int32 speciesId, Int32 groupId, Int32 cateId);
			Bool SpeciesModify(Int32 speciesId, Text::CString engName, Text::CString chiName, Text::CString sciName, Text::CString description, Text::CString dirName);
			Int32 UserfileAdd(Int32 userId, Int32 spId, Text::CString fileName, const UInt8 *fileCont, UOSInt fileSize);
			Bool UserfileMove(Int32 userfileId, Int32 speciesId, Int32 cateId);
			Bool UserfileUpdateDesc(Int32 userfileId, Text::CString descr);
			Bool UserfileUpdateRotType(Int32 userfileId, Int32 rotType);
			Bool SpeciesBookIsExist(Text::CString speciesName, Text::StringBuilderUTF8 *bookNameOut);
			Int32 GroupAdd(Text::CString engName, Text::CString chiName, Int32 parentId, Text::CString descr, Int32 groupTypeId, Int32 cateId, GroupFlags flags);
			Bool GroupModify(Int32 id, Text::CString engName, Text::CString chiName, Text::CString descr, Int32 groupTypeId, GroupFlags flags);
			Bool GroupDelete(Int32 id);
			Bool GroupMove(Int32 groupId, Int32 destGroupId, Int32 cateId);
			Bool GroupAddCounts(Int32 groupId, UOSInt totalCount, UOSInt photoCount, UOSInt myPhotoCount);
			Bool GroupSetPhotoSpecies(Int32 groupId, Int32 photoSpeciesId);
			Bool GroupSetPhotoGroup(Int32 groupId, Int32 photoGroupId);

			Net::WebServer::IWebSession *ParseRequestEnv(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, RequestEnv *env, Bool keepSess);
			static Bool __stdcall SvcPhoto(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcPhotoDown(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcGroup(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcGroupMod(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcSpecies(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcSpeciesMod(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcList(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcPhotoDetail(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcPhotoDetailD(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcPhotoYear(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcPhotoDay(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcPhotoUpload(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcPhotoUploadD(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcSearchInside(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcSearchInsideMoreS(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcSearchInsideMoreG(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcBookList(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcBook(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcBookView(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcLogin(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcLogout(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcReload(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcRestart(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcIndex(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcCate(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcFavicon(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);
			static Bool __stdcall SvcPublicPOI(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, Net::WebServer::WebServiceHandler *parent);

			static void AddPublicPOI(Text::StringBuilderUTF8 *sb, GroupInfo *group);

			void ResponsePhoto(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, WebUserInfo *user, Bool isMobile, Int32 speciesId, Int32 cateId, UInt32 imgWidth, UInt32 imgHeight, const UTF8Char *fileName);
			void ResponsePhotoId(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, WebUserInfo *user, Bool isMobile, Int32 speciesId, Int32 cateId, UInt32 imgWidth, UInt32 imgHeight, Int32 photoId);
			void ResponsePhotoWId(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, WebUserInfo *user, Bool isMobile, Int32 speciesId, Int32 cateId, UInt32 imgWidth, UInt32 imgHeight, Int32 photoWId);
			static void ResponseMstm(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, IO::MemoryStream *mstm, Text::CString contType);

			void WriteHeaderPart1(IO::Writer *writer, const UTF8Char *title, Bool isMobile);
			void WriteHeaderPart2(IO::Writer *writer, WebUserInfo *user, const UTF8Char *onLoadFunc);
			void WriteHeader(IO::Writer *writer, const UTF8Char *title, WebUserInfo *user, Bool isMobile);
			void WriteFooter(IO::Writer *writer);
			void WriteLocator(IO::Writer *writer, GroupInfo *group, CategoryInfo *cate);
			void WriteLocatorText(IO::Writer *writer, GroupInfo *group, CategoryInfo *cate);
			void WriteGroupTable(IO::Writer *writer, const Data::ReadingList<GroupInfo *> *groupList, UInt32 scnWidth, Bool showSelect);
			void WriteSpeciesTable(IO::Writer *writer, const Data::ArrayList<SpeciesInfo *> *spList, UInt32 scnWidth, Int32 cateId, Bool showSelect);
			void WritePickObjs(IO::Writer *writer, RequestEnv *env, const UTF8Char *url);

			static Bool __stdcall OnSessionDel(Net::WebServer::IWebSession* sess, void *userObj);
			static Bool __stdcall OnSessionCheck(Net::WebServer::IWebSession* sess, void *userObj);

			IO::ConfigFile *LangGet(Net::WebServer::IWebRequest *req);
			static Text::CString LangGetValue(IO::ConfigFile *lang, const UTF8Char *name, UOSInt nameLen);
		public:
			OrganWebHandler(Net::SocketFactory *sockf, Net::SSLEngine *ssl, IO::LogTool *log, DB::DBTool *db, Text::String *imageDir, UInt16 port, UInt16 sslPort, Text::String *cacheDir, Text::String *dataDir, UInt32 scnSize, Text::String *reloadPwd, Int32 unorganizedGroupId, Media::DrawEngine *eng, Text::CString osmCachePath);
			virtual ~OrganWebHandler();

			Bool IsError();

//			virtual void WebRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp);

			void Reload();
			void Restart();
		};
	}
}
#endif
