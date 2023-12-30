#include "Stdafx.h"
#include "Exporter/WebPExporter.h"
#include "Media/ImageList.h"
#include "Media/ImageUtil.h"
#include "Media/StaticImage.h"
#include "Text/MyString.h"
#include <webp/encode.h>
#include <webp/mux.h>

Exporter::WebPExporter::WebPExporter()
{
}

Exporter::WebPExporter::~WebPExporter()
{
}

Int32 Exporter::WebPExporter::GetName()
{
	return *(Int32*)"WEBP";
}

IO::FileExporter::SupportType Exporter::WebPExporter::IsObjectSupported(NotNullPtr<IO::ParsedObject> pobj)
{
	if (pobj->GetParserType() != IO::ParserType::ImageList)
		return IO::FileExporter::SupportType::NotSupported;
	NotNullPtr<Media::ImageList> imgList = NotNullPtr<Media::ImageList>::ConvertFrom(pobj);
	if (imgList->GetCount() != 1)
		return IO::FileExporter::SupportType::NotSupported;
	return IO::FileExporter::SupportType::NormalStream;
}

Bool Exporter::WebPExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("WebP"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.webp"));
		return true;
	}
	return false;
}

Bool Exporter::WebPExporter::ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CStringNN fileName, NotNullPtr<IO::ParsedObject> pobj, void *param)
{
	if (pobj->GetParserType() != IO::ParserType::ImageList)
		return false;
	NotNullPtr<Media::ImageList> imgList = NotNullPtr<Media::ImageList>::ConvertFrom(pobj);
	if (imgList->GetCount() != 1)
		return false;
	Int32 quality;
	if (param)
	{
		quality = *(Int32*)param;
	}
	else
	{
		quality = 80;
	}
	UInt8 *vp8 = 0;
	size_t vp8len;
	Media::Image *img = imgList->GetImage(0, 0);
	if (img->info.pf == Media::PF_B8G8R8)
	{
		UOSInt bpl = img->GetDataBpl();
		UInt8 *buff = MemAlloc(UInt8, bpl * img->info.dispSize.y);
		img->GetImageData(buff, 0, 0, img->info.dispSize.x, img->info.dispSize.y, bpl, false, img->info.rotateType);
		if (quality < 0)
		{
			vp8len = WebPEncodeLosslessBGR(buff, (int)img->info.dispSize.x, (int)img->info.dispSize.y, (int)bpl, &vp8);
		}
		else
		{
			vp8len = WebPEncodeBGR(buff, (int)img->info.dispSize.x, (int)img->info.dispSize.y, (int)bpl, (float)quality, &vp8);
		}
		MemFree(buff);
	}
	else if (img->info.pf == Media::PF_R8G8B8)
	{
		UOSInt bpl = img->GetDataBpl();
		UInt8 *buff = MemAlloc(UInt8, bpl * img->info.dispSize.y);
		img->GetImageData(buff, 0, 0, img->info.dispSize.x, img->info.dispSize.y, bpl, false, img->info.rotateType);
		if (quality < 0)
		{
			vp8len = WebPEncodeLosslessRGB(buff, (int)img->info.dispSize.x, (int)img->info.dispSize.y, (int)bpl, &vp8);
		}
		else
		{
			vp8len = WebPEncodeRGB(buff, (int)img->info.dispSize.x, (int)img->info.dispSize.y, (int)bpl, (float)quality, &vp8);
		}
		MemFree(buff);
	}
	else if (img->info.pf == Media::PF_B8G8R8A8)
	{
		UOSInt bpl = img->GetDataBpl();
		UInt8 *buff = MemAlloc(UInt8, bpl * img->info.dispSize.y);
		img->GetImageData(buff, 0, 0, img->info.dispSize.x, img->info.dispSize.y, bpl, false, img->info.rotateType);
		if (img->info.atype == Media::AT_NO_ALPHA)
		{
			UInt8 *imgBuff = MemAlloc(UInt8, img->info.dispSize.CalcArea() * 3);
			ImageUtil_ConvARGB32_B8G8R8(buff, imgBuff, img->info.dispSize.x, img->info.dispSize.y, (OSInt)bpl, (OSInt)img->info.dispSize.x * 3);
			if (quality < 0)
			{
				vp8len = WebPEncodeLosslessBGR(imgBuff, (int)img->info.dispSize.x, (int)img->info.dispSize.y, (int)img->info.dispSize.x * 3, &vp8);
			}
			else
			{
				vp8len = WebPEncodeBGR(imgBuff, (int)img->info.dispSize.x, (int)img->info.dispSize.y, (int)img->info.dispSize.x * 3, (float)quality, &vp8);
			}
			MemFree(imgBuff);
		}
		else
		{
			if (quality < 0)
			{
				vp8len = WebPEncodeLosslessBGRA(buff, (int)img->info.dispSize.x, (int)img->info.dispSize.y, (int)bpl, &vp8);
			}
			else
			{
				vp8len = WebPEncodeBGRA(buff, (int)img->info.dispSize.x, (int)img->info.dispSize.y, (int)bpl, (float)quality, &vp8);
			}
		}
		MemFree(buff);
	}
	else
	{
		NotNullPtr<Media::StaticImage> simg = img->CreateStaticImage();
		simg->To32bpp();
		UOSInt bpl = simg->GetDataBpl();
		if (quality < 0)
		{
			vp8len = WebPEncodeLosslessBGRA(simg->data, (int)simg->info.dispSize.x, (int)simg->info.dispSize.y, (int)bpl, &vp8);
		}
		else
		{
			vp8len = WebPEncodeBGRA(simg->data, (int)simg->info.dispSize.x, (int)simg->info.dispSize.y, (int)bpl, (float)quality, &vp8);
		}
		simg.Delete();
	}
	if (vp8)
	{
		WebPMux* mux = WebPMuxNew();
		WebPData data;
		data.bytes = vp8;
		data.size = vp8len;
		WebPMuxSetImage(mux, &data, 0);
		const UInt8 *icc = img->info.color.GetRAWICC();
		if (icc)
		{
			data.bytes = icc;
			data.size = (size_t)ReadMUInt32(icc);
			WebPMuxSetChunk(mux, "ICCP", &data, 0);
		}
		NotNullPtr<Media::EXIFData> exif;
		if (img->exif.SetTo(exif))
		{
			UInt64 exifSize;
			UInt64 endOfst;
			UInt32 k;
			UInt32 l;
			UInt8 *exifBuff;
			exif->GetExifBuffSize(&exifSize, &endOfst);
			exifBuff = MemAlloc(UInt8, (UOSInt)exifSize + 8);
			WriteInt16(&exifBuff[0], ReadInt16((const UInt8*)"II"));
			WriteInt16(&exifBuff[2], 42);
			WriteInt32(&exifBuff[4], 8);
			k = 8;
			l = (UInt32)endOfst + 8;
			exif->ToExifBuff(exifBuff, &k, &l);

			data.bytes = exifBuff;
			data.size = (size_t)exifSize + 8;
			WebPMuxSetChunk(mux, "EXIF", &data, 1);
			MemFree(exifBuff);
		}
		WebPMuxAssemble(mux, &data);
		WebPMuxDelete(mux);
		stm->Write(data.bytes, (UOSInt)data.size);
		WebPDataClear(&data);
		WebPFree(vp8);
		return true;
	}
	return false;
}

UOSInt Exporter::WebPExporter::GetParamCnt()
{
	return 1;
}

void *Exporter::WebPExporter::CreateParam(NotNullPtr<IO::ParsedObject> pobj)
{
	Int32 *val = MemAlloc(Int32, 1);
	*val = 100;
	return val;
}

void Exporter::WebPExporter::DeleteParam(void *param)
{
	MemFree(param);
}

Bool Exporter::WebPExporter::GetParamInfo(UOSInt index, NotNullPtr<ParamInfo> info)
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

Bool Exporter::WebPExporter::SetParamInt32(void *param, UOSInt index, Int32 val)
{
	if (index == 0)
	{
		if (val <= 100)
		{
			*(Int32*)param = val;
			return true;
		}
		return false;
	}
	return false;
}

Int32 Exporter::WebPExporter::GetParamInt32(void *param, UOSInt index)
{
	if (index == 0)
	{
		return *(Int32*)param;
	}
	return 0;
}
