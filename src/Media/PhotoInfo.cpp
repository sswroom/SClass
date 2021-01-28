#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/JPEGFile.h"
#include "Media/PhotoInfo.h"
#include "Text/Encoding.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

void Media::PhotoInfo::ParseXMF(Text::XMLDocument *xmf)
{
	Text::XMLNode *node;
	if ((node = xmf->SearchFirstNode((const UTF8Char*)"//@exif:ExposureTime")) != 0)
	{
		this->expTime = ParseFraction(node->value);
	}
	if ((node = xmf->SearchFirstNode((const UTF8Char*)"//@exif:FNumber")) != 0)
	{
		this->fNumber = ParseFraction(node->value);
	}
	if ((node = xmf->SearchFirstNode((const UTF8Char*)"//@aux:Lens")) != 0)
	{
		OSInt sz;
		UTF8Char *sbuff;
		if (this->lens)
			MemFree((void*)this->lens);
		sz = Text::StrCharCnt(node->value);
		sbuff = MemAlloc(UTF8Char, sz + 1);
		MemCopyNO(sbuff, node->value, sizeof(UTF8Char) * (sz + 1));
		this->lens = sbuff;
	}
	if ((node = xmf->SearchFirstNode((const UTF8Char*)"//@exif:FocalLength")) != 0)
	{
		this->focalLength = ParseFraction(node->value);
	}
	if ((node = xmf->SearchFirstNode((const UTF8Char*)"//exif:ISOSpeedRatings")) != 0)
	{
		Text::StringBuilderUTF8 sb;
		node->GetInnerText(&sb);
		sb.Trim();
		this->isoRating = Text::StrToInt32(sb.ToString());
	}
}

Double Media::PhotoInfo::ParseFraction(const UTF8Char *s)
{
	UTF8Char sbuff[64];
	UTF8Char *sarr[3];
	OSInt cnt;
	Text::StrConcat(sbuff, s);
	cnt = Text::StrSplit(sarr, 3, sbuff, '/');
	if (cnt == 1)
	{
		return Text::StrToDouble(sbuff);
	}
	else if (cnt == 2)
	{
		return Text::StrToInt32(sarr[0]) / (Double)Text::StrToInt32(sarr[1]);
	}
	else
	{
		return 0;
	}
}

Media::PhotoInfo::PhotoInfo(IO::IStreamData *fd)
{
	Media::EXIFData *exif;
	Text::XMLDocument *xmf;
	Media::ICCProfile *icc;
	Int32 width;
	Int32 height;
	this->make = 0;
	this->model = 0;
	this->photoDate = 0;
	this->width = 0;
	this->height = 0;
	this->fNumber = 0;
	this->expTime = 0;
	this->isoRating = 0;
	this->focalLength = 0;
	this->lens = 0;
	this->succ = false;

	if (Media::JPEGFile::ParseJPEGHeaders(fd, &exif, &xmf, &icc, &width, &height))
	{	
		const Char *ctxt;
		Data::DateTime dt;
		this->succ = true;

		if (exif)
		{
			ctxt = exif->GetPhotoMake();
			if (ctxt)
			{
				this->make = Text::StrCopyNew((const UTF8Char*)ctxt);
			}
			ctxt = exif->GetPhotoModel();
			if (ctxt)
			{
				this->model = Text::StrCopyNew((const UTF8Char*)ctxt);
			}
			this->fNumber = exif->GetPhotoFNumber();
			this->expTime = exif->GetPhotoExpTime();
			this->isoRating = exif->GetPhotoISO();
			this->focalLength = exif->GetPhotoFocalLength();

			if (exif->GetPhotoDate(&dt))
			{
				NEW_CLASS(this->photoDate, Data::DateTime(&dt));
			}

			Media::EXIFData::EXIFItem *item;
			if ((item = exif->GetExifItem(34665)) != 0)
			{
				if (item->type == Media::EXIFData::ET_SUBEXIF)
				{
					Media::EXIFData *exif2 = (Media::EXIFData*)item->dataBuff;
					if ((item = exif2->GetExifItem(37500)) != 0)
					{
						UInt8 *valBuff;
						if (item->size <= 4)
						{
							valBuff = (UInt8*)&item->value;
						}
						else
						{
							valBuff = (UInt8*)item->dataBuff;
						}
						Media::EXIFData *innerExif = exif->ParseMakerNote(valBuff, item->size);
						if (innerExif)
						{
							if (innerExif->GetPhotoExpTime())
								this->expTime = innerExif->GetPhotoExpTime();
							if (innerExif->GetPhotoFNumber())
								this->fNumber = innerExif->GetPhotoFNumber();
							ctxt = innerExif->GetPhotoLens();
							if (ctxt)
							{
								this->lens = Text::StrCopyNew((const UTF8Char*)ctxt);
							}
							if (innerExif->GetPhotoFocalLength())
								this->focalLength = innerExif->GetPhotoFocalLength();
							if (innerExif->GetPhotoISO())
								this->isoRating = innerExif->GetPhotoISO();
							DEL_CLASS(innerExif);
						}
					}
				}
			}
			else if ((item = exif->GetExifItem(700)) != 0)
			{
				Text::XMLDocument *doc;
				Text::EncodingFactory encFact;
				NEW_CLASS(doc, Text::XMLDocument());
				if (doc->ParseBuff(&encFact, (const UInt8*)item->dataBuff, item->size))
				{
					ParseXMF(doc);
				}
				DEL_CLASS(doc);
			}
			DEL_CLASS(exif);
		}
		if (xmf)
		{
			ParseXMF(xmf);
			DEL_CLASS(xmf);
		}
		if (icc)
		{
			DEL_CLASS(icc);
		}
		this->width = width;
		this->height = height;
	}
}

Media::PhotoInfo::~PhotoInfo()
{
	if (this->make)
		MemFree((void*)this->make);
	if (this->model)
		MemFree((void*)this->model);
	if (this->lens)
		MemFree((void*)this->lens);
	if (this->photoDate)
		DEL_CLASS(this->photoDate);
}

Bool Media::PhotoInfo::HasInfo()
{
	return this->succ;
}

Bool Media::PhotoInfo::GetPhotoDate(Data::DateTime *dt)
{
	if (this->photoDate)
	{
		dt->SetValue(this->photoDate);
		return true;
	}
	return false;
}

void Media::PhotoInfo::ToString(Text::StringBuilderUTF *sb)
{
	UTF8Char sbuff[32];
	if (this->make == 0)
	{
		if (this->model == 0)
		{
			sb->Append((const UTF8Char*)"Unknown camera");
		}
		else
		{
			sb->Append(this->model);
		}
	}
	else
	{
		if (this->model == 0)
		{
			sb->Append(this->make);
		}
		else if (Text::StrStartsWith(this->model, this->make))
		{
			sb->Append(this->model);
		}
		else
		{
			sb->Append(this->make);
			sb->Append((const UTF8Char*)" ");
			sb->Append(this->model);
		}
	}

	if (this->width != 0 && this->height != 0)
	{
		sb->Append((const UTF8Char*)" ");
		sb->AppendI32(this->width);
		sb->Append((const UTF8Char*)"x");
		sb->AppendI32(this->height);
	}

	if (this->fNumber != 0)
	{
		sb->Append((const UTF8Char*)" f/");
		Text::StrDoubleFmt(sbuff, this->fNumber, "0.0");
		sb->Append(sbuff);
	}

	if (this->expTime != 0)
	{
		sb->Append((const UTF8Char*)" ");
		if (this->expTime >= 1)
		{
			Text::StrDoubleFmt(sbuff, this->expTime, "0.0");
			sb->Append(sbuff);
		}
		else if (this->expTime < 0.3)
		{
			sb->Append((const UTF8Char*)"1/");
			sb->AppendI32(Math::Double2Int32(1.0 / this->expTime));
		}
		else
		{
			Double invExp = 1.0 / this->expTime;
			if (invExp == Math::Double2Int32(invExp))
			{
				sb->Append((const UTF8Char*)"1/");
				sb->AppendI32(Math::Double2Int32(invExp));
			}
			else
			{
				Text::StrDoubleFmt(sbuff, this->expTime, "0.0");
				sb->Append(sbuff);
			}
		}
		sb->Append((const UTF8Char*)"sec");
	}
	
	if (this->isoRating != 0)
	{
		sb->Append((const UTF8Char*)" ISO");
		sb->AppendI32(this->isoRating);
	}

	if (this->lens)
	{
		sb->Append((const UTF8Char*)" Lens ");
		sb->Append(this->lens);
	}

	if (this->focalLength != 0)
	{
		sb->Append((const UTF8Char*)"@");
		Text::SBAppendF64(sb, this->focalLength);
		sb->Append((const UTF8Char*)"mm");
	}
}
