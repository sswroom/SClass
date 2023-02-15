#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/ICCProfile.h"
#include "Media/ImageCopyC.h"
#include "Media/ImageList.h"
#include "Media/ImageUtil.h"
#include "Media/StaticImage.h"
#include "Parser/FileParser/HEIFParser.h"

#include <libheif/heif.h>

#define VERBOSE
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
		selector->AddFilter(CSTR("*.heic"), CSTR("HEIC"));
		selector->AddFilter(CSTR("*.heif"), CSTR("HEIF"));
		selector->AddFilter(CSTR("*.avif"), CSTR("AVIF"));
	}
}

IO::ParserType Parser::FileParser::HEIFParser::GetParserType()
{
	return IO::ParserType::ImageList;
}

Media::StaticImage *HEIFParser_DecodeImage(heif_image_handle *imgHdlr)
{
	Media::StaticImage *simg = 0;
	heif_image* img = 0;
#if LIBHEIF_HAVE_VERSION(1, 4, 0)
	int bpp = heif_image_handle_get_chroma_bits_per_pixel(imgHdlr);
#else
	int bpp = 8;
#endif
	int hasAlpha = heif_image_handle_has_alpha_channel(imgHdlr);
	int width = heif_image_handle_get_ispe_width(imgHdlr);
	int height = heif_image_handle_get_ispe_height(imgHdlr);
	int stride;
	const uint8_t *data;
	Media::ColorProfile color(Media::ColorProfile::CPT_PUNKNOWN);
	heif_decoding_options *options = heif_decoding_options_alloc();
	options->ignore_transformations = 1;
	if (bpp <= 8)
	{
		if (hasAlpha)
		{
			NEW_CLASS(simg, Media::StaticImage((UOSInt)width, (UOSInt)height, 0, 32, Media::PF_R8G8B8A8, (UOSInt)width * (UOSInt)height * 4, &color, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_ALPHA, Media::YCOFST_C_CENTER_LEFT));
			heif_decode_image(imgHdlr, &img, heif_colorspace_RGB, heif_chroma_interleaved_RGBA, options);
			data = heif_image_get_plane_readonly(img, heif_channel_interleaved, &stride);
			ImageCopy_ImgCopyR(data, simg->data, (UOSInt)width * 4, (UOSInt)height, (UOSInt)stride, simg->GetDataBpl(), false);
		}
		else
		{
			NEW_CLASS(simg, Media::StaticImage((UOSInt)width, (UOSInt)height, 0, 24, Media::PF_R8G8B8, (UOSInt)width * (UOSInt)height * 3, &color, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
			heif_decode_image(imgHdlr, &img, heif_colorspace_RGB, heif_chroma_interleaved_RGB, options);
			data = heif_image_get_plane_readonly(img, heif_channel_interleaved, &stride);
			ImageCopy_ImgCopyR(data, simg->data, (UOSInt)width * 3, (UOSInt)height, (UOSInt)stride, simg->GetDataBpl(), false);
		}
	}
#if LIBHEIF_HAVE_VERSION(1, 4, 0)
	else if (bpp <= 16)
	{
		if (hasAlpha)
		{
			NEW_CLASS(simg, Media::StaticImage((UOSInt)width, (UOSInt)height, 0, 64, Media::PF_LE_B16G16R16A16, (UOSInt)width * (UOSInt)height * 8, &color, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_ALPHA, Media::YCOFST_C_CENTER_LEFT));
			heif_decode_image(imgHdlr, &img, heif_colorspace_RGB, heif_chroma_interleaved_RRGGBBAA_LE, options);
			data = heif_image_get_plane_readonly(img, heif_channel_interleaved, &stride);
			ImageCopy_ImgCopyR(data, simg->data, (UOSInt)width * 8, (UOSInt)height, (UOSInt)stride, simg->GetDataBpl(), false);
			ImageUtil_SwapRGB(simg->data, (UOSInt)width * (UOSInt)height, 64);
		}
		else
		{
			NEW_CLASS(simg, Media::StaticImage((UOSInt)width, (UOSInt)height, 0, 48, Media::PF_R8G8B8, (UOSInt)width * (UOSInt)height * 6, &color, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
			heif_decode_image(imgHdlr, &img, heif_colorspace_RGB, heif_chroma_interleaved_RRGGBB_LE, options);
			data = heif_image_get_plane_readonly(img, heif_channel_interleaved, &stride);
			ImageCopy_ImgCopyR(data, simg->data, (UOSInt)width * 6, (UOSInt)height, (UOSInt)stride, simg->GetDataBpl(), false);
			ImageUtil_SwapRGB(simg->data, (UOSInt)width * (UOSInt)height, 48);
		}
	}
#endif
	heif_decoding_options_free(options);

	if (simg)
	{
		heif_color_profile_type cpType = heif_image_handle_get_color_profile_type(imgHdlr);
		switch (cpType)
		{
		case heif_color_profile_type_nclx:
		{
			heif_color_profile_nclx *cp;
			struct heif_error error = heif_image_handle_get_nclx_color_profile(imgHdlr, &cp);
			if (error.code == heif_error_Ok)
			{
				switch (cp->color_primaries)
				{
				case heif_color_primaries_ITU_R_BT_709_5:
					simg->info.color->primaries.SetColorType(Media::ColorProfile::CT_BT709);
					break;
				case heif_color_primaries_ITU_R_BT_470_6_System_M:
					simg->info.color->primaries.SetColorType(Media::ColorProfile::CT_BT470M);
					break;
				case heif_color_primaries_ITU_R_BT_470_6_System_B_G:
					simg->info.color->primaries.SetColorType(Media::ColorProfile::CT_BT470BG);
					break;
				case heif_color_primaries_ITU_R_BT_601_6:
					simg->info.color->primaries.SetColorType(Media::ColorProfile::CT_BT470BG);
					break;
				case heif_color_primaries_SMPTE_240M:
					simg->info.color->primaries.SetColorType(Media::ColorProfile::CT_SMPTE240M);
					break;
				case heif_color_primaries_generic_film:
					simg->info.color->primaries.SetColorType(Media::ColorProfile::CT_GENERIC_FILM);
					break;
				case heif_color_primaries_ITU_R_BT_2020_2_and_2100_0:
					simg->info.color->primaries.SetColorType(Media::ColorProfile::CT_BT2020);
					break;
				case heif_color_primaries_SMPTE_ST_428_1:
					simg->info.color->primaries.SetColorType(Media::ColorProfile::CT_SMPTE428);
					break;
				case heif_color_primaries_SMPTE_RP_431_2:
					simg->info.color->primaries.SetColorType(Media::ColorProfile::CT_SMPTE_RP431_2);
					break;
				case heif_color_primaries_SMPTE_EG_432_1:
					simg->info.color->primaries.SetColorType(Media::ColorProfile::CT_SMPTE_EG432_1);
					break;
				case heif_color_primaries_EBU_Tech_3213_E:
					simg->info.color->primaries.SetColorType(Media::ColorProfile::CT_EBU3213);
					break;
				case heif_color_primaries_unspecified:
				default:
					simg->info.color->primaries.SetColorType(Media::ColorProfile::CT_PUNKNOWN);
					break;
				}

				switch (cp->transfer_characteristics)
				{
				case heif_transfer_characteristic_ITU_R_BT_709_5:
					simg->info.color->rtransfer.Set(Media::CS::TRANT_BT709, 2.2);
					simg->info.color->gtransfer.Set(Media::CS::TRANT_BT709, 2.2);
					simg->info.color->btransfer.Set(Media::CS::TRANT_BT709, 2.2);
					break;
				case heif_transfer_characteristic_unspecified:
					simg->info.color->rtransfer.Set(Media::CS::TRANT_PUNKNOWN, 2.2);
					simg->info.color->gtransfer.Set(Media::CS::TRANT_PUNKNOWN, 2.2);
					simg->info.color->btransfer.Set(Media::CS::TRANT_PUNKNOWN, 2.2);
					break;
				case heif_transfer_characteristic_ITU_R_BT_470_6_System_M:
					simg->info.color->rtransfer.Set(Media::CS::TRANT_GAMMA, 2.2);
					simg->info.color->gtransfer.Set(Media::CS::TRANT_GAMMA, 2.2);
					simg->info.color->btransfer.Set(Media::CS::TRANT_GAMMA, 2.2);
					break;
				case heif_transfer_characteristic_ITU_R_BT_470_6_System_B_G:
					simg->info.color->rtransfer.Set(Media::CS::TRANT_GAMMA, 2.8);
					simg->info.color->gtransfer.Set(Media::CS::TRANT_GAMMA, 2.8);
					simg->info.color->btransfer.Set(Media::CS::TRANT_GAMMA, 2.8);
					break;
				case heif_transfer_characteristic_ITU_R_BT_601_6:
					simg->info.color->rtransfer.Set(Media::CS::TRANT_BT709, 2.2);
					simg->info.color->gtransfer.Set(Media::CS::TRANT_BT709, 2.2);
					simg->info.color->btransfer.Set(Media::CS::TRANT_BT709, 2.2);
					break;
				case heif_transfer_characteristic_SMPTE_240M:
					simg->info.color->rtransfer.Set(Media::CS::TRANT_SMPTE240, 2.2);
					simg->info.color->gtransfer.Set(Media::CS::TRANT_SMPTE240, 2.2);
					simg->info.color->btransfer.Set(Media::CS::TRANT_SMPTE240, 2.2);
					break;
				case heif_transfer_characteristic_linear:
					simg->info.color->rtransfer.Set(Media::CS::TRANT_LINEAR, 1.0);
					simg->info.color->gtransfer.Set(Media::CS::TRANT_LINEAR, 1.0);
					simg->info.color->btransfer.Set(Media::CS::TRANT_LINEAR, 1.0);
					break;
				case heif_transfer_characteristic_logarithmic_100:
					simg->info.color->rtransfer.Set(Media::CS::TRANT_LOG100, 2.2);
					simg->info.color->gtransfer.Set(Media::CS::TRANT_LOG100, 2.2);
					simg->info.color->btransfer.Set(Media::CS::TRANT_LOG100, 2.2);
					break;
				case heif_transfer_characteristic_logarithmic_100_sqrt10:
					simg->info.color->rtransfer.Set(Media::CS::TRANT_LOGSQRT10, 2.2);
					simg->info.color->gtransfer.Set(Media::CS::TRANT_LOGSQRT10, 2.2);
					simg->info.color->btransfer.Set(Media::CS::TRANT_LOGSQRT10, 2.2);
					break;
				case heif_transfer_characteristic_IEC_61966_2_4:
					simg->info.color->rtransfer.Set(Media::CS::TRANT_BT709, 2.2);
					simg->info.color->gtransfer.Set(Media::CS::TRANT_BT709, 2.2);
					simg->info.color->btransfer.Set(Media::CS::TRANT_BT709, 2.2);
					break;
				case heif_transfer_characteristic_ITU_R_BT_1361:
					simg->info.color->rtransfer.Set(Media::CS::TRANT_BT1361, 2.2);
					simg->info.color->gtransfer.Set(Media::CS::TRANT_BT1361, 2.2);
					simg->info.color->btransfer.Set(Media::CS::TRANT_BT1361, 2.2);
					break;
				case heif_transfer_characteristic_IEC_61966_2_1:
					simg->info.color->rtransfer.Set(Media::CS::TRANT_sRGB, 2.2);
					simg->info.color->gtransfer.Set(Media::CS::TRANT_sRGB, 2.2);
					simg->info.color->btransfer.Set(Media::CS::TRANT_sRGB, 2.2);
					break;
				case heif_transfer_characteristic_ITU_R_BT_2020_2_10bit:
					simg->info.color->rtransfer.Set(Media::CS::TRANT_BT709, 2.2);
					simg->info.color->gtransfer.Set(Media::CS::TRANT_BT709, 2.2);
					simg->info.color->btransfer.Set(Media::CS::TRANT_BT709, 2.2);
					break;
				case heif_transfer_characteristic_ITU_R_BT_2020_2_12bit:
					simg->info.color->rtransfer.Set(Media::CS::TRANT_BT709, 2.2);
					simg->info.color->gtransfer.Set(Media::CS::TRANT_BT709, 2.2);
					simg->info.color->btransfer.Set(Media::CS::TRANT_BT709, 2.2);
					break;
				case heif_transfer_characteristic_ITU_R_BT_2100_0_PQ:
					simg->info.color->rtransfer.Set(Media::CS::TRANT_BT2100, 2.2);
					simg->info.color->gtransfer.Set(Media::CS::TRANT_BT2100, 2.2);
					simg->info.color->btransfer.Set(Media::CS::TRANT_BT2100, 2.2);
					break;
				case heif_transfer_characteristic_SMPTE_ST_428_1:
					simg->info.color->rtransfer.Set(Media::CS::TRANT_SMPTE428, 2.2);
					simg->info.color->gtransfer.Set(Media::CS::TRANT_SMPTE428, 2.2);
					simg->info.color->btransfer.Set(Media::CS::TRANT_SMPTE428, 2.2);
					break;
				case heif_transfer_characteristic_ITU_R_BT_2100_0_HLG:
					simg->info.color->rtransfer.Set(Media::CS::TRANT_HLG, 2.2);
					simg->info.color->gtransfer.Set(Media::CS::TRANT_HLG, 2.2);
					simg->info.color->btransfer.Set(Media::CS::TRANT_HLG, 2.2);
					break;
				}

				switch (cp->matrix_coefficients)
				{
				case heif_matrix_coefficients_RGB_GBR:
					simg->info.yuvType = Media::ColorProfile::YUVT_UNKNOWN;
					break;
				case heif_matrix_coefficients_ITU_R_BT_709_5:
					simg->info.yuvType = Media::ColorProfile::YUVT_BT709;
					break;
				case heif_matrix_coefficients_unspecified:
				default:
					simg->info.yuvType = Media::ColorProfile::YUVT_UNKNOWN;
					break;
				case heif_matrix_coefficients_US_FCC_T47:
					simg->info.yuvType = Media::ColorProfile::YUVT_FCC;
					break;
				case heif_matrix_coefficients_ITU_R_BT_470_6_System_B_G:
					simg->info.yuvType = Media::ColorProfile::YUVT_BT470BG;
					break;
				case heif_matrix_coefficients_ITU_R_BT_601_6:
					simg->info.yuvType = Media::ColorProfile::YUVT_BT601;
					break;
				case heif_matrix_coefficients_SMPTE_240M:
					simg->info.yuvType = Media::ColorProfile::YUVT_SMPTE240M;
					break;
				case heif_matrix_coefficients_ITU_R_BT_2020_2_constant_luminance:
					simg->info.yuvType = Media::ColorProfile::YUVT_BT2020;
					break;
				case heif_matrix_coefficients_ITU_R_BT_2020_2_non_constant_luminance:
					simg->info.yuvType = Media::ColorProfile::YUVT_BT2020;
					break;
				case heif_matrix_coefficients_YCgCo:
				case heif_matrix_coefficients_SMPTE_ST_2085:
				case heif_matrix_coefficients_chromaticity_derived_non_constant_luminance:
				case heif_matrix_coefficients_chromaticity_derived_constant_luminance:
				case heif_matrix_coefficients_ICtCp:
					simg->info.yuvType = Media::ColorProfile::YUVT_UNKNOWN;
					break;
				}
				if (cp->full_range_flag != 0)
				{
					simg->info.yuvType = (Media::ColorProfile::YUVType)(simg->info.yuvType | Media::ColorProfile::YUVT_FLAG_YUV_0_255);
				}
			}
			break;
		}
		case heif_color_profile_type_prof:
		case heif_color_profile_type_rICC:
		{
			UOSInt iccSize = (UInt32)heif_image_handle_get_raw_color_profile_size(imgHdlr);
			UInt8 *buff = MemAlloc(UInt8, iccSize);
			struct heif_error error = heif_image_handle_get_raw_color_profile(imgHdlr, buff);
			if (error.code == heif_error_Ok)
			{
				Media::ICCProfile *icc = Media::ICCProfile::Parse(buff, iccSize);
				if (icc)
				{
					icc->SetToColorProfile(simg->info.color);
					simg->info.color->SetRAWICC(buff);
					DEL_CLASS(icc);
				}
			}
			MemFree(buff);
			break;
		}
		case heif_color_profile_type_not_present:
		default:
			break;
		}

	}
	heif_item_id metaIds[32];
	int n = heif_image_handle_get_list_of_metadata_block_IDs(imgHdlr, 0, metaIds, 32);
	if (simg && n > 0)
	{
		int i = 0;
		while (i < n)
		{
			size_t exifSize = heif_image_handle_get_metadata_size(imgHdlr, metaIds[i]);
			const char *type = heif_image_handle_get_metadata_type(imgHdlr, metaIds[i]);
#if defined(VERBOSE)
			printf("Metadata found: type = %s, size = %d\r\n", type, (Int32)exifSize);
#endif
			if (Text::StrEquals(type, "Exif"))
			{
				UInt8* exifData = MemAlloc(UInt8, (UOSInt)exifSize);
				struct heif_error error = heif_image_handle_get_metadata(imgHdlr, metaIds[i], exifData);
				if (error.code == heif_error_Ok)
				{
					Media::EXIFData *exif = Media::EXIFData::ParseExif(exifData, (UOSInt)exifSize);
					if (exif)
					{
						exif = simg->SetEXIFData(exif);
						SDEL_CLASS(exif);
					}
				}
				MemFree(exifData);
			}
			i++;
		}
	}
#if defined(VERBOSE)
	else
	{
		printf("bpp = %d\r\n", bpp);
	}
#endif
	if (simg)
	{
		heif_image_release(img);
	}
	return simg;
}

IO::ParsedObject *Parser::FileParser::HEIFParser::ParseFileHdr(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	if (ReadNInt32(&hdr[4]) != *(Int32*)"ftyp" || (ReadNInt32(&hdr[8]) != *(Int32*)"mif1" && ReadNInt32(&hdr[8]) != *(Int32*)"heic"))
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
#if LIBHEIF_HAVE_VERSION(1, 3, 0)
	heif_error err = heif_context_read_from_memory_without_copy(ctx, fileBuff, (size_t)fileLen, 0);
#else
	heif_error err = heif_context_read_from_memory(ctx, fileBuff, (size_t)fileLen, 0);
#endif
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
			heif_image_handle_release(imgHdlr);
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
				heif_image_handle_release(imgHdlr);
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
