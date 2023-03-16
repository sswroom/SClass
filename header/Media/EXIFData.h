#ifndef _SM_MEDIA_EXIFDATA
#define _SM_MEDIA_EXIFDATA
#include "Data/ByteOrder.h"
#include "Data/DateTime.h"
#include "Data/FastMap.h"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/FrameDetailHandler.h"
#include "Media/FrameInfo.h"
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"

namespace Media
{
	class EXIFData
	{
	public:
		typedef enum
		{
			ET_UNKNOWN,
			ET_BYTES,
			ET_STRING,
			ET_UINT16,
			ET_UINT32,
			ET_RATIONAL,
			ET_OTHER,
			ET_INT16,
			ET_SUBEXIF,
			ET_INT32,
			ET_SRATIONAL,
			ET_DOUBLE,
			ET_UINT64,
			ET_INT64
		} EXIFType;

		typedef struct
		{
			UInt32 id;
			EXIFType type;
			UInt64 cnt;
			Int32 value;
			void *dataBuff;
		} EXIFItem;

		typedef struct
		{
			UInt32 id;
			Text::CString name;
		} EXIFInfo;

		typedef enum
		{
			EM_STANDARD,
			EM_PANASONIC,
			EM_CANON,
			EM_OLYMPUS,
			EM_CASIO1,
			EM_CASIO2,
			EM_FLIR,
			EM_NIKON3,
			EM_SANYO,
			EM_APPLE
		} EXIFMaker;

	private:
		static EXIFInfo defInfos[];
		static EXIFInfo exifInfos[];
		static EXIFInfo gpsInfos[];
		static EXIFInfo panasonicInfos[];
		static EXIFInfo canonInfos[];
		static EXIFInfo olympusInfos[];
		static EXIFInfo olympus2010Infos[];
		static EXIFInfo olympus2020Infos[];
		static EXIFInfo olympus2030Infos[];
		static EXIFInfo olympus2040Infos[];
		static EXIFInfo olympus2050Infos[];
		static EXIFInfo casio1Infos[];
		static EXIFInfo casio2Infos[];
		static EXIFInfo flirInfos[];
		static EXIFInfo nikon3Infos[];
		static EXIFInfo sanyo1Infos[];
		static EXIFInfo appleInfos[];
		Data::FastMap<UInt32, EXIFItem*> exifMap;
		EXIFMaker exifMaker;

	private:
		void FreeItem(EXIFItem *item);
		void ToExifBuff(UInt8 *buff, const Data::ReadingList<EXIFItem*> *exifList, UInt32 *startOfst, UInt32 *otherOfst) const;
		void GetExifBuffSize(const Data::ReadingList<EXIFItem*> *exifList, UInt64 *size, UInt64 *endOfst) const;
	public:
		EXIFData(EXIFMaker exifMaker);
		~EXIFData();
		EXIFMaker GetEXIFMaker() const;
		Media::EXIFData *Clone() const;
		void AddBytes(UInt32 id, UInt64 cnt, const UInt8 *buff);
		void AddString(UInt32 id, UInt64 cnt, const Char *buff);
		void AddUInt16(UInt32 id, UInt64 cnt, const UInt16 *buff);
		void AddUInt32(UInt32 id, UInt64 cnt, const UInt32 *buff);
		void AddInt16(UInt32 id, UInt64 cnt, const Int16 *buff);
		void AddInt32(UInt32 id, UInt64 cnt, const Int32 *buff);
		void AddRational(UInt32 id, UInt64 cnt, const UInt32 *buff);
		void AddSRational(UInt32 id, UInt64 cnt, const Int32 *buff);
		void AddOther(UInt32 id, UInt64 cnt, const UInt8 *buff);
		void AddSubEXIF(UInt32 id, Media::EXIFData *exif);
		void AddDouble(UInt32 id, UInt64 cnt, const Double *buff);
		void AddUInt64(UInt32 id, UInt64 cnt, const UInt64 *buff);
		void AddInt64(UInt32 id, UInt64 cnt, const Int64 *buff);
		void Remove(UInt32 id);

		UOSInt GetExifIds(Data::ArrayList<UInt32> *idArr) const;
		EXIFType GetExifType(UInt32 id) const;
		UInt64 GetExifCount(UInt32 id) const;
		EXIFItem *GetExifItem(UInt32 id) const;
		UInt16 *GetExifUInt16(UInt32 id) const;
		UInt32 *GetExifUInt32(UInt32 id) const;
		Media::EXIFData *GetExifSubexif(UInt32 id) const;
		UInt8 *GetExifOther(UInt32 id) const;

		Bool GetPhotoDate(Data::DateTime *dt) const;
		Bool GetPhotoDate(Data::Timestamp *dt) const;
		Text::CString GetPhotoMake() const;
		Text::CString GetPhotoModel() const;
		Text::CString GetPhotoLens() const;
		Double GetPhotoFNumber() const;
		Double GetPhotoExpTime() const;
		UInt32 GetPhotoISO() const;
		Double GetPhotoFocalLength() const;
		Bool GetPhotoLocation(Double *lat, Double *lon, Double *altitude, Int64 *gpsTimeTick) const;
		Bool GetGeoBounds(UOSInt imgW, UOSInt imgH, UInt32 *srid, Double *minX, Double *minY, Double *maxX, Double *maxY) const;
		RotateType GetRotateType() const;
		void SetRotateType(RotateType rotateType);
		Double GetHDPI() const;
		Double GetVDPI() const;
		void SetWidth(UInt32 width);
		void SetHeight(UInt32 height);

		Bool ToString(Text::StringBuilderUTF8 *sb, Text::CString linePrefix) const;
		Bool ToStringCanonCameraSettings(Text::StringBuilderUTF8 *sb, Text::CString linePrefix, UInt16 *valBuff, UOSInt valCnt) const;
		Bool ToStringCanonFocalLength(Text::StringBuilderUTF8 *sb, Text::CString linePrefix, UInt16 *valBuff, UOSInt valCnt) const;
		Bool ToStringCanonShotInfo(Text::StringBuilderUTF8 *sb, Text::CString linePrefix, UInt16 *valBuff, UOSInt valCnt) const;
		Bool ToStringCanonLensType(Text::StringBuilderUTF8 *sb, UInt16 lensType) const;
		void ToExifBuff(UInt8 *buff, UInt32 *startOfst, UInt32 *otherOfst) const;
		void GetExifBuffSize(UInt64 *size, UInt64 *endOfst) const;

		EXIFData *ParseMakerNote(const UInt8 *buff, UOSInt buffSize) const;

		static Text::CString GetEXIFMakerName(EXIFMaker exifMaker);
		static Text::CString GetEXIFName(EXIFMaker exifMaker, UInt32 id);
		static Text::CString GetEXIFName(EXIFMaker exifMaker, UInt32 id, UInt32 subId);
		static Text::CString GetEXIFTypeName(EXIFType type);
		static Text::CString GetFieldTypeName(UInt32 ftype);
		static EXIFData *ParseIFD(const UInt8 *buff, UOSInt buffSize, Data::ByteOrder *byteOrder, UInt64 *nextOfst, EXIFMaker exifMaker, const UInt8 *basePtr);
		static EXIFData *ParseIFD(IO::StreamData *fd, UInt64 ofst, Data::ByteOrder *byteOrder, UInt64 *nextOfst, UInt64 readBase);
		static EXIFData *ParseIFD64(IO::StreamData *fd, UInt64 ofst, Data::ByteOrder *byteOrder, UInt64 *nextOfst, UInt64 readBase);
		static Bool ParseEXIFFrame(IO::FileAnalyse::FrameDetailHandler *frame, UOSInt frameOfst, IO::StreamData *fd, UInt64 ofst);
		static Bool ParseFrame(IO::FileAnalyse::FrameDetailHandler *frame, UOSInt frameOfst, IO::StreamData *fd, UInt64 ofst, Data::ByteOrder *byteOrder, UInt32 *nextOfst, UInt32 ifdId, UInt64 readBase);
		static EXIFData *ParseExif(const UInt8 *buff, UOSInt buffSize);
	};
}
#endif
