#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Exporter/ANIExporter.h"
#include "IO/MemoryStream.h"
#include "Math/Math.h"
#include "Media/ImageList.h"
#include "Media/StaticImage.h"
#include "Text/MyString.h"

Bool Exporter::ANIExporter::ImageSupported(NN<Media::RasterImage> img)
{
	if (img->info.dispSize.x <= 0 || img->info.dispSize.x > 256 || img->info.dispSize.y <= 0 || img->info.dispSize.y > 256)
	{
		return false;
	}
	if (img->info.fourcc != 0)
	{
		return false;
	}
	if (img->info.pf == Media::PF_B8G8R8A8)
	{
		return true;
	}
	else if (img->info.pf == Media::PF_B8G8R8A1)
	{
		return true;
	}
	else if (img->info.pf == Media::PF_PAL_1_A1)
	{
		return true;
	}
	else if (img->info.pf == Media::PF_PAL_4_A1)
	{
		return true;
	}
	else if (img->info.pf == Media::PF_PAL_8_A1)
	{
		return true;
	}
	return false;
}

OSInt Exporter::ANIExporter::CalcBuffSize(NN<Media::ImageList> imgList)
{
	OSInt i;
	OSInt j;
	UInt32 imgDelay;
	OSInt imgSize;
	OSInt maskSize;
	NN<Media::RasterImage> img;
	OSInt retSize = 6;
	i = 0;
	j = imgList->GetCount();
	while (i < j)
	{
		if (!imgList->GetImage(i, imgDelay).SetTo(img))
			return 0;
		if (img->info.fourcc != 0)
		{
			return 0;
		}
		if (img->info.dispSize.x <= 0 || img->info.dispSize.x > 256 || img->info.dispSize.y <= 0 || img->info.dispSize.y > 256)
		{
			return 0;
		}
		if (img->info.pf == Media::PF_B8G8R8A8)
		{
			imgSize = img->info.dispSize.x * 4;
			retSize += 16 + 40 + imgSize * img->info.dispSize.y;
		}
		else if (img->info.pf == Media::PF_B8G8R8A1)
		{
			imgSize = img->info.dispSize.x * 3;
			if (imgSize & 3)
			{
				imgSize += 4 - (imgSize & 3);
			}
			maskSize = (img->info.dispSize.x + 7) >> 3;
			if (maskSize & 3)
			{
				maskSize += 4 - (maskSize & 3);
			}
			retSize += 16 + 40 + (imgSize + maskSize) * img->info.dispSize.y;
		}
		else if (img->info.pf == Media::PF_PAL_1_A1)
		{
			imgSize = (img->info.dispSize.x + 7) >> 3;
			if (imgSize & 3)
			{
				imgSize += 4 - (imgSize & 3);
			}
			maskSize = (img->info.dispSize.x + 7) >> 3;
			if (maskSize & 3)
			{
				maskSize += 4 - (maskSize & 3);
			}
			retSize += 16 + 40 + 8 + (imgSize + maskSize) * img->info.dispSize.y;
		}
		else if (img->info.pf == Media::PF_PAL_4_A1)
		{
			imgSize = (img->info.dispSize.x + 1) >> 1;
			if (imgSize & 3)
			{
				imgSize += 4 - (imgSize & 3);
			}
			maskSize = (img->info.dispSize.x + 7) >> 3;
			if (maskSize & 3)
			{
				maskSize += 4 - (maskSize & 3);
			}
			retSize += 16 + 40 + 64 + (imgSize + maskSize) * img->info.dispSize.y;
		}
		else if (img->info.pf == Media::PF_PAL_8_A1)
		{
			imgSize = img->info.dispSize.x;
			if (imgSize & 3)
			{
				imgSize += 4 - (imgSize & 3);
			}
			maskSize = (img->info.dispSize.x + 7) >> 3;
			if (maskSize & 3)
			{
				maskSize += 4 - (maskSize & 3);
			}
			retSize += 16 + 40 + 1024 + (imgSize + maskSize) * img->info.dispSize.y;
		}
		else
		{
			return 0;
		}
		i++;
	}
	return retSize;
}

OSInt Exporter::ANIExporter::BuildBuff(UInt8 *buff, NN<Media::ImageList> imgList, Bool hasHotSpot)
{
	UInt8 *indexPtr;
	UInt8 *imgPtr;
	OSInt i;
	OSInt j;
	OSInt k;
	OSInt l;
	OSInt shiftCnt;
	UInt8 mask;
	UInt8 *srcPtr;
	UInt8 *currPtr;
	OSInt sbpl;
	UInt8 *maskPtr;
	UInt32 imgDelay;
	OSInt imgSize;
	OSInt maskSize;
	OSInt imgAdd;
	OSInt maskAdd;
	NN<Media::StaticImage> img;
	OSInt retSize = 6;
	i = 0;
	j = imgList->GetCount();
	WriteInt16(&buff[0], 0);
	if (hasHotSpot)
	{
		WriteInt16(&buff[2], 2);
	}
	else
	{
		WriteInt16(&buff[2], 1);
	}
	WriteInt16(&buff[4], j);
	indexPtr = buff;
	imgPtr = indexPtr + j * 16;
	retSize += j * 16;

	while (i < j)
	{
		imgList->ToStaticImage(i);
		if (!Optional<Media::StaticImage>::ConvertFrom(imgList->GetImage(i, imgDelay)).SetTo(img))
			return 0;
		if (img->info.fourcc != 0)
		{
			return 0;
		}
		if (img->info.dispSize.x <= 0 || img->info.dispSize.x > 256 || img->info.dispSize.y <= 0 || img->info.dispSize.y > 256)
		{
			return 0;
		}

		if (img->info.dispSize.x >= 256)
		{
			indexPtr[0] = 0;
		}
		else
		{
			indexPtr[0] = (UInt8)img->info.dispSize.x;
		}
		if (img->info.dispSize.y >= 256)
		{
			indexPtr[1] = 0;
		}
		else
		{
			indexPtr[1] = (UInt8)img->info.dispSize.y;
		}
		indexPtr[3] = 0;
		if (hasHotSpot)
		{
			if (img->HasHotSpot())
			{
				WriteInt16(&indexPtr[4], img->GetHotSpotX());
				WriteInt16(&indexPtr[6], img->GetHotSpotY());
			}
			else
			{
				WriteInt16(&indexPtr[4], img->info.dispSize.x >> 1);
				WriteInt16(&indexPtr[6], img->info.dispSize.y >> 1);
			}
		}
		else
		{
			WriteInt16(&indexPtr[4], 0);
		}
		WriteInt32(&indexPtr[12], (Int32)retSize);

		if (img->info.pf == Media::PF_B8G8R8A8)
		{
			imgSize = img->info.dispSize.x * 4;

			indexPtr[2] = 0;
			if (!hasHotSpot)
			{
				WriteInt16(&indexPtr[6], 32);
			}
			WriteInt32(&indexPtr[8], (Int32)(40 + imgSize * img->info.dispSize.y));

			WriteInt32(&imgPtr[0], 40);
			WriteInt32(&imgPtr[4], (Int32)img->info.dispSize.x);
			WriteInt32(&imgPtr[8], (Int32)img->info.dispSize.y * 2);
			WriteInt16(&imgPtr[12], 1);
			WriteInt16(&imgPtr[14], 32);
			WriteInt32(&imgPtr[16], 0); //compression
			WriteInt32(&imgPtr[20], (Int32)(imgSize * img->info.dispSize.y));
			WriteInt32(&imgPtr[24], 0);
			WriteInt32(&imgPtr[28], 0);
			WriteInt32(&imgPtr[32], 0);
			WriteInt32(&imgPtr[36], 0);
			imgPtr += 40;
			retSize += 40;

			srcPtr = img->data + (img->info.storeSize.x * 4 * img->info.dispSize.y);
			l = img->info.dispSize.y;
			while (l-- > 0)
			{
				srcPtr -= img->info.storeSize.x * 4;
				MemCopyNO(imgPtr, srcPtr, img->info.dispSize.x * 4);
				imgPtr += img->info.dispSize.x * 4;
			}
			retSize += imgSize * img->info.dispSize.y;
		}
		else if (img->info.pf == Media::PF_B8G8R8A1)
		{
			imgSize = img->info.dispSize.x * 3;
			imgAdd = 0;
			if (imgSize & 3)
			{
				imgSize += 4 - (imgSize & 3);
				imgAdd = 4 - (imgSize & 3);
			}
			maskSize = (img->info.dispSize.x + 7) >> 3;
			maskAdd = 0;
			if (maskSize & 3)
			{
				maskSize += 4 - (maskSize & 3);
				maskAdd = 4 - (maskSize & 3);
			}

			indexPtr[2] = 0;
			if (!hasHotSpot)
			{
				WriteInt16(&indexPtr[6], 24);
			}
			WriteInt32(&indexPtr[8], (Int32)(40 + (imgSize + maskSize) * img->info.dispSize.y));

			WriteInt32(&imgPtr[0], 40);
			WriteInt32(&imgPtr[4], (Int32)img->info.dispSize.x);
			WriteInt32(&imgPtr[8], (Int32)img->info.dispSize.y * 2);
			WriteInt16(&imgPtr[12], 1);
			WriteInt16(&imgPtr[14], 24);
			WriteInt32(&imgPtr[16], 0); //compression
			WriteInt32(&imgPtr[20], (Int32)(imgSize * img->info.dispSize.y));
			WriteInt32(&imgPtr[24], 0);
			WriteInt32(&imgPtr[28], 0);
			WriteInt32(&imgPtr[32], 0);
			WriteInt32(&imgPtr[36], 0);
			imgPtr += 40;
			retSize += 40;

			srcPtr = img->data + (img->info.storeSize.x * 4 * img->info.dispSize.y);
			maskPtr = imgPtr + imgSize * img->info.dispSize.y;
			l = img->info.dispSize.y;
			while (l-- > 0)
			{
				srcPtr -= img->info.storeSize.x * 4;
				currPtr = srcPtr;
				k = img->info.dispSize.x >> 3;
				while (k-- > 0)
				{
					imgPtr[0] = currPtr[0];
					imgPtr[1] = currPtr[1];
					imgPtr[2] = currPtr[2];

					imgPtr[3] = currPtr[4];
					imgPtr[4] = currPtr[5];
					imgPtr[5] = currPtr[6];

					imgPtr[6] = currPtr[8];
					imgPtr[7] = currPtr[9];
					imgPtr[8] = currPtr[10];

					imgPtr[9] = currPtr[12];
					imgPtr[10] = currPtr[13];
					imgPtr[11] = currPtr[14];

					imgPtr[12] = currPtr[16];
					imgPtr[13] = currPtr[17];
					imgPtr[14] = currPtr[18];

					imgPtr[15] = currPtr[20];
					imgPtr[16] = currPtr[21];
					imgPtr[17] = currPtr[22];

					imgPtr[18] = currPtr[24];
					imgPtr[19] = currPtr[25];
					imgPtr[20] = currPtr[26];

					imgPtr[21] = currPtr[28];
					imgPtr[22] = currPtr[29];
					imgPtr[23] = currPtr[30];

					imgPtr += 24;
					mask  = (UInt8)((currPtr[3] & 1) << 7);
					mask |= (UInt8)((currPtr[7] & 1) << 6);
					mask |= (UInt8)((currPtr[11] & 1) << 5);
					mask |= (UInt8)((currPtr[15] & 1) << 4);
					mask |= (UInt8)((currPtr[19] & 1) << 3);
					mask |= (UInt8)((currPtr[23] & 1) << 2);
					mask |= (UInt8)((currPtr[27] & 1) << 1);
					mask |= (UInt8)((currPtr[31] & 1) << 0);
					maskPtr[0] = ~mask;
					maskPtr++;
					currPtr += 32;
				}
				k = img->info.dispSize.x & 7;
				if (k != 0)
				{
					mask = 0;
					shiftCnt = 7;
					while (k-- > 0)
					{
						imgPtr[0] = currPtr[0];
						imgPtr[1] = currPtr[1];
						imgPtr[2] = currPtr[2];
						imgPtr += 3;

						mask |= (UInt8)(((currPtr[3] & 1) ^ 1) << shiftCnt);
						shiftCnt--;
						currPtr += 4;
					}
					maskPtr[0] = mask;
					maskPtr++;
				}
				k = imgAdd;
				while (k-- > 0)
				{
					imgPtr[0] = 0;
					imgPtr++;
				}
				k = maskAdd;
				while (k-- > 0)
				{
					maskPtr[0] = 0;
					maskPtr++;
				}
			}
			retSize += (imgSize + maskSize) * img->info.dispSize.y;
			imgPtr = maskPtr;
		}
		else if (img->info.pf == Media::PF_PAL_1_A1)
		{
			imgSize = (img->info.dispSize.x + 7) >> 3;
			imgAdd = 0;
			if (imgSize & 3)
			{
				imgSize += 4 - (imgSize & 3);
				imgAdd = 4 - (imgSize & 3);
			}
			maskSize = (img->info.dispSize.x + 7) >> 3;
			maskAdd = 0;
			if (maskSize & 3)
			{
				maskSize += 4 - (maskSize & 3);
				maskAdd = 4 - (maskSize & 3);
			}

			indexPtr[2] = 2;
			if (!hasHotSpot)
			{
				WriteInt16(&indexPtr[6], 1);
			}
			WriteInt32(&indexPtr[8], (Int32)(40 + 8 + (imgSize + maskSize) * img->info.dispSize.y));

			WriteInt32(&imgPtr[0], 40);
			WriteInt32(&imgPtr[4], (Int32)img->info.dispSize.x);
			WriteInt32(&imgPtr[8], (Int32)img->info.dispSize.y * 2);
			WriteInt16(&imgPtr[12], 1);
			WriteInt16(&imgPtr[14], 1);
			WriteInt32(&imgPtr[16], 0); //compression
			WriteInt32(&imgPtr[20], (Int32)(imgSize * img->info.dispSize.y));
			WriteInt32(&imgPtr[24], 0);
			WriteInt32(&imgPtr[28], 0);
			WriteInt32(&imgPtr[32], 0);
			WriteInt32(&imgPtr[36], 0);
			imgPtr += 40;
			retSize += 40;

			MemCopyNO(imgPtr, img->pal, 8);
			imgPtr += 8;
			retSize += 8;

			sbpl = ((img->info.dispSize.x + 7) >> 3) * 2;
			srcPtr = img->data + sbpl * img->info.dispSize.y;
			maskPtr = imgPtr + imgSize * img->info.dispSize.y;
			l = img->info.dispSize.y;
			while (l-- > 0)
			{
				srcPtr -= sbpl;
				currPtr = srcPtr;
				MemCopyNO(imgPtr, currPtr, (img->info.dispSize.x + 7) >> 3);
				currPtr += (img->info.dispSize.x + 7) >> 3;
				k = (img->info.dispSize.x + 7) >> 3;
				while (k-- > 0)
				{
					maskPtr[0] = ~currPtr[0];
					maskPtr++;
					currPtr++;
				}
				k = imgAdd;
				while (k-- > 0)
				{
					imgPtr[0] = 0;
					imgPtr++;
				}
				k = maskAdd;
				while (k-- > 0)
				{
					maskPtr[0] = 0;
					maskPtr++;
				}
			}
			retSize += (imgSize + maskSize) * img->info.dispSize.y;
			imgPtr = maskPtr;
		}
		else if (img->info.pf == Media::PF_PAL_4_A1)
		{
			imgSize = (img->info.dispSize.x + 1) >> 1;
			imgAdd = 0;
			if (imgSize & 3)
			{
				imgSize += 4 - (imgSize & 3);
				imgAdd = 4 - (imgSize & 3);
			}
			maskSize = (img->info.dispSize.x + 7) >> 3;
			maskAdd = 0;
			if (maskSize & 3)
			{
				maskSize += 4 - (maskSize & 3);
				maskAdd = 4 - (maskSize & 3);
			}

			indexPtr[2] = 16;
			if (!hasHotSpot)
			{
				WriteInt16(&indexPtr[6], 4);
			}
			WriteInt32(&indexPtr[8], (Int32)(40 + (imgSize + maskSize) * img->info.dispSize.y));

			WriteInt32(&imgPtr[0], 40);
			WriteInt32(&imgPtr[4], (Int32)img->info.dispSize.x);
			WriteInt32(&imgPtr[8], (Int32)img->info.dispSize.y * 2);
			WriteInt16(&imgPtr[12], 1);
			WriteInt16(&imgPtr[14], 4);
			WriteInt32(&imgPtr[16], 0); //compression
			WriteInt32(&imgPtr[20], (Int32)(imgSize * img->info.dispSize.y));
			WriteInt32(&imgPtr[24], 0);
			WriteInt32(&imgPtr[28], 0);
			WriteInt32(&imgPtr[32], 0);
			WriteInt32(&imgPtr[36], 0);
			imgPtr += 40;
			retSize += 40;

			MemCopyNO(imgPtr, img->pal, 64);
			imgPtr += 64;
			retSize += 64;

			sbpl = ((img->info.dispSize.x + 7) >> 3) + ((img->info.dispSize.x + 1) >> 1);
			srcPtr = img->data + sbpl * img->info.dispSize.y;
			maskPtr = imgPtr + imgSize * img->info.dispSize.y;
			l = img->info.dispSize.y;
			while (l-- > 0)
			{
				srcPtr -= sbpl;
				currPtr = srcPtr;
				MemCopyNO(imgPtr, currPtr, (img->info.dispSize.x + 1) >> 1);
				currPtr += (img->info.dispSize.x + 7) >> 3;
				k = (img->info.dispSize.x + 7) >> 3;
				while (k-- > 0)
				{
					maskPtr[0] = ~currPtr[0];
					maskPtr++;
					currPtr++;
				}
				k = imgAdd;
				while (k-- > 0)
				{
					imgPtr[0] = 0;
					imgPtr++;
				}
				k = maskAdd;
				while (k-- > 0)
				{
					maskPtr[0] = 0;
					maskPtr++;
				}
			}
			retSize += (imgSize + maskSize) * img->info.dispSize.y;
			imgPtr = maskPtr;
		}
		else if (img->info.pf == Media::PF_PAL_8_A1)
		{
			imgSize = img->info.dispSize.x;
			imgAdd = 0;
			if (imgSize & 3)
			{
				imgSize += 4 - (imgSize & 3);
				imgAdd = 4 - (imgSize & 3);
			}
			maskSize = (img->info.dispSize.x + 7) >> 3;
			maskAdd = 0;
			if (maskSize & 3)
			{
				maskSize += 4 - (maskSize & 3);
				maskAdd = 4 - (maskSize & 3);
			}

			indexPtr[2] = 0;
			if (!hasHotSpot)
			{
				WriteInt16(&indexPtr[6], 8);
			}
			WriteInt32(&indexPtr[8], (Int32)(40 + (imgSize + maskSize) * img->info.dispSize.y));

			WriteInt32(&imgPtr[0], 40);
			WriteInt32(&imgPtr[4], (Int32)img->info.dispSize.x);
			WriteInt32(&imgPtr[8], (Int32)img->info.dispSize.y * 2);
			WriteInt16(&imgPtr[12], 1);
			WriteInt16(&imgPtr[14], 8);
			WriteInt32(&imgPtr[16], 0); //compression
			WriteInt32(&imgPtr[20], (Int32)(imgSize * img->info.dispSize.y));
			WriteInt32(&imgPtr[24], 0);
			WriteInt32(&imgPtr[28], 0);
			WriteInt32(&imgPtr[32], 0);
			WriteInt32(&imgPtr[36], 0);
			imgPtr += 40;
			retSize += 40;

			MemCopyNO(imgPtr, img->pal, 1024);
			imgPtr += 1024;
			retSize += 1024;

			sbpl = ((img->info.dispSize.x + 7) >> 3) + img->info.dispSize.x;
			srcPtr = img->data + sbpl * img->info.dispSize.y;
			maskPtr = imgPtr + imgSize * img->info.dispSize.y;
			l = img->info.dispSize.y;
			while (l-- > 0)
			{
				srcPtr -= sbpl;
				currPtr = srcPtr;
				MemCopyNO(imgPtr, currPtr, img->info.dispSize.x);
				currPtr += (img->info.dispSize.x + 7) >> 3;
				k = (img->info.dispSize.x + 7) >> 3;
				while (k-- > 0)
				{
					maskPtr[0] = ~currPtr[0];
					maskPtr++;
					currPtr++;
				}
				k = imgAdd;
				while (k-- > 0)
				{
					imgPtr[0] = 0;
					imgPtr++;
				}
				k = maskAdd;
				while (k-- > 0)
				{
					maskPtr[0] = 0;
					maskPtr++;
				}
			}
			retSize += (imgSize + maskSize) * img->info.dispSize.y;
			imgPtr = maskPtr;
		}
		else
		{
			return 0;
		}
		indexPtr += 16;
		i++;
	}
	return retSize;
}

Exporter::ANIExporter::ANIExporter()
{
}

Exporter::ANIExporter::~ANIExporter()
{
}

Int32 Exporter::ANIExporter::GetName()
{
	return *(Int32*)"ANIE";
}

IO::FileExporter::SupportType Exporter::ANIExporter::IsObjectSupported(NN<IO::ParsedObject> pobj)
{
	if (pobj->GetParserType() != IO::ParserType::ImageList)
		return IO::FileExporter::SupportType::NotSupported;
	NN<Media::ImageList> imgList = NN<Media::ImageList>::ConvertFrom(pobj);
	UInt32 imgTime;
	NN<Media::RasterImage> img;
	UOSInt i = imgList->GetCount();
	if (i <= 0)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	
	while (i-- > 0)
	{
		if (!imgList->GetImage(0, imgTime).SetTo(img) || !img->HasHotSpot() || !ImageSupported(img))
		{
			return IO::FileExporter::SupportType::NotSupported;
		}
	}
	return IO::FileExporter::SupportType::NormalStream;
}

Bool Exporter::ANIExporter::GetOutputName(UOSInt index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("Cursor File"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.cur"));
		return true;
	}
	return false;
}

void Exporter::ANIExporter::SetCodePage(UInt32 codePage)
{
}

Bool Exporter::ANIExporter::ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param)
{
	if (pobj->GetParserType() != IO::ParserType::ImageList)
		return 0;
	NN<Media::ImageList> imgList = NN<Media::ImageList>::ConvertFrom(pobj);
	OSInt buffSize = CalcBuffSize(imgList);
	if (buffSize == 0)
		return false;
	UInt8 *buff = MemAlloc(UInt8, buffSize);
	if (buffSize != BuildBuff(buff, imgList, true))
	{
		MemFree(buff);
		return false;
	}
	stm->Write(buff, buffSize);
	MemFree(buff);
	return true;
}
