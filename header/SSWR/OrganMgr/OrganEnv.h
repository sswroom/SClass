#ifndef _SM_SSWR_ORGANMGR_ORGANENV
#define _SM_SSWR_ORGANMGR_ORGANENV

#include "Data/ArrayListInt64.h"
#include "Data/Comparator.hpp"
#include "Data/FastMapNN.hpp"
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
			NN<Text::String> chiName;
			NN<Text::String> dirName;
			Optional<Text::String> srcDir;
		} Category;

		typedef struct
		{
			Int32 id;
			NN<Text::String> dispName;
			NN<OrganBook> book;
		} SpeciesBook;

		typedef struct
		{
			Int32 id;
			NN<Text::String> userName;
			NN<Text::String> watermark;
			UserType userType;
		} OrganWebUser;

		typedef struct
		{
			Int32 id;
			Int32 fileType;
			Data::Timestamp startTime;
			Data::Timestamp endTime;
			NN<Text::String> oriFileName;
			NN<Text::String> fileName;
			Int32 webUserId;
		} DataFileInfo;

		struct UserFileInfo
		{
			Int32 id;
			Int32 fileType;
			NN<Text::String> oriFileName;
			Data::Timestamp fileTime;
			Double lat;
			Double lon;
			Int32 webuserId;
			Int32 speciesId;
			Data::Timestamp captureTime;
			NN<Text::String> dataFileName;
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
			NN<Text::String> imgUrl;
			NN<Text::String> srcUrl;
			NN<Text::String> location;
			Double cropLeft;
			Double cropTop;
			Double cropRight;
			Double cropBottom;
		} WebFileInfo;

		class SpeciesInfo
		{
		public:
			Int32 id;
			Data::ArrayListNN<UserFileInfo> files;
			Data::FastMapNN<Int32, WebFileInfo> wfileMap;
		};

		struct WebUserInfo
		{
			Int32 id;
			Data::SortableArrayListNative<Data::Timestamp> gpsFileIndex;
			Data::ArrayListNN<DataFileInfo> gpsFileObj;
			Data::SortableArrayListNative<Data::Timestamp> userFileIndex;
			Data::ArrayListNN<UserFileInfo> userFileObj;
		};

		class UserFileComparator : public Data::Comparator<NN<UserFileInfo>>
		{
		public:
			virtual ~UserFileComparator();

			virtual OSInt Compare(NN<UserFileInfo> a, NN<UserFileInfo> b) const;
		};

		class UserFileTimeComparator : public Data::Comparator<NN<UserFileInfo>>
		{
		public:
			virtual ~UserFileTimeComparator();

			virtual OSInt Compare(NN<UserFileInfo> a, NN<UserFileInfo> b) const;
		};

		class UserFileSpeciesComparator : public Data::Comparator<NN<UserFileInfo>>
		{
		public:
			virtual ~UserFileSpeciesComparator();

			virtual OSInt Compare(NN<UserFileInfo> a, NN<UserFileInfo> b) const;
		};

		class WebFileSpeciesComparator : public Data::Comparator<NN<WebFileInfo>>
		{
		public:
			virtual ~WebFileSpeciesComparator();

			virtual OSInt Compare(NN<WebFileInfo> a, NN<WebFileInfo> b) const;
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
			NN<Media::DrawEngine> drawEng;
			NN<Net::SocketFactory> sockf;
			NN<Net::TCPClientFactory> clif;
			Optional<Net::SSLEngine> ssl;
			Media::MonitorMgr monMgr;
			ErrorType errType;
			Optional<IO::ConfigFile> langFile;
			Data::ArrayListNN<Category> categories;
			Data::ArrayListNN<OrganGroupType> grpTypes;
			Category *currCate;
			Bool cateIsFullDir;
			Data::ArrayListInt32 *bookIds;
			Data::ArrayListNN<OrganBook> *bookObjs;
			Data::ArrayListNN<DataFileInfo> dataFiles;
			Int32 userId;
			Data::FastMapNN<Int32, SpeciesInfo> speciesMap;
			Data::FastMapNN<Int32, UserFileInfo> userFileMap;
			Data::FastMapNN<Int32, WebUserInfo> userMap;

			Data::ArrayListNN<Trip> trips;
			Data::ArrayListNN<Location> locs;
			Data::ArrayListNN<LocationType> locType;

			Data::Timestamp gpsStartTime;
			Data::Timestamp gpsEndTime;
			Optional<Map::GPSTrack> gpsTrk;
			Int32 gpsUserId;

		public:
			OrganEnv();
			virtual ~OrganEnv();

			NN<Media::DrawEngine> GetDrawEngine();
			NN<Parser::ParserList> GetParserList();
			NN<Net::SocketFactory> GetSocketFactory();
			NN<Net::TCPClientFactory> GetTCPClientFactory();
			Optional<Net::SSLEngine> GetSSLEngine();
			NN<Media::ColorManager> GetColorManager();
			NN<Media::MonitorMgr> GetMonitorMgr();
			ErrorType GetErrorType();
			Text::CStringNN GetLang(Text::CStringNN name);
			virtual Text::String *GetCacheDir() = 0;
			virtual Text::CStringNN GetMapFont() = 0;

			UOSInt GetCategories(NN<Data::ArrayListNN<Category>> categories);
			virtual UOSInt GetGroupItems(NN<Data::ArrayListNN<OrganGroupItem>> items, Optional<OrganGroup> grp) = 0;
			virtual UOSInt GetGroupImages(NN<Data::ArrayListNN<OrganImageItem>> items, NN<OrganGroup> grp) = 0;
			virtual UOSInt GetSpeciesImages(NN<Data::ArrayListNN<OrganImageItem>> items, NN<OrganSpecies> sp) = 0;
			virtual UOSInt GetGroupAllSpecies(NN<Data::ArrayListNN<OrganSpecies>> items, Optional<OrganGroup> grp) = 0;
			virtual UOSInt GetGroupAllUserFile(NN<Data::ArrayListNN<UserFileInfo>> items, NN<Data::ArrayList<UInt32>> colors, Optional<OrganGroup> grp) = 0;
			virtual UOSInt GetSpeciesItems(NN<Data::ArrayListNN<OrganGroupItem>> items, NN<Data::ArrayList<Int32>> speciesIds) = 0;
			NN<Data::ArrayListNN<OrganGroupType>> GetGroupTypes();
			virtual Optional<OrganGroup> GetGroup(Int32 groupId, OutParam<Int32> parentId) = 0;
			virtual Optional<OrganSpecies> GetSpecies(Int32 speciesId) = 0;
//			OrganTripList *GetTripList();
			virtual UnsafeArray<UTF8Char> GetSpeciesDir(NN<OrganSpecies> sp, UnsafeArray<UTF8Char> sbuff) = 0; /////////////////
//			virtual Bool CreateSpeciesDir(OrganSpecies *sp) = 0; ////////////////////////
			virtual Bool IsSpeciesExist(UnsafeArray<const UTF8Char> sName) = 0;
			virtual Bool IsBookSpeciesExist(UnsafeArray<const UTF8Char> sName, NN<Text::StringBuilderUTF8> sb) = 0;
			virtual Bool AddSpecies(NN<OrganSpecies> sp) = 0;
			virtual Bool DelSpecies(NN<OrganSpecies> sp) = 0;
			virtual FileStatus AddSpeciesFile(NN<OrganSpecies> sp, Text::CStringNN fileName, Bool firstPhoto, Bool moveFile, OptOut<Int32> fileId) = 0;
			virtual FileStatus AddSpeciesWebFile(NN<OrganSpecies> sp, NN<Text::String> srcURL, NN<Text::String> imgURL, IO::Stream *stm, UnsafeArrayOpt<UTF8Char> webFileName) = 0;
			virtual Bool UpdateSpeciesWebFile(NN<OrganSpecies> sp, NN<WebFileInfo> wfile, Text::String *srcURL, Text::String *location) = 0;
			Bool SetSpeciesImg(NN<OrganSpecies> sp, NN<OrganImageItem> img);
			Bool SetSpeciesMapColor(NN<OrganSpecies> sp, UInt32 mapColor);
			virtual Bool SaveSpecies(NN<OrganSpecies> sp) = 0;
			virtual Bool SaveGroup(NN<OrganGroup> grp) = 0;
			virtual UOSInt GetGroupCount(Int32 groupId) = 0;
			virtual UOSInt GetSpeciesCount(Int32 groupId) = 0;
			virtual Bool AddGroup(NN<OrganGroup> grp, Int32 parGroupId) = 0;
			virtual Bool DelGroup(Int32 groupId) = 0;
			virtual Bool SetGroupDefSp(NN<OrganGroup> grp, NN<OrganImageItem> img) = 0;

			virtual Bool MoveGroups(NN<Data::ArrayListNN<OrganGroup>> grpList, NN<OrganGroup> destGroup) = 0;
			virtual Bool MoveSpecies(NN<Data::ArrayListNN<OrganSpecies>> spList, NN<OrganGroup> destGroup) = 0;
			virtual Bool MoveImages(NN<Data::ArrayListNN<OrganImages>> imgList, NN<OrganSpecies> destSp, NN<UI::GUIForm> frm) = 0;
			virtual Bool CombineSpecies(NN<OrganSpecies> destSp, NN<OrganSpecies> srcSp) = 0;

			virtual UOSInt GetWebUsers(NN<Data::ArrayListNN<OrganWebUser>> userList) = 0;
			virtual Bool AddWebUser(UnsafeArray<const UTF8Char> userName, UnsafeArray<const UTF8Char> pwd, UnsafeArray<const UTF8Char> watermark, UserType userType) = 0;
			virtual Bool ModifyWebUser(Int32 id, UnsafeArray<const UTF8Char> userName, UnsafeArrayOpt<const UTF8Char> pwd, UnsafeArray<const UTF8Char> watermark) = 0;
			virtual void ReleaseWebUsers(NN<Data::ArrayListNN<OrganWebUser>> userList) = 0;

			UOSInt GetBooksAll(NN<Data::ArrayListNN<OrganBook>> items);
			UOSInt GetBooksOfYear(NN<Data::ArrayListNN<OrganBook>> items, Int32 year);
			virtual Bool IsSpeciesBookExist(Int32 speciesId, Int32 bookId) = 0;
			virtual Bool NewSpeciesBook(Int32 speciesId, Int32 bookId, UnsafeArray<const UTF8Char> dispName) = 0;
			virtual UOSInt GetSpeciesBooks(NN<Data::ArrayListNN<SpeciesBook>> items, Int32 speciesId) = 0;
			virtual void ReleaseSpeciesBooks(NN<Data::ArrayListNN<SpeciesBook>> items) = 0;
			virtual Int32 NewBook(Text::CString title, Text::CString author, Text::CString press, const Data::Timestamp &publishDate, Text::CString url) = 0;

			NN<WebUserInfo> GetWebUser(Int32 userId);
			virtual Bool AddDataFile(Text::CStringNN fileName) = 0;
			NN<Data::ArrayListNN<DataFileInfo>> GetDataFiles();
			virtual Bool DelDataFile(NN<DataFileInfo> dataFile) = 0;
			static void __stdcall ReleaseDataFile(NN<DataFileInfo> dataFile);
			virtual Bool GetGPSPos(Int32 userId, const Data::Timestamp &ts, OutParam<Math::Coord2DDbl> pos) = 0;
			virtual Optional<Map::GPSTrack> OpenGPSTrack(NN<DataFileInfo> dataFile) = 0;

			static void __stdcall ReleaseSpecies(NN<SpeciesInfo> species);
			static void __stdcall ReleaseUserFile(NN<UserFileInfo> userFile);
			virtual void UpdateUserFileCrop(NN<UserFileInfo> userFile, Double cropLeft, Double cropTop, Double cropRight, Double cropBottom) = 0;
			virtual void UpdateUserFileRot(NN<UserFileInfo> userFile, Int32 rotType) = 0;
			virtual Bool UpdateUserFilePos(NN<UserFileInfo> userFile, const Data::Timestamp &captureTime, Double lat, Double lon) = 0;
			UOSInt GetUserFiles(NN<Data::ArrayListNN<UserFileInfo>> userFiles, const Data::Timestamp &fromTime, const Data::Timestamp &toTime);
			virtual Bool GetUserFilePath(NN<UserFileInfo> userFile, NN<Text::StringBuilderUTF8> sb) = 0;
			virtual Bool UpdateUserFileDesc(NN<UserFileInfo> userFile, UnsafeArray<const UTF8Char> descript) = 0;
			virtual Bool UpdateUserFileLoc(NN<UserFileInfo> userFile, UnsafeArray<const UTF8Char> location) = 0;
			virtual void UpdateWebFileCrop(NN<WebFileInfo> userFile, Double cropLeft, Double cropTop, Double cropRight, Double cropBottom) = 0;

		protected:
			void TripRelease();
			virtual void TripReload(Int32 cateId) = 0;
		public:
			OSInt TripGetIndex(const Data::Timestamp &ts);
			Optional<Trip> TripGet(Int32 userId, const Data::Timestamp &ts);
			NN<Data::ArrayListNN<Trip>> TripGetList();
			virtual Bool TripAdd(const Data::Timestamp &fromDate, const Data::Timestamp &toDate, Int32 locId) = 0;

			OSInt LocationGetIndex(Int32 locId);
			Optional<Location> LocationGet(Int32 locId);
			NN<Data::ArrayListNN<Location>> LocationGetSub(Int32 locId);
			virtual Bool LocationUpdate(Int32 locId, Text::CStringNN engName, Text::CStringNN chiName) = 0;
			virtual Bool LocationAdd(Int32 locId, Text::CStringNN engName, Text::CStringNN chiName) = 0;
			OSInt LocationGetTypeIndex(Int32 lType);

			Optional<SpeciesInfo> GetSpeciesInfo(Int32 speciesId);
			NN<SpeciesInfo> GetSpeciesInfoCreate(Int32 speciesId);
			//void UpgradeFileStruct(OrganSpecies *sp);
		protected:
			virtual void BooksInit() = 0;
			void BooksDeinit();

		public:
			virtual Optional<Media::ImageList> ParseImage(NN<OrganImageItem> img, OptOut<Optional<UserFileInfo>> userFile, OptOut<Optional<WebFileInfo>> wfile) = 0;
			virtual Optional<Media::ImageList> ParseSpImage(NN<OrganSpecies> sp) = 0;
			virtual Optional<Media::ImageList> ParseFileImage(NN<UserFileInfo> userFile) = 0;
			virtual Optional<Media::ImageList> ParseWebImage(NN<WebFileInfo> webFile) = 0;
			Optional<Text::String> GetLocName(Int32 userId, const Data::Timestamp &ts, UI::GUIForm *ownerFrm, NN<UI::GUICore> ui);
			virtual OrganGroup *SearchObject(UnsafeArray<const UTF8Char> searchStr, UnsafeArray<UTF8Char> resultStr, UOSInt resultStrBuffSize, Int32 *parentId) = 0;

			void SetCurrCategory(NN<Category> currCate);
		protected:
			virtual void LoadGroupTypes() = 0;
			static void __stdcall FreeCategory(NN<Category> cate);
			Optional<Media::EXIFData> ParseJPGExif(Text::CStringNN fileName);
			Optional<Media::EXIFData> ParseTIFExif(Text::CStringNN fileName);

		public:
			void ExportWeb(UnsafeArray<const UTF8Char> exportDir, Bool includeWebPhoto, Bool includeNoPhoto, Int32 locId, UOSInt *photoCnt, UOSInt     *speciesCnt);
		private:
			virtual NN<Data::FastMapNN<Int32, Data::ArrayListNN<OrganGroup>>> GetGroupTree() = 0;
			void FreeGroupTree(NN<Data::FastMapNN<Int32, Data::ArrayListNN<OrganGroup>>> grpTree);
			virtual NN<Data::FastMapNN<Int32, Data::ArrayListNN<OrganSpecies>>> GetSpeciesTree() = 0;
			void FreeSpeciesTree(NN<Data::FastMapNN<Int32, Data::ArrayListNN<OrganSpecies>>> spTree);

			void ExportBeginPage(NN<IO::Writer> writer, UnsafeArray<const UTF8Char> title);
			void ExportEndPage(NN<IO::Writer> writer);
			void ExportGroup(NN<OrganGroup> grp, NN<Data::FastMapNN<Int32, Data::ArrayListNN<OrganGroup>>> grpTree, NN<Data::FastMapNN<Int32, Data::ArrayListNN<OrganSpecies>>> spTree, UnsafeArray<const UTF8Char> backURL, UnsafeArray<UTF8Char> fullPath, UnsafeArray<UTF8Char> pathAppend, Bool includeWebPhoto, Bool includeNoPhoto, Int32 locId, UOSInt *photoCnt, UOSInt *speciesCnt, UOSInt *phSpeciesCnt);
			Bool ExportSpecies(NN<OrganSpecies> sp, UnsafeArray<const UTF8Char> backURL, UnsafeArray<UTF8Char> fullPath, UnsafeArray<UTF8Char> pathAppend, Bool includeWebPhoto, Bool includeNoPhoto, Int32 locId, OutParam<UOSInt> photoCnt, OutParam<Bool> hasMyPhoto);////////////////////
		public:
			virtual void Test() = 0;

			Double GetMonitorHDPI(Optional<MonitorHandle> hMonitor);
			Double GetMonitorDDPI(Optional<MonitorHandle> hMonitor);
			void SetMonitorHDPI(Optional<MonitorHandle> hMonitor, Double monitorHDPI);

			virtual void ExportLite(UnsafeArray<const UTF8Char> folder) = 0;
		};
	}
}
#endif
