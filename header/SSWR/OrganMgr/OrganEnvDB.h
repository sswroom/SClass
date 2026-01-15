#ifndef _SM_SSWR_ORGANMGR_ORGANENVDB
#define _SM_SSWR_ORGANMGR_ORGANENVDB

#include "Data/ArrayListInt64.h"
#include "IO/ConfigFile.h"
#include "DB/DBTool.h"
#include "Map/GPSTrack.h"
#include "Media/ColorManager.h"
#include "Media/ImageList.h"
#include "Media/MonitorMgr.h"
#include "Parser/ParserList.h"
#include "SSWR/OrganMgr/OrganEnv.h"
#include "UI/GUIForm.h"

namespace SSWR
{
	namespace OrganMgr
	{
		class OrganEnvDB : public OrganEnv
		{
		private:
			Optional<DB::DBTool> db;
			IO::LogTool log;
			Optional<IO::ConfigFile> cfg;
			Text::String *cfgImgDirBase;
			Text::String *cfgDataPath;
			Text::String *cfgCacheDir;
			Text::String *cfgMapFont;

		public:
			OrganEnvDB();
			virtual ~OrganEnvDB();

			Text::String *GetDataDir();
			virtual Text::String *GetCacheDir();
			virtual Text::CStringNN GetMapFont();

			virtual UOSInt GetGroupItems(NN<Data::ArrayListNN<OrganGroupItem>> items, Optional<OrganGroup> grp);
			virtual UOSInt GetGroupImages(NN<Data::ArrayListNN<OrganImageItem>> items, NN<OrganGroup> grp);
			virtual UOSInt GetSpeciesImages(NN<Data::ArrayListNN<OrganImageItem>> items, NN<OrganSpecies> sp);
			virtual UOSInt GetGroupAllSpecies(NN<Data::ArrayListNN<OrganSpecies>> items, Optional<OrganGroup> grp);
			virtual UOSInt GetGroupAllUserFile(NN<Data::ArrayListNN<UserFileInfo>> items, NN<Data::ArrayListNative<UInt32>> colors, Optional<OrganGroup> grp);
			virtual UOSInt GetSpeciesItems(NN<Data::ArrayListNN<OrganGroupItem>> items, NN<Data::ArrayListNative<Int32>> speciesIds);
			virtual Optional<OrganGroup> GetGroup(Int32 groupId, OutParam<Int32> parentId);
			virtual Optional<OrganSpecies> GetSpecies(Int32 speciesId);
			virtual UnsafeArray<UTF8Char> GetSpeciesDir(NN<OrganSpecies> sp, UnsafeArray<UTF8Char> sbuff);
			virtual Bool CreateSpeciesDir(NN<OrganSpecies> sp);
			virtual Bool IsSpeciesExist(UnsafeArray<const UTF8Char> sName);
			virtual Bool IsBookSpeciesExist(UnsafeArray<const UTF8Char> sName, NN<Text::StringBuilderUTF8> sb);
			virtual Bool AddSpecies(NN<OrganSpecies> sp);
			virtual Bool DelSpecies(NN<OrganSpecies> sp);
			virtual FileStatus AddSpeciesFile(NN<OrganSpecies> sp, Text::CStringNN fileName, Bool firstPhoto, Bool moveFile, OptOut<Int32> fileId);
			virtual FileStatus AddSpeciesWebFile(NN<OrganSpecies> sp, NN<Text::String> srcURL, NN<Text::String> imgURL, IO::Stream *stm, UnsafeArrayOpt<UTF8Char> webFileName);
			FileStatus AddSpeciesWebFileOld(NN<OrganSpecies> sp, Text::String *srcURL, Text::String *imgURL, NN<IO::Stream> stm, UnsafeArrayOpt<UTF8Char> webFileName);
			virtual Bool UpdateSpeciesWebFile(NN<OrganSpecies> sp, NN<WebFileInfo> wfile, Text::String *srcURL, Text::String *location);
			Bool UpdateSpeciesWebFileOld(NN<OrganSpecies> sp, UnsafeArray<const UTF8Char> webFileName, UnsafeArray<const UTF8Char> srcURL);
			virtual Bool SaveSpecies(NN<OrganSpecies> sp);
			virtual Bool SaveGroup(NN<OrganGroup> grp);
			virtual UOSInt GetGroupCount(Int32 groupId);
			virtual UOSInt GetSpeciesCount(Int32 groupId);
			virtual Bool AddGroup(NN<OrganGroup> grp, Int32 parGroupId);
			virtual Bool DelGroup(Int32 groupId);
			virtual Bool SetGroupDefSp(NN<OrganGroup> grp, NN<OrganImageItem> img);

			virtual Bool MoveGroups(NN<Data::ArrayListNN<OrganGroup>> grpList, NN<OrganGroup> destGroup);
			virtual Bool MoveSpecies(NN<Data::ArrayListNN<OrganSpecies>> spList, NN<OrganGroup> destGroup);
			virtual Bool MoveImages(NN<Data::ArrayListNN<OrganImages>> imgList, NN<OrganSpecies> destSp, NN<UI::GUIForm> frm);
			virtual Bool CombineSpecies(NN<OrganSpecies> destSp, NN<OrganSpecies> srcSp);

			virtual UOSInt GetWebUsers(NN<Data::ArrayListNN<OrganWebUser>> userList);
			virtual Bool AddWebUser(UnsafeArray<const UTF8Char> userName, UnsafeArray<const UTF8Char> pwd, UnsafeArray<const UTF8Char> watermark, UserType userType);
			virtual Bool ModifyWebUser(Int32 id, UnsafeArray<const UTF8Char> userName, UnsafeArrayOpt<const UTF8Char> pwd, UnsafeArray<const UTF8Char> watermark);
			virtual void ReleaseWebUsers(NN<Data::ArrayListNN<OrganWebUser>> userList);

			virtual Bool IsSpeciesBookExist(Int32 speciesId, Int32 bookId);
			virtual Bool NewSpeciesBook(Int32 speciesId, Int32 bookId, UnsafeArray<const UTF8Char> dispName);
			virtual UOSInt GetSpeciesBooks(NN<Data::ArrayListNN<SpeciesBook>> items, Int32 speciesId);
			virtual void ReleaseSpeciesBooks(NN<Data::ArrayListNN<SpeciesBook>> items);
			virtual Int32 NewBook(Text::CString title, Text::CString author, Text::CString press, const Data::Timestamp &publishDate, Text::CString url);

			virtual Bool AddDataFile(Text::CStringNN fileName);
			virtual Bool DelDataFile(NN<DataFileInfo> dataFile);
			virtual Bool GetGPSPos(Int32 userId, const Data::Timestamp &ts, OutParam<Math::Coord2DDbl> pos);
			virtual Optional<Map::GPSTrack> OpenGPSTrack(NN<DataFileInfo> dataFile);

			virtual void UpdateUserFileCrop(NN<UserFileInfo> userFile, Double cropLeft, Double cropTop, Double cropRight, Double cropBottom);
			virtual void UpdateUserFileRot(NN<UserFileInfo> userFile, Int32 rotType);
			virtual Bool UpdateUserFilePos(NN<UserFileInfo> userFile, const Data::Timestamp &captureTime, Double lat, Double lon);
			virtual Bool GetUserFilePath(NN<UserFileInfo> userFile, NN<Text::StringBuilderUTF8> sb);
			virtual Bool UpdateUserFileDesc(NN<UserFileInfo> userFile, UnsafeArray<const UTF8Char> descript);
			virtual Bool UpdateUserFileLoc(NN<UserFileInfo> userFile, UnsafeArray<const UTF8Char> location);
			virtual void UpdateWebFileCrop(NN<WebFileInfo> webFile, Double cropLeft, Double cropTop, Double cropRight, Double cropBottom);

		protected:
			virtual void TripReload(Int32 cateId);
		public:
			virtual Bool TripAdd(const Data::Timestamp &fromDate, const Data::Timestamp &toDate, Int32 locId);
			virtual Bool LocationUpdate(Int32 locId, Text::CStringNN engName, Text::CStringNN chiName);
			virtual Bool LocationAdd(Int32 locId, Text::CStringNN engName, Text::CStringNN chiName);

		private:
			virtual void BooksInit();

		public:
			virtual Optional<Media::ImageList> ParseImage(NN<OrganImageItem> img, OptOut<Optional<UserFileInfo>> userFile, OptOut<Optional<WebFileInfo>> wfile);
			virtual Optional<Media::ImageList> ParseSpImage(NN<OrganSpecies> sp);
			virtual Optional<Media::ImageList> ParseFileImage(NN<UserFileInfo> userFile);
			virtual Optional<Media::ImageList> ParseWebImage(NN<WebFileInfo> webFile);
			virtual OrganGroup *SearchObject(UnsafeArray<const UTF8Char> searchStr, UnsafeArray<UTF8Char> resultStr, UOSInt resultStrBuffSize, Int32 *parentId);

		private:
			virtual void LoadGroupTypes();

		private:
			virtual NN<Data::FastMapNN<Int32, Data::ArrayListNN<OrganGroup>>> GetGroupTree();
			virtual NN<Data::FastMapNN<Int32, Data::ArrayListNN<OrganSpecies>>> GetSpeciesTree();

		public:
			virtual void Test();
			void UpgradeDB();
			void UpgradeDB2();
			void UpgradeFileStruct(NN<OrganSpecies> sp);

			virtual void ExportLite(UnsafeArray<const UTF8Char> folder);
		};
	}
}
#endif
