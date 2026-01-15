#ifndef _SM_MEDIA_EXIFDATA
#define _SM_MEDIA_EXIFDATA
#include "AnyType.h"
#include "Data/ByteOrder.h"
#include "Data/DateTime.h"
#include "Data/FastMapNN.hpp"
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
		void ToExifBuffImpl(UnsafeArray<UInt8> buff, NN<const Data::ReadingListNN<EXIFItem>> exifList, InOutParam<UInt32> startOfst, InOutParam<UInt32> otherOfst) const;
		void GetExifBuffSize(NN<const Data::ReadingListNN<EXIFItem>> exifList, OutParam<UInt64> size, OutParam<UInt64> endOfst) const;
	public:
		EXIFData(EXIFMaker exifMaker);
		~EXIFData();
		EXIFMaker GetEXIFMaker() const;
		NN<Media::EXIFData> Clone() const;
		void AddBytes(UInt32 id, UInt64 cnt, UnsafeArray<const UInt8> buff);
		void AddString(UInt32 id, UInt64 cnt, UnsafeArray<const Char> buff);
		void AddUInt16(UInt32 id, UInt64 cnt, UnsafeArray<const UInt16> buff);
		void AddUInt32(UInt32 id, UInt64 cnt, UnsafeArray<const UInt32> buff);
		void AddInt16(UInt32 id, UInt64 cnt, UnsafeArray<const Int16> buff);
		void AddInt32(UInt32 id, UInt64 cnt, UnsafeArray<const Int32> buff);
		void AddRational(UInt32 id, UInt64 cnt, UnsafeArray<const UInt32> buff);
		void AddSRational(UInt32 id, UInt64 cnt, UnsafeArray<const Int32> buff);
		void AddOther(UInt32 id, UInt64 cnt, UnsafeArray<const UInt8> buff);
		void AddSubEXIF(UInt32 id, NN<Media::EXIFData> exif);
		void AddDouble(UInt32 id, UInt64 cnt, UnsafeArray<const Double> buff);
		void AddUInt64(UInt32 id, UInt64 cnt, UnsafeArray<const UInt64> buff);
		void AddInt64(UInt32 id, UInt64 cnt, UnsafeArray<const Int64> buff);
		void Remove(UInt32 id);
		Bool RemoveLargest();

		UIntOS GetExifIds(NN<Data::ArrayListNative<UInt32>> idArr) const;
		EXIFType GetExifType(UInt32 id) const;
		UInt64 GetExifCount(UInt32 id) const;
		Optional<EXIFItem> GetExifItem(UInt32 id) const;
		UnsafeArrayOpt<UInt16> GetExifUInt16(UInt32 id) const;
		UnsafeArrayOpt<UInt32> GetExifUInt32(UInt32 id) const;
		Optional<Media::EXIFData> GetExifSubexif(UInt32 id) const;
		UnsafeArrayOpt<UInt8> GetExifOther(UInt32 id) const;

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
		Bool GetGeoBounds(Math::Size2D<UIntOS> imgSize, OutParam<UInt32> srid, OutParam<Double> minX, OutParam<Double> minY, OutParam<Double> maxX, OutParam<Double> maxY) const;
		RotateType GetRotateType() const;
		void SetRotateType(RotateType rotateType);
		Double GetHDPI() const;
		Double GetVDPI() const;
		void SetWidth(UInt32 width);
		void SetHeight(UInt32 height);

		Bool ToString(NN<Text::StringBuilderUTF8> sb, Text::CString linePrefix) const;
		Bool ToStringCanonCameraSettings(NN<Text::StringBuilderUTF8> sb, Text::CString linePrefix, UnsafeArray<UInt16> valBuff, UIntOS valCnt) const;
		Bool ToStringCanonFocalLength(NN<Text::StringBuilderUTF8> sb, Text::CString linePrefix, UnsafeArray<UInt16> valBuff, UIntOS valCnt) const;
		Bool ToStringCanonShotInfo(NN<Text::StringBuilderUTF8> sb, Text::CString linePrefix, UnsafeArray<UInt16> valBuff, UIntOS valCnt) const;
		Bool ToStringCanonLensType(NN<Text::StringBuilderUTF8> sb, UInt16 lensType) const;
		void ToExifBuff(UnsafeArray<UInt8> buff, InOutParam<UInt32> startOfst, InOutParam<UInt32> otherOfst) const;
		void GetExifBuffSize(OutParam<UInt64> size, OutParam<UInt64> endOfst) const;

		Optional<EXIFData> ParseMakerNote(UnsafeArray<const UInt8> buff, UIntOS buffSize) const;

		static Text::CString GetEXIFMakerName(EXIFMaker exifMaker);
		static Text::CStringNN GetEXIFName(EXIFMaker exifMaker, UInt32 id);
		static Text::CStringNN GetEXIFName(EXIFMaker exifMaker, UInt32 id, UInt32 subId);
		static Text::CString GetEXIFTypeName(EXIFType type);
		static Text::CString GetFieldTypeName(UInt32 ftype);
		static Optional<EXIFData> ParseIFD(UnsafeArray<const UInt8> buff, UIntOS buffSize, NN<Data::ByteOrder> byteOrder, OptOut<UInt64> nextOfst, EXIFMaker exifMaker, UnsafeArrayOpt<const UInt8> basePtr);
		static Optional<EXIFData> ParseIFD(NN<IO::StreamData> fd, UInt64 ofst, NN<Data::ByteOrder> byteOrder, OptOut<UInt64> nextOfst, UInt64 readBase);
		static Optional<EXIFData> ParseIFD64(NN<IO::StreamData> fd, UInt64 ofst, NN<Data::ByteOrder> byteOrder, OptOut<UInt64> nextOfst, UInt64 readBase);
		static Bool ParseEXIFFrame(NN<IO::FileAnalyse::FrameDetailHandler> frame, UIntOS frameOfst, NN<IO::StreamData> fd, UInt64 ofst);
		static Bool ParseFrame(NN<IO::FileAnalyse::FrameDetailHandler> frame, UIntOS frameOfst, NN<IO::StreamData> fd, UInt64 ofst, NN<Data::ByteOrder> byteOrder, OptOut<UInt32> nextOfst, UInt32 ifdId, UInt64 readBase);
		static Optional<EXIFData> ParseExifJPG(UnsafeArray<const UInt8> buff, UIntOS buffSize);
		static Optional<EXIFData> ParseExifDirect(UnsafeArray<const UInt8> buff, UIntOS buffSize);
	};
}
#endif
