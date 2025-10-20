#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Data/Compress/LZWEncStream2.h"
#include "Exporter/GIFExporter.h"
#include "IO/MemoryStream.h"
#include "Math/Math.h"
#include "Media/ImageList.h"
#include "Text/MyString.h"

Exporter::GIFExporter::GIFExporter()
{
}

Exporter::GIFExporter::~GIFExporter()
{
}

Int32 Exporter::GIFExporter::GetName()
{
	return *(Int32*)"GIFE";
}

IO::FileExporter::SupportType Exporter::GIFExporter::IsObjectSupported(NN<IO::ParsedObject> pobj)
{
	if (pobj->GetParserType() != IO::ParserType::ImageList)
		return IO::FileExporter::SupportType::NotSupported;
	NN<Media::ImageList> imgList = NN<Media::ImageList>::ConvertFrom(pobj);
	if (imgList->GetCount() != 1)
		return IO::FileExporter::SupportType::NotSupported;
	NN<Media::RasterImage> img;
	if (!imgList->GetImage(0, 0).SetTo(img) || img->info.fourcc != 0)
		return IO::FileExporter::SupportType::NotSupported;
	if (img->info.pf == Media::PF_PAL_8 || img->info.pf == Media::PF_PAL_W8)
	{
		if (img->info.atype == Media::AT_ALPHA_ALL_FF || img->info.atype == Media::AT_IGNORE_ALPHA)
			return IO::FileExporter::SupportType::NormalStream;
		OSInt i;
		Bool found = false;
		UnsafeArray<UInt8> pal;
		if (img->pal.SetTo(pal))
		{
			i = 0;
			while (i < 1024)
			{
				if (pal[i + 3] == 0xff)
				{
				}
				else if (pal[i + 3] == 0)
				{
					if (found)
						return IO::FileExporter::SupportType::NotSupported;
					found = true;
				}
				else
				{
					return IO::FileExporter::SupportType::NotSupported;
				}
				i += 4;
			}
			return IO::FileExporter::SupportType::NormalStream;
		}
	}
	return IO::FileExporter::SupportType::NotSupported;
}

Bool Exporter::GIFExporter::GetOutputName(UOSInt index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("GIF Image"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.gif"));
		return true;
	}
	return false;
}

void Exporter::GIFExporter::SetCodePage(UInt32 codePage)
{
}

Bool Exporter::GIFExporter::ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param)
{
	if (IsObjectSupported(pobj) == SupportType::NotSupported)
		return false;
	UInt8 buff[256];
	NN<Media::ImageList> imgList = NN<Media::ImageList>::ConvertFrom(pobj);
	NN<Media::RasterImage> img;
	UnsafeArray<UInt8> pal;
	if (!imgList->GetImage(0, 0).SetTo(img) || !img->pal.SetTo(pal))
		return false;
	UOSInt transparentIndex = INVALID_INDEX;
	UOSInt i;
	UOSInt j;
	if (img->info.pf == Media::PF_PAL_8 || img->info.pf == Media::PF_PAL_W8)
	{
		if (img->info.atype == Media::AT_IGNORE_ALPHA || img->info.atype == Media::AT_ALPHA_ALL_FF)
		{
		}
		else
		{
			i = 0;
			while (i < 1024)
			{
				if (pal[i + 3] == 0xff)
				{
				}
				else if (pal[i + 3] == 0)
				{
					transparentIndex = i >> 2;
					break;
				}
				i += 4;
			}
		}

		buff[0] = 'G';
		buff[1] = 'I';
		buff[2] = 'F';
		buff[3] = '8';
		buff[4] = '9';
		buff[5] = 'a';
		WriteInt16(&buff[6], (Int16)img->info.dispSize.x);
		WriteInt16(&buff[8], (Int16)img->info.dispSize.y);
		buff[10] = 0xf7;
		buff[11] = 0;
		buff[12] = (UInt8)(Double2Int32(img->info.par2 * 64.0) - 15);
		stm->Write(Data::ByteArrayR(buff, 13));

		UInt8 *palBuff = MemAlloc(UInt8, 768);
		i = 0;
		j = 0;
		while (j < 1024)
		{
			palBuff[i] = pal[j + 2];
			palBuff[i + 1] = pal[j + 1];
			palBuff[i + 2] = pal[j + 0];
			i += 3;
			j += 4;
		}

		stm->Write(Data::ByteArrayR(palBuff, 768));
		MemFree(palBuff);

		if (transparentIndex != INVALID_INDEX)
		{
			buff[0] = 0x21;
			buff[1] = 0xf9;
			buff[2] = 4;
			buff[3] = 1;
			WriteInt16(&buff[4], 0);
			buff[6] = (UInt8)transparentIndex;
			buff[7] = 0;
			stm->Write(Data::ByteArrayR(buff, 8));
		}

		buff[0] = 0x2c;
		WriteInt16(&buff[1], 0);
		WriteInt16(&buff[3], 0);
		WriteInt16(&buff[5], (Int16)img->info.dispSize.x);
		WriteInt16(&buff[7], (Int16)img->info.dispSize.y);
		buff[9] = 0;
		buff[10] = 8;
		stm->Write(Data::ByteArrayR(buff, 11));

		UnsafeArray<UInt8> imgData = MemAllocArr(UInt8, img->info.dispSize.CalcArea());
		UOSInt imgSize;
		Data::Compress::LZWEncStream2 *lzw;
		imgSize = img->info.dispSize.CalcArea() >> 1;
		if (imgSize < 4096)
			imgSize = 4096;
		IO::MemoryStream mstm(imgSize);
		NEW_CLASS(lzw, Data::Compress::LZWEncStream2(mstm, true, 8, 12, 0));
		img->GetRasterData(imgData, 0, 0, img->info.dispSize.x, img->info.dispSize.y, img->info.dispSize.x, false, Media::RotateType::None);
		lzw->Write(Data::ByteArrayR(imgData, img->info.dispSize.CalcArea()));
		MemFreeArr(imgData);
		DEL_CLASS(lzw);
		imgData = mstm.GetBuff(imgSize);
		i = 0;
		while (i < imgSize)
		{
			if (i + 255 <= imgSize)
			{
				buff[0] = 255;
				MemCopyNO(&buff[1], &imgData[i], 255);
				stm->Write(Data::ByteArrayR(buff, 256));
				i += 255;
			}
			else
			{
				buff[0] = (UInt8)(imgSize - i);
				MemCopyNO(&buff[1], &imgData[i], buff[0]);
				stm->Write(Data::ByteArrayR(buff, (UOSInt)buff[0] + 1));
				i += buff[0];
			}
		}
		buff[0] = 0;
		buff[1] = 0x3b;
		stm->Write(Data::ByteArrayR(buff, 2));
		return true;
	}
	else
	{
		return false;
	}
}
