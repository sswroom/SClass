#ifndef _SM_SSWR_DISCDB_DISCDBENV
#define _SM_SSWR_DISCDB_DISCDBENV
#include "Data/FastMapNN.h"
#include "Data/FastStringMapNN.h"
#include "DB/DBTool.h"
#include "IO/StreamData.h"
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
				NN<Text::String> discId;
				NN<Text::String> discTypeId;
				Data::Timestamp burntDate;
				Int32 status;
			} BurntDiscInfo;

			typedef struct
			{
				UInt32 fileId;
				NN<Text::String> fileName;
				UInt64 fileSize;
				UTF8Char category[6];
				Int32 videoId;
			} DiscFileInfo;

			typedef struct
			{
				NN<Text::String> id;
				NN<Text::String> name;
			} CategoryInfo;

			typedef struct
			{
				NN<Text::String> discTypeID;
				NN<Text::String> name;
				NN<Text::String> description;
			} DVDTypeInfo;

			typedef struct
			{
				NN<Text::String> discTypeId;
				NN<Text::String> brand;
				UnsafeArray<const UTF8Char> name;
				Double speed;
				UnsafeArray<const UTF8Char> dvdType;
				UnsafeArray<const UTF8Char> madeIn;
				UnsafeArrayOpt<const UTF8Char> mid;
				UnsafeArrayOpt<const UTF8Char> tid;
				UnsafeArrayOpt<const UTF8Char> revision;
				Bool qcTest;
				UnsafeArrayOpt<const UTF8Char> remark;
			} DiscTypeInfo;

			typedef struct
			{
				Int32 videoId;
				NN<Text::String> anime;
				Optional<Text::String> series;
				Optional<Text::String> volume;
				NN<Text::String> dvdType;
			} DVDVideoInfo;
		private:
			ErrorType err;
			NN<Net::SocketFactory> sockf;
			Optional<DB::DBTool> db;
			IO::LogTool log;
			Media::MonitorMgr *monMgr;
			Data::FastStringMapNN<BurntDiscInfo> discMap;
			Data::FastStringMapNN<DVDTypeInfo> dvdTypeMap;
			Data::FastStringMapNN<CategoryInfo> cateMap;
			Data::FastStringMapNN<DiscTypeInfo> discTypeMap;
			Data::FastMapNN<Int32, DVDVideoInfo> dvdVideoMap;
			
			void LoadDB();
		public:
			DiscDBEnv();
			~DiscDBEnv();

			ErrorType GetErrorType();

			Double GetMonitorHDPI(MonitorHandle *hMon);
			Double GetMonitorDDPI(MonitorHandle *hMon);

			Optional<const BurntDiscInfo> NewBurntDisc(Text::CString discId, Text::CString discTypeId, const Data::Timestamp &ts);
			UOSInt GetBurntDiscs(NN<Data::ArrayListNN<BurntDiscInfo>> discList);
			Optional<const BurntDiscInfo> GetBurntDisc(Text::CStringNN discId);
			OSInt GetBurntDiscIndex(Text::CStringNN discId);
			Bool NewBurntFile(UnsafeArray<const UTF8Char> discId, UOSInt fileId, UnsafeArray<const UTF8Char> name, UInt64 fileSize, Text::CString category, Int32 videoId);
			UOSInt GetBurntFiles(Text::CString discId, NN<Data::ArrayListNN<DiscFileInfo>> fileList);
			void FreeBurntFiles(NN<Data::ArrayListNN<DiscFileInfo>> fileList);
			UOSInt GetDVDTypeCount();
			Optional<const DVDTypeInfo> GetDVDType(UOSInt index);
			OSInt GetDVDTypeIndex(Text::CStringNN discTypeID);
			Bool ModifyDVDType(Text::CStringNN discTypeID, Text::CStringNN name, Text::CStringNN desc);
			Optional<const DVDTypeInfo> NewDVDType(Text::CStringNN discTypeID, Text::CString name, Text::CString desc);
			UOSInt GetCategories(NN<Data::ArrayListNN<CategoryInfo>> cateList);
			Optional<const DiscTypeInfo> GetDiscType(Text::CStringNN discTypeId);
			UOSInt GetDiscTypes(NN<Data::ArrayListNN<DiscTypeInfo>> discTypeList);
			UOSInt GetDiscTypesByBrand(NN<Data::ArrayListNN<const DiscTypeInfo>> discTypeList, UnsafeArray<const UTF8Char> brand, UOSInt brandLen);
			Int32 NewDVDVideo(UnsafeArray<const UTF8Char> anime, UnsafeArrayOpt<const UTF8Char> series, UnsafeArrayOpt<const UTF8Char> volume, UnsafeArray<const UTF8Char> dvdType);
			UOSInt GetDVDVideos(NN<Data::ArrayListNN<DVDVideoInfo>> dvdVideoList);
			Optional<const DVDVideoInfo> GetDVDVideo(Int32 videoId);
			Bool NewMovies(UnsafeArray<const UTF8Char> discId, UOSInt fileId, UnsafeArray<const UTF8Char> mainTitle, NN<Text::String> type, UnsafeArrayOpt<const UTF8Char> chapter, UnsafeArrayOpt<const UTF8Char> chapterTitle, Text::CString videoFormat, Int32 width, Int32 height, Int32 fps, Int32 length, Text::CString audioFormat, Int32 samplingRate, Int32 bitRate, UnsafeArrayOpt<const UTF8Char> aspectRatio, UnsafeArrayOpt<const UTF8Char> remark);
			Bool AddMD5(NN<IO::StreamData> fd);
		};
	}
}
#endif
