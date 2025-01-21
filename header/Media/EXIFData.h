#ifndef _SM_MEDIA_EXIFDATA
#define _SM_MEDIA_EXIFDATA
#include "AnyType.h"
#include "Data/ByteOrder.h"
#include "Data/DateTime.h"
#include "Data/FastMapNN.h"
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
			AnyType dataBuff;
		} EXIFItem;

		typedef struct
		{
			UInt32 id;
			Text::CStringNN name;
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
		Data::FastMapNN<UInt32, EXIFItem> exifMap;
		EXIFMaker exifMaker;

	private:
		void FreeItem(NN<EXIFItem> item);
		void ToExifBuffImpl(UInt8 *buff, NN<const Data::ReadingListNN<EXIFItem>> exifList, InOutParam<UInt32> startOfst, InOutParam<UInt32> otherOfst) const;
		void GetExifBuffSize(NN<const Data::ReadingListNN<EXIFItem>> exifList, OutParam<UInt64> size, OutParam<UInt64> endOfst) const;
	public:
		EXIFData(EXIFMaker exifMaker);
		~EXIFData();
		EXIFMaker GetEXIFMaker() const;
		NN<Media::EXIFData> Clone() const;
		void AddBytes(UInt32 id, UInt64 cnt, const UInt8 *buff);
		void AddString(UInt32 id, UInt64 cnt, const Char *buff);
		void AddUInt16(UInt32 id, UInt64 cnt, const UInt16 *buff);
		void AddUInt32(UInt32 id, UInt64 cnt, const UInt32 *buff);
		void AddInt16(UInt32 id, UInt64 cnt, const Int16 *buff);
		void AddInt32(UInt32 id, UInt64 cnt, const Int32 *buff);
		void AddRational(UInt32 id, UInt64 cnt, const UInt32 *buff);
		void AddSRational(UInt32 id, UInt64 cnt, const Int32 *buff);
		void AddOther(UInt32 id, UInt64 cnt, UnsafeArray<const UInt8> buff);
		void AddSubEXIF(UInt32 id, NN<Media::EXIFData> exif);
		void AddDouble(UInt32 id, UInt64 cnt, const Double *buff);
		void AddUInt64(UInt32 id, UInt64 cnt, const UInt64 *buff);
		void AddInt64(UInt32 id, UInt64 cnt, const Int64 *buff);
		void Remove(UInt32 id);
		Bool RemoveLargest();

		UOSInt GetExifIds(NN<Data::ArrayList<UInt32>> idArr) const;
		EXIFType GetExifType(UInt32 id) const;
		UInt64 GetExifCount(UInt32 id) const;
		Optional<EXIFItem> GetExifItem(UInt32 id) const;
		UInt16 *GetExifUInt16(UInt32 id) const;
		UInt32 *GetExifUInt32(UInt32 id) const;
		Media::EXIFData *GetExifSubexif(UInt32 id) const;
		UInt8 *GetExifOther(UInt32 id) const;

		Bool GetPhotoDate(NN<Data::DateTime> dt) const;
		Bool GetPhotoDate(OutParam<Data::Timestamp> dt) const;
		Text::CString GetPhotoMake() const;
		Text::CString GetPhotoModel() const;
		Text::CString GetPhotoLens() const;
		Double GetPhotoFNumber() const;
		Double GetPhotoExpTime() const;
		UInt32 GetPhotoISO() const;
		Double GetPhotoFocalLength() const;
		Bool GetPhotoLocation(OutParam<Double> lat, OutParam<Double> lon, OptOut<Double> altitude, OptOut<Int64> gpsTimeTick) const;
		Bool GetGeoBounds(Math::Size2D<UOSInt> imgSize, OutParam<UInt32> srid, OutParam<Double> minX, OutParam<Double> minY, OutParam<Double> maxX, OutParam<Double> maxY) const;
		RotateType GetRotateType() const;
		void SetRotateType(RotateType rotateType);
		Double GetHDPI() const;
		Double GetVDPI() const;
		void SetWidth(UInt32 width);
		void SetHeight(UInt32 height);

		Bool ToString(NN<Text::StringBuilderUTF8> sb, Text::CString linePrefix) const;
		Bool ToStringCanonCameraSettings(NN<Text::StringBuilderUTF8> sb, Text::CString linePrefix, UInt16 *valBuff, UOSInt valCnt) const;
		Bool ToStringCanonFocalLength(NN<Text::StringBuilderUTF8> sb, Text::CString linePrefix, UInt16 *valBuff, UOSInt valCnt) const;
		Bool ToStringCanonShotInfo(NN<Text::StringBuilderUTF8> sb, Text::CString linePrefix, UInt16 *valBuff, UOSInt valCnt) const;
		Bool ToStringCanonLensType(NN<Text::StringBuilderUTF8> sb, UInt16 lensType) const;
		void ToExifBuff(UInt8 *buff, InOutParam<UInt32> startOfst, InOutParam<UInt32> otherOfst) const;
		void GetExifBuffSize(OutParam<UInt64> size, OutParam<UInt64> endOfst) const;

		Optional<EXIFData> ParseMakerNote(UnsafeArray<const UInt8> buff, UOSInt buffSize) const;

		static Text::CString GetEXIFMakerName(EXIFMaker exifMaker);
		static Text::CStringNN GetEXIFName(EXIFMaker exifMaker, UInt32 id);
		static Text::CStringNN GetEXIFName(EXIFMaker exifMaker, UInt32 id, UInt32 subId);
		static Text::CString GetEXIFTypeName(EXIFType type);
		static Text::CString GetFieldTypeName(UInt32 ftype);
		static Optional<EXIFData> ParseIFD(UnsafeArray<const UInt8> buff, UOSInt buffSize, NN<Data::ByteOrder> byteOrder, OptOut<UInt64> nextOfst, EXIFMaker exifMaker, const UInt8 *basePtr);
		static Optional<EXIFData> ParseIFD(NN<IO::StreamData> fd, UInt64 ofst, NN<Data::ByteOrder> byteOrder, OptOut<UInt64> nextOfst, UInt64 readBase);
		static Optional<EXIFData> ParseIFD64(NN<IO::StreamData> fd, UInt64 ofst, NN<Data::ByteOrder> byteOrder, OptOut<UInt64> nextOfst, UInt64 readBase);
		static Bool ParseEXIFFrame(NN<IO::FileAnalyse::FrameDetailHandler> frame, UOSInt frameOfst, NN<IO::StreamData> fd, UInt64 ofst);
		static Bool ParseFrame(NN<IO::FileAnalyse::FrameDetailHandler> frame, UOSInt frameOfst, NN<IO::StreamData> fd, UInt64 ofst, NN<Data::ByteOrder> byteOrder, OptOut<UInt32> nextOfst, UInt32 ifdId, UInt64 readBase);
		static Optional<EXIFData> ParseExif(UnsafeArray<const UInt8> buff, UOSInt buffSize);
	};
}
#endif
