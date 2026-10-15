#include "Stdafx.h"
#include "Exporter/JXLExporter.h"
#include "Media/EXIFData.h"
#include "Media/ImageList.h"
#include "Media/StaticImage.h"
#include "Text/MyString.h"

#include <jxl/encode.h>
#include <jxl/encode_cxx.h>

Exporter::JXLExporter::JXLExporter()
{
}

Exporter::JXLExporter::~JXLExporter()
{
}

Int32 Exporter::JXLExporter::GetName()
{
	return *(Int32*)"JXLE";
}

IO::FileExporter::SupportType Exporter::JXLExporter::IsObjectSupported(NN<IO::ParsedObject> pobj)
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

Bool Exporter::JXLExporter::GetOutputName(UIntOS index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("JPEG XL"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.jxl"));
		return true;
	}
	return false;
}

Bool Exporter::JXLExporter::ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param)
{
	if (pobj->GetParserType() != IO::ParserType::ImageList)
		return false;
	NN<Media::ImageList> imgList = NN<Media::ImageList>::ConvertFrom(pobj);
	if (imgList->GetCount() != 1)
		return false;

	Int32 quality = 90;
	NN<ParamData> para;
	if (param.SetTo(para))
		quality = *(Int32*)para.Ptr();

	NN<Media::Image> img;
	if (!imgList->GetImage2(0, 0).SetTo(img) || img->GetImageType() != Media::ImageType::Raster)
		return false;

	NN<Media::RasterImage> rimg = NN<Media::RasterImage>::ConvertFrom(img);
	NN<Media::StaticImage> simg = rimg->CreateStaticImage();
	UIntOS width = simg->info.dispSize.x;
	UIntOS height = simg->info.dispSize.y;
	UIntOS sampleSize;
	UInt32 bitsPerSample;
	UInt32 exponentBitsPerSample = 0;
	JxlDataType dataType;
	UInt32 colorChannels;
	UInt32 sourceChannels;
	Bool alpha;
	Bool premulAlpha = simg->info.atype == Media::AT_PREMUL_ALPHA;
	Bool bgr;
	Bool gray;
	switch (simg->info.pf)
	{
	case Media::PF_R8G8B8:
		sampleSize = 1; bitsPerSample = 8; dataType = JXL_TYPE_UINT8; colorChannels = 3; sourceChannels = 3; alpha = false; bgr = false; gray = false;
		break;
	case Media::PF_R8G8B8A8:
		sampleSize = 1; bitsPerSample = 8; dataType = JXL_TYPE_UINT8; colorChannels = 3; sourceChannels = 4; alpha = simg->info.atype == Media::AT_ALPHA || simg->info.atype == Media::AT_PREMUL_ALPHA; bgr = false; gray = false;
		break;
	case Media::PF_B8G8R8:
		sampleSize = 1; bitsPerSample = 8; dataType = JXL_TYPE_UINT8; colorChannels = 3; sourceChannels = 3; alpha = false; bgr = true; gray = false;
		break;
	case Media::PF_B8G8R8A8:
		sampleSize = 1; bitsPerSample = 8; dataType = JXL_TYPE_UINT8; colorChannels = 3; sourceChannels = 4; alpha = simg->info.atype == Media::AT_ALPHA || simg->info.atype == Media::AT_PREMUL_ALPHA; bgr = true; gray = false;
		break;
	case Media::PF_W8A8:
		sampleSize = 1; bitsPerSample = 8; dataType = JXL_TYPE_UINT8; colorChannels = 1; sourceChannels = 2; alpha = simg->info.atype == Media::AT_ALPHA || simg->info.atype == Media::AT_PREMUL_ALPHA; bgr = false; gray = true;
		break;
	case Media::PF_LE_R16G16B16:
		sampleSize = 2; bitsPerSample = 16; dataType = JXL_TYPE_UINT16; colorChannels = 3; sourceChannels = 3; alpha = false; bgr = false; gray = false;
		break;
	case Media::PF_LE_R16G16B16A16:
		sampleSize = 2; bitsPerSample = 16; dataType = JXL_TYPE_UINT16; colorChannels = 3; sourceChannels = 4; alpha = simg->info.atype == Media::AT_ALPHA || simg->info.atype == Media::AT_PREMUL_ALPHA; bgr = false; gray = false;
		break;
	case Media::PF_LE_B16G16R16:
		sampleSize = 2; bitsPerSample = 16; dataType = JXL_TYPE_UINT16; colorChannels = 3; sourceChannels = 3; alpha = false; bgr = true; gray = false;
		break;
	case Media::PF_LE_B16G16R16A16:
		sampleSize = 2; bitsPerSample = 16; dataType = JXL_TYPE_UINT16; colorChannels = 3; sourceChannels = 4; alpha = simg->info.atype == Media::AT_ALPHA || simg->info.atype == Media::AT_PREMUL_ALPHA; bgr = true; gray = false;
		break;
	case Media::PF_LE_W16:
		sampleSize = 2; bitsPerSample = 16; dataType = JXL_TYPE_UINT16; colorChannels = 1; sourceChannels = 1; alpha = false; bgr = false; gray = true;
		break;
	case Media::PF_LE_W16A16:
		sampleSize = 2; bitsPerSample = 16; dataType = JXL_TYPE_UINT16; colorChannels = 1; sourceChannels = 2; alpha = simg->info.atype == Media::AT_ALPHA || simg->info.atype == Media::AT_PREMUL_ALPHA; bgr = false; gray = true;
		break;
	case Media::PF_LE_FR32G32B32:
		sampleSize = 4; bitsPerSample = 32; exponentBitsPerSample = 8; dataType = JXL_TYPE_FLOAT; colorChannels = 3; sourceChannels = 3; alpha = false; bgr = false; gray = false;
		break;
	case Media::PF_LE_FR32G32B32A32:
		sampleSize = 4; bitsPerSample = 32; exponentBitsPerSample = 8; dataType = JXL_TYPE_FLOAT; colorChannels = 3; sourceChannels = 4; alpha = simg->info.atype == Media::AT_ALPHA || simg->info.atype == Media::AT_PREMUL_ALPHA; bgr = false; gray = false;
		break;
	case Media::PF_LE_FB32G32R32:
		sampleSize = 4; bitsPerSample = 32; exponentBitsPerSample = 8; dataType = JXL_TYPE_FLOAT; colorChannels = 3; sourceChannels = 3; alpha = false; bgr = true; gray = false;
		break;
	case Media::PF_LE_FB32G32R32A32:
		sampleSize = 4; bitsPerSample = 32; exponentBitsPerSample = 8; dataType = JXL_TYPE_FLOAT; colorChannels = 3; sourceChannels = 4; alpha = simg->info.atype == Media::AT_ALPHA || simg->info.atype == Media::AT_PREMUL_ALPHA; bgr = true; gray = false;
		break;
	case Media::PF_LE_FW32:
		sampleSize = 4; bitsPerSample = 32; exponentBitsPerSample = 8; dataType = JXL_TYPE_FLOAT; colorChannels = 1; sourceChannels = 1; alpha = false; bgr = false; gray = true;
		break;
	case Media::PF_LE_FW32A32:
		sampleSize = 4; bitsPerSample = 32; exponentBitsPerSample = 8; dataType = JXL_TYPE_FLOAT; colorChannels = 1; sourceChannels = 2; alpha = simg->info.atype == Media::AT_ALPHA || simg->info.atype == Media::AT_PREMUL_ALPHA; bgr = false; gray = true;
		break;
	case Media::PF_UNKNOWN:
	case Media::PF_PAL_1:
	case Media::PF_PAL_2:
	case Media::PF_PAL_4:
	case Media::PF_PAL_8:
	case Media::PF_LE_R5G5B5:
	case Media::PF_LE_R5G6B5:
	case Media::PF_LE_A2B10G10R10:
	case Media::PF_PAL_W1:
	case Media::PF_PAL_W2:
	case Media::PF_PAL_W4:
	case Media::PF_PAL_W8:
	case Media::PF_PAL_1_A1:
	case Media::PF_PAL_2_A1:
	case Media::PF_PAL_4_A1:
	case Media::PF_PAL_8_A1:
	case Media::PF_B8G8R8A1:
	default:
		if (simg->info.storeBPP > 8 || !simg->ToB8G8R8A8())
		{
			simg.Delete();
			return false;
		}
		sampleSize = 1;
		bitsPerSample = 8;
		dataType = JXL_TYPE_UINT8;
		colorChannels = 3;
		sourceChannels = 4;
		alpha = simg->info.atype == Media::AT_ALPHA || simg->info.atype == Media::AT_PREMUL_ALPHA;
		bgr = true;
		gray = false;
		break;
	}

	UInt32 channels = colorChannels + (alpha ? 1 : 0);
	UIntOS bytesPerPixel = sampleSize * channels;
	UIntOS pixelSize = width * height * bytesPerPixel;
	UnsafeArray<UInt8> pixelData = MemAllocArr(UInt8, pixelSize);
	UIntOS srcBpl = simg->GetDataBpl();
	UIntOS y = 0;
	while (y < height)
	{
		UnsafeArray<const UInt8> srcRow = simg->data + y * srcBpl;
		UnsafeArray<UInt8> destRow = pixelData + y * width * bytesPerPixel;
		UIntOS x = 0;
		while (x < width)
		{
			UIntOS c = 0;
			while (c < channels)
			{
				UIntOS srcChannel;
				if (gray)
				{
					srcChannel = c;
				}
				else if (c == 3)
				{
					srcChannel = 3;
				}
				else
				{
					srcChannel = bgr ? 2 - c : c;
				}
				MemCopyNO(destRow.Ptr() + (x * channels + c) * sampleSize, srcRow.Ptr() + (x * sourceChannels + srcChannel) * sampleSize, sampleSize);
				c++;
			}
			x++;
		}
		y++;
	}

	JxlEncoderPtr enc = JxlEncoderMake(nullptr);
	if (!enc)
	{
		MemFreeArr(pixelData);
		simg.Delete();
		return false;
	}
	JxlBasicInfo info;
	JxlEncoderInitBasicInfo(&info);
	info.xsize = (uint32_t)width;
	info.ysize = (uint32_t)height;
	info.bits_per_sample = bitsPerSample;
	info.exponent_bits_per_sample = exponentBitsPerSample;
	info.num_color_channels = colorChannels;
	info.num_extra_channels = alpha ? 1 : 0;
	info.alpha_bits = alpha ? bitsPerSample : 0;
	info.alpha_premultiplied = alpha && premulAlpha;
	if (JxlEncoderSetBasicInfo(enc.get(), &info) != JXL_ENC_SUCCESS)
	{
		MemFreeArr(pixelData);
		simg.Delete();
		return false;
	}

	UnsafeArray<const UInt8> rawICC;
	if (simg->info.color.GetRAWICC().SetTo(rawICC))
	{
		UInt32 iccSize = ReadMUInt32(rawICC.Ptr());
		if (iccSize < 4 || JxlEncoderSetICCProfile(enc.get(), rawICC.Ptr(), iccSize) != JXL_ENC_SUCCESS)
		{
			MemFreeArr(pixelData);
			simg.Delete();
			return false;
		}
	}
	else
	{
		JxlColorEncoding colorEncoding;
		JxlColorEncodingSetToSRGB(&colorEncoding, JXL_FALSE);
		if (JxlEncoderSetColorEncoding(enc.get(), &colorEncoding) != JXL_ENC_SUCCESS)
		{
			MemFreeArr(pixelData);
			simg.Delete();
			return false;
		}
	}

	NN<Media::EXIFData> exif;
	if (rimg->exif.SetTo(exif))
	{
		NN<Media::EXIFData> outExif = exif->Clone();
		outExif->SetRotateType(Media::RotateType::None);
		UInt64 exifSize;
		UInt64 endOfst;
		UInt32 k;
		UInt32 l;
		outExif->GetExifBuffSize(exifSize, endOfst);
		UnsafeArray<UInt8> exifBuff = MemAllocArr(UInt8, (UIntOS)exifSize + 12);
		WriteMInt32(&exifBuff[0], 0);
		WriteLInt16(&exifBuff[4], ReadLInt16((const UInt8*)"II"));
		WriteLInt16(&exifBuff[6], 42);
		WriteLInt32(&exifBuff[8], 8);
		k = 8;
		l = (UInt32)endOfst + 8;
		outExif->ToExifBuff(exifBuff + 4, k, l);
		JxlBoxType boxType = {'E', 'x', 'i', 'f'};
		if (JxlEncoderUseBoxes(enc.get()) != JXL_ENC_SUCCESS ||
			JxlEncoderAddBox(enc.get(), boxType, exifBuff.Ptr(), (size_t)exifSize + 12, JXL_FALSE) != JXL_ENC_SUCCESS)
		{
			MemFreeArr(exifBuff);
			outExif.Delete();
			MemFreeArr(pixelData);
			simg.Delete();
			return false;
		}
		MemFreeArr(exifBuff);
		outExif.Delete();
	}

	JxlPixelFormat pixelFormat = {channels, dataType, JXL_LITTLE_ENDIAN, 0};
	JxlEncoderFrameSettings* frameSettings = JxlEncoderFrameSettingsCreate(enc.get(), nullptr);
	if (frameSettings == nullptr ||
		JxlEncoderSetFrameDistance(frameSettings, JxlEncoderDistanceFromQuality((float)quality)) != JXL_ENC_SUCCESS ||
		JxlEncoderAddImageFrame(frameSettings, &pixelFormat, pixelData.Ptr(), (size_t)pixelSize) != JXL_ENC_SUCCESS)
	{
		MemFreeArr(pixelData);
		simg.Delete();
		return false;
	}
	MemFreeArr(pixelData);
	simg.Delete();

	JxlEncoderCloseInput(enc.get());
	Data::ByteBuffer output(65536);
	UIntOS outputSize = 0;
	for (;;)
	{
		UInt8* nextOut = output.Ptr() + outputSize;
		size_t availOut = output.GetSize() - outputSize;
		JxlEncoderStatus status = JxlEncoderProcessOutput(enc.get(), &nextOut, &availOut);
		outputSize = (UIntOS)(nextOut - output.Ptr());
		if (status == JXL_ENC_SUCCESS)
			break;
		if (status != JXL_ENC_NEED_MORE_OUTPUT)
			return false;
		output.ChangeSizeAndKeep(output.GetSize() * 2);
	}
	return stm->Write(Data::ByteArrayR(output.Ptr(), outputSize)) == outputSize;
}

UIntOS Exporter::JXLExporter::GetParamCnt()
{
	return 1;
}

Optional<IO::FileExporter::ParamData> Exporter::JXLExporter::CreateParam(NN<IO::ParsedObject> pobj)
{
	UnsafeArray<Int32> val = MemAllocArr(Int32, 1);
	val[0] = 90;
	return (ParamData*)val.Ptr();
}

void Exporter::JXLExporter::DeleteParam(Optional<ParamData> param)
{
	NN<ParamData> para;
	if (param.SetTo(para))
		MemFree(para.Ptr());
}

Bool Exporter::JXLExporter::GetParamInfo(UIntOS index, NN<ParamInfo> info)
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

Bool Exporter::JXLExporter::SetParamInt32(Optional<ParamData> param, UIntOS index, Int32 val)
{
	NN<ParamData> para;
	if (index == 0 && param.SetTo(para))
	{
		if (val >= 0 && val <= 100)
		{
			*(Int32*)para.Ptr() = val;
			return true;
		}
	}
	return false;
}

Int32 Exporter::JXLExporter::GetParamInt32(Optional<ParamData> param, UIntOS index)
{
	NN<ParamData> para;
	if (index == 0 && param.SetTo(para))
		return *(Int32*)para.Ptr();
	return 0;
}
