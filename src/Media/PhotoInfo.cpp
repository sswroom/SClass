#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/JPEGFile.h"
#include "Media/PhotoInfo.h"
#include "Parser/FileParser/HEIFParser.h"
#include "Text/Encoding.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

void Media::PhotoInfo::ParseXMF(NN<Text::XMLDocument> xmf)
{
	NN<Text::XMLNode> node;
	if (xmf->SearchFirstNode(CSTR("//@exif:ExposureTime")).SetTo(node))
	{
		this->expTime = ParseFraction(Text::String::OrEmpty(node->value));
	}
	if (xmf->SearchFirstNode(CSTR("//@exif:FNumber")).SetTo(node))
	{
		this->fNumber = ParseFraction(Text::String::OrEmpty(node->value));
	}
	if (xmf->SearchFirstNode(CSTR("//@aux:Lens")).SetTo(node))
	{
		OPTSTR_DEL(this->lens);
		this->lens = Text::String::OrEmpty(node->value)->Clone();
	}
	if (xmf->SearchFirstNode(CSTR("//@exif:FocalLength")).SetTo(node))
	{
		this->focalLength = ParseFraction(Text::String::OrEmpty(node->value));
	}
	if (xmf->SearchFirstNode(CSTR("//exif:ISOSpeedRatings")).SetTo(node))
	{
		Text::StringBuilderUTF8 sb;
		node->GetInnerText(sb);
		sb.Trim();
		this->isoRating = Text::StrToUInt32(sb.ToString());
	}
}

Double Media::PhotoInfo::ParseFraction(NN<Text::String> s)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sarr[3];
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

Media::PhotoInfo::PhotoInfo(NN<IO::StreamData> fd)
{
	Optional<Media::EXIFData> exif;
	Optional<Text::XMLDocument> xmf;
	Optional<Media::ICCProfile> icc;
	NN<Media::EXIFData> nnexif;
	NN<Text::XMLDocument> nnxmf;
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

	if (Media::JPEGFile::ParseJPEGHeaders(fd, exif, xmf, icc, width, height) || Parser::FileParser::HEIFParser::ParseHeaders(fd, exif, xmf, icc, width, height))
	{	
		Text::CStringNN ctxt;
		Data::DateTime dt;
		this->succ = true;

		if (exif.SetTo(nnexif))
		{
			if (nnexif->GetPhotoMake().SetTo(ctxt))
			{
				this->make = Text::String::New(ctxt).Ptr();
			}
			if (nnexif->GetPhotoModel().SetTo(ctxt))
			{
				this->model = Text::String::New(ctxt).Ptr();
			}
			this->fNumber = nnexif->GetPhotoFNumber();
			this->expTime = nnexif->GetPhotoExpTime();
			this->isoRating = nnexif->GetPhotoISO();
			this->focalLength = nnexif->GetPhotoFocalLength();

			if (nnexif->GetPhotoDate(dt))
			{
				NEW_CLASS(this->photoDate, Data::DateTime(dt));
			}

			NN<Media::EXIFData::EXIFItem> item;
			if (nnexif->GetExifItem(34665).SetTo(item))
			{
				if (item->type == Media::EXIFData::ET_SUBEXIF)
				{
					NN<Media::EXIFData> exif2 = item->dataBuff.GetNN<Media::EXIFData>();
					if (exif2->GetExifItem(37500).SetTo(item))
					{
						UnsafeArray<UInt8> valBuff;
						if (item->cnt <= 4)
						{
							valBuff = UnsafeArray<UInt8>((UInt8*)&item->value);
						}
						else
						{
							valBuff = item->dataBuff.GetArray<UInt8>();
						}
						NN<Media::EXIFData> innerExif;
						if (nnexif->ParseMakerNote(valBuff, item->cnt).SetTo(innerExif))
						{
							if (innerExif->GetPhotoExpTime())
								this->expTime = innerExif->GetPhotoExpTime();
							if (innerExif->GetPhotoFNumber())
								this->fNumber = innerExif->GetPhotoFNumber();
							if (innerExif->GetPhotoLens().SetTo(ctxt))
							{
								this->lens = Text::String::New(ctxt).Ptr();
							}
							if (innerExif->GetPhotoFocalLength())
								this->focalLength = innerExif->GetPhotoFocalLength();
							if (innerExif->GetPhotoISO())
								this->isoRating = innerExif->GetPhotoISO();
							innerExif.Delete();
						}
					}
				}
			}
			else if (nnexif->GetExifItem(700).SetTo(item))
			{
				Text::XMLDocument doc;
				Text::EncodingFactory encFact;
				if (doc.ParseBuff(encFact, item->dataBuff.GetArray<const UInt8>(), item->cnt))
				{
					ParseXMF(doc);
				}
			}
			nnexif.Delete();
		}
		if (xmf.SetTo(nnxmf))
		{
			ParseXMF(nnxmf);
			nnxmf.Delete();
		}
		icc.Delete();
		this->width = width;
		this->height = height;
	}
}

Media::PhotoInfo::~PhotoInfo()
{
	OPTSTR_DEL(this->make);
	OPTSTR_DEL(this->model);
	OPTSTR_DEL(this->lens);
	if (this->photoDate)
		DEL_CLASS(this->photoDate);
}

Bool Media::PhotoInfo::HasInfo() const
{
	return this->succ;
}

Bool Media::PhotoInfo::GetPhotoDate(NN<Data::DateTime> dt) const
{
	NN<Data::DateTime> photoDate;
	if (photoDate.Set(this->photoDate))
	{
		dt->SetValue(photoDate);
		return true;
	}
	return false;
}

UInt32 Media::PhotoInfo::GetWidth() const
{
	return this->width;
}

UInt32 Media::PhotoInfo::GetHeight() const
{
	return this->height;
}

void Media::PhotoInfo::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	NN<Text::String> make;
	NN<Text::String> model;
	if (!this->make.SetTo(make))
	{
		if (!this->model.SetTo(model))
		{
			sb->AppendC(UTF8STRC("Unknown camera"));
		}
		else
		{
			sb->Append(model);
		}
	}
	else
	{
		if (!this->model.SetTo(model))
		{
			sb->Append(make);
		}
		else if (model->StartsWith(make))
		{
			sb->Append(model);
		}
		else
		{
			sb->Append(make);
			sb->AppendC(UTF8STRC(" "));
			sb->Append(model);
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

	if (this->lens.SetTo(model))
	{
		sb->AppendC(UTF8STRC(" Lens "));
		sb->Append(model);
	}

	if (this->focalLength != 0)
	{
		sb->AppendC(UTF8STRC("@"));
		Text::SBAppendF64(sb, this->focalLength);
		sb->AppendC(UTF8STRC("mm"));
	}
}
