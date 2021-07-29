#ifndef _SM_SSWR_ORGANMGR_ORGANENV
#define _SM_SSWR_ORGANMGR_ORGANENV

#include "Data/ArrayListInt64.h"
#include "IO/ConfigFile.h"
#include "IO/Writer.h"
#include "DB/DBTool.h"
#include "Map/GPSTrack.h"
#include "Media/ColorManager.h"
#include "Media/ImageList.h"
#include "Media/MonitorMgr.h"
#include "Net/SSLEngine.h"
#include "Parser/ParserList.h"
#include "SSWR/OrganMgr/OrganGroup.h"
#include "SSWR/OrganMgr/OrganGroupType.h"
#include "SSWR/OrganMgr/OrganBook.h"
#include "SSWR/OrganMgr/OrganSpecies.h"
#include "SSWR/OrganMgr/OrganTripList.h"
#include "UI/GUIForm.h"

namespace SSWR
{
	namespace OrganMgr
	{
		class OrganImages;
		class OrganImageItem;

		typedef enum
		{
			UT_ADMIN,
			UT_USER
		} UserType;

		typedef struct
		{
			Int32 cateId;
			const UTF8Char *chiName;
			const UTF8Char *dirName;
			const UTF8Char *srcDir;
		} Category;

		typedef struct
		{
			Int32 id;
			const UTF8Char *dispName;
			OrganBook *book;
		} SpeciesBook;

		typedef struct
		{
			Int32 id;
			const UTF8Char *userName;
			const UTF8Char *watermark;
			UserType userType;
		} OrganWebUser;

		typedef struct
		{
			Int32 id;
			Int32 fileType;
			Int64 startTimeTicks;
			Int64 endTimeTicks;
			const UTF8Char *oriFileName;
			const UTF8Char *fileName;
			Int32 webUserId;
		} DataFileInfo;

		typedef struct
		{
			Int32 id;
			Int32 fileType;
			const UTF8Char *oriFileName;
			Int64 fileTimeTicks;
			Double lat;
			Double lon;
			Int32 webuserId;
			Int32 speciesId;
			Int64 captureTimeTicks;
			const UTF8Char *dataFileName;
			UInt32 crcVal;
			Int32 rotType;
			const UTF8Char *camera;
			const UTF8Char *descript;
			Double cropLeft;
			Double cropTop;
			Double cropRight;
			Double cropBottom;
			const UTF8Char *location;
		} UserFileInfo;

		typedef struct
		{
			Int32 id;
			Int32 speciesId;
			UInt32 crcVal;
			const UTF8Char *imgUrl;
			const UTF8Char *srcUrl;
			const UTF8Char *location;
			Double cropLeft;
			Double cropTop;
			Double cropRight;
			Double cropBottom;
		} WebFileInfo;

		typedef struct
		{
			Int32 id;
			Data::ArrayList<UserFileInfo*> *files;
			Data::Int32Map<WebFileInfo*> *wfileMap;
		} SpeciesInfo;

		typedef struct
		{
			Int32 id;
			Data::ArrayListInt64 *gpsFileIndex;
			Data::ArrayList<DataFileInfo*> *gpsFileObj;
			Data::ArrayListInt64 *userFileIndex;
			Data::ArrayList<UserFileInfo*> *userFileObj;
		} WebUserInfo;

		class OrganEnv
		{
		public:
			typedef enum
			{
				ERR_NONE,
				ERR_CONFIG,
				ERR_DB
			} ErrorType;

			typedef enum
			{
				FS_SUCCESS,
				FS_NOTSUPPORT,
				FS_ERROR
			} FileStatus;

		protected:
			Media::ColorManager *colorMgr;
			Parser::ParserList *parsers;
			Media::DrawEngine *drawEng;
			Net::SocketFactory *sockf;
			Net::SSLEngine *ssl;
			Media::MonitorMgr *monMgr;
			ErrorType errType;
			IO::ConfigFile *langFile;
			Data::ArrayList<Category*> *categories;
			Data::ArrayList<OrganGroupType*> *grpTypes;
			Category *currCate;
			Bool cateIsFullDir;
			Data::ArrayListInt32 *bookIds;
			Data::ArrayList<OrganBook*> *bookObjs;
			Data::ArrayList<DataFileInfo*> *dataFiles;
			Int32 userId;
			Data::Int32Map<SpeciesInfo*> *speciesMap;
			Data::Int32Map<UserFileInfo*> *userFileMap;
			Data::Int32Map<WebUserInfo*> *userMap;

			Data::ArrayList<Trip*> *trips;
			Data::ArrayList<Location*> *locs;
			Data::ArrayList<LocationType*> *locType;

			Data::DateTime *gpsStartTime;
			Data::DateTime *gpsEndTime;
			Map::GPSTrack *gpsTrk;
			Int32 gpsUserId;

		public:
			OrganEnv();
			virtual ~OrganEnv();

			Media::DrawEngine *GetDrawEngine();
			Parser::ParserList *GetParserList();
			Net::SocketFactory *GetSocketFactory();
			Net::SSLEngine *GetSSLEngine();
			Media::ColorManager *GetColorMgr();
			Media::MonitorMgr *GetMonitorMgr();
			ErrorType GetErrorType();
			const UTF8Char *GetLang(const UTF8Char *name);
			virtual const UTF8Char *GetCacheDir() = 0;

			UOSInt GetCategories(Data::ArrayList<Category*> *categories);
			virtual UOSInt GetGroupItems(Data::ArrayList<OrganGroupItem*> *items, OrganGroup *grp) = 0;
			virtual UOSInt GetGroupImages(Data::ArrayList<OrganImageItem*> *items, OrganGroup *grp) = 0;
			virtual UOSInt GetSpeciesImages(Data::ArrayList<OrganImageItem*> *items, OrganSpecies *sp) = 0;
			virtual UOSInt GetGroupAllSpecies(Data::ArrayList<OrganSpecies*> *items, OrganGroup *grp) = 0;
			virtual UOSInt GetGroupAllUserFile(Data::ArrayList<UserFileInfo*> *items, Data::ArrayList<UInt32> *colors, OrganGroup *grp) = 0;
			virtual UOSInt GetSpeciesItems(Data::ArrayList<OrganGroupItem*> *items, Data::ArrayList<Int32> *speciesIds) = 0;
			Data::ArrayList<OrganGroupType*> *GetGroupTypes();
			virtual OrganGroup *GetGroup(Int32 groupId, Int32 *parentId) = 0;
			virtual OrganSpecies *GetSpecies(Int32 speciesId) = 0;
//			OrganTripList *GetTripList();
			virtual UTF8Char *GetSpeciesDir(OrganSpecies *sp, UTF8Char *sbuff) = 0; /////////////////
//			virtual Bool CreateSpeciesDir(OrganSpecies *sp) = 0; ////////////////////////
			virtual Bool IsSpeciesExist(const UTF8Char *sName) = 0;
			virtual Bool IsBookSpeciesExist(const UTF8Char *sName, Text::StringBuilderUTF *sb) = 0;
			virtual Bool AddSpecies(OrganSpecies *sp) = 0;
			virtual Bool DelSpecies(OrganSpecies *sp) = 0;
			virtual FileStatus AddSpeciesFile(OrganSpecies *sp, const UTF8Char *fileName, Bool firstPhoto, Bool moveFile, Int32 *fileId) = 0;
			virtual FileStatus AddSpeciesWebFile(OrganSpecies *sp, const UTF8Char *srcURL, const UTF8Char *imgURL, IO::Stream *stm, UTF8Char *webFileName) = 0;
			virtual Bool UpdateSpeciesWebFile(OrganSpecies *sp, WebFileInfo *wfile, const UTF8Char *srcURL, const UTF8Char *location) = 0;
			Bool SetSpeciesImg(OrganSpecies *sp, OrganImageItem *img);
			Bool SetSpeciesMapColor(OrganSpecies *sp, UInt32 mapColor);
			virtual Bool SaveSpecies(OrganSpecies *sp) = 0;
			virtual Bool SaveGroup(OrganGroup *grp) = 0;
			virtual UOSInt GetGroupCount(Int32 groupId) = 0;
			virtual UOSInt GetSpeciesCount(Int32 groupId) = 0;
			virtual Bool AddGroup(OrganGroup *grp, Int32 parGroupId) = 0;
			virtual Bool DelGroup(Int32 groupId) = 0;
			virtual Bool SetGroupDefSp(OrganGroup *grp, OrganImageItem *img) = 0;

			virtual Bool MoveGroups(Data::ArrayList<OrganGroup*> *grpList, OrganGroup *destGroup) = 0;
			virtual Bool MoveSpecies(Data::ArrayList<OrganSpecies*> *spList, OrganGroup *destGroup) = 0;
			virtual Bool MoveImages(Data::ArrayList<OrganImages*> *imgList, OrganSpecies *destSp, UI::GUIForm *frm) = 0;
			virtual Bool CombineSpecies(OrganSpecies *destSp, OrganSpecies *srcSp) = 0;

			virtual UOSInt GetWebUsers(Data::ArrayList<OrganWebUser*> *userList) = 0;
			virtual Bool AddWebUser(const UTF8Char *userName, const UTF8Char *pwd, const UTF8Char *watermark, UserType userType) = 0;
			virtual Bool ModifyWebUser(Int32 id, const UTF8Char *userName, const UTF8Char *pwd, const UTF8Char *watermark) = 0;
			virtual void ReleaseWebUsers(Data::ArrayList<OrganWebUser*> *userList) = 0;

			UOSInt GetBooksAll(Data::ArrayList<OrganBook*> *items);
			UOSInt GetBooksOfYear(Data::ArrayList<OrganBook*> *items, Int32 year);
			virtual Bool IsSpeciesBookExist(Int32 speciesId, Int32 bookId) = 0;
			virtual Bool NewSpeciesBook(Int32 speciesId, Int32 bookId, const UTF8Char *dispName) = 0;
			virtual OSInt GetSpeciesBooks(Data::ArrayList<SpeciesBook*> *items, Int32 speciesId) = 0;
			virtual void ReleaseSpeciesBooks(Data::ArrayList<SpeciesBook*> *items) = 0;
			virtual Int32 NewBook(const UTF8Char *title, const UTF8Char *author, const UTF8Char *press, Data::DateTime *publishDate, const UTF8Char *url) = 0;

			WebUserInfo *GetWebUser(Int32 userId);
			virtual Bool AddDataFile(const UTF8Char *fileName) = 0;
			Data::ArrayList<DataFileInfo*> *GetDataFiles();
			virtual Bool DelDataFile(DataFileInfo *dataFile) = 0;
			void ReleaseDataFile(DataFileInfo *dataFile);
			virtual Bool GetGPSPos(Int32 userId, Data::DateTime *t, Double *lat, Double *lon) = 0;
			virtual Map::GPSTrack *OpenGPSTrack(DataFileInfo *dataFile) = 0;

			void ReleaseSpecies(SpeciesInfo *species);
			void ReleaseUserFile(UserFileInfo *userFile);
			virtual void UpdateUserFileCrop(UserFileInfo *userFile, Double cropLeft, Double cropTop, Double cropRight, Double cropBottom) = 0;
			virtual void UpdateUserFileRot(UserFileInfo *userFile, Int32 rotType) = 0;
			virtual Bool UpdateUserFilePos(UserFileInfo *userFile, Data::DateTime *captureTime, Double lat, Double lon) = 0;
			UOSInt GetUserFiles(Data::ArrayList<UserFileInfo*> *userFiles, Int64 fromTimeTicks, Int64 toTimeTicks);
			virtual Bool GetUserFilePath(UserFileInfo *userFile, Text::StringBuilderUTF *sb) = 0;
			virtual Bool UpdateUserFileDesc(UserFileInfo *userFile, const UTF8Char *descript) = 0;
			virtual Bool UpdateUserFileLoc(UserFileInfo *userFile, const UTF8Char *location) = 0;
			virtual void UpdateWebFileCrop(WebFileInfo *userFile, Double cropLeft, Double cropTop, Double cropRight, Double cropBottom) = 0;

		protected:
			void TripRelease();
			virtual void TripReload(Int32 cateId) = 0;
		public:
			OSInt TripGetIndex(Data::DateTime *d);
			Trip *TripGet(Int32 userId, Data::DateTime *d);
			Data::ArrayList<Trip*> *TripGetList();
			virtual Bool TripAdd(Data::DateTime *fromDate, Data::DateTime *toDate, Int32 locId) = 0;

			OSInt LocationGetIndex(Int32 locId);
			Location *LocationGet(Int32 locId);
			Data::ArrayList<Location*> *LocationGetSub(Int32 locId);
			virtual Bool LocationUpdate(Int32 locId, const UTF8Char *engName, const UTF8Char *chiName) = 0;
			virtual Bool LocationAdd(Int32 locId, const UTF8Char *engName, const UTF8Char *chiName) = 0;
			OSInt LocationGetTypeIndex(Int32 lType);

			SpeciesInfo *GetSpeciesInfo(Int32 speciesId, Bool createNew);
			//void UpgradeFileStruct(OrganSpecies *sp);
		protected:
			virtual void BooksInit() = 0;
			void BooksDeinit();

		public:
			virtual Media::ImageList *ParseImage(OrganImageItem *img, UserFileInfo **userFile, WebFileInfo **wfile) = 0;
			virtual Media::ImageList *ParseSpImage(OrganSpecies *sp) = 0;
			virtual Media::ImageList *ParseFileImage(UserFileInfo *userFile) = 0;
			virtual Media::ImageList *ParseWebImage(WebFileInfo *webFile) = 0;
			const UTF8Char *GetLocName(Int32 userId, Data::DateTime *dt, UI::GUIForm *ownerFrm, UI::GUICore *ui);
			virtual OrganGroup *SearchObject(const UTF8Char *searchStr, UTF8Char *resultStr, UOSInt resultStrBuffSize, Int32 *parentId) = 0;

			void SetCurrCategory(Category *currCate);
		protected:
			virtual void LoadGroupTypes() = 0;
			void FreeCategory(Category *cate);
			Media::EXIFData *ParseJPGExif(const UTF8Char *fileName);
			Media::EXIFData *ParseTIFExif(const UTF8Char *fileName);

		public:
			void ExportWeb(const UTF8Char *exportDir, Bool includeWebPhoto, Bool includeNoPhoto, Int32 locId, UOSInt *photoCnt, UOSInt     *speciesCnt);
		private:
			virtual Data::Int32Map<Data::ArrayList<OrganGroup*>*> *GetGroupTree() = 0;
			void FreeGroupTree(Data::Int32Map<Data::ArrayList<OrganGroup*>*> *grpTree);
			virtual Data::Int32Map<Data::ArrayList<OrganSpecies*>*> *GetSpeciesTree() = 0;
			void FreeSpeciesTree(Data::Int32Map<Data::ArrayList<OrganSpecies*>*> *spTree);

			void ExportBeginPage(IO::Writer *writer, const UTF8Char *title);
			void ExportEndPage(IO::Writer *writer);
			void ExportGroup(OrganGroup *grp, Data::Int32Map<Data::ArrayList<OrganGroup*>*> *grpTree, Data::Int32Map<Data::ArrayList<OrganSpecies*>*> *spTree, const UTF8Char *backURL, UTF8Char *fullPath, UTF8Char *pathAppend, Bool includeWebPhoto, Bool includeNoPhoto, Int32 locId, UOSInt *photoCnt, UOSInt *speciesCnt, UOSInt *phSpeciesCnt);
			Bool ExportSpecies(OrganSpecies *sp, const UTF8Char *backURL, UTF8Char *fullPath, UTF8Char *pathAppend, Bool includeWebPhoto, Bool includeNoPhoto, Int32 locId, UOSInt *photoCnt, Bool *hasMyPhoto);////////////////////
		public:
			virtual void Test() = 0;

			Double GetMonitorHDPI(void *hMonitor);
			Double GetMonitorDDPI(void *hMonitor);
			void SetMonitorHDPI(void *hMonitor, Double monitorHDPI);

			virtual void ExportLite(const UTF8Char *folder) = 0;
		};
	}
}
#endif
