#include "Stdafx.h"
#include "Exporter/HEIFExporter.h"
#include "Media/ImageList.h"
#include "Media/ImageUtil.h"
#include "Media/StaticImage.h"
#include "Text/MyString.h"

#include <libheif/heif.h>

Exporter::HEIFExporter::HEIFExporter()
{
}

Exporter::HEIFExporter::~HEIFExporter()
{
}

Int32 Exporter::HEIFExporter::GetName()
{
	return *(Int32*)"HIEF";
}

IO::FileExporter::SupportType Exporter::HEIFExporter::IsObjectSupported(IO::ParsedObject *pobj)
{
	if (pobj->GetParserType() != IO::ParserType::ImageList)
		return IO::FileExporter::SupportType::NotSupported;
	return IO::FileExporter::SupportType::NormalStream;
}

Bool Exporter::HEIFExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("HEIF container"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.heic"));
		return true;
	}
	return false;
}

heif_error HEIFExporter_Write(struct heif_context* ctx, const void* data, size_t size, void* userdata)
{
	IO::SeekableStream *stm = (IO::SeekableStream*)userdata;
	heif_error heif_error;
	if (stm->Write((const UInt8*)data, (UOSInt)size) == (UOSInt)size)
	{
		heif_error.code = heif_error_Ok;
		heif_error.subcode = heif_suberror_Unspecified;
		heif_error.message = "";
		return heif_error;
	}
	else
	{
		heif_error.code = heif_error_Encoding_error;
		heif_error.subcode = heif_suberror_Cannot_write_output_data;
		heif_error.message = "Error in writing the output";
		return heif_error;
	}
}

heif_image *HEIFExporter_CreateImage(Media::Image *img)
{
	heif_chroma chroma;
	switch (img->info.pf)
	{
	case Media::PF_PAL_1:
	case Media::PF_PAL_2:
	case Media::PF_PAL_4:
	case Media::PF_PAL_8:
	case Media::PF_PAL_W1:
	case Media::PF_PAL_W2:
	case Media::PF_PAL_W4:
	case Media::PF_PAL_W8:
	case Media::PF_B8G8R8A8:
	case Media::PF_W8A8:
	case Media::PF_R8G8B8A8:
	case Media::PF_PAL_1_A1:
	case Media::PF_PAL_2_A1:
	case Media::PF_PAL_4_A1:
	case Media::PF_PAL_8_A1:
	case Media::PF_B8G8R8A1:
	case Media::PF_LE_R5G5B5:
	case Media::PF_LE_R5G6B5:
		chroma = heif_chroma_interleaved_RGBA;
		break;

	case Media::PF_B8G8R8:
	case Media::PF_R8G8B8:
		chroma = heif_chroma_interleaved_RGB;
		break;

	case Media::PF_LE_B16G16R16:
		chroma = heif_chroma_interleaved_RRGGBB_LE;
		break;

	case Media::PF_LE_W16:
	case Media::PF_LE_FB32G32R32:
	case Media::PF_LE_FW32:
	case Media::PF_LE_B16G16R16A16:
	case Media::PF_LE_A2B10G10R10:
	case Media::PF_LE_W16A16:
	case Media::PF_LE_FB32G32R32A32:
	case Media::PF_LE_FW32A32:
		chroma = heif_chroma_interleaved_RRGGBBAA_LE;
		break;
	case Media::PF_UNKNOWN:
	default:
		return 0;
	}
	heif_image *image = 0;
	heif_image_create((int)img->info.dispWidth, (int)img->info.dispHeight, heif_colorspace_RGB, chroma, &image);
	if (image == 0)
		return 0;
	int stride;
	uint8_t *data;
	Media::StaticImage *simg;
	switch (img->info.pf)
	{
	case Media::PF_PAL_1:
	case Media::PF_PAL_2:
	case Media::PF_PAL_4:
	case Media::PF_PAL_8:
	case Media::PF_PAL_W1:
	case Media::PF_PAL_W2:
	case Media::PF_PAL_W4:
	case Media::PF_PAL_W8:
	case Media::PF_W8A8:
	case Media::PF_PAL_1_A1:
	case Media::PF_PAL_2_A1:
	case Media::PF_PAL_4_A1:
	case Media::PF_PAL_8_A1:
	case Media::PF_B8G8R8A1:
	case Media::PF_LE_R5G5B5:
	case Media::PF_LE_R5G6B5:
		heif_image_add_plane(image, heif_channel_interleaved, (int)img->info.dispWidth, (int)img->info.dispHeight, 8);
		data = heif_image_get_plane(image, heif_channel_interleaved, &stride);

		simg = img->CreateStaticImage();
		simg->To32bpp();
		simg->GetImageData(data, 0, 0, simg->info.dispWidth, simg->info.dispHeight, (UOSInt)stride, false, img->info.rotateType);
		ImageUtil_SwapRGB(data, img->info.dispWidth * img->info.dispHeight, 32);
		DEL_CLASS(simg);
		break;
	case Media::PF_B8G8R8A8:
		heif_image_add_plane(image, heif_channel_interleaved, (int)img->info.dispWidth, (int)img->info.dispHeight, 8);
		data = heif_image_get_plane(image, heif_channel_interleaved, &stride);

		img->GetImageData(data, 0, 0, img->info.dispWidth, img->info.dispHeight, (UOSInt)stride, false, img->info.rotateType);
		ImageUtil_SwapRGB(data, img->info.dispWidth * img->info.dispHeight, 32);
		break;
	case Media::PF_R8G8B8A8:
		heif_image_add_plane(image, heif_channel_interleaved, (int)img->info.dispWidth, (int)img->info.dispHeight, 8);
		data = heif_image_get_plane(image, heif_channel_interleaved, &stride);

		img->GetImageData(data, 0, 0, img->info.dispWidth, img->info.dispHeight, (UOSInt)stride, false, img->info.rotateType);
		break;

	case Media::PF_B8G8R8:
		heif_image_add_plane(image, heif_channel_interleaved, (int)img->info.dispWidth, (int)img->info.dispHeight, 8);
		data = heif_image_get_plane(image, heif_channel_interleaved, &stride);

		img->GetImageData(data, 0, 0, img->info.dispWidth, img->info.dispHeight, (UOSInt)stride, false, img->info.rotateType);
		ImageUtil_SwapRGB(data, img->info.dispWidth * img->info.dispHeight, 24);
		break;
	case Media::PF_R8G8B8:
		heif_image_add_plane(image, heif_channel_interleaved, (int)img->info.dispWidth, (int)img->info.dispHeight, 8);
		data = heif_image_get_plane(image, heif_channel_interleaved, &stride);

		img->GetImageData(data, 0, 0, img->info.dispWidth, img->info.dispHeight, (UOSInt)stride, false, img->info.rotateType);
		break;

	case Media::PF_LE_B16G16R16:
		heif_image_add_plane(image, heif_channel_interleaved, (int)img->info.dispWidth, (int)img->info.dispHeight, 16);
		data = heif_image_get_plane(image, heif_channel_interleaved, &stride);

		img->GetImageData(data, 0, 0, img->info.dispWidth, img->info.dispHeight, (UOSInt)stride, false, img->info.rotateType);
		ImageUtil_SwapRGB(data, img->info.dispWidth * img->info.dispHeight, 48);
		break;

	case Media::PF_LE_W16:
	case Media::PF_LE_FB32G32R32:
	case Media::PF_LE_FW32:
	case Media::PF_LE_A2B10G10R10:
	case Media::PF_LE_W16A16:
	case Media::PF_LE_FB32G32R32A32:
	case Media::PF_LE_FW32A32:
		heif_image_add_plane(image, heif_channel_interleaved, (int)img->info.dispWidth, (int)img->info.dispHeight, 16);
		data = heif_image_get_plane(image, heif_channel_interleaved, &stride);

		simg = img->CreateStaticImage();
		simg->To64bpp();
		simg->GetImageData(data, 0, 0, simg->info.dispWidth, simg->info.dispHeight, (UOSInt)stride, false, simg->info.rotateType);
		ImageUtil_SwapRGB(data, img->info.dispWidth * img->info.dispHeight, 64);
		DEL_CLASS(simg);
		break;
	case Media::PF_LE_B16G16R16A16:
		heif_image_add_plane(image, heif_channel_interleaved, (int)img->info.dispWidth, (int)img->info.dispHeight, 16);
		data = heif_image_get_plane(image, heif_channel_interleaved, &stride);

		img->GetImageData(data, 0, 0, img->info.dispWidth, img->info.dispHeight, (UOSInt)stride, false, img->info.rotateType);
		ImageUtil_SwapRGB(data, img->info.dispWidth * img->info.dispHeight, 64);
		break;
	default:
	case Media::PF_UNKNOWN:
		return 0;
	}
	return image;
}

Bool Exporter::HEIFExporter::ExportFile(IO::SeekableStream *stm, Text::CString fileName, IO::ParsedObject *pobj, void *param)
{
	if (pobj->GetParserType() != IO::ParserType::ImageList)
		return false;
	Int32 quality;
	if (param)
	{
		quality = *(Int32*)param;
	}
	else
	{
		quality = 90;
	}
	Media::ImageList *imgList = (Media::ImageList*)pobj;
	heif_context *ctx = heif_context_alloc();
	heif_encoder* encoder;
	heif_context_get_encoder_for_format(ctx, heif_compression_HEVC, &encoder);
	heif_encoder_set_lossy_quality(encoder, quality);

	UOSInt i = 0;
	UOSInt j = imgList->GetCount();
	while (i < j)
	{
		heif_image *img = HEIFExporter_CreateImage(imgList->GetImage(i, 0));
		if (img)
		{
			heif_context_encode_image(ctx, img, encoder, 0, 0);
//			heif_image_release(img);
		}
		i++;
	}

	heif_encoder_release(encoder);
	heif_writer writer;
	writer.writer_api_version = 1;
	writer.write = HEIFExporter_Write;
	heif_error err = heif_context_write(ctx, &writer, stm);
	heif_context_free(ctx);
	return err.code == heif_error_Ok;
}

UOSInt Exporter::HEIFExporter::GetParamCnt()
{
	return 1;
}

void *Exporter::HEIFExporter::CreateParam(IO::ParsedObject *pobj)
{
	Int32 *val = MemAlloc(Int32, 1);
	*val = 100;
	return val;
}

void Exporter::HEIFExporter::DeleteParam(void *param)
{
	MemFree(param);
}

Bool Exporter::HEIFExporter::GetParamInfo(UOSInt index, ParamInfo *info)
{
	if (index == 0)
	{
		info->name = CSTR("Quality");
		info->paramType = IO::FileExporter::ParamType::INT32;
		info->allowNull = false;
		return true;
	}
	return false;
}

Bool Exporter::HEIFExporter::SetParamInt32(void *param, UOSInt index, Int32 val)
{
	if (index == 0)
	{
		if (val >= 0 && val <= 100)
		{
			*(Int32*)param = val;
			return true;
		}
		return false;
	}
	return false;
}

Int32 Exporter::HEIFExporter::GetParamInt32(void *param, UOSInt index)
{
	if (index == 0)
	{
		return *(Int32*)param;
	}
	return 0;
}
