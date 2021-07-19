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
			DB::DBTool *db;
			IO::LogTool *log;
			IO::ConfigFile *cfg;
			const UTF8Char *cfgImgDirBase;
			const UTF8Char *cfgDataPath;
			const UTF8Char *cfgCacheDir;

		public:
			OrganEnvDB();
			virtual ~OrganEnvDB();

			const UTF8Char *GetDataDir();
			virtual const UTF8Char *GetCacheDir();

			ErrorType GetErrorType();
			virtual UOSInt GetGroupItems(Data::ArrayList<OrganGroupItem*> *items, OrganGroup *grp);
			virtual UOSInt GetGroupImages(Data::ArrayList<OrganImageItem*> *items, OrganGroup *grp);
			virtual UOSInt GetSpeciesImages(Data::ArrayList<OrganImageItem*> *items, OrganSpecies *sp);
			virtual UOSInt GetGroupAllSpecies(Data::ArrayList<OrganSpecies*> *items, OrganGroup *grp);
			virtual UOSInt GetGroupAllUserFile(Data::ArrayList<UserFileInfo*> *items, Data::ArrayList<UInt32> *colors, OrganGroup *grp);
			virtual UOSInt GetSpeciesItems(Data::ArrayList<OrganGroupItem*> *items, Data::ArrayList<Int32> *speciesIds);
			virtual OrganGroup *GetGroup(Int32 groupId, Int32 *parentId);
			virtual OrganSpecies *GetSpecies(Int32 speciesId);
			virtual UTF8Char *GetSpeciesDir(OrganSpecies *sp, UTF8Char *sbuff);
			virtual Bool CreateSpeciesDir(OrganSpecies *sp);
			virtual Bool IsSpeciesExist(const UTF8Char *sName);
			virtual Bool IsBookSpeciesExist(const UTF8Char *sName, Text::StringBuilderUTF *sb);
			virtual Bool AddSpecies(OrganSpecies *sp);
			virtual Bool DelSpecies(OrganSpecies *sp);
			virtual FileStatus AddSpeciesFile(OrganSpecies *sp, const UTF8Char *fileName, Bool firstPhoto, Bool moveFile, Int32 *fileId);
			virtual FileStatus AddSpeciesWebFile(OrganSpecies *sp, const UTF8Char *srcURL, const UTF8Char *imgURL, IO::Stream *stm, UTF8Char *webFileName);
			FileStatus AddSpeciesWebFileOld(OrganSpecies *sp, const UTF8Char *srcURL, const UTF8Char *imgURL, IO::Stream *stm, UTF8Char *webFileName);
			virtual Bool UpdateSpeciesWebFile(OrganSpecies *sp, WebFileInfo *wfile, const UTF8Char *srcURL, const UTF8Char *location);
			Bool UpdateSpeciesWebFileOld(OrganSpecies *sp, const UTF8Char *webFileName, const UTF8Char *srcURL);
			virtual Bool SaveSpecies(OrganSpecies *sp);
			virtual Bool SaveGroup(OrganGroup *grp);
			virtual Int32 GetGroupCount(Int32 groupId);
			virtual Int32 GetSpeciesCount(Int32 groupId);
			virtual Bool AddGroup(OrganGroup *grp, Int32 parGroupId);
			virtual Bool DelGroup(Int32 groupId);
			virtual Bool SetGroupDefSp(OrganGroup *grp, OrganImageItem *img);

			virtual Bool MoveGroups(Data::ArrayList<OrganGroup*> *grpList, OrganGroup *destGroup);
			virtual Bool MoveSpecies(Data::ArrayList<OrganSpecies*> *spList, OrganGroup *destGroup);
			virtual Bool MoveImages(Data::ArrayList<OrganImages*> *imgList, OrganSpecies *destSp, UI::GUIForm *frm);
			virtual Bool CombineSpecies(OrganSpecies *destSp, OrganSpecies *srcSp);

			virtual UOSInt GetWebUsers(Data::ArrayList<OrganWebUser*> *userList);
			virtual Bool AddWebUser(const UTF8Char *userName, const UTF8Char *pwd, const UTF8Char *watermark, UserType userType);
			virtual Bool ModifyWebUser(Int32 id, const UTF8Char *userName, const UTF8Char *pwd, const UTF8Char *watermark);
			virtual void ReleaseWebUsers(Data::ArrayList<OrganWebUser*> *userList);

			virtual Bool IsSpeciesBookExist(Int32 speciesId, Int32 bookId);
			virtual Bool NewSpeciesBook(Int32 speciesId, Int32 bookId, const UTF8Char *dispName);
			virtual OSInt GetSpeciesBooks(Data::ArrayList<SpeciesBook*> *items, Int32 speciesId);
			virtual void ReleaseSpeciesBooks(Data::ArrayList<SpeciesBook*> *items);
			virtual Int32 NewBook(const UTF8Char *title, const UTF8Char *author, const UTF8Char *press, Data::DateTime *publishDate, const UTF8Char *url);

			virtual Bool AddDataFile(const UTF8Char *fileName);
			virtual Bool DelDataFile(DataFileInfo *dataFile);
			virtual Bool GetGPSPos(Int32 userId, Data::DateTime *t, Double *lat, Double *lon);
			virtual Map::GPSTrack *OpenGPSTrack(DataFileInfo *dataFile);

			virtual void UpdateUserFileCrop(UserFileInfo *userFile, Double cropLeft, Double cropTop, Double cropRight, Double cropBottom);
			virtual void UpdateUserFileRot(UserFileInfo *userFile, Int32 rotType);
			virtual Bool UpdateUserFilePos(UserFileInfo *userFile, Data::DateTime *captureTime, Double lat, Double lon);
			virtual Bool GetUserFilePath(UserFileInfo *userFile, Text::StringBuilderUTF *sb);
			virtual Bool UpdateUserFileDesc(UserFileInfo *userFile, const UTF8Char *descript);
			virtual Bool UpdateUserFileLoc(UserFileInfo *userFile, const UTF8Char *location);
			virtual void UpdateWebFileCrop(WebFileInfo *userFile, Double cropLeft, Double cropTop, Double cropRight, Double cropBottom);

		protected:
			virtual void TripReload(Int32 cateId);
		public:
			virtual Bool TripAdd(Data::DateTime *fromDate, Data::DateTime *toDate, Int32 locId);
			virtual Bool LocationUpdate(Int32 locId, const UTF8Char *engName, const UTF8Char *chiName);
			virtual Bool LocationAdd(Int32 locId, const UTF8Char *engName, const UTF8Char *chiName);

		private:
			virtual void BooksInit();

		public:
			virtual Media::ImageList *ParseImage(OrganImageItem *img, UserFileInfo **userFile, WebFileInfo **wfile);
			virtual Media::ImageList *ParseSpImage(OrganSpecies *sp);
			virtual Media::ImageList *ParseFileImage(UserFileInfo *userFile);
			virtual Media::ImageList *ParseWebImage(WebFileInfo *webFile);
			virtual OrganGroup *SearchObject(const UTF8Char *searchStr, UTF8Char *resultStr, UOSInt resultStrBuffSize, Int32 *parentId);

		private:
			virtual void LoadGroupTypes();

		private:
			virtual Data::Int32Map<Data::ArrayList<OrganGroup*>*> *GetGroupTree();
			virtual Data::Int32Map<Data::ArrayList<OrganSpecies*>*> *GetSpeciesTree();

		public:
			virtual void Test();
			void UpgradeDB();
			void UpgradeDB2();
			void UpgradeFileStruct(OrganSpecies *sp);

			virtual void ExportLite(const UTF8Char *folder);
		};
	}
}
#endif
