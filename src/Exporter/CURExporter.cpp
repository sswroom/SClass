#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Exporter/CURExporter.h"
#include "IO/MemoryStream.h"
#include "Math/Math.h"
#include "Media/ImageList.h"
#include "Media/StaticImage.h"
#include "Text/MyString.h"

Bool Exporter::CURExporter::ImageSupported(Media::Image *img)
{
	if (img->info->dispWidth <= 0 || img->info->dispWidth > 256 || img->info->dispHeight <= 0 || img->info->dispHeight > 256)
	{
		return false;
	}
	if (img->info->fourcc != 0)
	{
		return false;
	}
	if (img->info->pf == Media::PF_B8G8R8A8)
	{
		return true;
	}
	else if (img->info->pf == Media::PF_B8G8R8A1)
	{
		return true;
	}
	else if (img->info->pf == Media::PF_PAL_1_A1)
	{
		return true;
	}
	else if (img->info->pf == Media::PF_PAL_4_A1)
	{
		return true;
	}
	else if (img->info->pf == Media::PF_PAL_8_A1)
	{
		return true;
	}
	return false;
}

OSInt Exporter::CURExporter::CalcBuffSize(Media::ImageList *imgList)
{
	OSInt i;
	OSInt j;
	Int32 imgDelay;
	OSInt imgSize;
	OSInt maskSize;
	Media::Image *img;
	OSInt retSize = 6;
	i = 0;
	j = imgList->GetCount();
	while (i < j)
	{
		img = imgList->GetImage(i, &imgDelay);
		if (img->info->fourcc != 0)
		{
			return 0;
		}
		if (img->info->dispWidth <= 0 || img->info->dispWidth > 256 || img->info->dispHeight <= 0 || img->info->dispHeight > 256)
		{
			return 0;
		}
		if (img->info->pf == Media::PF_B8G8R8A8)
		{
			maskSize = (img->info->dispWidth + 7) >> 3;
			if (maskSize & 3)
			{
				maskSize += 4 - (maskSize & 3);
			}
			imgSize = img->info->dispWidth * 4;
			retSize += 16 + 40 + (imgSize + maskSize) * img->info->dispHeight;
		}
		else if (img->info->pf == Media::PF_B8G8R8A1)
		{
			imgSize = img->info->dispWidth * 3;
			if (imgSize & 3)
			{
				imgSize += 4 - (imgSize & 3);
			}
			maskSize = (img->info->dispWidth + 7) >> 3;
			if (maskSize & 3)
			{
				maskSize += 4 - (maskSize & 3);
			}
			retSize += 16 + 40 + (imgSize + maskSize) * img->info->dispHeight;
		}
		else if (img->info->pf == Media::PF_PAL_1_A1)
		{
			imgSize = (img->info->dispWidth + 7) >> 3;
			if (imgSize & 3)
			{
				imgSize += 4 - (imgSize & 3);
			}
			maskSize = (img->info->dispWidth + 7) >> 3;
			if (maskSize & 3)
			{
				maskSize += 4 - (maskSize & 3);
			}
			retSize += 16 + 40 + 8 + (imgSize + maskSize) * img->info->dispHeight;
		}
		else if (img->info->pf == Media::PF_PAL_4_A1)
		{
			imgSize = (img->info->dispWidth + 1) >> 1;
			if (imgSize & 3)
			{
				imgSize += 4 - (imgSize & 3);
			}
			maskSize = (img->info->dispWidth + 7) >> 3;
			if (maskSize & 3)
			{
				maskSize += 4 - (maskSize & 3);
			}
			retSize += 16 + 40 + 64 + (imgSize + maskSize) * img->info->dispHeight;
		}
		else if (img->info->pf == Media::PF_PAL_8_A1)
		{
			imgSize = img->info->dispWidth;
			if (imgSize & 3)
			{
				imgSize += 4 - (imgSize & 3);
			}
			maskSize = (img->info->dispWidth + 7) >> 3;
			if (maskSize & 3)
			{
				maskSize += 4 - (maskSize & 3);
			}
			retSize += 16 + 40 + 1024 + (imgSize + maskSize) * img->info->dispHeight;
		}
		else
		{
			return 0;
		}
		i++;
	}
	return retSize;
}

OSInt Exporter::CURExporter::BuildBuff(UInt8 *buff, Media::ImageList *imgList, Bool hasHotSpot)
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
	Int32 imgDelay;
	OSInt imgSize;
	OSInt maskSize;
	OSInt imgAdd;
	OSInt maskAdd;
	Media::StaticImage *img;
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
	indexPtr = &buff[6];
	imgPtr = indexPtr + j * 16;
	retSize += j * 16;

	while (i < j)
	{
		imgList->ToStaticImage(i);
		img = (Media::StaticImage*)imgList->GetImage(i, &imgDelay);
		if (img->info->fourcc != 0)
		{
			return 0;
		}
		if (img->info->dispWidth <= 0 || img->info->dispWidth > 256 || img->info->dispHeight <= 0 || img->info->dispHeight > 256)
		{
			return 0;
		}

		if (img->info->dispWidth >= 256)
		{
			indexPtr[0] = 0;
		}
		else
		{
			indexPtr[0] = (UInt8)img->info->dispWidth;
		}
		if (img->info->dispHeight >= 256)
		{
			indexPtr[1] = 0;
		}
		else
		{
			indexPtr[1] = (UInt8)img->info->dispHeight;
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
				WriteInt16(&indexPtr[4], img->info->dispWidth >> 1);
				WriteInt16(&indexPtr[6], img->info->dispHeight >> 1);
			}
		}
		else
		{
			WriteInt16(&indexPtr[4], 0);
		}
		WriteInt32(&indexPtr[12], (Int32)retSize);

		if (img->info->pf == Media::PF_B8G8R8A8)
		{
			imgSize = img->info->dispWidth * 4;
			maskSize = (img->info->dispWidth + 7) >> 3;
			maskAdd = 0;
			if (maskSize & 3)
			{
				maskAdd = 4 - (maskSize & 3);
				maskSize += 4 - (maskSize & 3);
			}

			indexPtr[2] = 0;
			if (!hasHotSpot)
			{
				WriteInt16(&indexPtr[6], 32);
			}
			WriteInt32(&indexPtr[8], (Int32)(40 + (imgSize + maskSize) * img->info->dispHeight));

			WriteInt32(&imgPtr[0], 40);
			WriteInt32(&imgPtr[4], (Int32)img->info->dispWidth);
			WriteInt32(&imgPtr[8], (Int32)img->info->dispHeight * 2);
			WriteInt16(&imgPtr[12], 1);
			WriteInt16(&imgPtr[14], 32);
			WriteInt32(&imgPtr[16], 0); //compression
			WriteInt32(&imgPtr[20], (Int32)(imgSize * img->info->dispHeight));
			WriteInt32(&imgPtr[24], 0);
			WriteInt32(&imgPtr[28], 0);
			WriteInt32(&imgPtr[32], 0);
			WriteInt32(&imgPtr[36], 0);
			imgPtr += 40;
			retSize += 40;

			srcPtr = img->data + (img->info->storeWidth * 4 * img->info->dispHeight);
			maskPtr = imgPtr + (img->info->storeWidth * 4) * img->info->dispHeight;
			l = img->info->dispHeight;
			while (l-- > 0)
			{
				srcPtr -= img->info->storeWidth * 4;
				MemCopyNO(imgPtr, srcPtr, img->info->dispWidth * 4);
				imgPtr += img->info->dispWidth * 4;

				currPtr = srcPtr;
				k = img->info->dispWidth >> 3;
				while (k-- > 0)
				{
					mask  = (UInt8)((currPtr[3] != 0) << 7);
					mask |= (UInt8)((currPtr[7] != 0) << 6);
					mask |= (UInt8)((currPtr[11] != 0) << 5);
					mask |= (UInt8)((currPtr[15] != 0) << 4);
					mask |= (UInt8)((currPtr[19] != 0) << 3);
					mask |= (UInt8)((currPtr[23] != 0) << 2);
					mask |= (UInt8)((currPtr[27] != 0) << 1);
					mask |= (UInt8)((currPtr[31] != 0) << 0);
					maskPtr[0] = ~mask;
					maskPtr++;
					currPtr += 32;
				}
				k = img->info->dispWidth & 7;
				if (k != 0)
				{
					mask = 0;
					shiftCnt = 7;
					while (k-- > 0)
					{
						mask |= (UInt8)(((currPtr[3] != 0) ^ 1) << shiftCnt);
						shiftCnt--;
						currPtr += 4;
					}
					maskPtr[0] = mask;
					maskPtr++;
				}
				k = maskAdd;
				while (k-- > 0)
				{
					maskPtr[0] = 0;
					maskPtr++;
				}
			}
			imgPtr = maskPtr;
			retSize += (imgSize + maskSize) * img->info->dispHeight;
		}
		else if (img->info->pf == Media::PF_B8G8R8A1)
		{
			imgSize = img->info->dispWidth * 3;
			imgAdd = 0;
			if (imgSize & 3)
			{
				imgAdd = 4 - (imgSize & 3);
				imgSize += 4 - (imgSize & 3);
			}
			maskSize = (img->info->dispWidth + 7) >> 3;
			maskAdd = 0;
			if (maskSize & 3)
			{
				maskAdd = 4 - (maskSize & 3);
				maskSize += 4 - (maskSize & 3);
			}

			indexPtr[2] = 0;
			if (!hasHotSpot)
			{
				WriteInt16(&indexPtr[6], 24);
			}
			WriteInt32(&indexPtr[8], (Int32)(40 + (imgSize + maskSize) * img->info->dispHeight));

			WriteInt32(&imgPtr[0], 40);
			WriteInt32(&imgPtr[4], (Int32)img->info->dispWidth);
			WriteInt32(&imgPtr[8], (Int32)img->info->dispHeight * 2);
			WriteInt16(&imgPtr[12], 1);
			WriteInt16(&imgPtr[14], 24);
			WriteInt32(&imgPtr[16], 0); //compression
			WriteInt32(&imgPtr[20], (Int32)(imgSize * img->info->dispHeight));
			WriteInt32(&imgPtr[24], 0);
			WriteInt32(&imgPtr[28], 0);
			WriteInt32(&imgPtr[32], 0);
			WriteInt32(&imgPtr[36], 0);
			imgPtr += 40;
			retSize += 40;

			srcPtr = img->data + (img->info->storeWidth * 4 * img->info->dispHeight);
			maskPtr = imgPtr + imgSize * img->info->dispHeight;
			l = img->info->dispHeight;
			while (l-- > 0)
			{
				srcPtr -= img->info->storeWidth * 4;
				currPtr = srcPtr;
				k = img->info->dispWidth >> 3;
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
				k = img->info->dispWidth & 7;
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
			retSize += (imgSize + maskSize) * img->info->dispHeight;
			imgPtr = maskPtr;
		}
		else if (img->info->pf == Media::PF_PAL_1_A1)
		{
			imgSize = (img->info->dispWidth + 7) >> 3;
			imgAdd = 0;
			if (imgSize & 3)
			{
				imgAdd = 4 - (imgSize & 3);
				imgSize += 4 - (imgSize & 3);
			}
			maskSize = (img->info->dispWidth + 7) >> 3;
			maskAdd = 0;
			if (maskSize & 3)
			{
				maskAdd = 4 - (maskSize & 3);
				maskSize += 4 - (maskSize & 3);
			}

			indexPtr[2] = 2;
			if (!hasHotSpot)
			{
				WriteInt16(&indexPtr[6], 1);
			}
			WriteInt32(&indexPtr[8], (Int32)(40 + 8 + (imgSize + maskSize) * img->info->dispHeight));

			WriteInt32(&imgPtr[0], 40);
			WriteInt32(&imgPtr[4], (Int32)img->info->dispWidth);
			WriteInt32(&imgPtr[8], (Int32)img->info->dispHeight * 2);
			WriteInt16(&imgPtr[12], 1);
			WriteInt16(&imgPtr[14], 1);
			WriteInt32(&imgPtr[16], 0); //compression
			WriteInt32(&imgPtr[20], (Int32)(imgSize * img->info->dispHeight));
			WriteInt32(&imgPtr[24], 0);
			WriteInt32(&imgPtr[28], 0);
			WriteInt32(&imgPtr[32], 2);
			WriteInt32(&imgPtr[36], 2);
			imgPtr += 40;
			retSize += 40;

			MemCopyNO(imgPtr, img->pal, 8);
			imgPtr += 8;
			retSize += 8;

			sbpl = ((img->info->dispWidth + 7) >> 3) * 2;
			srcPtr = img->data + sbpl * img->info->dispHeight;
			maskPtr = imgPtr + imgSize * img->info->dispHeight;
			l = img->info->dispHeight;
			while (l-- > 0)
			{
				srcPtr -= sbpl;
				currPtr = srcPtr;
				MemCopyNO(imgPtr, currPtr, (img->info->dispWidth + 7) >> 3);
				currPtr += (img->info->dispWidth + 7) >> 3;
				imgPtr += (img->info->dispWidth + 7) >> 3;
				k = (img->info->dispWidth + 7) >> 3;
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
			retSize += (imgSize + maskSize) * img->info->dispHeight;
			imgPtr = maskPtr;
		}
		else if (img->info->pf == Media::PF_PAL_4_A1)
		{
			imgSize = (img->info->dispWidth + 1) >> 1;
			imgAdd = 0;
			if (imgSize & 3)
			{
				imgAdd = 4 - (imgSize & 3);
				imgSize += 4 - (imgSize & 3);
			}
			maskSize = (img->info->dispWidth + 7) >> 3;
			maskAdd = 0;
			if (maskSize & 3)
			{
				maskAdd = 4 - (maskSize & 3);
				maskSize += 4 - (maskSize & 3);
			}

			indexPtr[2] = 16;
			if (!hasHotSpot)
			{
				WriteInt16(&indexPtr[6], 4);
			}
			WriteInt32(&indexPtr[8], (Int32)(40 + 64 + (imgSize + maskSize) * img->info->dispHeight));

			WriteInt32(&imgPtr[0], 40);
			WriteInt32(&imgPtr[4], (Int32)img->info->dispWidth);
			WriteInt32(&imgPtr[8], (Int32)img->info->dispHeight * 2);
			WriteInt16(&imgPtr[12], 1);
			WriteInt16(&imgPtr[14], 4);
			WriteInt32(&imgPtr[16], 0); //compression
			WriteInt32(&imgPtr[20], (Int32)(imgSize * img->info->dispHeight));
			WriteInt32(&imgPtr[24], 0);
			WriteInt32(&imgPtr[28], 0);
			WriteInt32(&imgPtr[32], 16);
			WriteInt32(&imgPtr[36], 16);
			imgPtr += 40;
			retSize += 40;

			MemCopyNO(imgPtr, img->pal, 64);
			imgPtr += 64;
			retSize += 64;

			sbpl = ((img->info->dispWidth + 7) >> 3) + ((img->info->dispWidth + 1) >> 1);
			srcPtr = img->data + sbpl * img->info->dispHeight;
			maskPtr = imgPtr + imgSize * img->info->dispHeight;
			l = img->info->dispHeight;
			while (l-- > 0)
			{
				srcPtr -= sbpl;
				currPtr = srcPtr;
				MemCopyNO(imgPtr, currPtr, (img->info->dispWidth + 1) >> 1);
				currPtr += (img->info->dispWidth + 1) >> 1;
				imgPtr += (img->info->dispWidth + 1) >> 1;
				k = (img->info->dispWidth + 7) >> 3;
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
			retSize += (imgSize + maskSize) * img->info->dispHeight;
			imgPtr = maskPtr;
		}
		else if (img->info->pf == Media::PF_PAL_8_A1)
		{
			imgSize = img->info->dispWidth;
			imgAdd = 0;
			if (imgSize & 3)
			{
				imgAdd = 4 - (imgSize & 3);
				imgSize += 4 - (imgSize & 3);
			}
			maskSize = (img->info->dispWidth + 7) >> 3;
			maskAdd = 0;
			if (maskSize & 3)
			{
				maskAdd = 4 - (maskSize & 3);
				maskSize += 4 - (maskSize & 3);
			}

			indexPtr[2] = 0;
			if (!hasHotSpot)
			{
				WriteInt16(&indexPtr[6], 8);
			}
			WriteInt32(&indexPtr[8], (Int32)(40 + 1024 + (imgSize + maskSize) * img->info->dispHeight));

			WriteInt32(&imgPtr[0], 40);
			WriteInt32(&imgPtr[4], (Int32)img->info->dispWidth);
			WriteInt32(&imgPtr[8], (Int32)img->info->dispHeight * 2);
			WriteInt16(&imgPtr[12], 1);
			WriteInt16(&imgPtr[14], 8);
			WriteInt32(&imgPtr[16], 0); //compression
			WriteInt32(&imgPtr[20], (Int32)(imgSize * img->info->dispHeight));
			WriteInt32(&imgPtr[24], 0);
			WriteInt32(&imgPtr[28], 0);
			WriteInt32(&imgPtr[32], 256);
			WriteInt32(&imgPtr[36], 256);
			imgPtr += 40;
			retSize += 40;

			MemCopyNO(imgPtr, img->pal, 1024);
			imgPtr += 1024;
			retSize += 1024;

			sbpl = ((img->info->dispWidth + 7) >> 3) + img->info->dispWidth;
			srcPtr = img->data + sbpl * img->info->dispHeight;
			maskPtr = imgPtr + imgSize * img->info->dispHeight;
			l = img->info->dispHeight;
			while (l-- > 0)
			{
				srcPtr -= sbpl;
				currPtr = srcPtr;
				MemCopyNO(imgPtr, currPtr, img->info->dispWidth);
				currPtr += img->info->dispWidth;
				imgPtr += img->info->dispWidth;
				k = (img->info->dispWidth + 7) >> 3;
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
			retSize += (imgSize + maskSize) * img->info->dispHeight;
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

Exporter::CURExporter::CURExporter()
{
}

Exporter::CURExporter::~CURExporter()
{
}

Int32 Exporter::CURExporter::GetName()
{
	return *(Int32*)"CURE";
}

IO::FileExporter::SupportType Exporter::CURExporter::IsObjectSupported(IO::ParsedObject *pobj)
{
	if (pobj->GetParserType() != IO::ParsedObject::PT_IMAGE_LIST_PARSER)
		return IO::FileExporter::ST_NOT_SUPPORTED;
	Media::ImageList *imgList = (Media::ImageList*)pobj;
	Int32 imgTime;
	Media::Image *img;
	OSInt i = imgList->GetCount();
	if (i <= 0)
	{
		return IO::FileExporter::ST_NOT_SUPPORTED;
	}
	
	while (i-- > 0)
	{
		img = imgList->GetImage(0, &imgTime);
		if (!img->HasHotSpot() || !ImageSupported(img))
		{
			return IO::FileExporter::ST_NOT_SUPPORTED;
		}
	}
	return IO::FileExporter::ST_NORMAL_STREAM;
}

Bool Exporter::CURExporter::GetOutputName(OSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcat(nameBuff, (const UTF8Char*)"Cursor File");
		Text::StrConcat(fileNameBuff, (const UTF8Char*)"*.cur");
		return true;
	}
	return false;
}

void Exporter::CURExporter::SetCodePage(Int32 codePage)
{
}

Bool Exporter::CURExporter::ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param)
{
	if (pobj->GetParserType() != IO::ParsedObject::PT_IMAGE_LIST_PARSER)
		return 0;
	Media::ImageList *imgList = (Media::ImageList*)pobj;
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
