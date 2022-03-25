#ifndef _SM_MEDIA_EXIFDATA
#define _SM_MEDIA_EXIFDATA
#include "Data/DateTime.h"
#include "Data/Int32Map.h"
#include "IO/IStreamData.h"
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
			Int32 id;
			EXIFType type;
			UInt32 cnt;
			Int32 value;
			void *dataBuff;
		} EXIFItem;

		typedef struct
		{
			Int32 id;
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
			EM_FLIR
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
		Data::Int32Map<EXIFItem*> *exifMap;
		EXIFMaker exifMaker;

	private:
		void FreeItem(EXIFItem *item);
		void ToExifBuff(UInt8 *buff, Data::ArrayList<EXIFItem*> *exifList, UInt32 *startOfst, UInt32 *otherOfst);
		void GetExifBuffSize(Data::ArrayList<EXIFItem*> *exifList, UInt32 *size, UInt32 *endOfst);
	public:
		EXIFData(EXIFMaker exifMaker);
		~EXIFData();
		EXIFMaker GetEXIFMaker();
		Media::EXIFData *Clone();
		void AddBytes(Int32 id, UInt32 cnt, const UInt8 *buff);
		void AddString(Int32 id, UInt32 cnt, const Char *buff);
		void AddUInt16(Int32 id, UInt32 cnt, const UInt16 *buff);
		void AddUInt32(Int32 id, UInt32 cnt, const UInt32 *buff);
		void AddRational(Int32 id, UInt32 cnt, const UInt32 *buff);
		void AddOther(Int32 id, UInt32 cnt, const UInt8 *buff);
		void AddInt16(Int32 id, UInt32 cnt, const Int16 *buff);
		void AddSubEXIF(Int32 id, Media::EXIFData *exif);
		void AddDouble(Int32 id, UInt32 cnt, const Double *buff);
		void Remove(Int32 id);

		UOSInt GetExifIds(Data::ArrayList<Int32> *idArr);
		EXIFType GetExifType(Int32 id);
		UInt32 GetExifCount(Int32 id);
		EXIFItem *GetExifItem(Int32 id);
		UInt16 *GetExifUInt16(Int32 id);
		UInt32 *GetExifUInt32(Int32 id);
		Media::EXIFData *GetExifSubexif(Int32 id);
		UInt8 *GetExifOther(Int32 id);

		Bool GetPhotoDate(Data::DateTime *dt);
		Text::CString GetPhotoMake();
		Text::CString GetPhotoModel();
		Text::CString GetPhotoLens();
		Double GetPhotoFNumber();
		Double GetPhotoExpTime();
		UInt32 GetPhotoISO();
		Double GetPhotoFocalLength();
		Bool GetPhotoLocation(Double *lat, Double *lon, Double *altitude, Int64 *gpsTimeTick);
		Bool GetGeoBounds(UOSInt imgW, UOSInt imgH, UInt32 *srid, Double *minX, Double *minY, Double *maxX, Double *maxY);
		RotateType GetRotateType();
		Double GetHDPI();
		Double GetVDPI();
		void SetWidth(UInt32 width);
		void SetHeight(UInt32 height);

		Bool ToString(Text::StringBuilderUTF8 *sb, Text::CString linePrefix);
		Bool ToStringCanonCameraSettings(Text::StringBuilderUTF8 *sb, Text::CString linePrefix, UInt16 *valBuff, UOSInt valCnt);
		Bool ToStringCanonFocalLength(Text::StringBuilderUTF8 *sb, Text::CString linePrefix, UInt16 *valBuff, UOSInt valCnt);
		Bool ToStringCanonShotInfo(Text::StringBuilderUTF8 *sb, Text::CString linePrefix, UInt16 *valBuff, UOSInt valCnt);
		Bool ToStringCanonLensType(Text::StringBuilderUTF8 *sb, UInt16 lensType);
		void ToExifBuff(UInt8 *buff, UInt32 *startOfst, UInt32 *otherOfst);
		void GetExifBuffSize(UInt32 *size, UInt32 *endOfst);

		EXIFData *ParseMakerNote(const UInt8 *buff, UOSInt buffSize);

		static Text::CString GetEXIFMakerName(EXIFMaker exifMaker);
		static Text::CString GetEXIFName(EXIFMaker exifMaker, Int32 id);
		static Text::CString GetEXIFName(EXIFMaker exifMaker, Int32 id, Int32 subId);
		static Text::CString GetEXIFTypeName(EXIFType type);
		static EXIFData *ParseIFD(const UInt8 *buff, UOSInt buffSize, RInt32Func readInt32, RInt16Func readInt16, UInt32 *nextOfst, EXIFMaker exifMaker, UInt32 readBase);
		static EXIFData *ParseIFD(IO::IStreamData *fd, UInt64 ofst, RInt32Func readInt32, RInt16Func readInt16, UInt32 *nextOfst, UInt64 readBase);
	};
}
#endif
