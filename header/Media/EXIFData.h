#ifndef _SM_MEDIA_EXIFDATA
#define _SM_MEDIA_EXIFDATA
#include "Data/DateTime.h"
#include "Data/UInt32Map.h"
#include "IO/IStreamData.h"
#include "IO/FileAnalyse/FrameDetailHandler.h"
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
			ET_DOUBLE
		} EXIFType;

		typedef enum
		{
			RT_NONE,
			RT_CW90,
			RT_CW180,
			RT_CW270
		} RotateType;

		typedef struct
		{
			UInt32 id;
			EXIFType type;
			UInt32 cnt;
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
			EM_NIKON3
		} EXIFMaker;

	public:
		static Int32 __stdcall TReadInt32(const UInt8 *pVal);
		static Int32 __stdcall TReadMInt32(const UInt8 *pVal);
		static Int16 __stdcall TReadInt16(const UInt8 *pVal);
		static Int16 __stdcall TReadMInt16(const UInt8 *pVal);
		static Single __stdcall TReadFloat16(const UInt8 *pVal);
		static Single __stdcall TReadMFloat16(const UInt8 *pVal);
		static Single __stdcall TReadFloat24(const UInt8 *pVal);
		static Single __stdcall TReadMFloat24(const UInt8 *pVal);
		static Single __stdcall TReadFloat32(const UInt8 *pVal);
		static Single __stdcall TReadMFloat32(const UInt8 *pVal);
		static void __stdcall TWriteInt32(UInt8 *pVal, Int32 v);
		static void __stdcall TWriteMInt32(UInt8 *pVal, Int32 v);
		static void __stdcall TWriteInt16(UInt8 *pVal, Int32 v);
		static void __stdcall TWriteMInt16(UInt8 *pVal, Int32 v);

		typedef Int32 (__stdcall *RInt32Func)(const UInt8* pVal);
		typedef Int16 (__stdcall *RInt16Func)(const UInt8* pVal);
		typedef Single (__stdcall *RFloatFunc)(const UInt8* pVal);
		typedef void (__stdcall *WInt32Func)(UInt8* pVal, Int32 v);
		typedef void (__stdcall *WInt16Func)(UInt8* pVal, Int32 v);

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
		Data::UInt32Map<EXIFItem*> exifMap;
		EXIFMaker exifMaker;

	private:
		void FreeItem(EXIFItem *item);
		void ToExifBuff(UInt8 *buff, const Data::ArrayList<EXIFItem*> *exifList, UInt32 *startOfst, UInt32 *otherOfst) const;
		void GetExifBuffSize(const Data::ArrayList<EXIFItem*> *exifList, UInt32 *size, UInt32 *endOfst) const;
	public:
		EXIFData(EXIFMaker exifMaker);
		~EXIFData();
		EXIFMaker GetEXIFMaker() const;
		Media::EXIFData *Clone() const;
		void AddBytes(UInt32 id, UInt32 cnt, const UInt8 *buff);
		void AddString(UInt32 id, UInt32 cnt, const Char *buff);
		void AddUInt16(UInt32 id, UInt32 cnt, const UInt16 *buff);
		void AddUInt32(UInt32 id, UInt32 cnt, const UInt32 *buff);
		void AddRational(UInt32 id, UInt32 cnt, const UInt32 *buff);
		void AddOther(UInt32 id, UInt32 cnt, const UInt8 *buff);
		void AddInt16(UInt32 id, UInt32 cnt, const Int16 *buff);
		void AddSubEXIF(UInt32 id, Media::EXIFData *exif);
		void AddDouble(UInt32 id, UInt32 cnt, const Double *buff);
		void Remove(UInt32 id);

		UOSInt GetExifIds(Data::ArrayList<UInt32> *idArr) const;
		EXIFType GetExifType(UInt32 id) const;
		UInt32 GetExifCount(UInt32 id) const;
		EXIFItem *GetExifItem(UInt32 id) const;
		UInt16 *GetExifUInt16(UInt32 id) const;
		UInt32 *GetExifUInt32(UInt32 id) const;
		Media::EXIFData *GetExifSubexif(UInt32 id) const;
		UInt8 *GetExifOther(UInt32 id) const;

		Bool GetPhotoDate(Data::DateTime *dt) const;
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
		void GetExifBuffSize(UInt32 *size, UInt32 *endOfst) const;

		EXIFData *ParseMakerNote(const UInt8 *buff, UOSInt buffSize) const;

		static Text::CString GetEXIFMakerName(EXIFMaker exifMaker);
		static Text::CString GetEXIFName(EXIFMaker exifMaker, UInt32 id);
		static Text::CString GetEXIFName(EXIFMaker exifMaker, UInt32 id, UInt32 subId);
		static Text::CString GetEXIFTypeName(EXIFType type);
		static Text::CString GetFieldTypeName(UInt32 ftype);
		static EXIFData *ParseIFD(const UInt8 *buff, UOSInt buffSize, RInt32Func readInt32, RInt16Func readInt16, UInt32 *nextOfst, EXIFMaker exifMaker, const UInt8 *basePtr);
		static EXIFData *ParseIFD(IO::IStreamData *fd, UInt64 ofst, RInt32Func readInt32, RInt16Func readInt16, UInt32 *nextOfst, UInt64 readBase);
		static Bool ParseEXIFFrame(IO::FileAnalyse::FrameDetailHandler *frame, UOSInt frameOfst, IO::IStreamData *fd, UInt64 ofst);
		static Bool ParseFrame(IO::FileAnalyse::FrameDetailHandler *frame, UOSInt frameOfst, IO::IStreamData *fd, UInt64 ofst, RInt32Func readInt32, RInt16Func readInt16, UInt32 *nextOfst, UInt32 ifdId, UInt64 readBase);
	};
}
#endif
