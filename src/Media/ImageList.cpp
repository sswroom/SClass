#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Math/Math.h"
#include "Media/ImageList.h"
#include "Media/StaticImage.h"
#include "Text/MyString.h"

Media::ImageList::ImageList(NN<Text::String> name) : IO::ParsedObject(name)
{
	this->author = 0;
	this->imgName = 0;
	this->thermoPtr = 0;
}

Media::ImageList::ImageList(Text::CStringNN fileName) : IO::ParsedObject(fileName)
{
	this->author = 0;
	this->imgName = 0;
	this->thermoPtr = 0;
}

Media::ImageList::~ImageList()
{
	UOSInt i;
	this->imgList.DeleteAll();
	i = this->valStr.GetCount();
	while (i-- > 0)
	{
		OPTSTR_DEL(this->valStr.GetItem(i));
	}
	if (this->thermoPtr)
	{
		MemFree(this->thermoPtr);
		this->thermoPtr = 0;
	}
	SDEL_TEXT(this->author);
	SDEL_TEXT(this->imgName);
}

IO::ParserType Media::ImageList::GetParserType() const
{
	return IO::ParserType::ImageList;
}

UOSInt Media::ImageList::AddImage(NN<Media::RasterImage> img, UInt32 imageDelay)
{
	this->imgTimes.Add(imageDelay);
	this->imgTypeList.Add(Media::ImageList::IT_UNKNOWN);
	return this->imgList.Add(img);
}

void Media::ImageList::ReplaceImage(UOSInt index, NN<Media::RasterImage> img)
{
	Optional<Media::RasterImage> oldImg = this->imgList.GetItem(index);
	this->imgList.SetItem(index, img);
	oldImg.Delete();
}

Bool  Media::ImageList::RemoveImage(UOSInt index, Bool toRelease)
{
	NN<Media::RasterImage> oldImg;
	if (!this->imgList.RemoveAt(index).SetTo(oldImg))
		return false;
	if (toRelease)
	{
		oldImg.Delete();
	}
	return true;
}

UOSInt Media::ImageList::GetCount() const
{
	return this->imgList.GetCount();
}

Optional<Media::RasterImage> Media::ImageList::GetImage(UOSInt index, OptOut<UInt32> imageDelay) const
{
	imageDelay.Set(this->imgTimes.GetItem(index));
	return this->imgList.GetItem(index);
}

UInt32 Media::ImageList::GetImageDelay(UOSInt index) const
{
	return this->imgTimes.GetItem(index);
}

Media::ImageList::ImageType Media::ImageList::GetImageType(UOSInt index) const
{
	return this->imgTypeList.GetItem(index);
}

void Media::ImageList::SetImageType(UOSInt index, ImageType imgType)
{
	if (index >= this->imgList.GetCount())
	{
		return;
	}
	this->imgTypeList.SetItem(index, imgType);
}

void Media::ImageList::ToStaticImage(UOSInt index)
{
	NN<Media::RasterImage> img;
	if (!this->imgList.GetItem(index).SetTo(img) || img->GetImageType() == Media::RasterImage::ImageType::Static)
		return;
	NN<Media::StaticImage> simg = img->CreateStaticImage();
	this->imgList.SetItem(index, simg);
	img.Delete();
}

void Media::ImageList::SetAuthor(UnsafeArray<const UTF8Char> name)
{
	UnsafeArray<const UTF8Char> nns;
	if (this->author.SetTo(nns))
	{
		Text::StrDelNew(nns);
	}
	this->author = Text::StrCopyNew(name);
}

void Media::ImageList::SetImageName(UnsafeArrayOpt<const UTF8Char> imgName)
{
	UnsafeArray<const UTF8Char> nns;
	if (this->imgName.SetTo(nns))
	{
		Text::StrDelNew(nns);
	}
	this->imgName = Text::StrSCopyNew(imgName);
}

void Media::ImageList::SetThermoImage(Math::Size2D<UOSInt> thermoSize, UOSInt thermoBPP, UnsafeArray<UInt8> thermoPtr, Double thermoEmissivity, Double thermoTransmission, Double thermoBKGTemp, ThermoType thermoType)
{
	this->thermoSize = thermoSize;
	this->thermoBPP = thermoBPP;
	UOSInt dataSize = thermoSize.CalcArea() * thermoBPP >> 3;
	if (this->thermoPtr)
	{
		MemFree(this->thermoPtr);
	}
	this->thermoPtr = MemAlloc(UInt8, dataSize);
	MemCopyNO(this->thermoPtr, thermoPtr.Ptr(), dataSize);
	this->thermoEmissivity = thermoEmissivity;
	this->thermoTransmission = thermoTransmission;
	this->thermoBKGTemp = thermoBKGTemp;
	this->thermoType = thermoType;
}

Bool Media::ImageList::HasThermoImage() const
{
	return this->thermoPtr != 0;
}

Double Media::ImageList::GetThermoValue(Double x, Double y) const
{
	if (this->thermoPtr == 0)
	{
		return 0;
	}
	OSInt xOfst = Double2Int32(x * UOSInt2Double(this->thermoSize.x));
	OSInt yOfst = Double2Int32(y * UOSInt2Double(this->thermoSize.y));
	if (xOfst < 0)
		xOfst = 0;
	else if (xOfst >= (OSInt)this->thermoSize.x)
		xOfst = (OSInt)this->thermoSize.x - 1;
	if (yOfst < 0)
		yOfst = 0;
	else if (yOfst >= (OSInt)this->thermoSize.y)
		yOfst = (OSInt)this->thermoSize.y - 1;
	Double v;
	if (this->thermoBPP == 16)
	{
		v = ReadInt16(&this->thermoPtr[(yOfst * (OSInt)this->thermoSize.x + xOfst) << 1]);
	}
	else if (this->thermoBPP == 8)
	{
		v = this->thermoPtr[yOfst * (OSInt)this->thermoSize.x + xOfst];
	}
	else if (this->thermoBPP == 32)
	{
		v = ReadInt32(&this->thermoPtr[(yOfst * (OSInt)this->thermoSize.x + xOfst) << 2]);
	}
	else
	{
		v = 0;
	}
	if (this->thermoType == TT_UNKNOWN)
	{
		return v;
	}
	else if (this->thermoType == TT_FLIR)
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

void Media::ImageList::SetValueStr(Media::ImageList::ValueType valType, Text::CStringNN val)
{
	this->valStr.Add(Text::String::New(val));
	this->valTypeStr.Add(valType);
}

Bool Media::ImageList::ToValueString(NN<Text::StringBuilderUTF8> sb) const
{
	UOSInt i;
	UOSInt j;
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
			if (vt == VT_FIRMWARE_VERSION)
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
			if (vt == VT_CAPTURE_DATE)
			{
				sb->AppendTSNoZone(Data::Timestamp(v, Data::DateTimeUtil::GetLocalTzQhr()));
			}
			else if (vt == VT_FIRMWARE_DATE)
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
	NN<Media::RasterImage> img;
	UInt32 delay;
	UOSInt i = 0;
	UOSInt j = this->GetCount();
	while (i < j)
	{
		if (this->GetImage(i, delay).SetTo(img))
		{
			if (hasData)
			{
				sb->AppendC(UTF8STRC("\r\n\r\n"));
			}
			hasData = true;
			sb->AppendC(UTF8STRC("Image "));
			sb->AppendUOSInt(i);
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
	case VT_IR_WIDTH:
		return CSTR("IR Width");
	case VT_IR_HEIGHT:
		return CSTR("IR Height");
	case VT_VISIBLE_WIDTH:
		return CSTR("Visible Width");
	case VT_VISIBLE_HEIGHT:
		return CSTR("Visible Height");
	case VT_FIRMWARE_DATE:
		return CSTR("Firmware Date");
	case VT_FIRMWARE_VERSION:
		return CSTR("Firmware Version");
	case VT_CAMERA_BRAND:
		return CSTR("Camera Brand");
	case VT_CAMERA_MODEL:
		return CSTR("Camera Model");
	case VT_CAMERA_SN:
		return CSTR("Camera SN");
	case VT_CAPTURE_DATE:
		return CSTR("Capture Date");
	case VT_CAPTURE_WIDTH:
		return CSTR("Capture Width");
	case VT_CAPTURE_HEIGHT:
		return CSTR("Capture Height");
	default:
		return CSTR("Unknown");
	}
}
