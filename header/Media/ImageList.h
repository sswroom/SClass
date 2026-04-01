#ifndef _SM_MEDIA_IMAGELIST
#define _SM_MEDIA_IMAGELIST
#include "Data/ArrayListNative.hpp"
#include "Data/ArrayListStringNN.h"
#include "Data/ArrayListUInt32.h"
#include "IO/ParsedObject.h"
#include "Media/PrintDocument.h"
#include "Media/RasterImage.h"
#include "Media/VectorGraph.h"
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"

namespace Media
{
	class ImageList : public IO::ParsedObject, public Media::PrintHandler
	{
	public:
		enum class ImageType
		{
			Unknown,
			VisibleImage,
			IRImage,
			OutputImage
		};

		enum class ValueType
		{
			IRWidth,
			IRHeight,
			VisibleWidth,
			VisibleHeight,
			FirmwareDate,
			FirmwareVersion,
			CameraBrand,
			CameraModel,
			CameraSN,
			CaptureDate,
			CaptureWidth,
			CaptureHeight,
			Author,
			DocumentName,
			Subject,
			Keywords,
			Creator,
			Producer,
			CreateTime,
			ModifyTime
		};

		enum class ThermoType
		{
			Unknown,
			FLIR
		};
	public:
		Data::ArrayListNN<Media::Image> imgList;
		Data::ArrayListNative<ImageType> imgTypeList;
		Data::ArrayListNative<UInt32> imgTimes;
		Data::ArrayListNative<ValueType> valTypeI32;
		Data::ArrayListNative<Int32> valI32;
		Data::ArrayListNative<ValueType> valTypeI64;
		Data::ArrayListNative<Int64> valI64;
		Data::ArrayListNative<ValueType> valTypeStr;
		Data::ArrayListStringNN valStr;

		Math::Size2D<UIntOS> thermoSize;
		UIntOS thermoBPP;
		UnsafeArrayOpt<UInt8> thermoPtr;
		Double thermoEmissivity;
		Double thermoTransmission;
		Double thermoBKGTemp;
		ThermoType thermoType;

		UIntOS printCurrGraph;
		Optional<Media::PrintDocument> printCurrDoc;

		ImageList(NN<Text::String> name);
		ImageList(Text::CStringNN name);
		virtual ~ImageList();

		virtual Bool BeginPrint(NN<PrintDocument> doc);
		virtual Bool PrintPage(NN<Media::DrawImage> printPage); //return has more pages 
		virtual Bool EndPrint(NN<PrintDocument> doc);

		virtual IO::ParserType GetParserType() const;

		UIntOS AddImage(NN<Media::Image> img, UInt32 imageDelay);
		NN<VectorGraph> AddGraph(UInt32 srid, NN<Media::DrawEngine> deng, Double width, Double height, Math::Unit::Distance::DistanceUnit unit);
		void ReplaceImage(UIntOS index, NN<Media::Image> img);
		Bool RemoveImage(UIntOS index, Bool toRelease);
		UIntOS GetCount() const;
		Optional<Media::Image> GetImage2(UIntOS index, OptOut<UInt32> imageDelay) const;
		UInt32 GetImageDelay(UIntOS index) const;
		ImageType GetImageType(UIntOS index) const;
		void SetImageType(UIntOS index, ImageType imgType);
		void ToStaticImage(UIntOS index);

		void SetThermoImage(Math::Size2D<UIntOS> thermoSize, UIntOS thermoBPP, UnsafeArray<UInt8> thermoPtr, Double thermoEmissivity, Double thermoTransmission, Double thermoBKGTemp, ThermoType thermoType);
		Bool HasThermoImage() const;
		Double GetThermoValue(Double x, Double y) const;

		void SetValueInt32(ValueType valType, Int32 val);
		void SetValueInt64(ValueType valType, Int64 val);
		Int64 GetValueInt64(ValueType valType) const;
		void SetValueStr(ValueType valType, Text::CStringNN val);
		Optional<Text::String> GetValueStr(ValueType valType) const;
		Bool ToValueString(NN<Text::StringBuilderUTF8> sb) const;

		void ToString(NN<Text::StringBuilderUTF8> sb) const;
		static Text::CStringNN ValueTypeGetName(ValueType valType);
	};
}
#endif
