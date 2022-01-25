#ifndef _SM_SSWR_DISCDB_DISCDBENV
#define _SM_SSWR_DISCDB_DISCDBENV
#include "Data/Int32Map.h"
#include "DB/DBTool.h"
#include "IO/IStreamData.h"
#include "Media/MonitorMgr.h"
#include "Net/SocketFactory.h"

namespace SSWR
{
	namespace DiscDB
	{
		class DiscDBEnv
		{
		public:
			typedef enum
			{
				ERR_NONE,
				ERR_CONFIG,
				ERR_DB
			} ErrorType;

			typedef struct
			{
				const UTF8Char *discId;
				const UTF8Char *discTypeId;
				Int64 burntDate;
				Int32 status;
			} BurntDiscInfo;

			typedef struct
			{
				UInt32 fileId;
				const UTF8Char *fileName;
				UInt64 fileSize;
				UTF8Char category[6];
				Int32 videoId;
			} DiscFileInfo;

			typedef struct
			{
				const UTF8Char *id;
				const UTF8Char *name;
			} CategoryInfo;

			typedef struct
			{
				const UTF8Char *discTypeID;
				const UTF8Char *name;
				const UTF8Char *description;
			} DVDTypeInfo;

			typedef struct
			{
				const UTF8Char *discTypeId;
				Text::String *brand;
				const UTF8Char *name;
				Double speed;
				const UTF8Char *dvdType;
				const UTF8Char *madeIn;
				const UTF8Char *mid;
				const UTF8Char *tid;
				const UTF8Char *revision;
				Bool qcTest;
				const UTF8Char *remark;
			} DiscTypeInfo;

			typedef struct
			{
				Int32 videoId;
				Text::String *anime;
				Text::String *series;
				Text::String *volume;
				const UTF8Char *dvdType;
			} DVDVideoInfo;
		private:
			ErrorType err;
			Net::SocketFactory *sockf;
			DB::DBTool *db;
			IO::LogTool *log;
			Media::MonitorMgr *monMgr;
			Data::StringUTF8Map<BurntDiscInfo*> *discMap;
			Data::StringUTF8Map<DVDTypeInfo*> *dvdTypeMap;
			Data::StringUTF8Map<CategoryInfo*> *cateMap;
			Data::StringUTF8Map<DiscTypeInfo*> *discTypeMap;
			Data::Int32Map<DVDVideoInfo*> *dvdVideoMap;
			
			void LoadDB();
		public:
			DiscDBEnv();
			~DiscDBEnv();

			ErrorType GetErrorType();

			Double GetMonitorHDPI(MonitorHandle *hMon);
			Double GetMonitorDDPI(MonitorHandle *hMon);

			const BurntDiscInfo *NewBurntDisc(const UTF8Char *discId, const UTF8Char *discTypeId, Data::DateTime *dt);
			UOSInt GetBurntDiscs(Data::ArrayList<BurntDiscInfo*> *discList);
			const BurntDiscInfo *GetBurntDisc(const UTF8Char *discId);
			OSInt GetBurntDiscIndex(const UTF8Char *discId);
			Bool NewBurntFile(const UTF8Char *discId, UOSInt fileId, const UTF8Char *name, UInt64 fileSize, const UTF8Char *category, Int32 videoId);
			UOSInt GetBurntFiles(const UTF8Char *discId, Data::ArrayList<DiscFileInfo*> *fileList);
			void FreeBurntFiles(Data::ArrayList<DiscFileInfo*> *fileList);
			UOSInt GetDVDTypeCount();
			const DVDTypeInfo *GetDVDType(UOSInt index);
			OSInt GetDVDTypeIndex(const UTF8Char *discTypeID);
			Bool ModifyDVDType(const UTF8Char *discTypeID, const UTF8Char *name, const UTF8Char *desc);
			const DVDTypeInfo *NewDVDType(const UTF8Char *discTypeID, const UTF8Char *name, const UTF8Char *desc);
			UOSInt GetCategories(Data::ArrayList<CategoryInfo*> *cateList);
			const DiscTypeInfo *GetDiscType(const UTF8Char *discTypeId);
			UOSInt GetDiscTypes(Data::ArrayList<DiscTypeInfo*> *discTypeList);
			UOSInt GetDiscTypesByBrand(Data::ArrayList<const DiscTypeInfo*> *discTypeList, const UTF8Char *brand, UOSInt brandLen);
			Int32 NewDVDVideo(const UTF8Char *anime, const UTF8Char *series, const UTF8Char *volume, const UTF8Char *dvdType);
			UOSInt GetDVDVideos(Data::ArrayList<DVDVideoInfo*> *dvdVideoList);
			const DVDVideoInfo *GetDVDVideo(Int32 videoId);
			Bool NewMovies(const UTF8Char *discId, UOSInt fileId, const UTF8Char *mainTitle, const UTF8Char *type, const UTF8Char *chapter, const UTF8Char *chapterTitle, const UTF8Char *videoFormat, Int32 width, Int32 height, Int32 fps, Int32 length, const UTF8Char *audioFormat, Int32 samplingRate, Int32 bitRate, const UTF8Char *aspectRatio, const UTF8Char *remark);
			Bool AddMD5(IO::IStreamData *fd);
		};
	}
}
#endif
