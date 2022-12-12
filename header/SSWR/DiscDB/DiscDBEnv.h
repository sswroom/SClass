#ifndef _SM_SSWR_DISCDB_DISCDBENV
#define _SM_SSWR_DISCDB_DISCDBENV
#include "Data/FastMap.h"
#include "Data/FastStringMap.h"
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
				Text::String *discId;
				Text::String *discTypeId;
				Data::Timestamp burntDate;
				Int32 status;
			} BurntDiscInfo;

			typedef struct
			{
				UInt32 fileId;
				Text::String *fileName;
				UInt64 fileSize;
				UTF8Char category[6];
				Int32 videoId;
			} DiscFileInfo;

			typedef struct
			{
				Text::String *id;
				Text::String *name;
			} CategoryInfo;

			typedef struct
			{
				Text::String *discTypeID;
				Text::String *name;
				Text::String *description;
			} DVDTypeInfo;

			typedef struct
			{
				Text::String *discTypeId;
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
				Text::String *dvdType;
			} DVDVideoInfo;
		private:
			ErrorType err;
			Net::SocketFactory *sockf;
			DB::DBTool *db;
			IO::LogTool *log;
			Media::MonitorMgr *monMgr;
			Data::FastStringMap<BurntDiscInfo*> *discMap;
			Data::FastStringMap<DVDTypeInfo*> *dvdTypeMap;
			Data::FastStringMap<CategoryInfo*> *cateMap;
			Data::FastStringMap<DiscTypeInfo*> *discTypeMap;
			Data::FastMap<Int32, DVDVideoInfo*> *dvdVideoMap;
			
			void LoadDB();
		public:
			DiscDBEnv();
			~DiscDBEnv();

			ErrorType GetErrorType();

			Double GetMonitorHDPI(MonitorHandle *hMon);
			Double GetMonitorDDPI(MonitorHandle *hMon);

			const BurntDiscInfo *NewBurntDisc(Text::CString discId, Text::CString discTypeId, const Data::Timestamp &ts);
			UOSInt GetBurntDiscs(Data::ArrayList<BurntDiscInfo*> *discList);
			const BurntDiscInfo *GetBurntDisc(Text::CString discId);
			OSInt GetBurntDiscIndex(Text::CString discId);
			Bool NewBurntFile(const UTF8Char *discId, UOSInt fileId, const UTF8Char *name, UInt64 fileSize, Text::CString category, Int32 videoId);
			UOSInt GetBurntFiles(Text::CString discId, Data::ArrayList<DiscFileInfo*> *fileList);
			void FreeBurntFiles(Data::ArrayList<DiscFileInfo*> *fileList);
			UOSInt GetDVDTypeCount();
			const DVDTypeInfo *GetDVDType(UOSInt index);
			OSInt GetDVDTypeIndex(Text::CString discTypeID);
			Bool ModifyDVDType(Text::CString discTypeID, Text::CString name, Text::CString desc);
			const DVDTypeInfo *NewDVDType(Text::CString discTypeID, Text::CString name, Text::CString desc);
			UOSInt GetCategories(Data::ArrayList<CategoryInfo*> *cateList);
			const DiscTypeInfo *GetDiscType(Text::CString discTypeId);
			UOSInt GetDiscTypes(Data::ArrayList<DiscTypeInfo*> *discTypeList);
			UOSInt GetDiscTypesByBrand(Data::ArrayList<const DiscTypeInfo*> *discTypeList, const UTF8Char *brand, UOSInt brandLen);
			Int32 NewDVDVideo(const UTF8Char *anime, const UTF8Char *series, const UTF8Char *volume, const UTF8Char *dvdType);
			UOSInt GetDVDVideos(Data::ArrayList<DVDVideoInfo*> *dvdVideoList);
			const DVDVideoInfo *GetDVDVideo(Int32 videoId);
			Bool NewMovies(const UTF8Char *discId, UOSInt fileId, const UTF8Char *mainTitle, Text::String *type, const UTF8Char *chapter, const UTF8Char *chapterTitle, Text::CString videoFormat, Int32 width, Int32 height, Int32 fps, Int32 length, Text::CString audioFormat, Int32 samplingRate, Int32 bitRate, const UTF8Char *aspectRatio, const UTF8Char *remark);
			Bool AddMD5(IO::IStreamData *fd);
		};
	}
}
#endif
