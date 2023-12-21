#ifndef _SM_SSWR_ORGANMGR_ORGANENV
#define _SM_SSWR_ORGANMGR_ORGANENV

#include "Data/ArrayListInt64.h"
#include "Data/Comparator.h"
#include "Data/Timestamp.h"
#include "IO/ConfigFile.h"
#include "IO/Writer.h"
#include "DB/DBTool.h"
#include "Map/GPSTrack.h"
#include "Math/Coord2DDbl.h"
#include "Media/ColorManager.h"
#include "Media/ImageList.h"
#include "Media/MonitorMgr.h"
#include "Net/SSLEngine.h"
#include "Parser/FullParserList.h"
#include "SSWR/OrganMgr/OrganGroup.h"
#include "SSWR/OrganMgr/OrganGroupType.h"
#include "SSWR/OrganMgr/OrganBook.h"
#include "SSWR/OrganMgr/OrganSpecies.h"
#include "SSWR/OrganMgr/OrganTripList.h"
#include "Text/CString.h"
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

		enum class LocType
		{
			Unknown,
			GPSTrack,
			PhotoExif,
			UserInput
		};

		typedef struct
		{
			Int32 cateId;
			NotNullPtr<Text::String> chiName;
			NotNullPtr<Text::String> dirName;
			Optional<Text::String> srcDir;
		} Category;

		typedef struct
		{
			Int32 id;
			NotNullPtr<Text::String> dispName;
			OrganBook *book;
		} SpeciesBook;

		typedef struct
		{
			Int32 id;
			NotNullPtr<Text::String> userName;
			NotNullPtr<Text::String> watermark;
			UserType userType;
		} OrganWebUser;

		typedef struct
		{
			Int32 id;
			Int32 fileType;
			Data::Timestamp startTime;
			Data::Timestamp endTime;
			NotNullPtr<Text::String> oriFileName;
			NotNullPtr<Text::String> fileName;
			Int32 webUserId;
		} DataFileInfo;

		struct UserFileInfo
		{
			Int32 id;
			Int32 fileType;
			NotNullPtr<Text::String> oriFileName;
			Data::Timestamp fileTime;
			Double lat;
			Double lon;
			Int32 webuserId;
			Int32 speciesId;
			Data::Timestamp captureTime;
			NotNullPtr<Text::String> dataFileName;
			UInt32 crcVal;
			Int32 rotType;
			Optional<Text::String> camera;
			Optional<Text::String> descript;
			Double cropLeft;
			Double cropTop;
			Double cropRight;
			Double cropBottom;
			Optional<Text::String> location;
			LocType locType;
		};

		typedef struct
		{
			Int32 id;
			Int32 speciesId;
			UInt32 crcVal;
			NotNullPtr<Text::String> imgUrl;
			NotNullPtr<Text::String> srcUrl;
			NotNullPtr<Text::String> location;
			Double cropLeft;
			Double cropTop;
			Double cropRight;
			Double cropBottom;
		} WebFileInfo;

		class SpeciesInfo
		{
		public:
			Int32 id;
			Data::ArrayList<UserFileInfo*> files;
			Data::FastMap<Int32, WebFileInfo*> wfileMap;
		};

		struct WebUserInfo
		{
			Int32 id;
			Data::SortableArrayListNative<Data::Timestamp> gpsFileIndex;
			Data::ArrayList<DataFileInfo*> gpsFileObj;
			Data::SortableArrayListNative<Data::Timestamp> userFileIndex;
			Data::ArrayList<UserFileInfo*> userFileObj;
		};

		class UserFileComparator : public Data::Comparator<UserFileInfo*>
		{
		public:
			virtual ~UserFileComparator();

			virtual OSInt Compare(UserFileInfo *a, UserFileInfo *b) const;
		};

		class UserFileTimeComparator : public Data::Comparator<UserFileInfo*>
		{
		public:
			virtual ~UserFileTimeComparator();

			virtual OSInt Compare(UserFileInfo *a, UserFileInfo *b) const;
		};

		class UserFileSpeciesComparator : public Data::Comparator<UserFileInfo*>
		{
		public:
			virtual ~UserFileSpeciesComparator();

			virtual OSInt Compare(UserFileInfo *a, UserFileInfo *b) const;
		};

		class WebFileSpeciesComparator : public Data::Comparator<WebFileInfo*>
		{
		public:
			virtual ~WebFileSpeciesComparator();

			virtual OSInt Compare(WebFileInfo *a, WebFileInfo *b) const;
		};

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
			Media::ColorManager colorMgr;
			Parser::FullParserList parsers;
			NotNullPtr<Media::DrawEngine> drawEng;
			NotNullPtr<Net::SocketFactory> sockf;
			Optional<Net::SSLEngine> ssl;
			Media::MonitorMgr monMgr;
			ErrorType errType;
			IO::ConfigFile *langFile;
			Data::ArrayList<Category*> categories;
			Data::ArrayList<OrganGroupType*> grpTypes;
			Category *currCate;
			Bool cateIsFullDir;
			Data::ArrayListInt32 *bookIds;
			Data::ArrayList<OrganBook*> *bookObjs;
			Data::ArrayList<DataFileInfo*> dataFiles;
			Int32 userId;
			Data::FastMap<Int32, SpeciesInfo*> speciesMap;
			Data::FastMap<Int32, UserFileInfo*> userFileMap;
			Data::FastMap<Int32, WebUserInfo*> userMap;

			Data::ArrayList<Trip*> trips;
			Data::ArrayList<Location*> locs;
			Data::ArrayList<LocationType*> locType;

			Data::Timestamp gpsStartTime;
			Data::Timestamp gpsEndTime;
			Map::GPSTrack *gpsTrk;
			Int32 gpsUserId;

		public:
			OrganEnv();
			virtual ~OrganEnv();

			NotNullPtr<Media::DrawEngine> GetDrawEngine();
			NotNullPtr<Parser::ParserList> GetParserList();
			NotNullPtr<Net::SocketFactory> GetSocketFactory();
			Optional<Net::SSLEngine> GetSSLEngine();
			Media::ColorManager *GetColorMgr();
			Media::MonitorMgr *GetMonitorMgr();
			ErrorType GetErrorType();
			Text::CStringNN GetLang(Text::CStringNN name);
			virtual Text::String *GetCacheDir() = 0;
			virtual Text::CString GetMapFont() = 0;

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
			virtual Bool IsBookSpeciesExist(const UTF8Char *sName, NotNullPtr<Text::StringBuilderUTF8> sb) = 0;
			virtual Bool AddSpecies(OrganSpecies *sp) = 0;
			virtual Bool DelSpecies(OrganSpecies *sp) = 0;
			virtual FileStatus AddSpeciesFile(OrganSpecies *sp, Text::CStringNN fileName, Bool firstPhoto, Bool moveFile, Int32 *fileId) = 0;
			virtual FileStatus AddSpeciesWebFile(OrganSpecies *sp, NotNullPtr<Text::String> srcURL, NotNullPtr<Text::String> imgURL, IO::Stream *stm, UTF8Char *webFileName) = 0;
			virtual Bool UpdateSpeciesWebFile(OrganSpecies *sp, WebFileInfo *wfile, Text::String *srcURL, Text::String *location) = 0;
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
			virtual UOSInt GetSpeciesBooks(Data::ArrayList<SpeciesBook*> *items, Int32 speciesId) = 0;
			virtual void ReleaseSpeciesBooks(Data::ArrayList<SpeciesBook*> *items) = 0;
			virtual Int32 NewBook(Text::CString title, Text::CString author, Text::CString press, const Data::Timestamp &publishDate, Text::CString url) = 0;

			WebUserInfo *GetWebUser(Int32 userId);
			virtual Bool AddDataFile(Text::CStringNN fileName) = 0;
			Data::ArrayList<DataFileInfo*> *GetDataFiles();
			virtual Bool DelDataFile(DataFileInfo *dataFile) = 0;
			void ReleaseDataFile(DataFileInfo *dataFile);
			virtual Bool GetGPSPos(Int32 userId, const Data::Timestamp &ts, OutParam<Math::Coord2DDbl> pos) = 0;
			virtual Map::GPSTrack *OpenGPSTrack(DataFileInfo *dataFile) = 0;

			void ReleaseSpecies(SpeciesInfo *species);
			void ReleaseUserFile(UserFileInfo *userFile);
			virtual void UpdateUserFileCrop(UserFileInfo *userFile, Double cropLeft, Double cropTop, Double cropRight, Double cropBottom) = 0;
			virtual void UpdateUserFileRot(UserFileInfo *userFile, Int32 rotType) = 0;
			virtual Bool UpdateUserFilePos(UserFileInfo *userFile, const Data::Timestamp &captureTime, Double lat, Double lon) = 0;
			UOSInt GetUserFiles(Data::ArrayList<UserFileInfo*> *userFiles, const Data::Timestamp &fromTime, const Data::Timestamp &toTime);
			virtual Bool GetUserFilePath(UserFileInfo *userFile, NotNullPtr<Text::StringBuilderUTF8> sb) = 0;
			virtual Bool UpdateUserFileDesc(UserFileInfo *userFile, const UTF8Char *descript) = 0;
			virtual Bool UpdateUserFileLoc(UserFileInfo *userFile, const UTF8Char *location) = 0;
			virtual void UpdateWebFileCrop(WebFileInfo *userFile, Double cropLeft, Double cropTop, Double cropRight, Double cropBottom) = 0;

		protected:
			void TripRelease();
			virtual void TripReload(Int32 cateId) = 0;
		public:
			OSInt TripGetIndex(const Data::Timestamp &ts);
			Trip *TripGet(Int32 userId, const Data::Timestamp &ts);
			Data::ArrayList<Trip*> *TripGetList();
			virtual Bool TripAdd(const Data::Timestamp &fromDate, const Data::Timestamp &toDate, Int32 locId) = 0;

			OSInt LocationGetIndex(Int32 locId);
			Location *LocationGet(Int32 locId);
			Data::ArrayList<Location*> *LocationGetSub(Int32 locId);
			virtual Bool LocationUpdate(Int32 locId, Text::CString engName, Text::CString chiName) = 0;
			virtual Bool LocationAdd(Int32 locId, Text::CString engName, Text::CString chiName) = 0;
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
			Text::String *GetLocName(Int32 userId, const Data::Timestamp &ts, UI::GUIForm *ownerFrm, NotNullPtr<UI::GUICore> ui);
			virtual OrganGroup *SearchObject(const UTF8Char *searchStr, UTF8Char *resultStr, UOSInt resultStrBuffSize, Int32 *parentId) = 0;

			void SetCurrCategory(Category *currCate);
		protected:
			virtual void LoadGroupTypes() = 0;
			void FreeCategory(Category *cate);
			Media::EXIFData *ParseJPGExif(Text::CStringNN fileName);
			Media::EXIFData *ParseTIFExif(Text::CStringNN fileName);

		public:
			void ExportWeb(const UTF8Char *exportDir, Bool includeWebPhoto, Bool includeNoPhoto, Int32 locId, UOSInt *photoCnt, UOSInt     *speciesCnt);
		private:
			virtual Data::FastMap<Int32, Data::ArrayList<OrganGroup*>*> *GetGroupTree() = 0;
			void FreeGroupTree(Data::FastMap<Int32, Data::ArrayList<OrganGroup*>*> *grpTree);
			virtual Data::FastMap<Int32, Data::ArrayList<OrganSpecies*>*> *GetSpeciesTree() = 0;
			void FreeSpeciesTree(Data::FastMap<Int32, Data::ArrayList<OrganSpecies*>*> *spTree);

			void ExportBeginPage(IO::Writer *writer, const UTF8Char *title);
			void ExportEndPage(IO::Writer *writer);
			void ExportGroup(OrganGroup *grp, Data::FastMap<Int32, Data::ArrayList<OrganGroup*>*> *grpTree, Data::FastMap<Int32, Data::ArrayList<OrganSpecies*>*> *spTree, const UTF8Char *backURL, UTF8Char *fullPath, UTF8Char *pathAppend, Bool includeWebPhoto, Bool includeNoPhoto, Int32 locId, UOSInt *photoCnt, UOSInt *speciesCnt, UOSInt *phSpeciesCnt);
			Bool ExportSpecies(OrganSpecies *sp, const UTF8Char *backURL, UTF8Char *fullPath, UTF8Char *pathAppend, Bool includeWebPhoto, Bool includeNoPhoto, Int32 locId, UOSInt *photoCnt, Bool *hasMyPhoto);////////////////////
		public:
			virtual void Test() = 0;

			Double GetMonitorHDPI(MonitorHandle *hMonitor);
			Double GetMonitorDDPI(MonitorHandle *hMonitor);
			void SetMonitorHDPI(MonitorHandle *hMonitor, Double monitorHDPI);

			virtual void ExportLite(const UTF8Char *folder) = 0;
		};
	}
}
#endif
