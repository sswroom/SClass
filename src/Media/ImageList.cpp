#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "IO/Path.h"
#include "Math/Math_C.h"
#include "Math/Unit/Distance.h"
#include "Media/ImageList.h"
#include "Media/RasterImage.h"
#include "Media/StaticImage.h"
#include "Media/SVGDocument.h"
#include "Media/VectorGraph.h"
#include "Text/MyString.h"

Media::ImageList::ImageList(NN<Text::String> name) : IO::ParsedObject(name)
{
	this->thermoPtr = nullptr;
	this->printCurrGraph = 0;
	this->printCurrDoc = nullptr;
}

Media::ImageList::ImageList(Text::CStringNN fileName) : IO::ParsedObject(fileName)
{
	this->thermoPtr = nullptr;
	this->printCurrGraph = 0;
	this->printCurrDoc = nullptr;
}

Media::ImageList::~ImageList()
{
	UnsafeArray<UInt8> thermoPtr;
	UIntOS i;
	this->imgList.DeleteAll();
	i = this->valStr.GetCount();
	while (i-- > 0)
	{
		OPTSTR_DEL(this->valStr.GetItem(i));
	}
	if (this->thermoPtr.SetTo(thermoPtr))
	{
		MemFreeArr(thermoPtr);
		this->thermoPtr = nullptr;
	}
}

Bool Media::ImageList::BeginPrint(NN<PrintDocument> doc)
{
	NN<Media::Image> img;
	Double width;
	Double height;
	NN<Text::String> docName;
	this->printCurrGraph = 0;
	this->printCurrDoc = doc;
	if (!this->imgList.GetItem(this->printCurrGraph).SetTo(img))
		return false;
	if (this->GetValueStr(ValueType::DocumentName).SetTo(docName))
		doc->SetDocName(docName->ToCString());
	else
	{
		Text::StringBuilderUTF8 sb;
		sb.Append(this->sourceName);
		UIntOS i = sb.LastIndexOf(IO::Path::PATH_SEPERATOR);
		doc->SetDocName(sb.ToCString().Substring(i + 1));
	}
	if (img->GetImageType() == Media::ImageType::Raster)
	{
		NN<Media::RasterImage> rimg = NN<Media::RasterImage>::ConvertFrom(img);
		width = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_PIXEL, Math::Unit::Distance::DU_MILLIMETER, UIntOS2Double(rimg->info.dispSize.x) * 96.0 / rimg->info.hdpi);
		height = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_PIXEL, Math::Unit::Distance::DU_MILLIMETER, UIntOS2Double(rimg->info.dispSize.y) * 96.0 / rimg->info.vdpi);
	}
	else if (img->GetImageType() == Media::ImageType::Vector)
	{
		NN<Media::VectorGraph> graph = NN<Media::VectorGraph>::ConvertFrom(img);
		width = graph->GetVisibleWidthMM();
		height = graph->GetVisibleHeightMM();
	}
	else if (img->GetImageType() == Media::ImageType::SVG)
	{
		NN<Media::SVGDocument> svg = NN<Media::SVGDocument>::ConvertFrom(img);
		width = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_PIXEL, Math::Unit::Distance::DU_MILLIMETER, svg->GetWidth());
		height = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_PIXEL, Math::Unit::Distance::DU_MILLIMETER, svg->GetHeight());
	}
	else
	{
		return false;
	}
	if (width > height)
	{
		doc->SetNextPagePaperSizeMM(height, width);
		doc->SetNextPageOrientation(Media::PrintDocument::PageOrientation::Landscape);
	}
	else
	{
		doc->SetNextPagePaperSizeMM(width, height);
		doc->SetNextPageOrientation(Media::PrintDocument::PageOrientation::Portrait);
	}
	return true;
}

Bool Media::ImageList::PrintPage(NN<Media::DrawImage> printPage)
{
	NN<Media::Image> img;
	Double width;
	Double height;
	NN<Media::PrintDocument> doc;
	if (!this->printCurrDoc.SetTo(doc) || !this->imgList.GetItem(this->printCurrGraph).SetTo(img))
		return false;
	if (img->GetImageType() == Media::ImageType::Raster)
	{
		NN<Media::RasterImage> rimg = NN<Media::RasterImage>::ConvertFrom(img);
		if (rimg->GetImageClass() == Media::RasterImage::ImageClass::StaticImage)
		{
			NN<Media::StaticImage> simg = NN<Media::StaticImage>::ConvertFrom(rimg);
			printPage->DrawSImagePt(simg, Math::Coord2DDbl(0, 0));
		}
		else
		{
			NN<Media::StaticImage> simg = rimg->CreateStaticImage();
			printPage->DrawSImagePt(simg, Math::Coord2DDbl(0, 0));
			simg.Delete();
		}
	}
	else if (img->GetImageType() == Media::ImageType::Vector)
	{
		NN<Media::VectorGraph> graph = NN<Media::VectorGraph>::ConvertFrom(img);
		graph->DrawTo(printPage, 0);
	}
	else if (img->GetImageType() == Media::ImageType::SVG)
	{
		//NN<Media::SVGDocument> svg = NN<Media::SVGDocument>::ConvertFrom(img);
		//svg->DrawTo(printPage, 0);
	}
	else
	{
		return false;
	}
	this->printCurrGraph++;
	if (!this->imgList.GetItem(this->printCurrGraph).SetTo(img))
		return false;
	if (img->GetImageType() == Media::ImageType::Raster)
	{
		NN<Media::RasterImage> rimg = NN<Media::RasterImage>::ConvertFrom(img);
		width = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_PIXEL, Math::Unit::Distance::DU_MILLIMETER, UIntOS2Double(rimg->info.dispSize.x) * 96.0 / rimg->info.hdpi);
		height = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_PIXEL, Math::Unit::Distance::DU_MILLIMETER, UIntOS2Double(rimg->info.dispSize.y) * 96.0 / rimg->info.vdpi);
	}
	else if (img->GetImageType() == Media::ImageType::Vector)
	{
		NN<Media::VectorGraph> graph = NN<Media::VectorGraph>::ConvertFrom(img);
		width = graph->GetVisibleWidthMM();
		height = graph->GetVisibleHeightMM();
	}
	else if (img->GetImageType() == Media::ImageType::SVG)
	{
		NN<Media::SVGDocument> svg = NN<Media::SVGDocument>::ConvertFrom(img);
		width = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_PIXEL, Math::Unit::Distance::DU_MILLIMETER, svg->GetWidth());
		height = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_PIXEL, Math::Unit::Distance::DU_MILLIMETER, svg->GetHeight());
	}
	else
	{
		return false;
	}
	if (width > height)
	{
		doc->SetNextPagePaperSizeMM(height, width);
		doc->SetNextPageOrientation(Media::PrintDocument::PageOrientation::Landscape);
	}
	else
	{
		doc->SetNextPagePaperSizeMM(width, height);
		doc->SetNextPageOrientation(Media::PrintDocument::PageOrientation::Portrait);
	}
	return true;
}

Bool Media::ImageList::EndPrint(NN<PrintDocument> doc)
{
	this->printCurrDoc = nullptr;
	return true;
}

IO::ParserType Media::ImageList::GetParserType() const
{
	return IO::ParserType::ImageList;
}

UIntOS Media::ImageList::AddImage(NN<Media::Image> img, UInt32 imageDelay)
{
	this->imgTimes.Add(imageDelay);
	this->imgTypeList.Add(Media::ImageList::ImageType::Unknown);
	return this->imgList.Add(img);
}

NN<Media::VectorGraph> Media::ImageList::AddGraph(UInt32 srid, NN<Media::DrawEngine> deng, Double width, Double height, Math::Unit::Distance::DistanceUnit unit)
{
	this->imgTimes.Add(0);
	this->imgTypeList.Add(Media::ImageList::ImageType::Unknown);
	NN<VectorGraph> graph;
	Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
	NEW_CLASSNN(graph, Media::VectorGraph(srid, Math::Unit::Distance::Convert(unit, Math::Unit::Distance::DU_PIXEL, width), Math::Unit::Distance::Convert(unit, Math::Unit::Distance::DU_PIXEL, height), deng, color));
	this->imgList.Add(graph);
	return graph;
}

void Media::ImageList::ReplaceImage(UIntOS index, NN<Media::Image> img)
{
	Optional<Media::Image> oldImg = this->imgList.GetItem(index);
	this->imgList.SetItem(index, img);
	oldImg.Delete();
}

Bool Media::ImageList::RemoveImage(UIntOS index, Bool toRelease)
{
	NN<Media::Image> oldImg;
	if (!this->imgList.RemoveAt(index).SetTo(oldImg))
		return false;
	if (toRelease)
	{
		oldImg.Delete();
	}
	return true;
}

UIntOS Media::ImageList::GetCount() const
{
	return this->imgList.GetCount();
}

Optional<Media::Image> Media::ImageList::GetImage2(UIntOS index, OptOut<UInt32> imageDelay) const
{
	imageDelay.Set(this->imgTimes.GetItem(index));
	return this->imgList.GetItem(index);
}

UInt32 Media::ImageList::GetImageDelay(UIntOS index) const
{
	return this->imgTimes.GetItem(index);
}

Media::ImageList::ImageType Media::ImageList::GetImageType(UIntOS index) const
{
	return this->imgTypeList.GetItem(index);
}

void Media::ImageList::SetImageType(UIntOS index, ImageType imgType)
{
	if (index >= this->imgList.GetCount())
	{
		return;
	}
	this->imgTypeList.SetItem(index, imgType);
}

void Media::ImageList::ToStaticImage(UIntOS index)
{
	NN<Media::Image> img;
	if (!this->imgList.GetItem(index).SetTo(img) || (img->GetImageType() == Media::ImageType::Raster && NN<Media::RasterImage>::ConvertFrom(img)->GetImageClass() == Media::RasterImage::ImageClass::StaticImage))
		return;
	NN<Media::StaticImage> simg = img->CreateStaticImage();
	this->imgList.SetItem(index, simg);
	img.Delete();
}

void Media::ImageList::SetThermoImage(Math::Size2D<UIntOS> thermoSize, UIntOS thermoBPP, UnsafeArray<UInt8> thermoPtr, Double thermoEmissivity, Double thermoTransmission, Double thermoBKGTemp, ThermoType thermoType)
{
	UnsafeArray<UInt8> nnThermoPtr;
	this->thermoSize = thermoSize;
	this->thermoBPP = thermoBPP;
	UIntOS dataSize = thermoSize.CalcArea() * thermoBPP >> 3;
	if (this->thermoPtr.SetTo(nnThermoPtr))
	{
		MemFreeArr(nnThermoPtr);
	}
	this->thermoPtr = nnThermoPtr = MemAllocArr(UInt8, dataSize);
	MemCopyNO(nnThermoPtr.Ptr(), thermoPtr.Ptr(), dataSize);
	this->thermoEmissivity = thermoEmissivity;
	this->thermoTransmission = thermoTransmission;
	this->thermoBKGTemp = thermoBKGTemp;
	this->thermoType = thermoType;
}

Bool Media::ImageList::HasThermoImage() const
{
	return this->thermoPtr.NotNull();
}

Double Media::ImageList::GetThermoValue(Double x, Double y) const
{
	UnsafeArray<UInt8> thermoPtr;
	if (!this->thermoPtr.SetTo(thermoPtr))
	{
		return 0;
	}
	IntOS xOfst = Double2Int32(x * UIntOS2Double(this->thermoSize.x));
	IntOS yOfst = Double2Int32(y * UIntOS2Double(this->thermoSize.y));
	if (xOfst < 0)
		xOfst = 0;
	else if (xOfst >= (IntOS)this->thermoSize.x)
		xOfst = (IntOS)this->thermoSize.x - 1;
	if (yOfst < 0)
		yOfst = 0;
	else if (yOfst >= (IntOS)this->thermoSize.y)
		yOfst = (IntOS)this->thermoSize.y - 1;
	Double v;
	if (this->thermoBPP == 16)
	{
		v = ReadInt16(&thermoPtr[(yOfst * (IntOS)this->thermoSize.x + xOfst) << 1]);
	}
	else if (this->thermoBPP == 8)
	{
		v = thermoPtr[yOfst * (IntOS)this->thermoSize.x + xOfst];
	}
	else if (this->thermoBPP == 32)
	{
		v = ReadInt32(&thermoPtr[(yOfst * (IntOS)this->thermoSize.x + xOfst) << 2]);
	}
	else
	{
		v = 0;
	}
	if (this->thermoType == ThermoType::Unknown)
	{
		return v;
	}
	else if (this->thermoType == ThermoType::FLIR)
	{
/*		Double B = 1500; ///////////////
		Double O = -7800; ///////////////
		Double R = 16030.829102; ////////////////////
		Double F = 1.25; //////////////////////////////
		Double T = B / Math_Ln(R / (v - O) + F);*/
		////////////////////////////////////////
		return v;
	}
	else
	{
		return v / (this->thermoEmissivity * this->thermoTransmission) - (2 - this->thermoEmissivity - this->thermoTransmission) * this->thermoBKGTemp;
	}
}

void Media::ImageList::SetValueInt32(Media::ImageList::ValueType valType, Int32 val)
{
	this->valI32.Add(val);
	this->valTypeI32.Add(valType);
}

void Media::ImageList::SetValueInt64(Media::ImageList::ValueType valType, Int64 val)
{
	this->valI64.Add(val);
	this->valTypeI64.Add(valType);
}

Int64 Media::ImageList::GetValueInt64(ValueType valType) const
{
	UIntOS i = 0;
	UIntOS j = this->valTypeI64.GetCount();
	while (i < j)
	{
		if (this->valTypeI64.GetItem(i) == valType)
		{
			return this->valI64.GetItem(i);
		}
		i++;
	}
	return 0;
}

void Media::ImageList::SetValueStr(Media::ImageList::ValueType valType, Text::CStringNN val)
{
	this->valStr.Add(Text::String::New(val));
	this->valTypeStr.Add(valType);
}

Optional<Text::String> Media::ImageList::GetValueStr(ValueType valType) const
{
	UIntOS i = 0;
	UIntOS j = this->valTypeStr.GetCount();
	while (i < j)
	{
		if (this->valTypeStr.GetItem(i) == valType)
		{
			return this->valStr.GetItem(i);
		}
		i++;
	}
	return nullptr;
}

Bool Media::ImageList::ToValueString(NN<Text::StringBuilderUTF8> sb) const
{
	UIntOS i;
	UIntOS j;
	Bool found = false;
	ValueType vt;
	NN<Text::String> s;
	if ((j = this->valStr.GetCount()) != 0)
	{
		i = 0;
		while (i < j)
		{
			if (found)
			{
				sb->AppendC(UTF8STRC("\r\n"));
			}
			vt = this->valTypeStr.GetItem(i);
			sb->Append(ValueTypeGetName(vt));
			sb->AppendC(UTF8STRC(" = "));
			if (this->valStr.GetItem(i).SetTo(s))
				sb->Append(s);
			found = true;
			i++;
		}
	}
	if ((j = this->valI32.GetCount()) != 0)
	{
		Int32 v;
		i = 0;
		while (i < j)
		{
			if (found)
			{
				sb->AppendC(UTF8STRC("\r\n"));
			}
			vt = this->valTypeI32.GetItem(i);
			sb->Append(ValueTypeGetName(vt));
			sb->AppendC(UTF8STRC(" = "));
			v = this->valI32.GetItem(i);
			if (vt == Media::ImageList::ValueType::FirmwareVersion)
			{
				sb->AppendI32(v >> 24);
				sb->AppendUTF8Char('.');
				sb->AppendI32((v >> 16) & 0xff);
				sb->AppendUTF8Char('.');
				sb->AppendI32(v & 0xffff);
			}
			else
			{
				sb->AppendI32(v);
			}
			found = true;
			i++;
		}
	}
	if ((j = this->valI64.GetCount()) != 0)
	{
		Int64 v;
		i = 0;
		while (i < j)
		{
			if (found)
			{
				sb->AppendC(UTF8STRC("\r\n"));
			}
			vt = this->valTypeI64.GetItem(i);
			sb->Append(ValueTypeGetName(vt));
			sb->AppendC(UTF8STRC(" = "));
			v = this->valI64.GetItem(i);
			if (vt == Media::ImageList::ValueType::CaptureDate)
			{
				sb->AppendTSNoZone(Data::Timestamp(v, Data::DateTimeUtil::GetLocalTzQhr()));
			}
			else if (vt == Media::ImageList::ValueType::FirmwareDate)
			{
				sb->AppendTSNoZone(Data::Timestamp(v, 0));
			}
			else
			{
				sb->AppendI64(v);
			}
			found = true;
			i++;
		}
	}
	return found;
}

void Media::ImageList::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	Bool hasData = this->ToValueString(sb);
	NN<Media::Image> img;
	UInt32 delay;
	UIntOS i = 0;
	UIntOS j = this->GetCount();
	while (i < j)
	{
		if (this->GetImage2(i, delay).SetTo(img))
		{
			if (hasData)
			{
				sb->AppendC(UTF8STRC("\r\n\r\n"));
			}
			hasData = true;
			sb->AppendC(UTF8STRC("Image "));
			sb->AppendUIntOS(i);
			sb->AppendC(UTF8STRC(":\r\nDelay = "));
			sb->AppendU32(delay);
			sb->AppendC(UTF8STRC("\r\n"));
			img->ToString(sb);
		}
		i++;
	}
}

Text::CStringNN Media::ImageList::ValueTypeGetName(Media::ImageList::ValueType valType)
{
	switch (valType)
	{
	case Media::ImageList::ValueType::IRWidth:
		return CSTR("IR Width");
	case Media::ImageList::ValueType::IRHeight:
		return CSTR("IR Height");
	case Media::ImageList::ValueType::VisibleWidth:
		return CSTR("Visible Width");
	case Media::ImageList::ValueType::VisibleHeight:
		return CSTR("Visible Height");
	case Media::ImageList::ValueType::FirmwareDate:
		return CSTR("Firmware Date");
	case Media::ImageList::ValueType::FirmwareVersion:
		return CSTR("Firmware Version");
	case Media::ImageList::ValueType::CameraBrand:
		return CSTR("Camera Brand");
	case Media::ImageList::ValueType::CameraModel:
		return CSTR("Camera Model");
	case Media::ImageList::ValueType::CameraSN:
		return CSTR("Camera SN");
	case Media::ImageList::ValueType::CaptureDate:
		return CSTR("Capture Date");
	case Media::ImageList::ValueType::CaptureWidth:
		return CSTR("Capture Width");
	case Media::ImageList::ValueType::CaptureHeight:
		return CSTR("Capture Height");
	case Media::ImageList::ValueType::Author:
		return CSTR("Author");
	case Media::ImageList::ValueType::DocumentName:
		return CSTR("Document Name");
	case Media::ImageList::ValueType::Subject:
		return CSTR("Subject");
	case Media::ImageList::ValueType::Keywords:
		return CSTR("Keywords");
	case Media::ImageList::ValueType::Creator:
		return CSTR("Creator");
	case Media::ImageList::ValueType::Producer:
		return CSTR("Producer");
	case Media::ImageList::ValueType::CreateTime:
		return CSTR("Create Time");
	case Media::ImageList::ValueType::ModifyTime:
		return CSTR("Modify Time");
	default:
		return CSTR("Unknown");
	}
}
