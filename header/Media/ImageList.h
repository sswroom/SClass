#ifndef _SM_MEDIA_IMAGELIST
#define _SM_MEDIA_IMAGELIST
#include "Media/Image.h"
#include "IO/ParsedObject.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListInt32.h"

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
		Data::ArrayList<Media::Image*> *imgList;
		Data::ArrayList<ImageType> *imgTypeList;
		Data::ArrayListInt32 *imgTimes;
		const UTF8Char *author;
		const UTF8Char *imgName;
		Data::ArrayList<ValueType> *valTypeI32;
		Data::ArrayList<Int32> *valI32;
		Data::ArrayList<ValueType> *valTypeI64;
		Data::ArrayList<Int64> *valI64;
		Data::ArrayList<ValueType> *valTypeStr;
		Data::ArrayList<const UTF8Char *> *valStr;

		UOSInt thermoWidth;
		UOSInt thermoHeight;
		UOSInt thermoBPP;
		UInt8 *thermoPtr;
		Double thermoEmissivity;
		Double thermoTransmission;
		Double thermoBKGTemp;
		ThermoType thermoType;

		ImageList(const UTF8Char *name);
		virtual ~ImageList();

		virtual IO::ParsedObject::ParserType GetParserType();

		UOSInt AddImage(Media::Image *img, Int32 imageDelay);
		void ReplaceImage(UOSInt index, Media::Image *img);
		Bool RemoveImage(UOSInt index, Bool toRelease);
		UOSInt GetCount();
		Media::Image *GetImage(UOSInt index, Int32 *imageDelay);
		ImageType GetImageType(UOSInt index);
		void SetImageType(UOSInt index, ImageType imgType);
		void ToStaticImage(UOSInt index);
		void SetAuthor(const UTF8Char *name);
		void SetImageName(const UTF8Char *imgName);

		void SetThermoImage(UOSInt thermoWidth, UOSInt thermoHeight, UOSInt thermoBPP, UInt8 *thermoPtr, Double thermoEmissivity, Double thermoTransmission, Double thermoBKGTemp, ThermoType thermoType);
		Bool HasThermoImage();
		Double GetThermoValue(Double x, Double y);

		void SetValueInt32(ValueType valType, Int32 val);
		void SetValueInt64(ValueType valType, Int64 val);
		void SetValueStr(ValueType valType, const UTF8Char *val);
		Bool ToValueString(Text::StringBuilderUTF *sb);

		void ToString(Text::StringBuilderUTF *sb);
		static const UTF8Char *GetValueTypeName(ValueType valType);
	};
}
#endif
