#ifndef _SM_MEDIA_IMAGELIST
#define _SM_MEDIA_IMAGELIST
#include "Data/ArrayListNative.hpp"
#include "Data/ArrayListStringNN.h"
#include "Data/ArrayListUInt32.h"
#include "IO/ParsedObject.h"
#include "Media/RasterImage.h"
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"

namespace Media
{
	class ImageList : public IO::ParsedObject
	{
	public:
		typedef enum
		{
			IT_UNKNOWN,
			IT_VISIBLEIMAGE,
			IT_IRIMAGE,
			IT_OUTPUTIMAGE
		} ImageType;

		typedef enum
		{
			VT_IR_WIDTH,
			VT_IR_HEIGHT,
			VT_VISIBLE_WIDTH,
			VT_VISIBLE_HEIGHT,
			VT_FIRMWARE_DATE,
			VT_FIRMWARE_VERSION,
			VT_CAMERA_BRAND,
			VT_CAMERA_MODEL,
			VT_CAMERA_SN,
			VT_CAPTURE_DATE,
			VT_CAPTURE_WIDTH,
			VT_CAPTURE_HEIGHT
		} ValueType;

		typedef enum
		{
			TT_UNKNOWN,
			TT_FLIR
		} ThermoType;
	public:
		Data::ArrayListNN<Media::RasterImage> imgList;
		Data::ArrayListNative<ImageType> imgTypeList;
		Data::ArrayListNative<UInt32> imgTimes;
		UnsafeArrayOpt<const UTF8Char> author;
		UnsafeArrayOpt<const UTF8Char> imgName;
		Data::ArrayListNative<ValueType> valTypeI32;
		Data::ArrayListNative<Int32> valI32;
		Data::ArrayListNative<ValueType> valTypeI64;
		Data::ArrayListNative<Int64> valI64;
		Data::ArrayListNative<ValueType> valTypeStr;
		Data::ArrayListStringNN valStr;

		Math::Size2D<UOSInt> thermoSize;
		UOSInt thermoBPP;
		UInt8 *thermoPtr;
		Double thermoEmissivity;
		Double thermoTransmission;
		Double thermoBKGTemp;
		ThermoType thermoType;

		ImageList(NN<Text::String> name);
		ImageList(Text::CStringNN name);
		virtual ~ImageList();

		virtual IO::ParserType GetParserType() const;

		UOSInt AddImage(NN<Media::RasterImage> img, UInt32 imageDelay);
		void ReplaceImage(UOSInt index, NN<Media::RasterImage> img);
		Bool RemoveImage(UOSInt index, Bool toRelease);
		UOSInt GetCount() const;
		Optional<Media::RasterImage> GetImage(UOSInt index, OptOut<UInt32> imageDelay) const;
		UInt32 GetImageDelay(UOSInt index) const;
		ImageType GetImageType(UOSInt index) const;
		void SetImageType(UOSInt index, ImageType imgType);
		void ToStaticImage(UOSInt index);
		void SetAuthor(UnsafeArray<const UTF8Char> name);
		void SetImageName(UnsafeArrayOpt<const UTF8Char> imgName);

		void SetThermoImage(Math::Size2D<UOSInt> thermoSize, UOSInt thermoBPP, UnsafeArray<UInt8> thermoPtr, Double thermoEmissivity, Double thermoTransmission, Double thermoBKGTemp, ThermoType thermoType);
		Bool HasThermoImage() const;
		Double GetThermoValue(Double x, Double y) const;

		void SetValueInt32(ValueType valType, Int32 val);
		void SetValueInt64(ValueType valType, Int64 val);
		void SetValueStr(ValueType valType, Text::CStringNN val);
		Bool ToValueString(NN<Text::StringBuilderUTF8> sb) const;

		void ToString(NN<Text::StringBuilderUTF8> sb) const;
		static Text::CStringNN ValueTypeGetName(ValueType valType);
	};
}
#endif
