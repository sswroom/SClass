#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/ICCProfile.h"
#include "Media/ImageCopyC.h"
#include "Media/ImageList.h"
#include "Media/ImageUtil.h"
#include "Media/StaticImage.h"
#include "Parser/FileParser/HEIFParser.h"

#include <libheif/heif.h>

//#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif
Parser::FileParser::HEIFParser::HEIFParser()
{
}

Parser::FileParser::HEIFParser::~HEIFParser()
{
}

Int32 Parser::FileParser::HEIFParser::GetName()
{
	return *(Int32*)"HEIF";
}

void Parser::FileParser::HEIFParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::ImageList)
	{
		selector->AddFilter(CSTR("*.heic"), CSTR("HEIF"));
	}
}

IO::ParserType Parser::FileParser::HEIFParser::GetParserType()
{
	return IO::ParserType::ImageList;
}

Media::StaticImage *HEIFParser_DecodeImage(heif_image_handle *imgHdlr)
{
	Media::StaticImage *simg = 0;
	heif_image* img;
	int bpp = heif_image_handle_get_chroma_bits_per_pixel(imgHdlr);
	int hasAlpha = heif_image_handle_has_alpha_channel(imgHdlr);
	int width = heif_image_handle_get_width(imgHdlr);
	int height = heif_image_handle_get_height(imgHdlr);
	int stride;
	const uint8_t *data;
	Media::ColorProfile color(Media::ColorProfile::CPT_PUNKNOWN);
	if (bpp <= 8)
	{
		if (hasAlpha)
		{
			NEW_CLASS(simg, Media::StaticImage((UOSInt)width, (UOSInt)height, 0, 32, Media::PF_R8G8B8A8, (UOSInt)width * (UOSInt)height * 4, &color, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_ALPHA, Media::YCOFST_C_CENTER_LEFT));
			heif_decode_image(imgHdlr, &img, heif_colorspace_RGB, heif_chroma_interleaved_RGBA, 0);
			data = heif_image_get_plane_readonly(img, heif_channel_interleaved, &stride);
			ImageCopy_ImgCopyR(data, simg->data, (UOSInt)width * 4, (UOSInt)height, (UOSInt)stride, simg->GetDataBpl(), false);
		}
		else
		{
			NEW_CLASS(simg, Media::StaticImage((UOSInt)width, (UOSInt)height, 0, 24, Media::PF_R8G8B8, (UOSInt)width * (UOSInt)height * 3, &color, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
			heif_decode_image(imgHdlr, &img, heif_colorspace_RGB, heif_chroma_interleaved_RGB, 0);
			data = heif_image_get_plane_readonly(img, heif_channel_interleaved, &stride);
			ImageCopy_ImgCopyR(data, simg->data, (UOSInt)width * 3, (UOSInt)height, (UOSInt)stride, simg->GetDataBpl(), false);
		}
	}
	else if (bpp <= 16)
	{
		if (hasAlpha)
		{
			NEW_CLASS(simg, Media::StaticImage((UOSInt)width, (UOSInt)height, 0, 64, Media::PF_LE_B16G16R16A16, (UOSInt)width * (UOSInt)height * 8, &color, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_ALPHA, Media::YCOFST_C_CENTER_LEFT));
			heif_decode_image(imgHdlr, &img, heif_colorspace_RGB, heif_chroma_interleaved_RRGGBBAA_LE, 0);
			data = heif_image_get_plane_readonly(img, heif_channel_interleaved, &stride);
			ImageCopy_ImgCopyR(data, simg->data, (UOSInt)width * 8, (UOSInt)height, (UOSInt)stride, simg->GetDataBpl(), false);
			ImageUtil_SwapRGB(simg->data, (UOSInt)width * (UOSInt)height, 64);
		}
		else
		{
			NEW_CLASS(simg, Media::StaticImage((UOSInt)width, (UOSInt)height, 0, 48, Media::PF_R8G8B8, (UOSInt)width * (UOSInt)height * 6, &color, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
			heif_decode_image(imgHdlr, &img, heif_colorspace_RGB, heif_chroma_interleaved_RGB, 0);
			data = heif_image_get_plane_readonly(img, heif_channel_interleaved, &stride);
			ImageCopy_ImgCopyR(data, simg->data, (UOSInt)width * 6, (UOSInt)height, (UOSInt)stride, simg->GetDataBpl(), false);
			ImageUtil_SwapRGB(simg->data, (UOSInt)width * (UOSInt)height, 48);
		}
	}
#if defined(VERBOSE)
	else
	{
		printf("bpp = %d\r\n", bpp);
	}
#endif
	return simg;
}

IO::ParsedObject *Parser::FileParser::HEIFParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	UInt8 buff[24];
	if (fd->GetRealData(0, 24, buff) != 24)
		return 0;
	if (ReadNInt32(&buff[4]) != *(Int32*)"ftyp" || ReadNInt32(&buff[8]) != *(Int32*)"mif1")
		return 0;
	
	UInt64 fileLen = fd->GetDataSize();
	if (fileLen < 100 || fileLen > 104857600)
		return 0;
	UInt8 *fileBuff = MemAlloc(UInt8, (UOSInt)fileLen);
	if (fd->GetRealData(0, (UOSInt)fileLen, fileBuff) != fileLen)
	{
		MemFree(fileBuff);
		return 0;
	}
	Media::ImageList *imgList = 0;
	heif_context *ctx = heif_context_alloc();
	heif_error err = heif_context_read_from_memory_without_copy(ctx, fileBuff, (size_t)fileLen, 0);
	if (err.code == heif_error_Ok)
	{
		heif_image_handle *imgHdlr;
		Media::StaticImage *simg;
		NEW_CLASS(imgList, Media::ImageList(fd->GetFullFileName()));
		int nImages = heif_context_get_number_of_top_level_images(ctx);
		if (nImages == 1)
		{
			heif_context_get_primary_image_handle(ctx, &imgHdlr);
			simg = HEIFParser_DecodeImage(imgHdlr);
			if (simg)
			{
				imgList->AddImage(simg, 0);
			}
		}
		else
		{
			heif_item_id *idArr = MemAlloc(heif_item_id, (UOSInt)nImages);
			heif_context_get_list_of_top_level_image_IDs(ctx, idArr, nImages);
			int i = 0;
			while (i < nImages)
			{
				heif_context_get_image_handle(ctx, idArr[i], &imgHdlr);
				simg = HEIFParser_DecodeImage(imgHdlr);
				if (simg)
				{
					imgList->AddImage(simg, 0);
				}
				i++;
			}
			MemFree(idArr);
		}
#if defined(VERBOSE)
		printf("Number of images: %d\r\n", heif_context_get_number_of_top_level_images(ctx));
#endif
	}
	heif_context_free(ctx);
	MemFree(fileBuff);
	return imgList;
}
