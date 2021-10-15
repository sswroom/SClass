#include "Stdafx.h"
#include "Crypto/Hash/CRC32R.h"
#include "Data/ByteTool.h"
#include "Data/Compress/Inflate.h"
#include "Exporter/PNGExporter.h"
#include "Math/Math.h"
#include "Math/Unit/Distance.h"
#include "Media/ImageList.h"
#include "Media/StaticImage.h"
#include "Text/MyString.h"

UOSInt PNGExporter_WritePal(IO::Stream *stm, Media::StaticImage *img, Crypto::Hash::CRC32R *crc)
{
	UInt8 *palPtr = img->pal;
	if (palPtr == 0)
		return 0;
	UOSInt colorCnt = (UOSInt)1 << img->info->storeBPP;
	UInt8 *tmpBuff = MemAlloc(UInt8, colorCnt * 3 + 12);
	UInt8 *tmpPtr;
	UOSInt i;
	WriteMUInt32(&tmpBuff[0], (UInt32)(colorCnt * 3));
	*(Int32*)&tmpBuff[4] = *(Int32*)"PLTE";
	tmpPtr = &tmpBuff[8];
	i = colorCnt;
	while (i-- > 0)
	{
		tmpPtr[0] = palPtr[2];
		tmpPtr[1] = palPtr[1];
		tmpPtr[2] = palPtr[0];
		tmpPtr += 3;
		palPtr += 4;
	}
	crc->Clear();
	crc->Calc(&tmpBuff[4], colorCnt * 3 + 4);
	crc->GetValue(&tmpBuff[colorCnt * 3 + 8]);
	stm->Write(tmpBuff, colorCnt * 3 + 12);
	MemFree(tmpBuff);
	return colorCnt * 3 + 12;
}

UInt8 PNGExporter_PaethPredictor(UInt8 a, UInt8 b, UInt8 c)
{
	Int32 p = a + (Int32)b - (Int32)c;
	Int32 pa = p - a;
	Int32 pb = p - b;
	Int32 pc = p - c;
	if (pa < 0)
		pa = -pa;
	if (pb < 0)
		pb = -pb;
	if (pc < 0)
		pc = -pc;
	if (pa <= pb && pa <= pc)
	{
		return a;
	}
	else if (pb <= pc)
	{
		return b;
	}
	else
	{
		return c;
	}
}

void PNGExporter_FilterByte(UInt8 *imgBuff, UOSInt lineByteCnt, UOSInt height)
{
	UOSInt oriHeight = height;
	UOSInt i;
	UOSInt j;
	UOSInt compSize[5];
	UInt8 lastPx;
	UInt8 thisPx;
	UInt8 *lastLineStart = 0;
	UInt8 *lineStart = imgBuff;
	UInt8 *tmpBuff = MemAlloc(UInt8, lineByteCnt + 1);
	UInt8 *tmpBuff2 = MemAlloc(UInt8, lineByteCnt + 12);
	UInt8 *newBuff = MemAlloc(UInt8, (lineByteCnt + 1) * height);
	UInt8 *outBuff = newBuff;
	tmpBuff[0] = 1;
	lastPx = tmpBuff[1] = lineStart[1];
	i = 1;
	while (i < lineByteCnt)
	{
		thisPx = (UInt8)(lineStart[i + 1] - lastPx);
		lastPx = lineStart[i + 1];
		tmpBuff[i + 1] = thisPx;
		i++;
	}
	compSize[0] = Data::Compress::Inflate::Compress(lineStart, lineByteCnt + 1, tmpBuff2, false);
	compSize[1] = Data::Compress::Inflate::Compress(tmpBuff, lineByteCnt + 1, tmpBuff2, false);
	compSize[2] = 0;
	compSize[3] = 0;
	compSize[4] = 0;
	while (height-- > 0)
	{
		i = compSize[0];
		j = 0;
		if (compSize[1] > 0 && compSize[1] < i)
		{
			i = compSize[1];
			j = 1;
		}
		if (compSize[2] > 0 && compSize[2] < i)
		{
			i = compSize[2];
			j = 2;
		}
		if (compSize[3] > 0 && compSize[3] < i)
		{
			i = compSize[3];
			j = 3;
		}
		if (compSize[4] > 0 && compSize[4] < i)
		{
			i = compSize[4];
			j = 4;
		}
		if (j == 0)
		{
			MemCopyNO(outBuff, lineStart, lineByteCnt + 1);
		}
		else if (j == 1)
		{
			outBuff[0] = 1;
			outBuff[1] = lineStart[1];
			i = 1;
			while (i < lineByteCnt)
			{
				outBuff[i + 1] = (UInt8)(lineStart[i + 1] - lineStart[i]);
				i++;
			}
		}
		else if (j == 2)
		{
			outBuff[0] = 2;
			i = 0;
			while (i < lineByteCnt)
			{
				outBuff[i + 1] = (UInt8)(lineStart[i + 1] - lastLineStart[i + 1]);
				i++;
			}
		}
		else if (j == 3)
		{
			outBuff[0] = 3;
			lastPx = 0;
			i = 0;
			while (i < lineByteCnt)
			{
				outBuff[i + 1] = (UInt8)(lineStart[i + 1] - ((lastLineStart[i + 1] + lastPx) >> 1));
				lastPx = lineStart[i + 1];
				i++;
			}
		}
		else if (j == 4)
		{
			outBuff[0] = 4;
			outBuff[1] = (UInt8)(lineStart[1] - lastLineStart[1]);
			i = 1;
			while (i < lineByteCnt)
			{
				outBuff[i + 1] = (UInt8)(lineStart[i + 1] - PNGExporter_PaethPredictor(lineStart[i], lastLineStart[i + 1], lastLineStart[i]));
				i++;
			}
		}
		lastLineStart = lineStart;
		lineStart = lineStart + lineByteCnt + 1;
		outBuff += lineByteCnt + 1;
		if (height <= 0)
			break;

		compSize[0] = Data::Compress::Inflate::Compress(lineStart, lineByteCnt + 1, tmpBuff2, false);

		tmpBuff[0] = 1;
		tmpBuff[1] = lineStart[1];
		i = 1;
		while (i < lineByteCnt)
		{
			tmpBuff[i + 1] = (UInt8)(lineStart[i + 1] - lineStart[i]);
			i++;
		}
		compSize[1] = Data::Compress::Inflate::Compress(tmpBuff, lineByteCnt + 1, tmpBuff2, false);

		tmpBuff[0] = 2;
		i = 0;
		while (i < lineByteCnt)
		{
			tmpBuff[i + 1] = (UInt8)(lineStart[i + 1] - lastLineStart[i + 1]);
			i++;
		}
		compSize[2] = Data::Compress::Inflate::Compress(tmpBuff, lineByteCnt + 1, tmpBuff2, false);

		tmpBuff[0] = 3;
		lastPx = 0;
		i = 0;
		while (i < lineByteCnt)
		{
			tmpBuff[i + 1] = (UInt8)(lineStart[i + 1] - ((lastLineStart[i + 1] + lastPx) >> 1));
			lastPx = lineStart[i + 1];
			i++;
		}
		compSize[3] = Data::Compress::Inflate::Compress(tmpBuff, lineByteCnt + 1, tmpBuff2, false);

		tmpBuff[0] = 4;
		tmpBuff[1] = (UInt8)(lineStart[1] - lastLineStart[1]);
		i = 1;
		while (i < lineByteCnt)
		{
			tmpBuff[i + 1] = (UInt8)(lineStart[i + 1] - PNGExporter_PaethPredictor(lineStart[i], lastLineStart[i + 1], lastLineStart[i]));
			i++;
		}
		compSize[4] = Data::Compress::Inflate::Compress(tmpBuff, lineByteCnt + 1, tmpBuff2, false);
	}
	MemCopyNO(imgBuff, newBuff, (lineByteCnt + 1) * oriHeight);
	MemFree(newBuff);
	MemFree(tmpBuff);
	MemFree(tmpBuff2);
}

void PNGExporter_FilterByte2(UInt8 *imgBuff, UOSInt lineByteCnt, UOSInt height)
{
	UOSInt oriHeight = height;
	UOSInt i;
	UOSInt j;
	UOSInt compSize[5];
	UInt8 lastPx[2];
	UInt8 thisPx[2];
	UInt8 *lastLineStart = 0;
	UInt8 *lineStart = imgBuff;
	UInt8 *tmpBuff = MemAlloc(UInt8, lineByteCnt + 1);
	UInt8 *tmpBuff2 = MemAlloc(UInt8, lineByteCnt + 12);
	UInt8 *newBuff = MemAlloc(UInt8, (lineByteCnt + 1) * height);
	UInt8 *outBuff = newBuff;
	tmpBuff[0] = 1;
	lastPx[0] = tmpBuff[1] = lineStart[1];
	lastPx[1] = tmpBuff[2] = lineStart[2];
	i = 2;
	while (i < lineByteCnt)
	{
		thisPx[0] = (UInt8)(lineStart[i + 1] - lastPx[0]);
		thisPx[1] = (UInt8)(lineStart[i + 2] - lastPx[1]);
		lastPx[0] = lineStart[i + 1];
		lastPx[1] = lineStart[i + 2];
		tmpBuff[i + 1] = thisPx[0];
		tmpBuff[i + 2] = thisPx[1];
		i += 2;
	}
	compSize[0] = Data::Compress::Inflate::Compress(lineStart, lineByteCnt + 1, tmpBuff2, false);
	compSize[1] = Data::Compress::Inflate::Compress(tmpBuff, lineByteCnt + 1, tmpBuff2, false);
	compSize[2] = 0;
	compSize[3] = 0;
	compSize[4] = 0;
	while (height-- > 0)
	{
		i = compSize[0];
		j = 0;
		if (compSize[1] > 0 && compSize[1] < i)
		{
			i = compSize[1];
			j = 1;
		}
		if (compSize[2] > 0 && compSize[2] < i)
		{
			i = compSize[2];
			j = 2;
		}
		if (compSize[3] > 0 && compSize[3] < i)
		{
			i = compSize[3];
			j = 3;
		}
		if (compSize[4] > 0 && compSize[4] < i)
		{
			i = compSize[4];
			j = 4;
		}
		if (j == 0)
		{
			MemCopyNO(outBuff, lineStart, lineByteCnt + 1);
		}
		else if (j == 1)
		{
			outBuff[0] = 1;
			outBuff[1] = lineStart[1];
			outBuff[2] = lineStart[2];
			i = 2;
			while (i < lineByteCnt)
			{
				outBuff[i + 1] = (UInt8)(lineStart[i + 1] - lineStart[i - 1]);
				outBuff[i + 2] = (UInt8)(lineStart[i + 2] - lineStart[i]);
				i += 2;
			}
		}
		else if (j == 2)
		{
			outBuff[0] = 2;
			i = 0;
			while (i < lineByteCnt)
			{
				outBuff[i + 1] = (UInt8)(lineStart[i + 1] - lastLineStart[i + 1]);
				outBuff[i + 2] = (UInt8)(lineStart[i + 2] - lastLineStart[i + 2]);
				i += 2;
			}
		}
		else if (j == 3)
		{
			outBuff[0] = 3;
			lastPx[0] = 0;
			lastPx[1] = 0;
			i = 0;
			while (i < lineByteCnt)
			{
				outBuff[i + 1] = (UInt8)(lineStart[i + 1] - ((lastLineStart[i + 1] + lastPx[0]) >> 1));
				outBuff[i + 2] = (UInt8)(lineStart[i + 2] - ((lastLineStart[i + 2] + lastPx[1]) >> 1));
				lastPx[0] = lineStart[i + 1];
				lastPx[1] = lineStart[i + 2];
				i += 2;
			}
		}
		else if (j == 4)
		{
			outBuff[0] = 4;
			outBuff[1] = (UInt8)(lineStart[1] - lastLineStart[1]);
			outBuff[2] = (UInt8)(lineStart[2] - lastLineStart[2]);
			i = 2;
			while (i < lineByteCnt)
			{
				outBuff[i + 1] = (UInt8)(lineStart[i + 1] - PNGExporter_PaethPredictor(lineStart[i - 1], lastLineStart[i + 1], lastLineStart[i - 1]));
				outBuff[i + 2] = (UInt8)(lineStart[i + 2] - PNGExporter_PaethPredictor(lineStart[i - 0], lastLineStart[i + 2], lastLineStart[i - 0]));
				i += 2;
			}
		}
		lastLineStart = lineStart;
		lineStart = lineStart + lineByteCnt + 1;
		outBuff += lineByteCnt + 1;
		if (height <= 0)
			break;

		compSize[0] = Data::Compress::Inflate::Compress(lineStart, lineByteCnt + 1, tmpBuff2, false);

		tmpBuff[0] = 1;
		tmpBuff[1] = lineStart[1];
		tmpBuff[2] = lineStart[2];
		i = 2;
		while (i < lineByteCnt)
		{
			tmpBuff[i + 1] = (UInt8)(lineStart[i + 1] - lineStart[i - 1]);
			tmpBuff[i + 2] = (UInt8)(lineStart[i + 2] - lineStart[i - 0]);
			i += 2;
		}
		compSize[1] = Data::Compress::Inflate::Compress(tmpBuff, lineByteCnt + 1, tmpBuff2, false);

		tmpBuff[0] = 2;
		i = 0;
		while (i < lineByteCnt)
		{
			tmpBuff[i + 1] = (UInt8)(lineStart[i + 1] - lastLineStart[i + 1]);
			tmpBuff[i + 2] = (UInt8)(lineStart[i + 2] - lastLineStart[i + 2]);
			i += 2;
		}
		compSize[2] = Data::Compress::Inflate::Compress(tmpBuff, lineByteCnt + 1, tmpBuff2, false);

		tmpBuff[0] = 3;
		lastPx[0] = 0;
		lastPx[1] = 0;
		i = 0;
		while (i < lineByteCnt)
		{
			tmpBuff[i + 1] = (UInt8)(lineStart[i + 1] - ((lastLineStart[i + 1] + lastPx[0]) >> 1));
			tmpBuff[i + 2] = (UInt8)(lineStart[i + 2] - ((lastLineStart[i + 2] + lastPx[1]) >> 1));
			lastPx[0] = lineStart[i + 1];
			lastPx[1] = lineStart[i + 2];
			i += 2;
		}
		compSize[3] = Data::Compress::Inflate::Compress(tmpBuff, lineByteCnt + 1, tmpBuff2, false);

		tmpBuff[0] = 4;
		tmpBuff[1] = (UInt8)(lineStart[1] - lastLineStart[1]);
		tmpBuff[2] = (UInt8)(lineStart[2] - lastLineStart[2]);
		i = 2;
		while (i < lineByteCnt)
		{
			tmpBuff[i + 1] = (UInt8)(lineStart[i + 1] - PNGExporter_PaethPredictor(lineStart[i - 1], lastLineStart[i + 1], lastLineStart[i - 1]));
			tmpBuff[i + 2] = (UInt8)(lineStart[i + 2] - PNGExporter_PaethPredictor(lineStart[i - 0], lastLineStart[i + 2], lastLineStart[i - 0]));
			i += 2;
		}
		compSize[4] = Data::Compress::Inflate::Compress(tmpBuff, lineByteCnt + 1, tmpBuff2, false);
	}
	MemCopyNO(imgBuff, newBuff, (lineByteCnt + 1) * oriHeight);
	MemFree(newBuff);
	MemFree(tmpBuff);
	MemFree(tmpBuff2);
}

void PNGExporter_FilterByte3(UInt8 *imgBuff, UOSInt lineByteCnt, UOSInt height)
{
	UOSInt oriHeight = height;
	UOSInt i;
	UOSInt j;
	UOSInt compSize[5];
	UInt8 lastPx[3];
	UInt8 thisPx[3];
	UInt8 *lastLineStart = 0;
	UInt8 *lineStart = imgBuff;
	UInt8 *tmpBuff = MemAlloc(UInt8, lineByteCnt + 1);
	UInt8 *tmpBuff2 = MemAlloc(UInt8, lineByteCnt + 12);
	UInt8 *newBuff = MemAlloc(UInt8, (lineByteCnt + 1) * height);
	UInt8 *outBuff = newBuff;
	tmpBuff[0] = 1;
	lastPx[0] = tmpBuff[1] = lineStart[1];
	lastPx[1] = tmpBuff[2] = lineStart[2];
	lastPx[2] = tmpBuff[3] = lineStart[3];
	i = 3;
	while (i < lineByteCnt)
	{
		thisPx[0] = (UInt8)(lineStart[i + 1] - lastPx[0]);
		thisPx[1] = (UInt8)(lineStart[i + 2] - lastPx[1]);
		thisPx[2] = (UInt8)(lineStart[i + 3] - lastPx[2]);
		lastPx[0] = lineStart[i + 1];
		lastPx[1] = lineStart[i + 2];
		lastPx[2] = lineStart[i + 3];
		tmpBuff[i + 1] = thisPx[0];
		tmpBuff[i + 2] = thisPx[1];
		tmpBuff[i + 3] = thisPx[2];
		i += 3;
	}
	compSize[0] = Data::Compress::Inflate::Compress(lineStart, lineByteCnt + 1, tmpBuff2, false);
	compSize[1] = Data::Compress::Inflate::Compress(tmpBuff, lineByteCnt + 1, tmpBuff2, false);
	compSize[2] = 0;
	compSize[3] = 0;
	compSize[4] = 0;
	while (height-- > 0)
	{
		i = compSize[0];
		j = 0;
		if (compSize[1] > 0 && compSize[1] < i)
		{
			i = compSize[1];
			j = 1;
		}
		if (compSize[2] > 0 && compSize[2] < i)
		{
			i = compSize[2];
			j = 2;
		}
		if (compSize[3] > 0 && compSize[3] < i)
		{
			i = compSize[3];
			j = 3;
		}
		if (compSize[4] > 0 && compSize[4] < i)
		{
			i = compSize[4];
			j = 4;
		}
		if (j == 0)
		{
			MemCopyNO(outBuff, lineStart, lineByteCnt + 1);
		}
		else if (j == 1)
		{
			outBuff[0] = 1;
			outBuff[1] = lineStart[1];
			outBuff[2] = lineStart[2];
			outBuff[3] = lineStart[3];
			i = 3;
			while (i < lineByteCnt)
			{
				outBuff[i + 1] = (UInt8)(lineStart[i + 1] - lineStart[i - 2]);
				outBuff[i + 2] = (UInt8)(lineStart[i + 2] - lineStart[i - 1]);
				outBuff[i + 3] = (UInt8)(lineStart[i + 3] - lineStart[i]);
				i += 3;
			}
		}
		else if (j == 2)
		{
			outBuff[0] = 2;
			i = 0;
			while (i < lineByteCnt)
			{
				outBuff[i + 1] = (UInt8)(lineStart[i + 1] - lastLineStart[i + 1]);
				outBuff[i + 2] = (UInt8)(lineStart[i + 2] - lastLineStart[i + 2]);
				outBuff[i + 3] = (UInt8)(lineStart[i + 3] - lastLineStart[i + 3]);
				i += 3;
			}
		}
		else if (j == 3)
		{
			outBuff[0] = 3;
			lastPx[0] = 0;
			lastPx[1] = 0;
			lastPx[2] = 0;
			i = 0;
			while (i < lineByteCnt)
			{
				outBuff[i + 1] = (UInt8)(lineStart[i + 1] - ((lastLineStart[i + 1] + lastPx[0]) >> 1));
				outBuff[i + 2] = (UInt8)(lineStart[i + 2] - ((lastLineStart[i + 2] + lastPx[1]) >> 1));
				outBuff[i + 3] = (UInt8)(lineStart[i + 3] - ((lastLineStart[i + 3] + lastPx[2]) >> 1));
				lastPx[0] = lineStart[i + 1];
				lastPx[1] = lineStart[i + 2];
				lastPx[2] = lineStart[i + 3];
				i += 3;
			}
		}
		else if (j == 4)
		{
			outBuff[0] = 4;
			outBuff[1] = (UInt8)(lineStart[1] - lastLineStart[1]);
			outBuff[2] = (UInt8)(lineStart[2] - lastLineStart[2]);
			outBuff[3] = (UInt8)(lineStart[3] - lastLineStart[3]);
			i = 3;
			while (i < lineByteCnt)
			{
				outBuff[i + 1] = (UInt8)(lineStart[i + 1] - PNGExporter_PaethPredictor(lineStart[i - 2], lastLineStart[i + 1], lastLineStart[i - 2]));
				outBuff[i + 2] = (UInt8)(lineStart[i + 2] - PNGExporter_PaethPredictor(lineStart[i - 1], lastLineStart[i + 2], lastLineStart[i - 1]));
				outBuff[i + 3] = (UInt8)(lineStart[i + 3] - PNGExporter_PaethPredictor(lineStart[i - 0], lastLineStart[i + 3], lastLineStart[i - 0]));
				i += 3;
			}
		}
		lastLineStart = lineStart;
		lineStart = lineStart + lineByteCnt + 1;
		outBuff += lineByteCnt + 1;
		if (height <= 0)
			break;

		compSize[0] = Data::Compress::Inflate::Compress(lineStart, lineByteCnt + 1, tmpBuff2, false);

		tmpBuff[0] = 1;
		tmpBuff[1] = lineStart[1];
		tmpBuff[2] = lineStart[2];
		tmpBuff[3] = lineStart[3];
		i = 3;
		while (i < lineByteCnt)
		{
			tmpBuff[i + 1] = (UInt8)(lineStart[i + 1] - lineStart[i - 2]);
			tmpBuff[i + 2] = (UInt8)(lineStart[i + 2] - lineStart[i - 1]);
			tmpBuff[i + 3] = (UInt8)(lineStart[i + 3] - lineStart[i - 0]);
			i += 3;
		}
		compSize[1] = Data::Compress::Inflate::Compress(tmpBuff, lineByteCnt + 1, tmpBuff2, false);

		tmpBuff[0] = 2;
		i = 0;
		while (i < lineByteCnt)
		{
			tmpBuff[i + 1] = (UInt8)(lineStart[i + 1] - lastLineStart[i + 1]);
			tmpBuff[i + 2] = (UInt8)(lineStart[i + 2] - lastLineStart[i + 2]);
			tmpBuff[i + 3] = (UInt8)(lineStart[i + 3] - lastLineStart[i + 3]);
			i += 3;
		}
		compSize[2] = Data::Compress::Inflate::Compress(tmpBuff, lineByteCnt + 1, tmpBuff2, false);

		tmpBuff[0] = 3;
		lastPx[0] = 0;
		lastPx[1] = 0;
		lastPx[2] = 0;
		i = 0;
		while (i < lineByteCnt)
		{
			tmpBuff[i + 1] = (UInt8)(lineStart[i + 1] - ((lastLineStart[i + 1] + lastPx[0]) >> 1));
			tmpBuff[i + 2] = (UInt8)(lineStart[i + 2] - ((lastLineStart[i + 2] + lastPx[1]) >> 1));
			tmpBuff[i + 3] = (UInt8)(lineStart[i + 3] - ((lastLineStart[i + 3] + lastPx[2]) >> 1));
			lastPx[0] = lineStart[i + 1];
			lastPx[1] = lineStart[i + 2];
			lastPx[2] = lineStart[i + 3];
			i += 3;
		}
		compSize[3] = Data::Compress::Inflate::Compress(tmpBuff, lineByteCnt + 1, tmpBuff2, false);

		tmpBuff[0] = 4;
		tmpBuff[1] = (UInt8)(lineStart[1] - lastLineStart[1]);
		tmpBuff[2] = (UInt8)(lineStart[2] - lastLineStart[2]);
		tmpBuff[3] = (UInt8)(lineStart[3] - lastLineStart[3]);
		i = 3;
		while (i < lineByteCnt)
		{
			tmpBuff[i + 1] = (UInt8)(lineStart[i + 1] - PNGExporter_PaethPredictor(lineStart[i - 2], lastLineStart[i + 1], lastLineStart[i - 2]));
			tmpBuff[i + 2] = (UInt8)(lineStart[i + 2] - PNGExporter_PaethPredictor(lineStart[i - 1], lastLineStart[i + 2], lastLineStart[i - 1]));
			tmpBuff[i + 3] = (UInt8)(lineStart[i + 3] - PNGExporter_PaethPredictor(lineStart[i - 0], lastLineStart[i + 3], lastLineStart[i - 0]));
			i += 3;
		}
		compSize[4] = Data::Compress::Inflate::Compress(tmpBuff, lineByteCnt + 1, tmpBuff2, false);
	}
	MemCopyNO(imgBuff, newBuff, (lineByteCnt + 1) * oriHeight);
	MemFree(newBuff);
	MemFree(tmpBuff);
	MemFree(tmpBuff2);
}

void PNGExporter_FilterByte4(UInt8 *imgBuff, UOSInt lineByteCnt, UOSInt height)
{
	UOSInt oriHeight = height;
	UOSInt i;
	UOSInt j;
	UOSInt compSize[5];
	UInt8 lastPx[4];
	UInt8 thisPx[4];
	UInt8 *lastLineStart = 0;
	UInt8 *lineStart = imgBuff;
	UInt8 *tmpBuff = MemAlloc(UInt8, lineByteCnt + 1);
	UInt8 *tmpBuff2 = MemAlloc(UInt8, lineByteCnt + 12);
	UInt8 *newBuff = MemAlloc(UInt8, (lineByteCnt + 1) * height);
	UInt8 *outBuff = newBuff;
	tmpBuff[0] = 1;
	lastPx[0] = tmpBuff[1] = lineStart[1];
	lastPx[1] = tmpBuff[2] = lineStart[2];
	lastPx[2] = tmpBuff[3] = lineStart[3];
	lastPx[3] = tmpBuff[4] = lineStart[4];
	i = 4;
	while (i < lineByteCnt)
	{
		thisPx[0] = (UInt8)(lineStart[i + 1] - lastPx[0]);
		thisPx[1] = (UInt8)(lineStart[i + 2] - lastPx[1]);
		thisPx[2] = (UInt8)(lineStart[i + 3] - lastPx[2]);
		thisPx[3] = (UInt8)(lineStart[i + 4] - lastPx[3]);
		lastPx[0] = lineStart[i + 1];
		lastPx[1] = lineStart[i + 2];
		lastPx[2] = lineStart[i + 3];
		lastPx[3] = lineStart[i + 4];
		tmpBuff[i + 1] = thisPx[0];
		tmpBuff[i + 2] = thisPx[1];
		tmpBuff[i + 3] = thisPx[2];
		tmpBuff[i + 4] = thisPx[3];
		i += 4;
	}
	compSize[0] = Data::Compress::Inflate::Compress(lineStart, lineByteCnt + 1, tmpBuff2, false);
	compSize[1] = Data::Compress::Inflate::Compress(tmpBuff, lineByteCnt + 1, tmpBuff2, false);
	compSize[2] = 0;
	compSize[3] = 0;
	compSize[4] = 0;
	while (height-- > 0)
	{
		i = compSize[0];
		j = 0;
		if (compSize[1] > 0 && compSize[1] < i)
		{
			i = compSize[1];
			j = 1;
		}
		if (compSize[2] > 0 && compSize[2] < i)
		{
			i = compSize[2];
			j = 2;
		}
		if (compSize[3] > 0 && compSize[3] < i)
		{
			i = compSize[3];
			j = 3;
		}
		if (compSize[4] > 0 && compSize[4] < i)
		{
			i = compSize[4];
			j = 4;
		}
		if (j == 0)
		{
			MemCopyNO(outBuff, lineStart, lineByteCnt + 1);
		}
		else if (j == 1)
		{
			outBuff[0] = 1;
			outBuff[1] = lineStart[1];
			outBuff[2] = lineStart[2];
			outBuff[3] = lineStart[3];
			outBuff[4] = lineStart[4];
			i = 4;
			while (i < lineByteCnt)
			{
				outBuff[i + 1] = (UInt8)(lineStart[i + 1] - lineStart[i - 3]);
				outBuff[i + 2] = (UInt8)(lineStart[i + 2] - lineStart[i - 2]);
				outBuff[i + 3] = (UInt8)(lineStart[i + 3] - lineStart[i - 1]);
				outBuff[i + 4] = (UInt8)(lineStart[i + 4] - lineStart[i]);
				i += 4;
			}
		}
		else if (j == 2)
		{
			outBuff[0] = 2;
			i = 0;
			while (i < lineByteCnt)
			{
				outBuff[i + 1] = (UInt8)(lineStart[i + 1] - lastLineStart[i + 1]);
				outBuff[i + 2] = (UInt8)(lineStart[i + 2] - lastLineStart[i + 2]);
				outBuff[i + 3] = (UInt8)(lineStart[i + 3] - lastLineStart[i + 3]);
				outBuff[i + 4] = (UInt8)(lineStart[i + 4] - lastLineStart[i + 4]);
				i += 4;
			}
		}
		else if (j == 3)
		{
			outBuff[0] = 3;
			lastPx[0] = 0;
			lastPx[1] = 0;
			lastPx[2] = 0;
			lastPx[3] = 0;
			i = 0;
			while (i < lineByteCnt)
			{
				outBuff[i + 1] = (UInt8)(lineStart[i + 1] - ((lastLineStart[i + 1] + lastPx[0]) >> 1));
				outBuff[i + 2] = (UInt8)(lineStart[i + 2] - ((lastLineStart[i + 2] + lastPx[1]) >> 1));
				outBuff[i + 3] = (UInt8)(lineStart[i + 3] - ((lastLineStart[i + 3] + lastPx[2]) >> 1));
				outBuff[i + 4] = (UInt8)(lineStart[i + 4] - ((lastLineStart[i + 4] + lastPx[3]) >> 1));
				lastPx[0] = lineStart[i + 1];
				lastPx[1] = lineStart[i + 2];
				lastPx[2] = lineStart[i + 3];
				lastPx[3] = lineStart[i + 4];
				i += 4;
			}
		}
		else if (j == 4)
		{
			outBuff[0] = 4;
			outBuff[1] = (UInt8)(lineStart[1] - lastLineStart[1]);
			outBuff[2] = (UInt8)(lineStart[2] - lastLineStart[2]);
			outBuff[3] = (UInt8)(lineStart[3] - lastLineStart[3]);
			outBuff[4] = (UInt8)(lineStart[4] - lastLineStart[4]);
			i = 4;
			while (i < lineByteCnt)
			{
				outBuff[i + 1] = (UInt8)(lineStart[i + 1] - PNGExporter_PaethPredictor(lineStart[i - 3], lastLineStart[i + 1], lastLineStart[i - 3]));
				outBuff[i + 2] = (UInt8)(lineStart[i + 2] - PNGExporter_PaethPredictor(lineStart[i - 2], lastLineStart[i + 2], lastLineStart[i - 2]));
				outBuff[i + 3] = (UInt8)(lineStart[i + 3] - PNGExporter_PaethPredictor(lineStart[i - 1], lastLineStart[i + 3], lastLineStart[i - 1]));
				outBuff[i + 4] = (UInt8)(lineStart[i + 4] - PNGExporter_PaethPredictor(lineStart[i - 0], lastLineStart[i + 4], lastLineStart[i - 0]));
				i += 4;
			}
		}
		lastLineStart = lineStart;
		lineStart = lineStart + lineByteCnt + 1;
		outBuff += lineByteCnt + 1;
		if (height <= 0)
			break;

		compSize[0] = Data::Compress::Inflate::Compress(lineStart, lineByteCnt + 1, tmpBuff2, false);

		tmpBuff[0] = 1;
		tmpBuff[1] = lineStart[1];
		tmpBuff[2] = lineStart[2];
		tmpBuff[3] = lineStart[3];
		tmpBuff[4] = lineStart[4];
		i = 4;
		while (i < lineByteCnt)
		{
			tmpBuff[i + 1] = (UInt8)(lineStart[i + 1] - lineStart[i - 3]);
			tmpBuff[i + 2] = (UInt8)(lineStart[i + 2] - lineStart[i - 2]);
			tmpBuff[i + 3] = (UInt8)(lineStart[i + 3] - lineStart[i - 1]);
			tmpBuff[i + 4] = (UInt8)(lineStart[i + 4] - lineStart[i - 0]);
			i += 4;
		}
		compSize[1] = Data::Compress::Inflate::Compress(tmpBuff, lineByteCnt + 1, tmpBuff2, false);

		tmpBuff[0] = 2;
		i = 0;
		while (i < lineByteCnt)
		{
			tmpBuff[i + 1] = (UInt8)(lineStart[i + 1] - lastLineStart[i + 1]);
			tmpBuff[i + 2] = (UInt8)(lineStart[i + 2] - lastLineStart[i + 2]);
			tmpBuff[i + 3] = (UInt8)(lineStart[i + 3] - lastLineStart[i + 3]);
			tmpBuff[i + 4] = (UInt8)(lineStart[i + 4] - lastLineStart[i + 4]);
			i += 4;
		}
		compSize[2] = Data::Compress::Inflate::Compress(tmpBuff, lineByteCnt + 1, tmpBuff2, false);

		tmpBuff[0] = 3;
		lastPx[0] = 0;
		lastPx[1] = 0;
		lastPx[2] = 0;
		lastPx[3] = 0;
		i = 0;
		while (i < lineByteCnt)
		{
			tmpBuff[i + 1] = (UInt8)(lineStart[i + 1] - ((lastLineStart[i + 1] + lastPx[0]) >> 1));
			tmpBuff[i + 2] = (UInt8)(lineStart[i + 2] - ((lastLineStart[i + 2] + lastPx[1]) >> 1));
			tmpBuff[i + 3] = (UInt8)(lineStart[i + 3] - ((lastLineStart[i + 3] + lastPx[2]) >> 1));
			tmpBuff[i + 4] = (UInt8)(lineStart[i + 4] - ((lastLineStart[i + 4] + lastPx[3]) >> 1));
			lastPx[0] = lineStart[i + 1];
			lastPx[1] = lineStart[i + 2];
			lastPx[2] = lineStart[i + 3];
			lastPx[3] = lineStart[i + 4];
			i += 4;
		}
		compSize[3] = Data::Compress::Inflate::Compress(tmpBuff, lineByteCnt + 1, tmpBuff2, false);

		tmpBuff[0] = 4;
		tmpBuff[1] = (UInt8)(lineStart[1] - lastLineStart[1]);
		tmpBuff[2] = (UInt8)(lineStart[2] - lastLineStart[2]);
		tmpBuff[3] = (UInt8)(lineStart[3] - lastLineStart[3]);
		tmpBuff[4] = (UInt8)(lineStart[4] - lastLineStart[4]);
		i = 4;
		while (i < lineByteCnt)
		{
			tmpBuff[i + 1] = (UInt8)(lineStart[i + 1] - PNGExporter_PaethPredictor(lineStart[i - 3], lastLineStart[i + 1], lastLineStart[i - 3]));
			tmpBuff[i + 2] = (UInt8)(lineStart[i + 2] - PNGExporter_PaethPredictor(lineStart[i - 2], lastLineStart[i + 2], lastLineStart[i - 2]));
			tmpBuff[i + 3] = (UInt8)(lineStart[i + 3] - PNGExporter_PaethPredictor(lineStart[i - 1], lastLineStart[i + 3], lastLineStart[i - 1]));
			tmpBuff[i + 4] = (UInt8)(lineStart[i + 4] - PNGExporter_PaethPredictor(lineStart[i - 0], lastLineStart[i + 4], lastLineStart[i - 0]));
			i += 4;
		}
		compSize[4] = Data::Compress::Inflate::Compress(tmpBuff, lineByteCnt + 1, tmpBuff2, false);
	}
	MemCopyNO(imgBuff, newBuff, (lineByteCnt + 1) * oriHeight);
	MemFree(newBuff);
	MemFree(tmpBuff);
	MemFree(tmpBuff2);
}

void PNGExporter_FilterByte6(UInt8 *imgBuff, UOSInt lineByteCnt, UOSInt height)
{
	UOSInt oriHeight = height;
	UOSInt i;
	UOSInt j;
	UOSInt compSize[5];
	UInt8 lastPx[6];
	UInt8 thisPx[6];
	UInt8 *lastLineStart = 0;
	UInt8 *lineStart = imgBuff;
	UInt8 *tmpBuff = MemAlloc(UInt8, lineByteCnt + 1);
	UInt8 *tmpBuff2 = MemAlloc(UInt8, lineByteCnt + 12);
	UInt8 *newBuff = MemAlloc(UInt8, (lineByteCnt + 1) * height);
	UInt8 *outBuff = newBuff;
	tmpBuff[0] = 1;
	lastPx[0] = tmpBuff[1] = lineStart[1];
	lastPx[1] = tmpBuff[2] = lineStart[2];
	lastPx[2] = tmpBuff[3] = lineStart[3];
	lastPx[3] = tmpBuff[4] = lineStart[4];
	lastPx[4] = tmpBuff[5] = lineStart[5];
	lastPx[5] = tmpBuff[6] = lineStart[6];
	i = 6;
	while (i < lineByteCnt)
	{
		thisPx[0] = (UInt8)(lineStart[i + 1] - lastPx[0]);
		thisPx[1] = (UInt8)(lineStart[i + 2] - lastPx[1]);
		thisPx[2] = (UInt8)(lineStart[i + 3] - lastPx[2]);
		thisPx[3] = (UInt8)(lineStart[i + 4] - lastPx[3]);
		thisPx[4] = (UInt8)(lineStart[i + 5] - lastPx[4]);
		thisPx[5] = (UInt8)(lineStart[i + 6] - lastPx[5]);
		lastPx[0] = lineStart[i + 1];
		lastPx[1] = lineStart[i + 2];
		lastPx[2] = lineStart[i + 3];
		lastPx[3] = lineStart[i + 4];
		lastPx[4] = lineStart[i + 5];
		lastPx[5] = lineStart[i + 6];
		tmpBuff[i + 1] = thisPx[0];
		tmpBuff[i + 2] = thisPx[1];
		tmpBuff[i + 3] = thisPx[2];
		tmpBuff[i + 4] = thisPx[3];
		tmpBuff[i + 5] = thisPx[4];
		tmpBuff[i + 6] = thisPx[5];
		i += 6;
	}
	compSize[0] = Data::Compress::Inflate::Compress(lineStart, lineByteCnt + 1, tmpBuff2, false);
	compSize[1] = Data::Compress::Inflate::Compress(tmpBuff, lineByteCnt + 1, tmpBuff2, false);
	compSize[2] = 0;
	compSize[3] = 0;
	compSize[4] = 0;
	while (height-- > 0)
	{
		i = compSize[0];
		j = 0;
		if (compSize[1] > 0 && compSize[1] < i)
		{
			i = compSize[1];
			j = 1;
		}
		if (compSize[2] > 0 && compSize[2] < i)
		{
			i = compSize[2];
			j = 2;
		}
		if (compSize[3] > 0 && compSize[3] < i)
		{
			i = compSize[3];
			j = 3;
		}
		if (compSize[4] > 0 && compSize[4] < i)
		{
			i = compSize[4];
			j = 4;
		}
		if (j == 0)
		{
			MemCopyNO(outBuff, lineStart, lineByteCnt + 1);
		}
		else if (j == 1)
		{
			outBuff[0] = 1;
			outBuff[1] = lineStart[1];
			outBuff[2] = lineStart[2];
			outBuff[3] = lineStart[3];
			outBuff[4] = lineStart[4];
			outBuff[5] = lineStart[5];
			outBuff[6] = lineStart[6];
			i = 6;
			while (i < lineByteCnt)
			{
				outBuff[i + 1] = (UInt8)(lineStart[i + 1] - lineStart[i - 5]);
				outBuff[i + 2] = (UInt8)(lineStart[i + 2] - lineStart[i - 4]);
				outBuff[i + 3] = (UInt8)(lineStart[i + 3] - lineStart[i - 3]);
				outBuff[i + 4] = (UInt8)(lineStart[i + 4] - lineStart[i - 2]);
				outBuff[i + 5] = (UInt8)(lineStart[i + 5] - lineStart[i - 1]);
				outBuff[i + 6] = (UInt8)(lineStart[i + 6] - lineStart[i - 0]);
				i += 6;
			}
		}
		else if (j == 2)
		{
			outBuff[0] = 2;
			i = 0;
			while (i < lineByteCnt)
			{
				outBuff[i + 1] = (UInt8)(lineStart[i + 1] - lastLineStart[i + 1]);
				outBuff[i + 2] = (UInt8)(lineStart[i + 2] - lastLineStart[i + 2]);
				outBuff[i + 3] = (UInt8)(lineStart[i + 3] - lastLineStart[i + 3]);
				outBuff[i + 4] = (UInt8)(lineStart[i + 4] - lastLineStart[i + 4]);
				outBuff[i + 5] = (UInt8)(lineStart[i + 5] - lastLineStart[i + 5]);
				outBuff[i + 6] = (UInt8)(lineStart[i + 6] - lastLineStart[i + 6]);
				i += 6;
			}
		}
		else if (j == 3)
		{
			outBuff[0] = 3;
			lastPx[0] = 0;
			lastPx[1] = 0;
			lastPx[2] = 0;
			lastPx[3] = 0;
			lastPx[4] = 0;
			lastPx[5] = 0;
			i = 0;
			while (i < lineByteCnt)
			{
				outBuff[i + 1] = (UInt8)(lineStart[i + 1] - ((lastLineStart[i + 1] + lastPx[0]) >> 1));
				outBuff[i + 2] = (UInt8)(lineStart[i + 2] - ((lastLineStart[i + 2] + lastPx[1]) >> 1));
				outBuff[i + 3] = (UInt8)(lineStart[i + 3] - ((lastLineStart[i + 3] + lastPx[2]) >> 1));
				outBuff[i + 4] = (UInt8)(lineStart[i + 4] - ((lastLineStart[i + 4] + lastPx[3]) >> 1));
				outBuff[i + 5] = (UInt8)(lineStart[i + 5] - ((lastLineStart[i + 5] + lastPx[4]) >> 1));
				outBuff[i + 6] = (UInt8)(lineStart[i + 6] - ((lastLineStart[i + 6] + lastPx[5]) >> 1));
				lastPx[0] = lineStart[i + 1];
				lastPx[1] = lineStart[i + 2];
				lastPx[2] = lineStart[i + 3];
				lastPx[3] = lineStart[i + 4];
				lastPx[4] = lineStart[i + 5];
				lastPx[5] = lineStart[i + 6];
				i += 6;
			}
		}
		else if (j == 4)
		{
			outBuff[0] = 4;
			outBuff[1] = (UInt8)(lineStart[1] - lastLineStart[1]);
			outBuff[2] = (UInt8)(lineStart[2] - lastLineStart[2]);
			outBuff[3] = (UInt8)(lineStart[3] - lastLineStart[3]);
			outBuff[4] = (UInt8)(lineStart[4] - lastLineStart[4]);
			outBuff[5] = (UInt8)(lineStart[5] - lastLineStart[5]);
			outBuff[6] = (UInt8)(lineStart[6] - lastLineStart[6]);
			i = 6;
			while (i < lineByteCnt)
			{
				outBuff[i + 1] = (UInt8)(lineStart[i + 1] - PNGExporter_PaethPredictor(lineStart[i - 5], lastLineStart[i + 1], lastLineStart[i - 5]));
				outBuff[i + 2] = (UInt8)(lineStart[i + 2] - PNGExporter_PaethPredictor(lineStart[i - 4], lastLineStart[i + 2], lastLineStart[i - 4]));
				outBuff[i + 3] = (UInt8)(lineStart[i + 3] - PNGExporter_PaethPredictor(lineStart[i - 3], lastLineStart[i + 3], lastLineStart[i - 3]));
				outBuff[i + 4] = (UInt8)(lineStart[i + 4] - PNGExporter_PaethPredictor(lineStart[i - 2], lastLineStart[i + 4], lastLineStart[i - 2]));
				outBuff[i + 5] = (UInt8)(lineStart[i + 5] - PNGExporter_PaethPredictor(lineStart[i - 1], lastLineStart[i + 5], lastLineStart[i - 1]));
				outBuff[i + 6] = (UInt8)(lineStart[i + 6] - PNGExporter_PaethPredictor(lineStart[i - 0], lastLineStart[i + 6], lastLineStart[i - 0]));
				i += 6;
			}
		}
		lastLineStart = lineStart;
		lineStart = lineStart + lineByteCnt + 1;
		outBuff += lineByteCnt + 1;
		if (height <= 0)
			break;

		compSize[0] = Data::Compress::Inflate::Compress(lineStart, lineByteCnt + 1, tmpBuff2, false);

		tmpBuff[0] = 1;
		tmpBuff[1] = lineStart[1];
		tmpBuff[2] = lineStart[2];
		tmpBuff[3] = lineStart[3];
		tmpBuff[4] = lineStart[4];
		tmpBuff[5] = lineStart[5];
		tmpBuff[6] = lineStart[6];
		i = 6;
		while (i < lineByteCnt)
		{
			tmpBuff[i + 1] = (UInt8)(lineStart[i + 1] - lineStart[i - 5]);
			tmpBuff[i + 2] = (UInt8)(lineStart[i + 2] - lineStart[i - 4]);
			tmpBuff[i + 3] = (UInt8)(lineStart[i + 3] - lineStart[i - 3]);
			tmpBuff[i + 4] = (UInt8)(lineStart[i + 4] - lineStart[i - 2]);
			tmpBuff[i + 5] = (UInt8)(lineStart[i + 5] - lineStart[i - 1]);
			tmpBuff[i + 6] = (UInt8)(lineStart[i + 6] - lineStart[i - 0]);
			i += 6;
		}
		compSize[1] = Data::Compress::Inflate::Compress(tmpBuff, lineByteCnt + 1, tmpBuff2, false);

		tmpBuff[0] = 2;
		i = 0;
		while (i < lineByteCnt)
		{
			tmpBuff[i + 1] = (UInt8)(lineStart[i + 1] - lastLineStart[i + 1]);
			tmpBuff[i + 2] = (UInt8)(lineStart[i + 2] - lastLineStart[i + 2]);
			tmpBuff[i + 3] = (UInt8)(lineStart[i + 3] - lastLineStart[i + 3]);
			tmpBuff[i + 4] = (UInt8)(lineStart[i + 4] - lastLineStart[i + 4]);
			tmpBuff[i + 5] = (UInt8)(lineStart[i + 5] - lastLineStart[i + 5]);
			tmpBuff[i + 6] = (UInt8)(lineStart[i + 6] - lastLineStart[i + 6]);
			i += 6;
		}
		compSize[2] = Data::Compress::Inflate::Compress(tmpBuff, lineByteCnt + 1, tmpBuff2, false);

		tmpBuff[0] = 3;
		lastPx[0] = 0;
		lastPx[1] = 0;
		lastPx[2] = 0;
		lastPx[3] = 0;
		lastPx[4] = 0;
		lastPx[5] = 0;
		i = 0;
		while (i < lineByteCnt)
		{
			tmpBuff[i + 1] = (UInt8)(lineStart[i + 1] - ((lastLineStart[i + 1] + lastPx[0]) >> 1));
			tmpBuff[i + 2] = (UInt8)(lineStart[i + 2] - ((lastLineStart[i + 2] + lastPx[1]) >> 1));
			tmpBuff[i + 3] = (UInt8)(lineStart[i + 3] - ((lastLineStart[i + 3] + lastPx[2]) >> 1));
			tmpBuff[i + 4] = (UInt8)(lineStart[i + 4] - ((lastLineStart[i + 4] + lastPx[3]) >> 1));
			tmpBuff[i + 5] = (UInt8)(lineStart[i + 5] - ((lastLineStart[i + 5] + lastPx[4]) >> 1));
			tmpBuff[i + 6] = (UInt8)(lineStart[i + 6] - ((lastLineStart[i + 6] + lastPx[5]) >> 1));
			lastPx[0] = lineStart[i + 1];
			lastPx[1] = lineStart[i + 2];
			lastPx[2] = lineStart[i + 3];
			lastPx[3] = lineStart[i + 4];
			lastPx[4] = lineStart[i + 5];
			lastPx[5] = lineStart[i + 6];
			i += 6;
		}
		compSize[3] = Data::Compress::Inflate::Compress(tmpBuff, lineByteCnt + 1, tmpBuff2, false);

		tmpBuff[0] = 4;
		tmpBuff[1] = (UInt8)(lineStart[1] - lastLineStart[1]);
		tmpBuff[2] = (UInt8)(lineStart[2] - lastLineStart[2]);
		tmpBuff[3] = (UInt8)(lineStart[3] - lastLineStart[3]);
		tmpBuff[4] = (UInt8)(lineStart[4] - lastLineStart[4]);
		tmpBuff[5] = (UInt8)(lineStart[5] - lastLineStart[5]);
		tmpBuff[6] = (UInt8)(lineStart[6] - lastLineStart[6]);
		i = 6;
		while (i < lineByteCnt)
		{
			tmpBuff[i + 1] = (UInt8)(lineStart[i + 1] - PNGExporter_PaethPredictor(lineStart[i - 5], lastLineStart[i + 1], lastLineStart[i - 5]));
			tmpBuff[i + 2] = (UInt8)(lineStart[i + 2] - PNGExporter_PaethPredictor(lineStart[i - 4], lastLineStart[i + 2], lastLineStart[i - 4]));
			tmpBuff[i + 3] = (UInt8)(lineStart[i + 3] - PNGExporter_PaethPredictor(lineStart[i - 3], lastLineStart[i + 3], lastLineStart[i - 3]));
			tmpBuff[i + 4] = (UInt8)(lineStart[i + 4] - PNGExporter_PaethPredictor(lineStart[i - 2], lastLineStart[i + 4], lastLineStart[i - 2]));
			tmpBuff[i + 5] = (UInt8)(lineStart[i + 5] - PNGExporter_PaethPredictor(lineStart[i - 1], lastLineStart[i + 5], lastLineStart[i - 1]));
			tmpBuff[i + 6] = (UInt8)(lineStart[i + 6] - PNGExporter_PaethPredictor(lineStart[i - 0], lastLineStart[i + 6], lastLineStart[i - 0]));
			i += 6;
		}
		compSize[4] = Data::Compress::Inflate::Compress(tmpBuff, lineByteCnt + 1, tmpBuff2, false);
	}
	MemCopyNO(imgBuff, newBuff, (lineByteCnt + 1) * oriHeight);
	MemFree(newBuff);
	MemFree(tmpBuff);
	MemFree(tmpBuff2);
}

void PNGExporter_FilterByte8(UInt8 *imgBuff, UOSInt lineByteCnt, UOSInt height)
{
	UOSInt oriHeight = height;
	UOSInt i;
	UOSInt j;
	UOSInt compSize[5];
	UInt8 lastPx[8];
	UInt8 thisPx[8];
	UInt8 *lastLineStart = 0;
	UInt8 *lineStart = imgBuff;
	UInt8 *tmpBuff = MemAlloc(UInt8, lineByteCnt + 1);
	UInt8 *tmpBuff2 = MemAlloc(UInt8, lineByteCnt + 12);
	UInt8 *newBuff = MemAlloc(UInt8, (lineByteCnt + 1) * height);
	UInt8 *outBuff = newBuff;
	tmpBuff[0] = 1;
	lastPx[0] = tmpBuff[1] = lineStart[1];
	lastPx[1] = tmpBuff[2] = lineStart[2];
	lastPx[2] = tmpBuff[3] = lineStart[3];
	lastPx[3] = tmpBuff[4] = lineStart[4];
	lastPx[4] = tmpBuff[5] = lineStart[5];
	lastPx[5] = tmpBuff[6] = lineStart[6];
	lastPx[6] = tmpBuff[7] = lineStart[7];
	lastPx[7] = tmpBuff[8] = lineStart[8];
	i = 8;
	while (i < lineByteCnt)
	{
		thisPx[0] = (UInt8)(lineStart[i + 1] - lastPx[0]);
		thisPx[1] = (UInt8)(lineStart[i + 2] - lastPx[1]);
		thisPx[2] = (UInt8)(lineStart[i + 3] - lastPx[2]);
		thisPx[3] = (UInt8)(lineStart[i + 4] - lastPx[3]);
		thisPx[4] = (UInt8)(lineStart[i + 5] - lastPx[4]);
		thisPx[5] = (UInt8)(lineStart[i + 6] - lastPx[5]);
		thisPx[6] = (UInt8)(lineStart[i + 7] - lastPx[6]);
		thisPx[7] = (UInt8)(lineStart[i + 8] - lastPx[7]);
		lastPx[0] = lineStart[i + 1];
		lastPx[1] = lineStart[i + 2];
		lastPx[2] = lineStart[i + 3];
		lastPx[3] = lineStart[i + 4];
		lastPx[4] = lineStart[i + 5];
		lastPx[5] = lineStart[i + 6];
		lastPx[6] = lineStart[i + 7];
		lastPx[7] = lineStart[i + 8];
		tmpBuff[i + 1] = thisPx[0];
		tmpBuff[i + 2] = thisPx[1];
		tmpBuff[i + 3] = thisPx[2];
		tmpBuff[i + 4] = thisPx[3];
		tmpBuff[i + 5] = thisPx[4];
		tmpBuff[i + 6] = thisPx[5];
		tmpBuff[i + 7] = thisPx[6];
		tmpBuff[i + 8] = thisPx[7];
		i += 8;
	}
	compSize[0] = Data::Compress::Inflate::Compress(lineStart, lineByteCnt + 1, tmpBuff2, false);
	compSize[1] = Data::Compress::Inflate::Compress(tmpBuff, lineByteCnt + 1, tmpBuff2, false);
	compSize[2] = 0;
	compSize[3] = 0;
	compSize[4] = 0;
	while (height-- > 0)
	{
		i = compSize[0];
		j = 0;
		if (compSize[1] > 0 && compSize[1] < i)
		{
			i = compSize[1];
			j = 1;
		}
		if (compSize[2] > 0 && compSize[2] < i)
		{
			i = compSize[2];
			j = 2;
		}
		if (compSize[3] > 0 && compSize[3] < i)
		{
			i = compSize[3];
			j = 3;
		}
		if (compSize[4] > 0 && compSize[4] < i)
		{
			i = compSize[4];
			j = 4;
		}
		if (j == 0)
		{
			MemCopyNO(outBuff, lineStart, lineByteCnt + 1);
		}
		else if (j == 1)
		{
			outBuff[0] = 1;
			outBuff[1] = lineStart[1];
			outBuff[2] = lineStart[2];
			outBuff[3] = lineStart[3];
			outBuff[4] = lineStart[4];
			outBuff[5] = lineStart[5];
			outBuff[6] = lineStart[6];
			outBuff[7] = lineStart[7];
			outBuff[8] = lineStart[8];
			i = 8;
			while (i < lineByteCnt)
			{
				outBuff[i + 1] = (UInt8)(lineStart[i + 1] - lineStart[i - 7]);
				outBuff[i + 2] = (UInt8)(lineStart[i + 2] - lineStart[i - 6]);
				outBuff[i + 3] = (UInt8)(lineStart[i + 3] - lineStart[i - 5]);
				outBuff[i + 4] = (UInt8)(lineStart[i + 4] - lineStart[i - 4]);
				outBuff[i + 5] = (UInt8)(lineStart[i + 5] - lineStart[i - 3]);
				outBuff[i + 6] = (UInt8)(lineStart[i + 6] - lineStart[i - 2]);
				outBuff[i + 7] = (UInt8)(lineStart[i + 7] - lineStart[i - 1]);
				outBuff[i + 8] = (UInt8)(lineStart[i + 8] - lineStart[i - 0]);
				i += 8;
			}
		}
		else if (j == 2)
		{
			outBuff[0] = 2;
			i = 0;
			while (i < lineByteCnt)
			{
				outBuff[i + 1] = (UInt8)(lineStart[i + 1] - lastLineStart[i + 1]);
				outBuff[i + 2] = (UInt8)(lineStart[i + 2] - lastLineStart[i + 2]);
				outBuff[i + 3] = (UInt8)(lineStart[i + 3] - lastLineStart[i + 3]);
				outBuff[i + 4] = (UInt8)(lineStart[i + 4] - lastLineStart[i + 4]);
				outBuff[i + 5] = (UInt8)(lineStart[i + 5] - lastLineStart[i + 5]);
				outBuff[i + 6] = (UInt8)(lineStart[i + 6] - lastLineStart[i + 6]);
				outBuff[i + 7] = (UInt8)(lineStart[i + 7] - lastLineStart[i + 7]);
				outBuff[i + 8] = (UInt8)(lineStart[i + 8] - lastLineStart[i + 8]);
				i += 8;
			}
		}
		else if (j == 3)
		{
			outBuff[0] = 3;
			lastPx[0] = 0;
			lastPx[1] = 0;
			lastPx[2] = 0;
			lastPx[3] = 0;
			lastPx[4] = 0;
			lastPx[5] = 0;
			lastPx[6] = 0;
			lastPx[7] = 0;
			i = 0;
			while (i < lineByteCnt)
			{
				outBuff[i + 1] = (UInt8)(lineStart[i + 1] - ((lastLineStart[i + 1] + lastPx[0]) >> 1));
				outBuff[i + 2] = (UInt8)(lineStart[i + 2] - ((lastLineStart[i + 2] + lastPx[1]) >> 1));
				outBuff[i + 3] = (UInt8)(lineStart[i + 3] - ((lastLineStart[i + 3] + lastPx[2]) >> 1));
				outBuff[i + 4] = (UInt8)(lineStart[i + 4] - ((lastLineStart[i + 4] + lastPx[3]) >> 1));
				outBuff[i + 5] = (UInt8)(lineStart[i + 5] - ((lastLineStart[i + 5] + lastPx[4]) >> 1));
				outBuff[i + 6] = (UInt8)(lineStart[i + 6] - ((lastLineStart[i + 6] + lastPx[5]) >> 1));
				outBuff[i + 7] = (UInt8)(lineStart[i + 7] - ((lastLineStart[i + 7] + lastPx[6]) >> 1));
				outBuff[i + 8] = (UInt8)(lineStart[i + 8] - ((lastLineStart[i + 8] + lastPx[7]) >> 1));
				lastPx[0] = lineStart[i + 1];
				lastPx[1] = lineStart[i + 2];
				lastPx[2] = lineStart[i + 3];
				lastPx[3] = lineStart[i + 4];
				lastPx[4] = lineStart[i + 5];
				lastPx[5] = lineStart[i + 6];
				lastPx[6] = lineStart[i + 7];
				lastPx[7] = lineStart[i + 8];
				i += 8;
			}
		}
		else if (j == 4)
		{
			outBuff[0] = 4;
			outBuff[1] = (UInt8)(lineStart[1] - lastLineStart[1]);
			outBuff[2] = (UInt8)(lineStart[2] - lastLineStart[2]);
			outBuff[3] = (UInt8)(lineStart[3] - lastLineStart[3]);
			outBuff[4] = (UInt8)(lineStart[4] - lastLineStart[4]);
			outBuff[5] = (UInt8)(lineStart[5] - lastLineStart[5]);
			outBuff[6] = (UInt8)(lineStart[6] - lastLineStart[6]);
			outBuff[7] = (UInt8)(lineStart[7] - lastLineStart[7]);
			outBuff[8] = (UInt8)(lineStart[8] - lastLineStart[8]);
			i = 8;
			while (i < lineByteCnt)
			{
				outBuff[i + 1] = (UInt8)(lineStart[i + 1] - PNGExporter_PaethPredictor(lineStart[i - 7], lastLineStart[i + 1], lastLineStart[i - 7]));
				outBuff[i + 2] = (UInt8)(lineStart[i + 2] - PNGExporter_PaethPredictor(lineStart[i - 6], lastLineStart[i + 2], lastLineStart[i - 6]));
				outBuff[i + 3] = (UInt8)(lineStart[i + 3] - PNGExporter_PaethPredictor(lineStart[i - 5], lastLineStart[i + 3], lastLineStart[i - 5]));
				outBuff[i + 4] = (UInt8)(lineStart[i + 4] - PNGExporter_PaethPredictor(lineStart[i - 4], lastLineStart[i + 4], lastLineStart[i - 4]));
				outBuff[i + 5] = (UInt8)(lineStart[i + 5] - PNGExporter_PaethPredictor(lineStart[i - 3], lastLineStart[i + 5], lastLineStart[i - 3]));
				outBuff[i + 6] = (UInt8)(lineStart[i + 6] - PNGExporter_PaethPredictor(lineStart[i - 2], lastLineStart[i + 6], lastLineStart[i - 2]));
				outBuff[i + 7] = (UInt8)(lineStart[i + 7] - PNGExporter_PaethPredictor(lineStart[i - 1], lastLineStart[i + 7], lastLineStart[i - 1]));
				outBuff[i + 8] = (UInt8)(lineStart[i + 8] - PNGExporter_PaethPredictor(lineStart[i - 0], lastLineStart[i + 8], lastLineStart[i - 0]));
				i += 8;
			}
		}
		lastLineStart = lineStart;
		lineStart = lineStart + lineByteCnt + 1;
		outBuff += lineByteCnt + 1;
		if (height <= 0)
			break;

		compSize[0] = Data::Compress::Inflate::Compress(lineStart, lineByteCnt + 1, tmpBuff2, false);

		tmpBuff[0] = 1;
		tmpBuff[1] = lineStart[1];
		tmpBuff[2] = lineStart[2];
		tmpBuff[3] = lineStart[3];
		tmpBuff[4] = lineStart[4];
		tmpBuff[5] = lineStart[5];
		tmpBuff[6] = lineStart[6];
		tmpBuff[7] = lineStart[7];
		tmpBuff[8] = lineStart[8];
		i = 8;
		while (i < lineByteCnt)
		{
			tmpBuff[i + 1] = (UInt8)(lineStart[i + 1] - lineStart[i - 7]);
			tmpBuff[i + 2] = (UInt8)(lineStart[i + 2] - lineStart[i - 6]);
			tmpBuff[i + 3] = (UInt8)(lineStart[i + 3] - lineStart[i - 5]);
			tmpBuff[i + 4] = (UInt8)(lineStart[i + 4] - lineStart[i - 4]);
			tmpBuff[i + 5] = (UInt8)(lineStart[i + 5] - lineStart[i - 3]);
			tmpBuff[i + 6] = (UInt8)(lineStart[i + 6] - lineStart[i - 2]);
			tmpBuff[i + 7] = (UInt8)(lineStart[i + 7] - lineStart[i - 1]);
			tmpBuff[i + 8] = (UInt8)(lineStart[i + 8] - lineStart[i - 0]);
			i += 8;
		}
		compSize[1] = Data::Compress::Inflate::Compress(tmpBuff, lineByteCnt + 1, tmpBuff2, false);

		tmpBuff[0] = 2;
		i = 0;
		while (i < lineByteCnt)
		{
			tmpBuff[i + 1] = (UInt8)(lineStart[i + 1] - lastLineStart[i + 1]);
			tmpBuff[i + 2] = (UInt8)(lineStart[i + 2] - lastLineStart[i + 2]);
			tmpBuff[i + 3] = (UInt8)(lineStart[i + 3] - lastLineStart[i + 3]);
			tmpBuff[i + 4] = (UInt8)(lineStart[i + 4] - lastLineStart[i + 4]);
			tmpBuff[i + 5] = (UInt8)(lineStart[i + 5] - lastLineStart[i + 5]);
			tmpBuff[i + 6] = (UInt8)(lineStart[i + 6] - lastLineStart[i + 6]);
			tmpBuff[i + 7] = (UInt8)(lineStart[i + 7] - lastLineStart[i + 7]);
			tmpBuff[i + 8] = (UInt8)(lineStart[i + 8] - lastLineStart[i + 8]);
			i += 8;
		}
		compSize[2] = Data::Compress::Inflate::Compress(tmpBuff, lineByteCnt + 1, tmpBuff2, false);

		tmpBuff[0] = 3;
		lastPx[0] = 0;
		lastPx[1] = 0;
		lastPx[2] = 0;
		lastPx[3] = 0;
		lastPx[4] = 0;
		lastPx[5] = 0;
		lastPx[6] = 0;
		lastPx[7] = 0;
		i = 0;
		while (i < lineByteCnt)
		{
			tmpBuff[i + 1] = (UInt8)(lineStart[i + 1] - ((lastLineStart[i + 1] + lastPx[0]) >> 1));
			tmpBuff[i + 2] = (UInt8)(lineStart[i + 2] - ((lastLineStart[i + 2] + lastPx[1]) >> 1));
			tmpBuff[i + 3] = (UInt8)(lineStart[i + 3] - ((lastLineStart[i + 3] + lastPx[2]) >> 1));
			tmpBuff[i + 4] = (UInt8)(lineStart[i + 4] - ((lastLineStart[i + 4] + lastPx[3]) >> 1));
			tmpBuff[i + 5] = (UInt8)(lineStart[i + 5] - ((lastLineStart[i + 5] + lastPx[4]) >> 1));
			tmpBuff[i + 6] = (UInt8)(lineStart[i + 6] - ((lastLineStart[i + 6] + lastPx[5]) >> 1));
			tmpBuff[i + 7] = (UInt8)(lineStart[i + 7] - ((lastLineStart[i + 7] + lastPx[6]) >> 1));
			tmpBuff[i + 8] = (UInt8)(lineStart[i + 8] - ((lastLineStart[i + 8] + lastPx[7]) >> 1));
			lastPx[0] = lineStart[i + 1];
			lastPx[1] = lineStart[i + 2];
			lastPx[2] = lineStart[i + 3];
			lastPx[3] = lineStart[i + 4];
			lastPx[4] = lineStart[i + 5];
			lastPx[5] = lineStart[i + 6];
			lastPx[6] = lineStart[i + 7];
			lastPx[7] = lineStart[i + 8];
			i += 8;
		}
		compSize[3] = Data::Compress::Inflate::Compress(tmpBuff, lineByteCnt + 1, tmpBuff2, false);

		tmpBuff[0] = 4;
		tmpBuff[1] = (UInt8)(lineStart[1] - lastLineStart[1]);
		tmpBuff[2] = (UInt8)(lineStart[2] - lastLineStart[2]);
		tmpBuff[3] = (UInt8)(lineStart[3] - lastLineStart[3]);
		tmpBuff[4] = (UInt8)(lineStart[4] - lastLineStart[4]);
		tmpBuff[5] = (UInt8)(lineStart[5] - lastLineStart[5]);
		tmpBuff[6] = (UInt8)(lineStart[6] - lastLineStart[6]);
		tmpBuff[7] = (UInt8)(lineStart[7] - lastLineStart[7]);
		tmpBuff[8] = (UInt8)(lineStart[8] - lastLineStart[8]);
		i = 8;
		while (i < lineByteCnt)
		{
			tmpBuff[i + 1] = (UInt8)(lineStart[i + 1] - PNGExporter_PaethPredictor(lineStart[i - 7], lastLineStart[i + 1], lastLineStart[i - 7]));
			tmpBuff[i + 2] = (UInt8)(lineStart[i + 2] - PNGExporter_PaethPredictor(lineStart[i - 6], lastLineStart[i + 2], lastLineStart[i - 6]));
			tmpBuff[i + 3] = (UInt8)(lineStart[i + 3] - PNGExporter_PaethPredictor(lineStart[i - 5], lastLineStart[i + 3], lastLineStart[i - 5]));
			tmpBuff[i + 4] = (UInt8)(lineStart[i + 4] - PNGExporter_PaethPredictor(lineStart[i - 4], lastLineStart[i + 4], lastLineStart[i - 4]));
			tmpBuff[i + 5] = (UInt8)(lineStart[i + 5] - PNGExporter_PaethPredictor(lineStart[i - 3], lastLineStart[i + 5], lastLineStart[i - 3]));
			tmpBuff[i + 6] = (UInt8)(lineStart[i + 6] - PNGExporter_PaethPredictor(lineStart[i - 2], lastLineStart[i + 6], lastLineStart[i - 2]));
			tmpBuff[i + 7] = (UInt8)(lineStart[i + 7] - PNGExporter_PaethPredictor(lineStart[i - 1], lastLineStart[i + 7], lastLineStart[i - 1]));
			tmpBuff[i + 8] = (UInt8)(lineStart[i + 8] - PNGExporter_PaethPredictor(lineStart[i - 0], lastLineStart[i + 8], lastLineStart[i - 0]));
			i += 8;
		}
		compSize[4] = Data::Compress::Inflate::Compress(tmpBuff, lineByteCnt + 1, tmpBuff2, false);
	}
	MemCopyNO(imgBuff, newBuff, (lineByteCnt + 1) * oriHeight);
	MemFree(newBuff);
	MemFree(tmpBuff);
	MemFree(tmpBuff2);
}

Exporter::PNGExporter::PNGExporter()
{
}

Exporter::PNGExporter::~PNGExporter()
{
}

Int32 Exporter::PNGExporter::GetName()
{
	return *(Int32*)"PNGE";
}

IO::FileExporter::SupportType Exporter::PNGExporter::IsObjectSupported(IO::ParsedObject *pobj)
{
	if (pobj->GetParserType() != IO::ParserType::ImageList)
		return IO::FileExporter::SupportType::NotSupported;
	Media::ImageList *imgList = (Media::ImageList*)pobj;
	UInt32 imgTime;
	if (imgList->GetCount() != 1)
		return IO::FileExporter::SupportType::NotSupported;
	Media::Image *img = imgList->GetImage(0, &imgTime);
	if (img->info->fourcc != 0)
		return IO::FileExporter::SupportType::NotSupported;
	if (img->info->pf == Media::PF_B8G8R8)
		return IO::FileExporter::SupportType::NormalStream;
	if (img->info->pf == Media::PF_B8G8R8A8)
		return IO::FileExporter::SupportType::NormalStream;
	if (img->info->pf == Media::PF_LE_B16G16R16)
		return IO::FileExporter::SupportType::NormalStream;
	if (img->info->pf == Media::PF_LE_B16G16R16A16)
		return IO::FileExporter::SupportType::NormalStream;
	if (img->info->pf == Media::PF_LE_W16)
		return IO::FileExporter::SupportType::NormalStream;
	if (img->info->pf == Media::PF_LE_W16A16)
		return IO::FileExporter::SupportType::NormalStream;
	if (img->info->pf == Media::PF_PAL_1)
		return IO::FileExporter::SupportType::NormalStream;
	if (img->info->pf == Media::PF_PAL_2)
		return IO::FileExporter::SupportType::NormalStream;
	if (img->info->pf == Media::PF_PAL_4)
		return IO::FileExporter::SupportType::NormalStream;
	if (img->info->pf == Media::PF_PAL_8)
		return IO::FileExporter::SupportType::NormalStream;
	if (img->info->pf == Media::PF_PAL_W1)
		return IO::FileExporter::SupportType::NormalStream;
	if (img->info->pf == Media::PF_PAL_W2)
		return IO::FileExporter::SupportType::NormalStream;
	if (img->info->pf == Media::PF_PAL_W4)
		return IO::FileExporter::SupportType::NormalStream;
	if (img->info->pf == Media::PF_PAL_W8)
		return IO::FileExporter::SupportType::NormalStream;
	if (img->info->pf == Media::PF_W8A8)
		return IO::FileExporter::SupportType::NormalStream;
	return IO::FileExporter::SupportType::NotSupported;
}

Bool Exporter::PNGExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcat(nameBuff, (const UTF8Char*)"Portable Network Graphic");
		Text::StrConcat(fileNameBuff, (const UTF8Char*)"*.png");
		return true;
	}
	return false;
}

Bool Exporter::PNGExporter::ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param)
{
	if (IsObjectSupported(pobj) == SupportType::NotSupported)
		return false;
	Media::ImageList *imgList = (Media::ImageList*)pobj;
	UInt32 imgTime;
	imgList->ToStaticImage(0);
	Media::StaticImage *img = (Media::StaticImage*)imgList->GetImage(0, &imgTime);
	UInt8 *tmpBuff;
	UInt8 *tmpBuff2;
	UInt8 *imgPtr1;
	UInt8 *imgPtr2;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	Crypto::Hash::CRC32R crc;
	UInt8 hdr[128];
	hdr[0] = 0x89;
	hdr[1] = 0x50;
	hdr[2] = 0x4E;
	hdr[3] = 0x47;
	hdr[4] = 0x0D;
	hdr[5] = 0x0A;
	hdr[6] = 0x1A;
	hdr[7] = 0x0A;
	WriteMInt32(&hdr[8], 13);
	*(Int32*)&hdr[12] = *(Int32*)"IHDR";
	WriteMInt32(&hdr[16], (Int32)img->info->dispWidth);
	WriteMInt32(&hdr[20], (Int32)img->info->dispHeight);
	switch (img->info->pf)
	{
	case Media::PF_B8G8R8:
		hdr[24] = 8;
		hdr[25] = 2;
		break;
	case Media::PF_B8G8R8A8:
		hdr[24] = 8;
		hdr[25] = 6;
		break;
	case Media::PF_LE_B16G16R16:
		hdr[24] = 16;
		hdr[25] = 2;
		break;
	case Media::PF_LE_B16G16R16A16:
		hdr[24] = 16;
		hdr[25] = 6;
		break;
	case Media::PF_LE_W16:
		hdr[24] = 16;
		hdr[25] = 0;
		break;
	case Media::PF_LE_W16A16:
		hdr[24] = 16;
		hdr[25] = 4;
		break;
	case Media::PF_PAL_1:
		hdr[24] = 1;
		hdr[25] = 3;
		break;
	case Media::PF_PAL_2:
		hdr[24] = 2;
		hdr[25] = 3;
		break;
	case Media::PF_PAL_4:
		hdr[24] = 4;
		hdr[25] = 3;
		break;
	case Media::PF_PAL_8:
		hdr[24] = 8;
		hdr[25] = 3;
		break;
	case Media::PF_PAL_W1:
		hdr[24] = 1;
		hdr[25] = 0;
		break;
	case Media::PF_PAL_W2:
		hdr[24] = 2;
		hdr[25] = 0;
		break;
	case Media::PF_PAL_W4:
		hdr[24] = 4;
		hdr[25] = 0;
		break;
	case Media::PF_PAL_W8:
		hdr[24] = 8;
		hdr[25] = 0;
		break;
	case Media::PF_W8A8:
		hdr[24] = 8;
		hdr[25] = 4;
		break;
	case Media::PF_LE_R5G5B5:
	case Media::PF_LE_R5G6B5:
	case Media::PF_LE_A2B10G10R10:
	case Media::PF_LE_FB32G32R32A32:
	case Media::PF_LE_FB32G32R32:
	case Media::PF_LE_FW32A32:
	case Media::PF_LE_FW32:
	case Media::PF_R8G8B8A8:
	case Media::PF_R8G8B8:
	case Media::PF_PAL_1_A1:
	case Media::PF_PAL_2_A1:
	case Media::PF_PAL_4_A1:
	case Media::PF_PAL_8_A1:
	case Media::PF_B8G8R8A1:
	case Media::PF_UNKNOWN:
	default:
		return false;
	}
	hdr[26] = 0; //Compression Method
	hdr[27] = 0; //Filter Method
	hdr[28] = 0; //Interlace Method;
	crc.Clear();
	crc.Calc(&hdr[12], 13 + 4);
	crc.GetValue(&hdr[29]);
	stm->Write(hdr, 33);

	if (img->info->color)
	{
		const UInt8 *iccBuff = img->info->color->GetRAWICC();
		if (iccBuff)
		{
			UInt32 iccSize = ReadMUInt32(iccBuff);
			tmpBuff = MemAlloc(UInt8, iccSize + 35 + 11);
			*(Int32*)&tmpBuff[4] = *(Int32*)"iCCP";
			Text::StrConcat((Char*)&tmpBuff[8], "Photoshop ICC profile");
			tmpBuff[30] = 0;

			i = Data::Compress::Inflate::Compress(iccBuff, iccSize, &tmpBuff[31], true);
			if (i > 0)
			{
				WriteMInt32(&tmpBuff[0], 23 + (Int32)i);
				crc.Clear();
				crc.Calc(&tmpBuff[4], 27 + i);
				crc.GetValue(&tmpBuff[31 + i]);
				stm->Write(tmpBuff, 35 + i);
			}
			MemFree(tmpBuff);
		}
		else
		{
			if (img->info->color->rtransfer && img->info->color->rtransfer->GetTranType() == Media::CS::TRANT_GAMMA)
			{
				WriteMInt32(&hdr[0], 4);
				*(Int32*)&hdr[4] = *(Int32*)"gAMA";
				WriteMInt32(&hdr[8], Math::Double2Int32(100000.0 / img->info->color->rtransfer->GetGamma()));
				crc.Clear();
				crc.Calc(&hdr[4], 8);
				crc.GetValue(&hdr[12]);
				stm->Write(hdr, 16);
			}
			else if (img->info->color->rtransfer && img->info->color->rtransfer->GetTranType() == Media::CS::TRANT_sRGB)
			{
				WriteMInt32(&hdr[0], 1);
				*(Int32*)&hdr[4] = *(Int32*)"sRGB";
				hdr[8] = 0;
				crc.Clear();
				crc.Calc(&hdr[4], 5);
				crc.GetValue(&hdr[9]);
				stm->Write(hdr, 13);
			}
			
			if (img->info->color->primaries.colorType != Media::ColorProfile::CT_VUNKNOWN && img->info->color->primaries.colorType != Media::ColorProfile::CT_PUNKNOWN)
			{
				WriteMInt32(&hdr[0], 32);
				*(Int32*)&hdr[4] = *(Int32*)"cHRM";
				WriteMInt32(&hdr[8], Math::Double2Int32(100000.0 * img->info->color->primaries.wx));
				WriteMInt32(&hdr[12], Math::Double2Int32(100000.0 * img->info->color->primaries.wy));
				WriteMInt32(&hdr[16], Math::Double2Int32(100000.0 * img->info->color->primaries.rx));
				WriteMInt32(&hdr[20], Math::Double2Int32(100000.0 * img->info->color->primaries.ry));
				WriteMInt32(&hdr[24], Math::Double2Int32(100000.0 * img->info->color->primaries.gx));
				WriteMInt32(&hdr[28], Math::Double2Int32(100000.0 * img->info->color->primaries.gy));
				WriteMInt32(&hdr[32], Math::Double2Int32(100000.0 * img->info->color->primaries.bx));
				WriteMInt32(&hdr[36], Math::Double2Int32(100000.0 * img->info->color->primaries.by));
				crc.Clear();
				crc.Calc(&hdr[4], 36);
				crc.GetValue(&hdr[40]);
				stm->Write(hdr, 44);
			}
		}
	}

	if (img->info->hdpi != 72.0 || img->info->vdpi != 72.0)
	{
		Int32 hVal = Math::Double2Int32(1.0 / Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 1.0 / img->info->hdpi));
		Int32 vVal = Math::Double2Int32(1.0 / Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 1.0 / img->info->vdpi));
		WriteMInt32(&hdr[0], 9);
		*(Int32*)&hdr[4] = *(Int32*)"pHYs";
		WriteMInt32(&hdr[8], hVal);
		WriteMInt32(&hdr[12], vVal);
		hdr[16] = 1;
		crc.Clear();
		crc.Calc(&hdr[4], 13);
		crc.GetValue(&hdr[17]);
		stm->Write(hdr, 21);
	}

	switch (img->info->pf)
	{
	case Media::PF_PAL_1:
		PNGExporter_WritePal(stm, img, &crc);
	case Media::PF_PAL_W1:
		k = ((img->info->dispWidth + 7) >> 3);
		tmpBuff = MemAlloc(UInt8, (k + 1) * img->info->dispHeight);
		imgPtr1 = img->data;
		imgPtr2 = tmpBuff;
		i = img->info->dispHeight;
		while (i-- > 0)
		{
			j = img->info->dispWidth;
			*imgPtr2++ = 0;
			MemCopyNO(imgPtr2, imgPtr1, k);
			imgPtr2 += k;
			imgPtr1 += img->info->storeWidth >> 3;
		}
		tmpBuff2 = MemAlloc(UInt8, 12 + (k + 1) * img->info->dispHeight + 11);
		*(Int32*)&tmpBuff2[4] = *(Int32*)"IDAT";
		i = Data::Compress::Inflate::Compress(tmpBuff, (k + 1) * img->info->dispHeight, &tmpBuff2[8], true);
		if (i < 0)
		{
			i = 0;
		}
		WriteMInt32(&tmpBuff2[0], (Int32)i);
		crc.Clear();
		crc.Calc(&tmpBuff2[4], i + 4);
		crc.GetValue(&tmpBuff2[i + 8]);
		stm->Write(tmpBuff2, i + 12);
		MemFree(tmpBuff2);
		MemFree(tmpBuff);
		break;
	case Media::PF_PAL_2:
		PNGExporter_WritePal(stm, img, &crc);
	case Media::PF_PAL_W2:
		k = ((img->info->dispWidth + 3) >> 2);
		tmpBuff = MemAlloc(UInt8, (k + 1) * img->info->dispHeight);
		imgPtr1 = img->data;
		imgPtr2 = tmpBuff;
		i = img->info->dispHeight;
		while (i-- > 0)
		{
			j = img->info->dispWidth;
			*imgPtr2++ = 0;
			MemCopyNO(imgPtr2, imgPtr1, k);
			imgPtr2 += k;
			imgPtr1 += img->info->storeWidth >> 2;
		}
		tmpBuff2 = MemAlloc(UInt8, 12 + (k + 1) * img->info->dispHeight + 11);
		*(Int32*)&tmpBuff2[4] = *(Int32*)"IDAT";
		i = Data::Compress::Inflate::Compress(tmpBuff, (k + 1) * img->info->dispHeight, &tmpBuff2[8], true);
		if (i < 0)
		{
			i = 0;
		}
		WriteMInt32(&tmpBuff2[0], (Int32)i);
		crc.Clear();
		crc.Calc(&tmpBuff2[4], i + 4);
		crc.GetValue(&tmpBuff2[i + 8]);
		stm->Write(tmpBuff2, i + 12);
		MemFree(tmpBuff2);
		MemFree(tmpBuff);
		break;
	case Media::PF_PAL_4:
		PNGExporter_WritePal(stm, img, &crc);
	case Media::PF_PAL_W4:
		k = ((img->info->dispWidth + 1) >> 1);
		tmpBuff = MemAlloc(UInt8, (k + 1) * img->info->dispHeight);
		imgPtr1 = img->data;
		imgPtr2 = tmpBuff;
		i = img->info->dispHeight;
		while (i-- > 0)
		{
			j = img->info->dispWidth;
			*imgPtr2++ = 0;
			MemCopyNO(imgPtr2, imgPtr1, k);
			imgPtr2 += k;
			imgPtr1 += img->info->storeWidth >> 1;
		}
		tmpBuff2 = MemAlloc(UInt8, 12 + (k + 1) * img->info->dispHeight + 11);
		*(Int32*)&tmpBuff2[4] = *(Int32*)"IDAT";
		i = Data::Compress::Inflate::Compress(tmpBuff, (k + 1) * img->info->dispHeight, &tmpBuff2[8], true);
		if (i < 0)
		{
			i = 0;
		}
		WriteMInt32(&tmpBuff2[0], (Int32)i);
		crc.Clear();
		crc.Calc(&tmpBuff2[4], i + 4);
		crc.GetValue(&tmpBuff2[i + 8]);
		stm->Write(tmpBuff2, i + 12);
		MemFree(tmpBuff2);
		MemFree(tmpBuff);
		break;
	case Media::PF_PAL_8:
		PNGExporter_WritePal(stm, img, &crc);
	case Media::PF_PAL_W8:
		k = img->info->dispWidth;
		tmpBuff = MemAlloc(UInt8, (k + 1) * img->info->dispHeight);
		imgPtr1 = img->data;
		imgPtr2 = tmpBuff;
		i = img->info->dispHeight;
		while (i-- > 0)
		{
			j = img->info->dispWidth;
			*imgPtr2++ = 0;
			MemCopyNO(imgPtr2, imgPtr1, k);
			imgPtr2 += k;
			imgPtr1 += img->info->storeWidth;
		}
		PNGExporter_FilterByte(tmpBuff, k, img->info->dispHeight);
		tmpBuff2 = MemAlloc(UInt8, 12 + (k + 1) * img->info->dispHeight + 11);
		*(Int32*)&tmpBuff2[4] = *(Int32*)"IDAT";
		i = Data::Compress::Inflate::Compress(tmpBuff, (k + 1) * img->info->dispHeight, &tmpBuff2[8], true);
		if (i < 0)
		{
			i = 0;
		}
		WriteMInt32(&tmpBuff2[0], (Int32)i);
		crc.Clear();
		crc.Calc(&tmpBuff2[4], i + 4);
		crc.GetValue(&tmpBuff2[i + 8]);
		stm->Write(tmpBuff2, i + 12);
		MemFree(tmpBuff2);
		MemFree(tmpBuff);
		break;
	case Media::PF_LE_W16:
		k = img->info->dispWidth << 1;
		tmpBuff = MemAlloc(UInt8, (k + 1) * img->info->dispHeight);
		imgPtr1 = img->data;
		imgPtr2 = tmpBuff;
		i = img->info->dispHeight;
		while (i-- > 0)
		{
			j = img->info->dispWidth;
			*imgPtr2++ = 0;
			while (j-- > 0)
			{
				imgPtr2[0] = imgPtr1[1];
				imgPtr2[1] = imgPtr1[0];
				imgPtr2 += 2;
				imgPtr1 += 2;
			}
			imgPtr1 += (img->info->storeWidth - img->info->dispWidth) << 1;
		}
		PNGExporter_FilterByte2(tmpBuff, k, img->info->dispHeight);
		tmpBuff2 = MemAlloc(UInt8, 12 + (k + 1) * img->info->dispHeight + 11);
		*(Int32*)&tmpBuff2[4] = *(Int32*)"IDAT";
		i = Data::Compress::Inflate::Compress(tmpBuff, (k + 1) * img->info->dispHeight, &tmpBuff2[8], true);
		if (i < 0)
		{
			i = 0;
		}
		WriteMInt32(&tmpBuff2[0], (Int32)i);
		crc.Clear();
		crc.Calc(&tmpBuff2[4], i + 4);
		crc.GetValue(&tmpBuff2[i + 8]);
		stm->Write(tmpBuff2, i + 12);
		MemFree(tmpBuff2);
		MemFree(tmpBuff);
		break;
	case Media::PF_W8A8:
		k = img->info->dispWidth << 1;
		tmpBuff = MemAlloc(UInt8, (k + 1) * img->info->dispHeight);
		imgPtr1 = img->data;
		imgPtr2 = tmpBuff;
		i = img->info->dispHeight;
		while (i-- > 0)
		{
			j = img->info->dispWidth;
			*imgPtr2++ = 0;
			MemCopyNO(imgPtr2, imgPtr1, j << 1);
			imgPtr1 += img->info->storeWidth << 1;
			imgPtr2 += img->info->dispWidth << 1;
		}
		PNGExporter_FilterByte2(tmpBuff, k, img->info->dispHeight);
		tmpBuff2 = MemAlloc(UInt8, 12 + (k + 1) * img->info->dispHeight + 11);
		*(Int32*)&tmpBuff2[4] = *(Int32*)"IDAT";
		i = Data::Compress::Inflate::Compress(tmpBuff, (k + 1) * img->info->dispHeight, &tmpBuff2[8], true);
		if (i < 0)
		{
			i = 0;
		}
		WriteMInt32(&tmpBuff2[0], (Int32)i);
		crc.Clear();
		crc.Calc(&tmpBuff2[4], i + 4);
		crc.GetValue(&tmpBuff2[i + 8]);
		stm->Write(tmpBuff2, i + 12);
		MemFree(tmpBuff2);
		MemFree(tmpBuff);
		break;
	case Media::PF_LE_W16A16:
		k = img->info->dispWidth << 2;
		tmpBuff = MemAlloc(UInt8, (k + 1) * img->info->dispHeight);
		imgPtr1 = img->data;
		imgPtr2 = tmpBuff;
		i = img->info->dispHeight;
		while (i-- > 0)
		{
			j = img->info->dispWidth;
			*imgPtr2++ = 0;
			while (j-- > 0)
			{
				imgPtr2[0] = imgPtr1[1];
				imgPtr2[1] = imgPtr1[0];
				imgPtr2[2] = imgPtr1[3];
				imgPtr2[3] = imgPtr1[2];
				imgPtr2 += 4;
				imgPtr1 += 4;
			}
			imgPtr1 += (img->info->storeWidth - img->info->dispWidth) << 2;
		}
		PNGExporter_FilterByte4(tmpBuff, k, img->info->dispHeight);
		tmpBuff2 = MemAlloc(UInt8, 12 + (k + 1) * img->info->dispHeight + 11);
		*(Int32*)&tmpBuff2[4] = *(Int32*)"IDAT";
		i = Data::Compress::Inflate::Compress(tmpBuff, (k + 1) * img->info->dispHeight, &tmpBuff2[8], true);
		if (i < 0)
		{
			i = 0;
		}
		WriteMInt32(&tmpBuff2[0], (Int32)i);
		crc.Clear();
		crc.Calc(&tmpBuff2[4], i + 4);
		crc.GetValue(&tmpBuff2[i + 8]);
		stm->Write(tmpBuff2, i + 12);
		MemFree(tmpBuff2);
		MemFree(tmpBuff);
		break;
	case Media::PF_B8G8R8:
		k = img->info->dispWidth * 3;
		tmpBuff = MemAlloc(UInt8, (k + 1) * img->info->dispHeight);
		imgPtr1 = img->data;
		imgPtr2 = tmpBuff;
		i = img->info->dispHeight;
		while (i-- > 0)
		{
			j = img->info->dispWidth;
			*imgPtr2++ = 0;
			while (j-- > 0)
			{
				imgPtr2[0] = imgPtr1[2];
				imgPtr2[1] = imgPtr1[1];
				imgPtr2[2] = imgPtr1[0];
				imgPtr2 += 3;
				imgPtr1 += 3;
			}
			imgPtr1 += (img->info->storeWidth - img->info->dispWidth) * 3;
		}
		PNGExporter_FilterByte3(tmpBuff, k, img->info->dispHeight);
		tmpBuff2 = MemAlloc(UInt8, 12 + (k + 1) * img->info->dispHeight + 11);
		*(Int32*)&tmpBuff2[4] = *(Int32*)"IDAT";
		i = Data::Compress::Inflate::Compress(tmpBuff, (k + 1) * img->info->dispHeight, &tmpBuff2[8], true);
		if (i < 0)
		{
			i = 0;
		}
		WriteMInt32(&tmpBuff2[0], (Int32)i);
		crc.Clear();
		crc.Calc(&tmpBuff2[4], i + 4);
		crc.GetValue(&tmpBuff2[i + 8]);
		stm->Write(tmpBuff2, i + 12);
		MemFree(tmpBuff2);
		MemFree(tmpBuff);
		break;
	case Media::PF_B8G8R8A8:
		k = img->info->dispWidth << 2;
		tmpBuff = MemAlloc(UInt8, (k + 1) * img->info->dispHeight);
		imgPtr1 = img->data;
		imgPtr2 = tmpBuff;
		i = img->info->dispHeight;
		while (i-- > 0)
		{
			j = img->info->dispWidth;
			*imgPtr2++ = 0;
			while (j-- > 0)
			{
				imgPtr2[0] = imgPtr1[2];
				imgPtr2[1] = imgPtr1[1];
				imgPtr2[2] = imgPtr1[0];
				imgPtr2[3] = imgPtr1[3];
				imgPtr2 += 4;
				imgPtr1 += 4;
			}
			imgPtr1 += (img->info->storeWidth - img->info->dispWidth) << 2;
		}
		PNGExporter_FilterByte4(tmpBuff, k, img->info->dispHeight);
		tmpBuff2 = MemAlloc(UInt8, 12 + (k + 1) * img->info->dispHeight + 11);
		*(Int32*)&tmpBuff2[4] = *(Int32*)"IDAT";
		i = Data::Compress::Inflate::Compress(tmpBuff, (k + 1) * img->info->dispHeight, &tmpBuff2[8], true);
		if (i < 0)
		{
			i = 0;
		}
		WriteMInt32(&tmpBuff2[0], (Int32)i);
		crc.Clear();
		crc.Calc(&tmpBuff2[4], i + 4);
		crc.GetValue(&tmpBuff2[i + 8]);
		stm->Write(tmpBuff2, i + 12);
		MemFree(tmpBuff2);
		MemFree(tmpBuff);
		break;
	case Media::PF_LE_B16G16R16:
		k = img->info->dispWidth * 6;
		tmpBuff = MemAlloc(UInt8, (k + 1) * img->info->dispHeight);
		imgPtr1 = img->data;
		imgPtr2 = tmpBuff;
		i = img->info->dispHeight;
		while (i-- > 0)
		{
			j = img->info->dispWidth;
			*imgPtr2++ = 0;
			while (j-- > 0)
			{
				imgPtr2[0] = imgPtr1[5];
				imgPtr2[1] = imgPtr1[4];
				imgPtr2[2] = imgPtr1[3];
				imgPtr2[3] = imgPtr1[2];
				imgPtr2[4] = imgPtr1[1];
				imgPtr2[5] = imgPtr1[0];
				imgPtr2 += 6;
				imgPtr1 += 6;
			}
			imgPtr1 += (img->info->storeWidth - img->info->dispWidth) * 6;
		}
		PNGExporter_FilterByte6(tmpBuff, k, img->info->dispHeight);
		tmpBuff2 = MemAlloc(UInt8, 12 + (k + 1) * img->info->dispHeight + 11);
		*(Int32*)&tmpBuff2[4] = *(Int32*)"IDAT";
		i = Data::Compress::Inflate::Compress(tmpBuff, (k + 1) * img->info->dispHeight, &tmpBuff2[8], true);
		if (i < 0)
		{
			i = 0;
		}
		WriteMInt32(&tmpBuff2[0], (Int32)i);
		crc.Clear();
		crc.Calc(&tmpBuff2[4], i + 4);
		crc.GetValue(&tmpBuff2[i + 8]);
		stm->Write(tmpBuff2, i + 12);
		MemFree(tmpBuff2);
		MemFree(tmpBuff);
		break;
	case Media::PF_LE_B16G16R16A16:
		k = img->info->dispWidth << 3;
		tmpBuff = MemAlloc(UInt8, (k + 1) * img->info->dispHeight);
		imgPtr1 = img->data;
		imgPtr2 = tmpBuff;
		i = img->info->dispHeight;
		while (i-- > 0)
		{
			j = img->info->dispWidth;
			*imgPtr2++ = 0;
			while (j-- > 0)
			{
				imgPtr2[0] = imgPtr1[5];
				imgPtr2[1] = imgPtr1[4];
				imgPtr2[2] = imgPtr1[3];
				imgPtr2[3] = imgPtr1[2];
				imgPtr2[4] = imgPtr1[1];
				imgPtr2[5] = imgPtr1[0];
				imgPtr2[6] = imgPtr1[7];
				imgPtr2[7] = imgPtr1[6];
				imgPtr2 += 8;
				imgPtr1 += 8;
			}
			imgPtr1 += (img->info->storeWidth - img->info->dispWidth) << 3;
		}
		PNGExporter_FilterByte8(tmpBuff, k, img->info->dispHeight);
		tmpBuff2 = MemAlloc(UInt8, 12 + (k + 1) * img->info->dispHeight + 11);
		*(Int32*)&tmpBuff2[4] = *(Int32*)"IDAT";
		i = Data::Compress::Inflate::Compress(tmpBuff, (k + 1) * img->info->dispHeight, &tmpBuff2[8], true);
		if (i < 0)
		{
			i = 0;
		}
		WriteMInt32(&tmpBuff2[0], (Int32)i);
		crc.Clear();
		crc.Calc(&tmpBuff2[4], i + 4);
		crc.GetValue(&tmpBuff2[i + 8]);
		stm->Write(tmpBuff2, i + 12);
		MemFree(tmpBuff2);
		MemFree(tmpBuff);
		break;
	case Media::PF_LE_R5G5B5:
	case Media::PF_LE_R5G6B5:
	case Media::PF_LE_A2B10G10R10:
	case Media::PF_LE_FB32G32R32A32:
	case Media::PF_LE_FB32G32R32:
	case Media::PF_LE_FW32A32:
	case Media::PF_LE_FW32:
	case Media::PF_R8G8B8A8:
	case Media::PF_R8G8B8:
	case Media::PF_PAL_1_A1:
	case Media::PF_PAL_2_A1:
	case Media::PF_PAL_4_A1:
	case Media::PF_PAL_8_A1:
	case Media::PF_B8G8R8A1:
	case Media::PF_UNKNOWN:
	default:
		break;
	}
	

	WriteMInt32(&hdr[0], 0);
	*(Int32*)&hdr[4] = *(Int32*)"IEND";
	WriteMUInt32(&hdr[8], 0xAE426082);
	stm->Write(hdr, 12);
	return true;
}
