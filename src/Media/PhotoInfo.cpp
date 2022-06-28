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
	if ((node = xmf->SearchFirstNode(CSTR("//@exif:ExposureTime"))) != 0)
	{
		this->expTime = ParseFraction(node->value);
	}
	if ((node = xmf->SearchFirstNode(CSTR("//@exif:FNumber"))) != 0)
	{
		this->fNumber = ParseFraction(node->value);
	}
	if ((node = xmf->SearchFirstNode(CSTR("//@aux:Lens"))) != 0)
	{
		SDEL_STRING(this->lens);
		this->lens = node->value->Clone();
	}
	if ((node = xmf->SearchFirstNode(CSTR("//@exif:FocalLength"))) != 0)
	{
		this->focalLength = ParseFraction(node->value);
	}
	if ((node = xmf->SearchFirstNode(CSTR("//exif:ISOSpeedRatings"))) != 0)
	{
		Text::StringBuilderUTF8 sb;
		node->GetInnerText(&sb);
		sb.Trim();
		this->isoRating = Text::StrToUInt32(sb.ToString());
	}
}

Double Media::PhotoInfo::ParseFraction(Text::String *s)
{
	UTF8Char sbuff[64];
	UTF8Char *sarr[3];
	UOSInt cnt;
	s->ConcatTo(sbuff);
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
	UInt32 width;
	UInt32 height;
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
		Text::CString ctxt;
		Data::DateTime dt;
		this->succ = true;

		if (exif)
		{
			ctxt = exif->GetPhotoMake();
			if (ctxt.v)
			{
				this->make = Text::String::New(ctxt);
			}
			ctxt = exif->GetPhotoModel();
			if (ctxt.v)
			{
				this->model = Text::String::New(ctxt);
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
						if (item->cnt <= 4)
						{
							valBuff = (UInt8*)&item->value;
						}
						else
						{
							valBuff = (UInt8*)item->dataBuff;
						}
						Media::EXIFData *innerExif = exif->ParseMakerNote(valBuff, item->cnt);
						if (innerExif)
						{
							if (innerExif->GetPhotoExpTime())
								this->expTime = innerExif->GetPhotoExpTime();
							if (innerExif->GetPhotoFNumber())
								this->fNumber = innerExif->GetPhotoFNumber();
							ctxt = innerExif->GetPhotoLens();
							if (ctxt.v)
							{
								this->lens = Text::String::New(ctxt);
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
				if (doc->ParseBuff(&encFact, (const UInt8*)item->dataBuff, item->cnt))
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
	SDEL_STRING(this->make);
	SDEL_STRING(this->model);
	SDEL_STRING(this->lens);
	if (this->photoDate)
		DEL_CLASS(this->photoDate);
}

Bool Media::PhotoInfo::HasInfo() const
{
	return this->succ;
}

Bool Media::PhotoInfo::GetPhotoDate(Data::DateTime *dt) const
{
	if (this->photoDate)
	{
		dt->SetValue(this->photoDate);
		return true;
	}
	return false;
}

void Media::PhotoInfo::ToString(Text::StringBuilderUTF8 *sb) const
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	if (this->make == 0)
	{
		if (this->model == 0)
		{
			sb->AppendC(UTF8STRC("Unknown camera"));
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
		else if (this->model->StartsWith(this->make))
		{
			sb->Append(this->model);
		}
		else
		{
			sb->Append(this->make);
			sb->AppendC(UTF8STRC(" "));
			sb->Append(this->model);
		}
	}

	if (this->width != 0 && this->height != 0)
	{
		sb->AppendC(UTF8STRC(" "));
		sb->AppendU32(this->width);
		sb->AppendC(UTF8STRC("x"));
		sb->AppendU32(this->height);
	}

	if (this->fNumber != 0)
	{
		sb->AppendC(UTF8STRC(" f/"));
		sptr = Text::StrDoubleFmt(sbuff, this->fNumber, "0.0");
		sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
	}

	if (this->expTime != 0)
	{
		sb->AppendC(UTF8STRC(" "));
		if (this->expTime >= 1)
		{
			sptr = Text::StrDoubleFmt(sbuff, this->expTime, "0.0");
			sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
		}
		else if (this->expTime < 0.3)
		{
			sb->AppendC(UTF8STRC("1/"));
			sb->AppendI32(Double2Int32(1.0 / this->expTime));
		}
		else
		{
			Double invExp = 1.0 / this->expTime;
			if (invExp == Double2Int32(invExp))
			{
				sb->AppendC(UTF8STRC("1/"));
				sb->AppendI32(Double2Int32(invExp));
			}
			else
			{
				sptr = Text::StrDoubleFmt(sbuff, this->expTime, "0.0");
				sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
			}
		}
		sb->AppendC(UTF8STRC("sec"));
	}
	
	if (this->isoRating != 0)
	{
		sb->AppendC(UTF8STRC(" ISO"));
		sb->AppendU32(this->isoRating);
	}

	if (this->lens)
	{
		sb->AppendC(UTF8STRC(" Lens "));
		sb->Append(this->lens);
	}

	if (this->focalLength != 0)
	{
		sb->AppendC(UTF8STRC("@"));
		Text::SBAppendF64(sb, this->focalLength);
		sb->AppendC(UTF8STRC("mm"));
	}
}
