#include "Stdafx.h"
#include "Exporter/AVIFExporter.h"
#include "Media/ImageList.h"
#include "Media/EXIFData.h"
#include "Media/StaticImage.h"
#include "Text/MyString.h"

#include <avif/avif.h>

Exporter::AVIFExporter::AVIFExporter()
{
}

Exporter::AVIFExporter::~AVIFExporter()
{
}

Int32 Exporter::AVIFExporter::GetName()
{
	return *(Int32*)"AVIF";
}

IO::FileExporter::SupportType Exporter::AVIFExporter::IsObjectSupported(NN<IO::ParsedObject> pobj)
{
	if (pobj->GetParserType() != IO::ParserType::ImageList)
		return IO::FileExporter::SupportType::NotSupported;

	NN<Media::ImageList> imgList = NN<Media::ImageList>::ConvertFrom(pobj);
	if (imgList->GetCount() != 1)
		return IO::FileExporter::SupportType::NotSupported;

	NN<Media::Image> img;
	if (!imgList->GetImage2(0, 0).SetTo(img) || img->GetImageType() != Media::ImageType::Raster)
		return IO::FileExporter::SupportType::NotSupported;
	return IO::FileExporter::SupportType::NormalStream;
}

Bool Exporter::AVIFExporter::GetOutputName(UIntOS index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("AVIF image"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.avif"));
		return true;
	}
	return false;
}

static Bool AVIFExporter_Use16Bit(NN<Media::RasterImage> img)
{
	return img->info.pf == Media::PF_LE_B16G16R16A16 ||
		img->info.pf == Media::PF_LE_B16G16R16 ||
		img->info.pf == Media::PF_LE_W16 ||
		img->info.pf == Media::PF_LE_W16A16 ||
		img->info.pf == Media::PF_LE_R16G16B16A16 ||
		img->info.pf == Media::PF_LE_R16G16B16 ||
		img->info.pf == Media::PF_LE_FB32G32R32A32 ||
		img->info.pf == Media::PF_LE_FB32G32R32 ||
		img->info.pf == Media::PF_LE_FR32G32B32A32 ||
		img->info.pf == Media::PF_LE_FR32G32B32 ||
		img->info.pf == Media::PF_LE_FW32A32 ||
		img->info.pf == Media::PF_LE_FW32 ||
		img->info.pf == Media::PF_LE_A2B10G10R10;
}

static avifImage *AVIFExporter_CreateImage(NN<Media::RasterImage> img)
{
	Bool use16Bit = AVIFExporter_Use16Bit(img);
	NN<Media::StaticImage> simg = img->CreateStaticImage();
	if (use16Bit)
	{
		simg->ToB16G16R16A16();
	}
	else
	{
		simg->ToB8G8R8A8();
	}

	avifImage *avifImg = avifImageCreate((uint32_t)simg->info.dispSize.x, (uint32_t)simg->info.dispSize.y, use16Bit ? 16 : 8, AVIF_PIXEL_FORMAT_YUV444);
	if (avifImg == 0)
	{
		simg.Delete();
		return 0;
	}

	avifRGBImage rgb;
	avifRGBImageSetDefaults(&rgb, avifImg);
	rgb.format = AVIF_RGB_FORMAT_BGRA;
	rgb.depth = use16Bit ? 16 : 8;
	rgb.pixels = simg->data.Ptr();
	rgb.rowBytes = (uint32_t)simg->GetDataBpl();
	avifResult result = avifImageRGBToYUV(avifImg, &rgb);
	simg.Delete();
	if (result != AVIF_RESULT_OK)
	{
		avifImageDestroy(avifImg);
		return 0;
	}
	return avifImg;
}

static Bool AVIFExporter_CopyMetadata(NN<Media::RasterImage> img, avifImage *avifImg)
{
	UnsafeArray<const UInt8> rawICC;
	if (img->info.color.GetRAWICC().SetTo(rawICC))
	{
		if (avifImageSetProfileICC(avifImg, rawICC.Ptr(), ReadMUInt32(&rawICC[0])) != AVIF_RESULT_OK)
		{
			return false;
		}
	}

	NN<Media::EXIFData> exif;
	if (img->exif.SetTo(exif))
	{
		NN<Media::EXIFData> outExif = exif->Clone();
		outExif->SetRotateType(Media::RotateType::None);

		UInt64 exifSize;
		UInt64 endOfst;
		UInt32 k;
		UInt32 l;
		UnsafeArray<UInt8> exifBuff;
		outExif->GetExifBuffSize(exifSize, endOfst);
		exifBuff = MemAllocArr(UInt8, (UIntOS)exifSize + 8);
		WriteInt16(&exifBuff[0], ReadInt16((const UInt8*)"II"));
		WriteInt16(&exifBuff[2], 42);
		WriteInt32(&exifBuff[4], 8);
		k = 8;
		l = (UInt32)endOfst + 8;
		outExif->ToExifBuff(exifBuff, k, l);

		if (avifRWDataSet(&avifImg->exif, exifBuff.Ptr(), (size_t)exifSize + 8) != AVIF_RESULT_OK)
		{
			MemFreeArr(exifBuff);
			outExif.Delete();
			return false;
		}
		MemFreeArr(exifBuff);
		outExif.Delete();
	}

	return true;
}

Bool Exporter::AVIFExporter::ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param)
{
	if (pobj->GetParserType() != IO::ParserType::ImageList)
		return false;

	NN<Media::ImageList> imgList = NN<Media::ImageList>::ConvertFrom(pobj);
	if (imgList->GetCount() != 1)
		return false;

	Int32 quality;
	NN<ParamData> para;
	if (param.SetTo(para))
	{
		quality = *(Int32*)para.Ptr();
	}
	else
	{
		quality = 90;
	}

	NN<Media::Image> img;
	if (!imgList->GetImage2(0, 0).SetTo(img) || img->GetImageType() != Media::ImageType::Raster)
		return false;

	avifImage *avifImg = AVIFExporter_CreateImage(NN<Media::RasterImage>::ConvertFrom(img));
	if (avifImg == 0)
		return false;
	if (!AVIFExporter_CopyMetadata(NN<Media::RasterImage>::ConvertFrom(img), avifImg))
	{
		avifImageDestroy(avifImg);
		return false;
	}

	avifEncoder *encoder = avifEncoderCreate();
	if (encoder == 0)
	{
		avifImageDestroy(avifImg);
		return false;
	}
	encoder->quality = quality;
	encoder->qualityAlpha = quality;

	avifRWData output = AVIF_DATA_EMPTY;
	avifResult result = avifEncoderWrite(encoder, avifImg, &output);
	avifEncoderDestroy(encoder);
	avifImageDestroy(avifImg);
	if (result != AVIF_RESULT_OK)
	{
		return false;
	}

	Bool succ = stm->Write(Data::ByteArrayR(output.data, (UIntOS)output.size)) == (UIntOS)output.size;
	avifRWDataFree(&output);
	return succ;
}

UIntOS Exporter::AVIFExporter::GetParamCnt()
{
	return 1;
}

Optional<IO::FileExporter::ParamData> Exporter::AVIFExporter::CreateParam(NN<IO::ParsedObject> pobj)
{
	UnsafeArray<Int32> val = MemAllocArr(Int32, 1);
	val[0] = 90;
	return (ParamData*)val.Ptr();
}

void Exporter::AVIFExporter::DeleteParam(Optional<ParamData> param)
{
	NN<ParamData> para;
	if (param.SetTo(para))
	{
		MemFree(para.Ptr());
	}
}

Bool Exporter::AVIFExporter::GetParamInfo(UIntOS index, NN<ParamInfo> info)
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

Bool Exporter::AVIFExporter::SetParamInt32(Optional<ParamData> param, UIntOS index, Int32 val)
{
	NN<ParamData> para;
	if (index == 0 && param.SetTo(para))
	{
		if (val >= 0 && val <= 100)
		{
			*(Int32*)para.Ptr() = val;
			return true;
		}
		return false;
	}
	return false;
}

Int32 Exporter::AVIFExporter::GetParamInt32(Optional<ParamData> param, UIntOS index)
{
	NN<ParamData> para;
	if (index == 0 && param.SetTo(para))
	{
		return *(Int32*)para.Ptr();
	}
	return 0;
}