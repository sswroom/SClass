#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Media/ICCProfile.h"
#include "Media/ImageList.h"
#include "Media/StaticImage.h"
#include "Parser/FileParser/AVIFParser.h"
#include "Text/EncodingFactory.h"

#include <avif/avif.h>

Bool AVIFParser_IsBrand(Data::ByteArrayR buff, UIntOS ofst)
{
	if (ofst + 4 > buff.GetSize())
		return false;
	Int32 brand = ReadNInt32(&buff[ofst]);
	return brand == *(Int32*)"avif" || brand == *(Int32*)"avis";
}

Bool AVIFParser_IsHeader(Data::ByteArrayR hdr)
{
	if (hdr.GetSize() < 16 || ReadNInt32(&hdr[4]) != *(Int32*)"ftyp")
		return false;
	if (AVIFParser_IsBrand(hdr, 8))
		return true;

	UInt32 boxSize = ReadMUInt32(&hdr[0]);
	if (boxSize > hdr.GetSize())
		boxSize = (UInt32)hdr.GetSize();
	if (boxSize < 16)
		return false;

	UIntOS i = 16;
	while (i + 4 <= boxSize)
	{
		if (AVIFParser_IsBrand(hdr, i))
			return true;
		i += 4;
	}
	return false;
}

Optional<Media::StaticImage> AVIFParser_DecodeImage(avifImage *img)
{
	if (img == 0 || img->width <= 0 || img->height <= 0)
		return nullptr;

	Bool hasAlpha = img->alphaPlane != 0;
	Bool is16Bit = img->depth > 8;
	Media::ColorProfile color(Media::ColorProfile::CPT_PUNKNOWN);
	NN<Media::StaticImage> simg;
	if (is16Bit)
	{
		if (hasAlpha)
		{
			NEW_CLASSNN(simg, Media::StaticImage(Math::Size2D<UIntOS>((UIntOS)img->width, (UIntOS)img->height), 0, 64, Media::PF_LE_R16G16B16A16, (UIntOS)img->width * (UIntOS)img->height * 8, color, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_ALPHA, Media::YCOFST_C_CENTER_LEFT));
		}
		else
		{
			NEW_CLASSNN(simg, Media::StaticImage(Math::Size2D<UIntOS>((UIntOS)img->width, (UIntOS)img->height), 0, 48, Media::PF_LE_R16G16B16, (UIntOS)img->width * (UIntOS)img->height * 6, color, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_ALPHA_ALL_FF, Media::YCOFST_C_CENTER_LEFT));
		}
	}
	else
	{
		if (hasAlpha)
		{
			NEW_CLASSNN(simg, Media::StaticImage(Math::Size2D<UIntOS>((UIntOS)img->width, (UIntOS)img->height), 0, 32, Media::PF_R8G8B8A8, (UIntOS)img->width * (UIntOS)img->height * 4, color, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_ALPHA, Media::YCOFST_C_CENTER_LEFT));
		}
		else
		{
			NEW_CLASSNN(simg, Media::StaticImage(Math::Size2D<UIntOS>((UIntOS)img->width, (UIntOS)img->height), 0, 24, Media::PF_R8G8B8, (UIntOS)img->width * (UIntOS)img->height * 3, color, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_ALPHA_ALL_FF, Media::YCOFST_C_CENTER_LEFT));
		}
	}

	avifRGBImage rgb;
	avifRGBImageSetDefaults(&rgb, img);
	rgb.depth = is16Bit ? 16 : 8;
	rgb.format = hasAlpha ? AVIF_RGB_FORMAT_RGBA : AVIF_RGB_FORMAT_RGB;
	rgb.pixels = simg->data.Ptr();
	rgb.rowBytes = (uint32_t)simg->GetDataBpl();
	avifResult result = avifImageYUVToRGB(img, &rgb);
	if (result != AVIF_RESULT_OK)
	{
		simg.Delete();
		return nullptr;
	}

	if (img->icc.data != 0 && img->icc.size > 0)
	{
		NN<Media::ICCProfile> profile;
		if (Media::ICCProfile::Parse(Data::ByteArrayR(img->icc.data, img->icc.size)).SetTo(profile))
		{
			profile->SetToColorProfile(simg->info.color);
			profile.Delete();
		}
	}
	if (img->exif.data != 0 && img->exif.size > 0)
	{
		NN<Media::EXIFData> exif;
		if (Media::EXIFData::ParseExifJPG(img->exif.data, img->exif.size).SetTo(exif))
		{
			simg->SetEXIFData(exif).Delete();
		}
	}
	return simg;
}

Parser::FileParser::AVIFParser::AVIFParser()
{
}

Parser::FileParser::AVIFParser::~AVIFParser()
{
}

Int32 Parser::FileParser::AVIFParser::GetName()
{
	return *(Int32*)"AVIF";
}

void Parser::FileParser::AVIFParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::ImageList)
	{
		selector->AddFilter(CSTR("*.avif"), CSTR("AVIF"));
	}
}

IO::ParserType Parser::FileParser::AVIFParser::GetParserType()
{
	return IO::ParserType::ImageList;
}

Optional<IO::ParsedObject> Parser::FileParser::AVIFParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	if (!AVIFParser_IsHeader(hdr))
		return nullptr;

	UInt64 fileLen = fd->GetDataSize();
	if (fileLen < 16 || fileLen > 104857600)
		return nullptr;
	Data::ByteBuffer fileBuff((UIntOS)fileLen);
	if (fd->GetRealData(0, (UIntOS)fileLen, fileBuff) != fileLen)
	{
		return nullptr;
	}

	Optional<Media::ImageList> imgList = nullptr;
	NN<Media::ImageList> nnimgList;
	avifDecoder *decoder = avifDecoderCreate();
	if (decoder == 0)
		return nullptr;

	decoder->strictFlags = AVIF_STRICT_DISABLED;
	avifResult result = avifDecoderSetIOMemory(decoder, fileBuff.Arr().Ptr(), fileBuff.GetSize());
	if (result == AVIF_RESULT_OK)
		result = avifDecoderParse(decoder);
	if (result == AVIF_RESULT_OK)
	{
		NEW_CLASSNN(nnimgList, Media::ImageList(fd->GetFullFileName()));
		imgList = nnimgList;
		NN<Media::StaticImage> simg;
		while ((result = avifDecoderNextImage(decoder)) == AVIF_RESULT_OK)
		{
			if (AVIFParser_DecodeImage(decoder->image).SetTo(simg))
			{
				nnimgList->AddImage(simg, 0);
			}
		}
		if (nnimgList->GetCount() == 0)
		{
			nnimgList.Delete();
			imgList = nullptr;
		}
	}

	avifDecoderDestroy(decoder);
	return imgList;
}

Bool Parser::FileParser::AVIFParser::ParseHeaders(NN<IO::StreamData> fd, OutParam<Optional<Media::EXIFData>> exif, OutParam<Optional<Text::XMLDocument>> xmf, OutParam<Optional<Media::ICCProfile>> icc, OutParam<UInt32> width, OutParam<UInt32> height)
{
	UInt8 hdr[64];
	if (fd->GetRealData(0, 64, BYTEARR(hdr)) < 16)
		return false;
	if (!AVIFParser_IsHeader(Data::ByteArrayR(hdr, 64)))
		return false;

	UInt64 fileLen = fd->GetDataSize();
	if (fileLen < 16 || fileLen > 104857600)
		return false;
	Data::ByteBuffer fileBuff((UIntOS)fileLen);
	if (fd->GetRealData(0, (UIntOS)fileLen, fileBuff) != fileLen)
		return false;

	exif.Set(nullptr);
	xmf.Set(nullptr);
	icc.Set(nullptr);
	width.Set(0);
	height.Set(0);

	Bool succ = false;
	avifDecoder *decoder = avifDecoderCreate();
	if (decoder == 0)
		return false;

	decoder->strictFlags = AVIF_STRICT_DISABLED;
	avifResult result = avifDecoderSetIOMemory(decoder, fileBuff.Arr().Ptr(), fileBuff.GetSize());
	if (result == AVIF_RESULT_OK)
		result = avifDecoderParse(decoder);
	if (result == AVIF_RESULT_OK)
		result = avifDecoderNextImage(decoder);
	if (result == AVIF_RESULT_OK)
	{
		avifImage *img = decoder->image;
		width.Set((UInt32)img->width);
		height.Set((UInt32)img->height);

		if (img->icc.data != 0 && img->icc.size > 0)
		{
			icc.Set(Media::ICCProfile::Parse(Data::ByteArrayR(img->icc.data, img->icc.size)));
		}
		if (img->exif.data != 0 && img->exif.size > 0)
		{
			exif.Set(Media::EXIFData::ParseExifJPG(img->exif.data, img->exif.size));
		}
		if (img->xmp.data != 0 && img->xmp.size > 0)
		{
			NN<Text::XMLDocument> doc;
			Text::EncodingFactory encFact;
			NEW_CLASSNN(doc, Text::XMLDocument());
			if (doc->ParseBuff(encFact, img->xmp.data, img->xmp.size))
			{
				xmf.Set(doc);
			}
			else
			{
				doc.Delete();
			}
		}
		succ = true;
	}

	avifDecoderDestroy(decoder);
	return succ;
}