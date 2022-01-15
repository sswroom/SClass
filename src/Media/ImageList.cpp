#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Math/Math.h"
#include "Media/ImageList.h"
#include "Media/StaticImage.h"
#include "Text/MyString.h"

Media::ImageList::ImageList(Text::String *name) : IO::ParsedObject(name)
{
	NEW_CLASS(this->imgList, Data::ArrayList<Media::Image*>());
	NEW_CLASS(this->imgTimes, Data::ArrayListUInt32());
	NEW_CLASS(this->imgTypeList, Data::ArrayList<ImageType>());
	NEW_CLASS(this->valTypeI32, Data::ArrayList<ValueType>());
	NEW_CLASS(this->valI32, Data::ArrayList<Int32>());
	NEW_CLASS(this->valTypeI64, Data::ArrayList<ValueType>());
	NEW_CLASS(this->valI64, Data::ArrayList<Int64>());
	NEW_CLASS(this->valTypeStr, Data::ArrayList<ValueType>());
	NEW_CLASS(this->valStr, Data::ArrayList<const UTF8Char *>());
	this->author = 0;
	this->imgName = 0;
	this->thermoPtr = 0;
}

Media::ImageList::ImageList(const UTF8Char *fileName) : IO::ParsedObject(fileName)
{
	NEW_CLASS(this->imgList, Data::ArrayList<Media::Image*>());
	NEW_CLASS(this->imgTimes, Data::ArrayListUInt32());
	NEW_CLASS(this->imgTypeList, Data::ArrayList<ImageType>());
	NEW_CLASS(this->valTypeI32, Data::ArrayList<ValueType>());
	NEW_CLASS(this->valI32, Data::ArrayList<Int32>());
	NEW_CLASS(this->valTypeI64, Data::ArrayList<ValueType>());
	NEW_CLASS(this->valI64, Data::ArrayList<Int64>());
	NEW_CLASS(this->valTypeStr, Data::ArrayList<ValueType>());
	NEW_CLASS(this->valStr, Data::ArrayList<const UTF8Char *>());
	this->author = 0;
	this->imgName = 0;
	this->thermoPtr = 0;
}

Media::ImageList::~ImageList()
{
	UOSInt i = imgList->GetCount();
	while (i-- > 0)
	{
		DEL_CLASS(imgList->RemoveAt(i));
	}
	DEL_CLASS(this->imgList);
	DEL_CLASS(this->imgTimes);
	DEL_CLASS(this->imgTypeList);
	DEL_CLASS(this->valTypeI32);
	DEL_CLASS(this->valI32);
	DEL_CLASS(this->valTypeI64);
	DEL_CLASS(this->valI64);
	DEL_CLASS(this->valTypeStr);
	i = this->valStr->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->valStr->GetItem(i));
	}
	DEL_CLASS(this->valStr);
	if (this->thermoPtr)
	{
		MemFree(this->thermoPtr);
		this->thermoPtr = 0;
	}
	if (this->author)
	{
		Text::StrDelNew(this->author);
		this->author = 0;
	}
	if (this->imgName)
	{
		Text::StrDelNew(this->imgName);
		this->imgName = 0;
	}
}

IO::ParserType Media::ImageList::GetParserType()
{
	return IO::ParserType::ImageList;
}

UOSInt Media::ImageList::AddImage(Media::Image *img, UInt32 imageDelay)
{
	this->imgTimes->Add(imageDelay);
	this->imgTypeList->Add(Media::ImageList::IT_UNKNOWN);
	return this->imgList->Add(img);
}

void Media::ImageList::ReplaceImage(UOSInt index, Media::Image *img)
{
	Media::Image *oldImg = this->imgList->GetItem(index);
	this->imgList->SetItem(index, img);
	DEL_CLASS(oldImg);
}

Bool  Media::ImageList::RemoveImage(UOSInt index, Bool toRelease)
{
	Media::Image *oldImg = this->imgList->RemoveAt(index);
	if (oldImg == 0)
		return false;
	if (toRelease)
	{
		DEL_CLASS(oldImg);
	}
	return true;
}

UOSInt Media::ImageList::GetCount()
{
	return imgList->GetCount();
}

Media::Image *Media::ImageList::GetImage(UOSInt index, UInt32 *imageDelay)
{
	if (imageDelay)
	{
		*imageDelay = imgTimes->GetItem(index);
	}
	return (Media::Image*)imgList->GetItem(index);
}

Media::ImageList::ImageType Media::ImageList::GetImageType(UOSInt index)
{
	return this->imgTypeList->GetItem(index);
}

void Media::ImageList::SetImageType(UOSInt index, ImageType imgType)
{
	if (index >= imgList->GetCount())
	{
		return;
	}
	this->imgTypeList->SetItem(index, imgType);
}

void Media::ImageList::ToStaticImage(UOSInt index)
{
	Media::Image *img = imgList->GetItem(index);
	if (img == 0 || img->GetImageType() == Media::Image::IT_STATIC)
		return;
	Media::StaticImage *simg = img->CreateStaticImage();
	imgList->SetItem(index, simg);
	DEL_CLASS(img);
}

void Media::ImageList::SetAuthor(const UTF8Char *name)
{
	if (this->author)
	{
		Text::StrDelNew(this->author);
	}
	this->author = Text::StrCopyNew(name);
}

void Media::ImageList::SetImageName(const UTF8Char *imgName)
{
	if (this->imgName)
	{
		Text::StrDelNew(this->imgName);
	}
	if (imgName)
	{
		this->imgName = Text::StrCopyNew(imgName);
	}
	else
	{
		this->imgName = 0;
	}
}

void Media::ImageList::SetThermoImage(UOSInt thermoWidth, UOSInt thermoHeight, UOSInt thermoBPP, UInt8 *thermoPtr, Double thermoEmissivity, Double thermoTransmission, Double thermoBKGTemp, ThermoType thermoType)
{
	this->thermoWidth = thermoWidth;
	this->thermoHeight = thermoHeight;
	this->thermoBPP = thermoBPP;
	UOSInt dataSize = thermoWidth * thermoHeight * thermoBPP >> 3;
	if (this->thermoPtr)
	{
		MemFree(this->thermoPtr);
	}
	this->thermoPtr = MemAlloc(UInt8, dataSize);
	MemCopyNO(this->thermoPtr, thermoPtr, dataSize);
	this->thermoEmissivity = thermoEmissivity;
	this->thermoTransmission = thermoTransmission;
	this->thermoBKGTemp = thermoBKGTemp;
	this->thermoType = thermoType;
}

Bool Media::ImageList::HasThermoImage()
{
	return this->thermoPtr != 0;
}

Double Media::ImageList::GetThermoValue(Double x, Double y)
{
	if (this->thermoPtr == 0)
	{
		return 0;
	}
	OSInt xOfst = Math::Double2Int32(x * Math::UOSInt2Double(this->thermoWidth));
	OSInt yOfst = Math::Double2Int32(y * Math::UOSInt2Double(this->thermoHeight));
	if (xOfst < 0)
		xOfst = 0;
	else if (xOfst >= (OSInt)this->thermoWidth)
		xOfst = (OSInt)this->thermoWidth - 1;
	if (yOfst < 0)
		yOfst = 0;
	else if (yOfst >= (OSInt)this->thermoHeight)
		yOfst = (OSInt)this->thermoHeight - 1;
	Double v;
	if (this->thermoBPP == 16)
	{
		v = ReadInt16(&this->thermoPtr[(yOfst * (OSInt)this->thermoWidth + xOfst) << 1]);
	}
	else if (this->thermoBPP == 8)
	{
		v = this->thermoPtr[yOfst * (OSInt)this->thermoWidth + xOfst];
	}
	else if (this->thermoBPP == 32)
	{
		v = ReadInt32(&this->thermoPtr[(yOfst * (OSInt)this->thermoWidth + xOfst) << 2]);
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
	this->valI32->Add(val);
	this->valTypeI32->Add(valType);
}

void Media::ImageList::SetValueInt64(Media::ImageList::ValueType valType, Int64 val)
{
	this->valI64->Add(val);
	this->valTypeI64->Add(valType);
}

void Media::ImageList::SetValueStr(Media::ImageList::ValueType valType, const UTF8Char *val)
{
	this->valStr->Add(Text::StrCopyNew(val));
	this->valTypeStr->Add(valType);
}

Bool Media::ImageList::ToValueString(Text::StringBuilderUTF *sb)
{
	UOSInt i;
	UOSInt j;
	Bool found = false;
	ValueType vt;
	if ((j = this->valStr->GetCount()) != 0)
	{
		i = 0;
		while (i < j)
		{
			if (found)
			{
				sb->AppendC(UTF8STRC("\r\n"));
			}
			vt = this->valTypeStr->GetItem(i);
			sb->Append(GetValueTypeName(vt));
			sb->AppendC(UTF8STRC(" = "));
			sb->Append(this->valStr->GetItem(i));
			found = true;
			i++;
		}
	}
	if ((j = this->valI32->GetCount()) != 0)
	{
		Int32 v;
		i = 0;
		while (i < j)
		{
			if (found)
			{
				sb->AppendC(UTF8STRC("\r\n"));
			}
			vt = this->valTypeI32->GetItem(i);
			sb->Append(GetValueTypeName(vt));
			sb->AppendC(UTF8STRC(" = "));
			v = this->valI32->GetItem(i);
			if (vt == VT_FIRMWARE_VERSION)
			{
				sb->AppendI32(v >> 24);
				sb->AppendChar('.', 1);
				sb->AppendI32((v >> 16) & 0xff);
				sb->AppendChar('.', 1);
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
	if ((j = this->valI64->GetCount()) != 0)
	{
		Data::DateTime dt;
		Int64 v;
		i = 0;
		while (i < j)
		{
			if (found)
			{
				sb->AppendC(UTF8STRC("\r\n"));
			}
			vt = this->valTypeI64->GetItem(i);
			sb->Append(GetValueTypeName(vt));
			sb->AppendC(UTF8STRC(" = "));
			v = this->valI64->GetItem(i);
			if (vt == VT_CAPTURE_DATE)
			{
				dt.SetTicks(v);
				dt.ToLocalTime();
				sb->AppendDate(&dt);
			}
			else if (vt == VT_FIRMWARE_DATE)
			{
				dt.SetTicks(v);
				dt.ToUTCTime();
				sb->AppendDate(&dt);
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

void Media::ImageList::ToString(Text::StringBuilderUTF *sb)
{
	Bool hasData = this->ToValueString(sb);
	Media::Image *img;
	UInt32 delay;
	UOSInt i = 0;
	UOSInt j = this->GetCount();
	while (i < j)
	{
		img = this->GetImage(i, &delay);
		if (img)
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

const UTF8Char *Media::ImageList::GetValueTypeName(Media::ImageList::ValueType valType)
{
	switch (valType)
	{
	case VT_IR_WIDTH:
		return (const UTF8Char*)"IR Width";
	case VT_IR_HEIGHT:
		return (const UTF8Char*)"IR Height";
	case VT_VISIBLE_WIDTH:
		return (const UTF8Char*)"Visible Width";
	case VT_VISIBLE_HEIGHT:
		return (const UTF8Char*)"Visible Height";
	case VT_FIRMWARE_DATE:
		return (const UTF8Char*)"Firmware Date";
	case VT_FIRMWARE_VERSION:
		return (const UTF8Char*)"Firmware Version";
	case VT_CAMERA_BRAND:
		return (const UTF8Char*)"Camera Brand";
	case VT_CAMERA_MODEL:
		return (const UTF8Char*)"Camera Model";
	case VT_CAMERA_SN:
		return (const UTF8Char*)"Camera SN";
	case VT_CAPTURE_DATE:
		return (const UTF8Char*)"Capture Date";
	case VT_CAPTURE_WIDTH:
		return (const UTF8Char*)"Capture Width";
	case VT_CAPTURE_HEIGHT:
		return (const UTF8Char*)"Capture Height";
	default:
		return (const UTF8Char*)"Unknown";
	}
}
