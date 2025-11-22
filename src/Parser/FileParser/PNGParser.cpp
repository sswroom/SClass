#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Core/ByteTool_C.h"
#include "Data/Compress/Inflater.h"
#include "IO/MemoryStream.h"
#include "IO/WriteCacheStream.h"
#include "Media/ICCProfile.h"
#include "Media/StaticImage.h"
#include "Math/Unit/Distance.h"
#include "Parser/FileParser/PNGParser.h"

UInt8 PNGParser_PaethPredictor(UInt8 a, UInt8 b, UInt8 c)
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

UnsafeArray<UInt8> PNGParser_ParsePixelsBits(UnsafeArray<UInt8> srcData, UInt8 *destBuff, UOSInt bpl, UOSInt initX, UOSInt initY, UOSInt maxX, UOSInt maxY, UOSInt xAdd, UOSInt yAdd, OSInt pxMask, OSInt pxAMask, OSInt pxShift)
{
	UInt8 *lineStart = destBuff + initY * bpl;
	UOSInt currX;
	UOSInt currY;
	OSInt pxId;
	UInt8 px;
	currY = initY;
	switch (pxShift)
	{
	case 1:
		while (currY < maxY)
		{
			currX = initX;
			srcData++;

			while (currX < maxX)
			{
				px = *srcData++;
				lineStart[currX] = (UInt8)(px >> 7);
				lineStart[currX + xAdd * 1] = (px >> 6) & 1;
				lineStart[currX + xAdd * 2] = (px >> 5) & 1;
				lineStart[currX + xAdd * 3] = (px >> 4) & 1;
				lineStart[currX + xAdd * 4] = (px >> 3) & 1;
				lineStart[currX + xAdd * 5] = (px >> 2) & 1;
				lineStart[currX + xAdd * 6] = (px >> 1) & 1;
				lineStart[currX + xAdd * 7] = px & 1;
				currX += xAdd * 8;
			}
			currY += yAdd;
			lineStart += bpl * yAdd;
		}
		break;
	case 2:
		while (currY < maxY)
		{
			currX = initX;
			srcData++;

			while (currX < maxX)
			{
				px = *srcData++;
				lineStart[currX] = (UInt8)(px >> 6);
				lineStart[currX + xAdd * 1] = (px >> 4) & 3;
				lineStart[currX + xAdd * 2] = (px >> 2) & 3;
				lineStart[currX + xAdd * 3] = px & 3;
				currX += xAdd * 4;
			}
			currY += yAdd;
			lineStart += bpl * yAdd;
		}
		break;
	case 4:
		while (currY < maxY)
		{
			currX = initX;
			srcData++;

			while (currX < maxX)
			{
				px = *srcData++;
				lineStart[currX] = (UInt8)(px >> 4);
				lineStart[currX + xAdd * 1] = px & 15;
				currX += xAdd * 2;
			}
			currY += yAdd;
			lineStart += bpl * yAdd;
		}
		break;
	default:
		while (currY < maxY)
		{
			currX = initX;
			pxId = 0;
			px = *srcData++;

			if (px == 0)
			{
				while (currX < maxX)
				{
					if ((pxId & pxMask) == 0)
					{
						px = *srcData++;
					}
					lineStart[currX] = (UInt8)((px >> (8 - pxShift)) & pxAMask);
					px = (UInt8)(px << pxShift);
					pxId++;
					currX += xAdd;
				}
			}
			else
			{
				while (currX < maxX)
				{
					if ((pxId & pxMask) == 0)
					{
						px = *srcData++;
					}
					lineStart[currX] = (UInt8)((px >> (8 - pxShift)) & pxAMask);
					px = (UInt8)(px << pxShift);
					pxId++;
					currX += xAdd;
				}
			}
			currY += yAdd;
			lineStart += bpl * yAdd;
		}

	}
	return srcData;
}

UnsafeArray<UInt8> PNGParser_ParsePixelsBits1(UnsafeArray<UInt8> srcData, UnsafeArray<UInt8> destBuff, UOSInt bpl, UOSInt initX, UOSInt initY, UOSInt maxX, UOSInt maxY, UOSInt xAdd, UOSInt yAdd)
{
	UnsafeArray<UInt8> lineStart = destBuff + initY * bpl;
	UOSInt currX;
	UOSInt currY;
	currY = initY;
	while (currY < maxY)
	{
		currX = initX;
		srcData++;

		while (currX < maxX)
		{
			lineStart[currX >> 3] = *srcData++;
			currX += xAdd * 8;
		}
		currY += yAdd;
		lineStart += bpl * yAdd;
	}
	return srcData;
}

UnsafeArray<UInt8> PNGParser_ParsePixelsBits2(UnsafeArray<UInt8> srcData, UnsafeArray<UInt8> destBuff, UOSInt bpl, UOSInt initX, UOSInt initY, UOSInt maxX, UOSInt maxY, UOSInt xAdd, UOSInt yAdd)
{
	UnsafeArray<UInt8> lineStart = destBuff + initY * bpl;
	UOSInt currX;
	UOSInt currY;
	currY = initY;
	while (currY < maxY)
	{
		currX = initX;
		srcData++;

		while (currX < maxX)
		{
			lineStart[currX >> 2] = *srcData++;
			currX += xAdd * 4;
		}
		currY += yAdd;
		lineStart += bpl * yAdd;
	}
	return srcData;
}

UnsafeArray<UInt8> PNGParser_ParsePixelsBits4(UnsafeArray<UInt8> srcData, UnsafeArray<UInt8> destBuff, UOSInt bpl, UOSInt initX, UOSInt initY, UOSInt maxX, UOSInt maxY, UOSInt xAdd, UOSInt yAdd)
{
	UnsafeArray<UInt8> lineStart = destBuff + initY * bpl;
	UOSInt currX;
	UOSInt currY;
	currY = initY;
	while (currY < maxY)
	{
		currX = initX;
		srcData++;

		while (currX < maxX)
		{
			lineStart[currX >> 1] = *srcData++;
			currX += xAdd * 2;
		}
		currY += yAdd;
		lineStart += bpl * yAdd;
	}
	return srcData;
}

UnsafeArray<UInt8> PNGParser_ParsePixelsByte(UnsafeArray<UInt8> srcData, UnsafeArray<UInt8> destBuff, UOSInt bpl, UOSInt initX, UOSInt initY, UOSInt maxX, UOSInt maxY, UOSInt xAdd, UOSInt yAdd)
{
	UnsafeArray<UInt8> lineStart = destBuff + initY * bpl;
	UOSInt currX;
	UOSInt currY;
	UInt8 px;
	UnsafeArrayOpt<UInt8> lastLineStart;
	UnsafeArray<UInt8> nnlastLineStart;
	UInt8 lastPx;
	lastLineStart = 0;
	currY = initY;
	while (currY < maxY)
	{
		currX = initX;
		px = *srcData++;
		if (px == 0)
		{
			while (currX < maxX)
			{
				lineStart[currX] = *srcData++;
				currX += xAdd;
			}
		}
		else if (px == 1)
		{
			lastPx = 0;
			while (currX < maxX)
			{
				lastPx = (UInt8)(lastPx + *srcData++);
				lineStart[currX] = lastPx;
				currX += xAdd;
			}
		}
		else if (px == 2)
		{
			if (lastLineStart.SetTo(nnlastLineStart))
			{
				while (currX < maxX)
				{
					lineStart[currX] = (UInt8)(nnlastLineStart[currX] + *srcData++);
					currX += xAdd;
				}
			}
			else
			{
				while (currX < maxX)
				{
					lineStart[currX] = *srcData++;
					currX += xAdd;
				}
			}
		}
		else if (px == 3)
		{
			if (lastLineStart.SetTo(nnlastLineStart))
			{
				lastPx = 0;
				while (currX < maxX)
				{
					lastPx = (UInt8)(((lastPx + nnlastLineStart[currX]) >> 1) + *srcData++);
					lineStart[currX] = lastPx;
					currX += xAdd;
				}
			}
			else
			{
				lastPx = 0;
				while (currX < maxX)
				{
					lastPx = (UInt8)((lastPx >> 1) + *srcData++);
					lineStart[currX] = lastPx;
					currX += xAdd;
				}
			}
		}
		else if (px == 4)
		{
			if (lastLineStart.SetTo(nnlastLineStart))
			{
				lastPx = 0;
				while (currX < maxX)
				{
					if (currX < xAdd)
					{
						lastPx = (UInt8)(nnlastLineStart[currX] + *srcData++);
					}
					else
					{
						lastPx = (UInt8)(PNGParser_PaethPredictor(lastPx, nnlastLineStart[currX], nnlastLineStart[currX - xAdd]) + *srcData++);
					}
					lineStart[currX] = lastPx;
					currX += xAdd;
				}
			}
			else
			{
				lastPx = 0;
				while (currX < maxX)
				{
					if (currX < xAdd)
					{
						lastPx = *srcData++;
					}
					else
					{
						lastPx = (UInt8)(lastPx + *srcData++);
					}
					lineStart[currX] = lastPx;
					currX += xAdd;
				}
			}
		}
		else
		{
			while (currX < maxX)
			{
				lineStart[currX] = *srcData++;
				currX += xAdd;
			}
		}
		currY += yAdd;
		lastLineStart = lineStart;
		lineStart += bpl * yAdd;
	}
	return srcData;
}

UnsafeArray<UInt8> PNGParser_ParsePixelsWord(UnsafeArray<UInt8> srcData, UnsafeArray<UInt8> destBuff, UOSInt bpl, UOSInt initX, UOSInt initY, UOSInt maxX, UOSInt maxY, UOSInt xAdd, UOSInt yAdd)
{
	UnsafeArray<UInt8> lineStart = destBuff + initY * bpl;
	UOSInt currX;
	UOSInt currY;
	UInt8 px;
	UnsafeArrayOpt<UInt8> lastLineStart;
	UnsafeArray<UInt8> nnlastLineStart;
	UInt8 lastPx[2];
	lastLineStart = 0;
	currY = initY;
	while (currY < maxY)
	{
		currX = initX;
		px = *srcData++;
		if (px == 0)
		{
			while (currX < maxX)
			{
				lineStart[currX * 2 + 0] = srcData[1];
				lineStart[currX * 2 + 1] = srcData[0];
				srcData += 2;
				currX += xAdd;
			}
		}
		else if (px == 1)
		{
			lastPx[0] = 0;
			lastPx[1] = 0;
			while (currX < maxX)
			{
				lastPx[0] = (UInt8)(lastPx[0] + srcData[1]);
				lastPx[1] = (UInt8)(lastPx[1] + srcData[0]);
				lineStart[currX * 2 + 0] = lastPx[0];
				lineStart[currX * 2 + 1] = lastPx[1];
				srcData += 2;
				currX += xAdd;
			}
		}
		else if (px == 2)
		{
			if (lastLineStart.SetTo(nnlastLineStart))
			{
				while (currX < maxX)
				{
					lineStart[currX * 2 + 0] = (UInt8)(nnlastLineStart[currX * 2 + 0] + srcData[1]);
					lineStart[currX * 2 + 1] = (UInt8)(nnlastLineStart[currX * 2 + 1] + srcData[0]);
					srcData += 2;
					currX += xAdd;
				}
			}
			else
			{
				while (currX < maxX)
				{
					lineStart[currX * 2 + 0] = srcData[1];
					lineStart[currX * 2 + 1] = srcData[0];
					srcData += 2;
					currX += xAdd;
				}
			}
		}
		else if (px == 3)
		{
			if (lastLineStart.SetTo(nnlastLineStart))
			{
				lastPx[0] = 0;
				lastPx[1] = 0;
				while (currX < maxX)
				{
					lastPx[0] = (UInt8)(((lastPx[0] + nnlastLineStart[currX * 2 + 0]) >> 1) + srcData[1]);
					lastPx[1] = (UInt8)(((lastPx[1] + nnlastLineStart[currX * 2 + 1]) >> 1) + srcData[0]);
					lineStart[currX * 2 + 0] = lastPx[0];
					lineStart[currX * 2 + 1] = lastPx[1];
					srcData += 2;
					currX += xAdd;
				}
			}
			else
			{
				lastPx[0] = 0;
				lastPx[1] = 0;
				while (currX < maxX)
				{
					lastPx[0] = (UInt8)((lastPx[0] >> 1) + srcData[1]);
					lastPx[1] = (UInt8)((lastPx[1] >> 1) + srcData[0]);
					lineStart[currX * 2 + 0] = lastPx[0];
					lineStart[currX * 2 + 1] = lastPx[1];
					srcData += 2;
					currX += xAdd;
				}
			}
		}
		else if (px == 4)
		{
			if (lastLineStart.SetTo(nnlastLineStart))
			{
				lastPx[0] = 0;
				lastPx[1] = 0;
				while (currX < maxX)
				{
					if (currX < xAdd)
					{
						lastPx[0] = (UInt8)(nnlastLineStart[currX * 2 + 0] + srcData[1]);
						lastPx[1] = (UInt8)(nnlastLineStart[currX * 2 + 1] + srcData[0]);
					}
					else
					{
						lastPx[0] = (UInt8)(PNGParser_PaethPredictor(lastPx[0], nnlastLineStart[currX * 2 + 0], nnlastLineStart[(currX - xAdd) * 2 + 0]) + srcData[1]);
						lastPx[1] = (UInt8)(PNGParser_PaethPredictor(lastPx[1], nnlastLineStart[currX * 2 + 1], nnlastLineStart[(currX - xAdd) * 2 + 1]) + srcData[0]);
					}
					lineStart[currX * 2 + 0] = lastPx[0];
					lineStart[currX * 2 + 1] = lastPx[1];
					srcData += 2;
					currX += xAdd;
				}
			}
			else
			{
				lastPx[0] = 0;
				lastPx[1] = 0;
				while (currX < maxX)
				{
					if (currX < xAdd)
					{
						lastPx[0] = srcData[1];
						lastPx[1] = srcData[0];
					}
					else
					{
						lastPx[0] = (UInt8)(lastPx[0] + srcData[1]);
						lastPx[1] = (UInt8)(lastPx[1] + srcData[0]);
					}
					lineStart[currX * 2 + 0] = lastPx[0];
					lineStart[currX * 2 + 1] = lastPx[1];
					srcData += 2;
					currX += xAdd;
				}
			}
		}
		else
		{
			while (currX < maxX)
			{
				lineStart[currX * 2 + 0] = srcData[1];
				lineStart[currX * 2 + 1] = srcData[0];
				srcData += 2;
				currX += xAdd;
			}
		}
		currY += yAdd;
		lastLineStart = lineStart;
		lineStart += bpl * yAdd;
	}
	return srcData;
}

UnsafeArray<UInt8> PNGParser_ParsePixelsRGB24(UnsafeArray<UInt8> srcData, UnsafeArray<UInt8> destBuff, UOSInt bpl, UOSInt initX, UOSInt initY, UOSInt maxX, UOSInt maxY, UOSInt xAdd, UOSInt yAdd)
{
	UnsafeArray<UInt8> lineStart = destBuff + initY * bpl;
	UOSInt currX;
	UOSInt currY;
	UInt8 px;
	UnsafeArrayOpt<UInt8> lastLineStart;
	UnsafeArray<UInt8> nnlastLineStart;
	UInt8 lastPx[3];
	lastLineStart = 0;
	currY = initY;
	while (currY < maxY)
	{
		currX = initX;
		px = *srcData++;
		if (px == 0)
		{
			while (currX < maxX)
			{
				lineStart[currX * 3 + 0] = srcData[2];
				lineStart[currX * 3 + 1] = srcData[1];
				lineStart[currX * 3 + 2] = srcData[0];
				srcData += 3;
				currX += xAdd;
			}
		}
		else if (px == 1)
		{
			lastPx[0] = 0;
			lastPx[1] = 0;
			lastPx[2] = 0;
			while (currX < maxX)
			{
				lastPx[0] = (UInt8)(lastPx[0] + srcData[2]);
				lastPx[1] = (UInt8)(lastPx[1] + srcData[1]);
				lastPx[2] = (UInt8)(lastPx[2] + srcData[0]);
				lineStart[currX * 3 + 0] = lastPx[0];
				lineStart[currX * 3 + 1] = lastPx[1];
				lineStart[currX * 3 + 2] = lastPx[2];
				srcData += 3;
				currX += xAdd;
			}
		}
		else if (px == 2)
		{
			if (lastLineStart.SetTo(nnlastLineStart))
			{
				while (currX < maxX)
				{
					lineStart[currX * 3 + 0] = (UInt8)(nnlastLineStart[currX * 3 + 0] + srcData[2]);
					lineStart[currX * 3 + 1] = (UInt8)(nnlastLineStart[currX * 3 + 1] + srcData[1]);
					lineStart[currX * 3 + 2] = (UInt8)(nnlastLineStart[currX * 3 + 2] + srcData[0]);
					srcData += 3;
					currX += xAdd;
				}
			}
			else
			{
				while (currX < maxX)
				{
					lineStart[currX * 3 + 0] = srcData[2];
					lineStart[currX * 3 + 1] = srcData[1];
					lineStart[currX * 3 + 2] = srcData[0];
					srcData += 3;
					currX += xAdd;
				}
			}
		}
		else if (px == 3)
		{
			if (lastLineStart.SetTo(nnlastLineStart))
			{
				lastPx[0] = 0;
				lastPx[1] = 0;
				lastPx[2] = 0;
				while (currX < maxX)
				{
					lastPx[0] = (UInt8)(((lastPx[0] + nnlastLineStart[currX * 3 + 0]) >> 1) + srcData[2]);
					lastPx[1] = (UInt8)(((lastPx[1] + nnlastLineStart[currX * 3 + 1]) >> 1) + srcData[1]);
					lastPx[2] = (UInt8)(((lastPx[2] + nnlastLineStart[currX * 3 + 2]) >> 1) + srcData[0]);
					lineStart[currX * 3 + 0] = lastPx[0];
					lineStart[currX * 3 + 1] = lastPx[1];
					lineStart[currX * 3 + 2] = lastPx[2];
					srcData += 3;
					currX += xAdd;
				}
			}
			else
			{
				lastPx[0] = 0;
				lastPx[1] = 0;
				lastPx[2] = 0;
				while (currX < maxX)
				{
					lastPx[0] = (UInt8)((lastPx[0] >> 1) + srcData[2]);
					lastPx[1] = (UInt8)((lastPx[1] >> 1) + srcData[1]);
					lastPx[2] = (UInt8)((lastPx[2] >> 1) + srcData[0]);
					lineStart[currX * 3 + 0] = lastPx[0];
					lineStart[currX * 3 + 1] = lastPx[1];
					lineStart[currX * 3 + 2] = lastPx[2];
					srcData += 3;
					currX += xAdd;
				}
			}
		}
		else if (px == 4)
		{
			if (lastLineStart.SetTo(nnlastLineStart))
			{
				lastPx[0] = 0;
				lastPx[1] = 0;
				lastPx[2] = 0;
				while (currX < maxX)
				{
					if (currX < xAdd)
					{
						lastPx[0] = (UInt8)(nnlastLineStart[currX * 3 + 0] + srcData[2]);
						lastPx[1] = (UInt8)(nnlastLineStart[currX * 3 + 1] + srcData[1]);
						lastPx[2] = (UInt8)(nnlastLineStart[currX * 3 + 2] + srcData[0]);
					}
					else
					{
						lastPx[0] = (UInt8)(PNGParser_PaethPredictor(lastPx[0], nnlastLineStart[currX * 3 + 0], nnlastLineStart[(currX - xAdd) * 3 + 0]) + srcData[2]);
						lastPx[1] = (UInt8)(PNGParser_PaethPredictor(lastPx[1], nnlastLineStart[currX * 3 + 1], nnlastLineStart[(currX - xAdd) * 3 + 1]) + srcData[1]);
						lastPx[2] = (UInt8)(PNGParser_PaethPredictor(lastPx[2], nnlastLineStart[currX * 3 + 2], nnlastLineStart[(currX - xAdd) * 3 + 2]) + srcData[0]);
					}
					lineStart[currX * 3 + 0] = lastPx[0];
					lineStart[currX * 3 + 1] = lastPx[1];
					lineStart[currX * 3 + 2] = lastPx[2];
					srcData += 3;
					currX += xAdd;
				}
			}
			else
			{
				lastPx[0] = 0;
				lastPx[1] = 0;
				lastPx[2] = 0;
				while (currX < maxX)
				{
					if (currX < xAdd)
					{
						lastPx[0] = srcData[2];
						lastPx[1] = srcData[1];
						lastPx[2] = srcData[0];
					}
					else
					{
						lastPx[0] = (UInt8)(lastPx[0] + srcData[2]);
						lastPx[1] = (UInt8)(lastPx[1] + srcData[1]);
						lastPx[2] = (UInt8)(lastPx[2] + srcData[0]);
					}
					lineStart[currX * 3 + 0] = lastPx[0];
					lineStart[currX * 3 + 1] = lastPx[1];
					lineStart[currX * 3 + 2] = lastPx[2];
					srcData += 3;
					currX += xAdd;
				}
			}
		}
		else
		{
			while (currX < maxX)
			{
				lineStart[currX * 3 + 0] = srcData[2];
				lineStart[currX * 3 + 1] = srcData[1];
				lineStart[currX * 3 + 2] = srcData[0];
				srcData += 3;
				currX += xAdd;
			}
		}
		currY += yAdd;
		lastLineStart = lineStart;
		lineStart += bpl * yAdd;
	}
	return srcData;
}

UnsafeArray<UInt8> PNGParser_ParsePixelsRGB48(UnsafeArray<UInt8> srcData, UnsafeArray<UInt8> destBuff, UOSInt bpl, UOSInt initX, UOSInt initY, UOSInt maxX, UOSInt maxY, UOSInt xAdd, UOSInt yAdd)
{
	UnsafeArray<UInt8> lineStart = destBuff + initY * bpl;
	UOSInt currX;
	UOSInt currY;
	UInt8 px;
	UnsafeArrayOpt<UInt8> lastLineStart;
	UnsafeArray<UInt8> nnlastLineStart;
	UInt8 lastPx[6];
	lastLineStart = 0;
	currY = initY;
	while (currY < maxY)
	{
		currX = initX;
		px = *srcData++;
		if (px == 0)
		{
			while (currX < maxX)
			{
				lineStart[currX * 6 + 0] = srcData[5];
				lineStart[currX * 6 + 1] = srcData[4];
				lineStart[currX * 6 + 2] = srcData[3];
				lineStart[currX * 6 + 3] = srcData[2];
				lineStart[currX * 6 + 4] = srcData[1];
				lineStart[currX * 6 + 5] = srcData[0];
				srcData += 6;
				currX += xAdd;
			}
		}
		else if (px == 1)
		{
			lastPx[0] = 0;
			lastPx[1] = 0;
			lastPx[2] = 0;
			lastPx[3] = 0;
			lastPx[4] = 0;
			lastPx[5] = 0;
			while (currX < maxX)
			{
				lastPx[0] = (UInt8)(lastPx[0] + srcData[5]);
				lastPx[1] = (UInt8)(lastPx[1] + srcData[4]);
				lastPx[2] = (UInt8)(lastPx[2] + srcData[3]);
				lastPx[3] = (UInt8)(lastPx[3] + srcData[2]);
				lastPx[4] = (UInt8)(lastPx[4] + srcData[1]);
				lastPx[5] = (UInt8)(lastPx[5] + srcData[0]);
				lineStart[currX * 6 + 0] = lastPx[0];
				lineStart[currX * 6 + 1] = lastPx[1];
				lineStart[currX * 6 + 2] = lastPx[2];
				lineStart[currX * 6 + 3] = lastPx[3];
				lineStart[currX * 6 + 4] = lastPx[4];
				lineStart[currX * 6 + 5] = lastPx[5];
				srcData += 6;
				currX += xAdd;
			}
		}
		else if (px == 2)
		{
			if (lastLineStart.SetTo(nnlastLineStart))
			{
				while (currX < maxX)
				{
					lineStart[currX * 6 + 0] = (UInt8)(nnlastLineStart[currX * 6 + 0] + srcData[5]);
					lineStart[currX * 6 + 1] = (UInt8)(nnlastLineStart[currX * 6 + 1] + srcData[4]);
					lineStart[currX * 6 + 2] = (UInt8)(nnlastLineStart[currX * 6 + 2] + srcData[3]);
					lineStart[currX * 6 + 3] = (UInt8)(nnlastLineStart[currX * 6 + 3] + srcData[2]);
					lineStart[currX * 6 + 4] = (UInt8)(nnlastLineStart[currX * 6 + 4] + srcData[1]);
					lineStart[currX * 6 + 5] = (UInt8)(nnlastLineStart[currX * 6 + 5] + srcData[0]);
					srcData += 6;
					currX += xAdd;
				}
			}
			else
			{
				while (currX < maxX)
				{
					lineStart[currX * 6 + 0] = srcData[5];
					lineStart[currX * 6 + 1] = srcData[4];
					lineStart[currX * 6 + 2] = srcData[3];
					lineStart[currX * 6 + 3] = srcData[2];
					lineStart[currX * 6 + 4] = srcData[1];
					lineStart[currX * 6 + 5] = srcData[0];
					srcData += 6;
					currX += xAdd;
				}
			}
		}
		else if (px == 3)
		{
			if (lastLineStart.SetTo(nnlastLineStart))
			{
				lastPx[0] = 0;
				lastPx[1] = 0;
				lastPx[2] = 0;
				lastPx[3] = 0;
				lastPx[4] = 0;
				lastPx[5] = 0;
				while (currX < maxX)
				{
					lastPx[0] = (UInt8)(((lastPx[0] + nnlastLineStart[currX * 6 + 0]) >> 1) + srcData[5]);
					lastPx[1] = (UInt8)(((lastPx[1] + nnlastLineStart[currX * 6 + 1]) >> 1) + srcData[4]);
					lastPx[2] = (UInt8)(((lastPx[2] + nnlastLineStart[currX * 6 + 2]) >> 1) + srcData[3]);
					lastPx[3] = (UInt8)(((lastPx[3] + nnlastLineStart[currX * 6 + 3]) >> 1) + srcData[2]);
					lastPx[4] = (UInt8)(((lastPx[4] + nnlastLineStart[currX * 6 + 4]) >> 1) + srcData[1]);
					lastPx[5] = (UInt8)(((lastPx[5] + nnlastLineStart[currX * 6 + 5]) >> 1) + srcData[0]);
					lineStart[currX * 6 + 0] = lastPx[0];
					lineStart[currX * 6 + 1] = lastPx[1];
					lineStart[currX * 6 + 2] = lastPx[2];
					lineStart[currX * 6 + 3] = lastPx[3];
					lineStart[currX * 6 + 4] = lastPx[4];
					lineStart[currX * 6 + 5] = lastPx[5];
					srcData += 6;
					currX += xAdd;
				}
			}
			else
			{
				lastPx[0] = 0;
				lastPx[1] = 0;
				lastPx[2] = 0;
				lastPx[3] = 0;
				lastPx[4] = 0;
				lastPx[5] = 0;
				while (currX < maxX)
				{
					lastPx[0] = (UInt8)((lastPx[0] >> 1) + srcData[5]);
					lastPx[1] = (UInt8)((lastPx[1] >> 1) + srcData[4]);
					lastPx[2] = (UInt8)((lastPx[2] >> 1) + srcData[3]);
					lastPx[3] = (UInt8)((lastPx[3] >> 1) + srcData[2]);
					lastPx[4] = (UInt8)((lastPx[4] >> 1) + srcData[1]);
					lastPx[5] = (UInt8)((lastPx[5] >> 1) + srcData[0]);
					lineStart[currX * 6 + 0] = lastPx[0];
					lineStart[currX * 6 + 1] = lastPx[1];
					lineStart[currX * 6 + 2] = lastPx[2];
					lineStart[currX * 6 + 3] = lastPx[3];
					lineStart[currX * 6 + 4] = lastPx[4];
					lineStart[currX * 6 + 5] = lastPx[5];
					srcData += 6;
					currX += xAdd;
				}
			}
		}
		else if (px == 4)
		{
			if (lastLineStart.SetTo(nnlastLineStart))
			{
				lastPx[0] = 0;
				lastPx[1] = 0;
				lastPx[2] = 0;
				lastPx[3] = 0;
				lastPx[4] = 0;
				lastPx[5] = 0;
				while (currX < maxX)
				{
					if (currX < xAdd)
					{
						lastPx[0] = (UInt8)(nnlastLineStart[currX * 6 + 0] + srcData[5]);
						lastPx[1] = (UInt8)(nnlastLineStart[currX * 6 + 1] + srcData[4]);
						lastPx[2] = (UInt8)(nnlastLineStart[currX * 6 + 2] + srcData[3]);
						lastPx[3] = (UInt8)(nnlastLineStart[currX * 6 + 3] + srcData[2]);
						lastPx[4] = (UInt8)(nnlastLineStart[currX * 6 + 4] + srcData[1]);
						lastPx[5] = (UInt8)(nnlastLineStart[currX * 6 + 5] + srcData[0]);
					}
					else
					{
						lastPx[0] = (UInt8)(PNGParser_PaethPredictor(lastPx[0], nnlastLineStart[currX * 6 + 0], nnlastLineStart[(currX - xAdd) * 6 + 0]) + srcData[5]);
						lastPx[1] = (UInt8)(PNGParser_PaethPredictor(lastPx[1], nnlastLineStart[currX * 6 + 1], nnlastLineStart[(currX - xAdd) * 6 + 1]) + srcData[4]);
						lastPx[2] = (UInt8)(PNGParser_PaethPredictor(lastPx[2], nnlastLineStart[currX * 6 + 2], nnlastLineStart[(currX - xAdd) * 6 + 2]) + srcData[3]);
						lastPx[3] = (UInt8)(PNGParser_PaethPredictor(lastPx[3], nnlastLineStart[currX * 6 + 3], nnlastLineStart[(currX - xAdd) * 6 + 3]) + srcData[2]);
						lastPx[4] = (UInt8)(PNGParser_PaethPredictor(lastPx[4], nnlastLineStart[currX * 6 + 4], nnlastLineStart[(currX - xAdd) * 6 + 4]) + srcData[1]);
						lastPx[5] = (UInt8)(PNGParser_PaethPredictor(lastPx[5], nnlastLineStart[currX * 6 + 5], nnlastLineStart[(currX - xAdd) * 6 + 5]) + srcData[0]);
					}
					lineStart[currX * 6 + 0] = lastPx[0];
					lineStart[currX * 6 + 1] = lastPx[1];
					lineStart[currX * 6 + 2] = lastPx[2];
					lineStart[currX * 6 + 3] = lastPx[3];
					lineStart[currX * 6 + 4] = lastPx[4];
					lineStart[currX * 6 + 5] = lastPx[5];
					srcData += 6;
					currX += xAdd;
				}
			}
			else
			{
				lastPx[0] = 0;
				lastPx[1] = 0;
				lastPx[2] = 0;
				lastPx[3] = 0;
				lastPx[4] = 0;
				lastPx[5] = 0;
				while (currX < maxX)
				{
					if (currX < xAdd)
					{
						lastPx[0] = srcData[5];
						lastPx[1] = srcData[4];
						lastPx[2] = srcData[3];
						lastPx[3] = srcData[2];
						lastPx[4] = srcData[1];
						lastPx[5] = srcData[0];
					}
					else
					{
						lastPx[0] = (UInt8)(lastPx[0] + srcData[5]);
						lastPx[1] = (UInt8)(lastPx[1] + srcData[4]);
						lastPx[2] = (UInt8)(lastPx[2] + srcData[3]);
						lastPx[3] = (UInt8)(lastPx[3] + srcData[2]);
						lastPx[4] = (UInt8)(lastPx[4] + srcData[1]);
						lastPx[5] = (UInt8)(lastPx[5] + srcData[0]);
					}
					lineStart[currX * 6 + 0] = lastPx[0];
					lineStart[currX * 6 + 1] = lastPx[1];
					lineStart[currX * 6 + 2] = lastPx[2];
					lineStart[currX * 6 + 3] = lastPx[3];
					lineStart[currX * 6 + 4] = lastPx[4];
					lineStart[currX * 6 + 5] = lastPx[5];
					srcData += 6;
					currX += xAdd;
				}
			}
		}
		else
		{
			while (currX < maxX)
			{
				lineStart[currX * 6 + 0] = srcData[5];
				lineStart[currX * 6 + 1] = srcData[4];
				lineStart[currX * 6 + 2] = srcData[3];
				lineStart[currX * 6 + 3] = srcData[2];
				lineStart[currX * 6 + 4] = srcData[1];
				lineStart[currX * 6 + 5] = srcData[0];
				srcData += 6;
				currX += xAdd;
			}
		}
		currY += yAdd;
		lastLineStart = lineStart;
		lineStart += bpl * yAdd;
	}
	return srcData;
}

UnsafeArray<UInt8> PNGParser_ParsePixelsARGB32(UnsafeArray<UInt8> srcData, UnsafeArray<UInt8> destBuff, UOSInt bpl, UOSInt initX, UOSInt initY, UOSInt maxX, UOSInt maxY, UOSInt xAdd, UOSInt yAdd, Bool *alphaFound)
{
	UnsafeArray<UInt8> lineStart = destBuff + initY * bpl;
	UOSInt currX;
	UOSInt currY;
	UInt8 px;
	UnsafeArrayOpt<UInt8> lastLineStart;
	UnsafeArray<UInt8> nnlastLineStart;
	UInt8 lastPx[4];
	UInt8 a;
	UInt8 aAnd = 0xff;
	lastLineStart = 0;
	currY = initY;
	while (currY < maxY)
	{
		currX = initX;
		px = *srcData++;
		if (px == 0)
		{
			while (currX < maxX)
			{
				lineStart[currX * 4 + 0] = srcData[2];
				lineStart[currX * 4 + 1] = srcData[1];
				lineStart[currX * 4 + 2] = srcData[0];
				a = lineStart[currX * 4 + 3] = srcData[3];
				aAnd &= a;
				srcData += 4;
				currX += xAdd;
			}
		}
		else if (px == 1)
		{
			lastPx[0] = 0;
			lastPx[1] = 0;
			lastPx[2] = 0;
			lastPx[3] = 0;
			while (currX < maxX)
			{
				lastPx[0] = (UInt8)(lastPx[0] + srcData[2]);
				lastPx[1] = (UInt8)(lastPx[1] + srcData[1]);
				lastPx[2] = (UInt8)(lastPx[2] + srcData[0]);
				lastPx[3] = (UInt8)(lastPx[3] + srcData[3]);
				WriteNUInt32(&lineStart[currX * 4], ReadNUInt32(&lastPx[0]));
				aAnd &= lastPx[3];
				srcData += 4;
				currX += xAdd;
			}
		}
		else if (px == 2)
		{
			if (lastLineStart.SetTo(nnlastLineStart))
			{
				while (currX < maxX)
				{
					lineStart[currX * 4 + 0] = (UInt8)(nnlastLineStart[currX * 4 + 0] + srcData[2]);
					lineStart[currX * 4 + 1] = (UInt8)(nnlastLineStart[currX * 4 + 1] + srcData[1]);
					lineStart[currX * 4 + 2] = (UInt8)(nnlastLineStart[currX * 4 + 2] + srcData[0]);
					a = lineStart[currX * 4 + 3] = (UInt8)(nnlastLineStart[currX * 4 + 3] + srcData[3]);
					aAnd &= a;
					srcData += 4;
					currX += xAdd;
				}
			}
			else
			{
				while (currX < maxX)
				{
					lineStart[currX * 4 + 0] = srcData[2];
					lineStart[currX * 4 + 1] = srcData[1];
					lineStart[currX * 4 + 2] = srcData[0];
					a = lineStart[currX * 4 + 3] = srcData[3];
					aAnd &= a;
					srcData += 4;
					currX += xAdd;
				}
			}
		}
		else if (px == 3)
		{
			if (lastLineStart.SetTo(nnlastLineStart))
			{
				lastPx[0] = 0;
				lastPx[1] = 0;
				lastPx[2] = 0;
				lastPx[3] = 0;
				while (currX < maxX)
				{
					lastPx[0] = (UInt8)(((lastPx[0] + nnlastLineStart[currX * 4 + 0]) >> 1) + srcData[2]);
					lastPx[1] = (UInt8)(((lastPx[1] + nnlastLineStart[currX * 4 + 1]) >> 1) + srcData[1]);
					lastPx[2] = (UInt8)(((lastPx[2] + nnlastLineStart[currX * 4 + 2]) >> 1) + srcData[0]);
					lastPx[3] = (UInt8)(((lastPx[3] + nnlastLineStart[currX * 4 + 3]) >> 1) + srcData[3]);
					WriteNUInt32(&lineStart[currX * 4], ReadNUInt32(&lastPx[0]));
					aAnd &= lastPx[3];
					srcData += 4;
					currX += xAdd;
				}
			}
			else
			{
				lastPx[0] = 0;
				lastPx[1] = 0;
				lastPx[2] = 0;
				lastPx[3] = 0;
				while (currX < maxX)
				{
					lastPx[0] = (UInt8)((lastPx[0] >> 1) + srcData[2]);
					lastPx[1] = (UInt8)((lastPx[1] >> 1) + srcData[1]);
					lastPx[2] = (UInt8)((lastPx[2] >> 1) + srcData[0]);
					lastPx[3] = (UInt8)((lastPx[3] >> 1) + srcData[3]);
					WriteNUInt32(&lineStart[currX * 4], ReadNUInt32(&lastPx[0]));
					aAnd &= lastPx[3];
					srcData += 4;
					currX += xAdd;
				}
			}
		}
		else if (px == 4)
		{
			if (lastLineStart.SetTo(nnlastLineStart))
			{
				lastPx[0] = 0;
				lastPx[1] = 0;
				lastPx[2] = 0;
				lastPx[3] = 0;
				while (currX < maxX)
				{
					if (currX < xAdd)
					{
						lastPx[0] = (UInt8)(nnlastLineStart[currX * 4 + 0] + srcData[2]);
						lastPx[1] = (UInt8)(nnlastLineStart[currX * 4 + 1] + srcData[1]);
						lastPx[2] = (UInt8)(nnlastLineStart[currX * 4 + 2] + srcData[0]);
						lastPx[3] = (UInt8)(nnlastLineStart[currX * 4 + 3] + srcData[3]);
					}
					else
					{
						lastPx[0] = (UInt8)(PNGParser_PaethPredictor(lastPx[0], nnlastLineStart[currX * 4 + 0], nnlastLineStart[(currX - xAdd) * 4 + 0]) + srcData[2]);
						lastPx[1] = (UInt8)(PNGParser_PaethPredictor(lastPx[1], nnlastLineStart[currX * 4 + 1], nnlastLineStart[(currX - xAdd) * 4 + 1]) + srcData[1]);
						lastPx[2] = (UInt8)(PNGParser_PaethPredictor(lastPx[2], nnlastLineStart[currX * 4 + 2], nnlastLineStart[(currX - xAdd) * 4 + 2]) + srcData[0]);
						lastPx[3] = (UInt8)(PNGParser_PaethPredictor(lastPx[3], nnlastLineStart[currX * 4 + 3], nnlastLineStart[(currX - xAdd) * 4 + 3]) + srcData[3]);
					}
					WriteNUInt32(&lineStart[currX * 4], ReadNUInt32(&lastPx[0]));
					aAnd &= lastPx[3];
					srcData += 4;
					currX += xAdd;
				}
			}
			else
			{
				lastPx[0] = 0;
				lastPx[1] = 0;
				lastPx[2] = 0;
				lastPx[3] = 0;
				while (currX < maxX)
				{
					if (currX < xAdd)
					{
						lastPx[0] = srcData[2];
						lastPx[1] = srcData[1];
						lastPx[2] = srcData[0];
						lastPx[3] = srcData[3];
					}
					else
					{
						lastPx[0] = (UInt8)(lastPx[0] + srcData[2]);
						lastPx[1] = (UInt8)(lastPx[1] + srcData[1]);
						lastPx[2] = (UInt8)(lastPx[2] + srcData[0]);
						lastPx[3] = (UInt8)(lastPx[3] + srcData[3]);
					}
					WriteNUInt32(&lineStart[currX * 4], ReadNUInt32(&lastPx[0]));
					aAnd &= lastPx[3];
					srcData += 4;
					currX += xAdd;
				}
			}
		}
		else
		{
			while (currX < maxX)
			{
				lineStart[currX * 4 + 0] = srcData[2];
				lineStart[currX * 4 + 1] = srcData[1];
				lineStart[currX * 4 + 2] = srcData[0];
				a = lineStart[currX * 4 + 3] = srcData[3];
				aAnd &= a;
				srcData += 4;
				currX += xAdd;
			}
		}
		currY += yAdd;
		lastLineStart = lineStart;
		lineStart += bpl * yAdd;
	}
	if (aAnd != 0xff)
	{
		*alphaFound = true;
	}
	return srcData;
}

UnsafeArray<UInt8> PNGParser_ParsePixelsARGB64(UnsafeArray<UInt8> srcData, UnsafeArray<UInt8> destBuff, UOSInt bpl, UOSInt initX, UOSInt initY, UOSInt maxX, UOSInt maxY, UOSInt xAdd, UOSInt yAdd, Bool *alphaFound)
{
	UnsafeArray<UInt8> lineStart = destBuff + initY * bpl;
	UOSInt currX;
	UOSInt currY;
	UInt8 px;
	UnsafeArrayOpt<UInt8> lastLineStart;
	UnsafeArray<UInt8> nnlastLineStart;
	UInt8 lastPx[8];
	UInt8 a0;
	UInt8 a1;
	Bool semiTr = false;
	lastLineStart = 0;
	currY = initY;
	while (currY < maxY)
	{
		currX = initX;
		px = *srcData++;
		if (px == 0)
		{
			while (currX < maxX)
			{
				lineStart[currX * 8 + 0] = srcData[5];
				lineStart[currX * 8 + 1] = srcData[4];
				lineStart[currX * 8 + 2] = srcData[3];
				lineStart[currX * 8 + 3] = srcData[2];
				lineStart[currX * 8 + 4] = srcData[1];
				lineStart[currX * 8 + 5] = srcData[0];
				a0 = lineStart[currX * 8 + 6] = srcData[7];
				a1 = lineStart[currX * 8 + 7] = srcData[6];
				if (a0 != 0xff || a1 != 0xff)
				{
					semiTr = true;
				}
				srcData += 8;
				currX += xAdd;
			}
		}
		else if (px == 1)
		{
			lastPx[0] = 0;
			lastPx[1] = 0;
			lastPx[2] = 0;
			lastPx[3] = 0;
			lastPx[4] = 0;
			lastPx[5] = 0;
			lastPx[6] = 0;
			lastPx[7] = 0;
			while (currX < maxX)
			{
				lastPx[0] = (UInt8)(lastPx[0] + srcData[5]);
				lastPx[1] = (UInt8)(lastPx[1] + srcData[4]);
				lastPx[2] = (UInt8)(lastPx[2] + srcData[3]);
				lastPx[3] = (UInt8)(lastPx[3] + srcData[2]);
				lastPx[4] = (UInt8)(lastPx[4] + srcData[1]);
				lastPx[5] = (UInt8)(lastPx[5] + srcData[0]);
				lastPx[6] = (UInt8)(lastPx[6] + srcData[7]);
				lastPx[7] = (UInt8)(lastPx[7] + srcData[6]);
				lineStart[currX * 8 + 0] = lastPx[0];
				lineStart[currX * 8 + 1] = lastPx[1];
				lineStart[currX * 8 + 2] = lastPx[2];
				lineStart[currX * 8 + 3] = lastPx[3];
				lineStart[currX * 8 + 4] = lastPx[4];
				lineStart[currX * 8 + 5] = lastPx[5];
				lineStart[currX * 8 + 6] = lastPx[6];
				lineStart[currX * 8 + 7] = lastPx[7];
				if (lastPx[6] != 0xff || lastPx[7] != 0xff)
				{
					semiTr = true;
				}
				srcData += 8;
				currX += xAdd;
			}
		}
		else if (px == 2)
		{
			if (lastLineStart.SetTo(nnlastLineStart))
			{
				while (currX < maxX)
				{
					lineStart[currX * 8 + 0] = (UInt8)(nnlastLineStart[currX * 8 + 0] + srcData[5]);
					lineStart[currX * 8 + 1] = (UInt8)(nnlastLineStart[currX * 8 + 1] + srcData[4]);
					lineStart[currX * 8 + 2] = (UInt8)(nnlastLineStart[currX * 8 + 2] + srcData[3]);
					lineStart[currX * 8 + 3] = (UInt8)(nnlastLineStart[currX * 8 + 3] + srcData[2]);
					lineStart[currX * 8 + 4] = (UInt8)(nnlastLineStart[currX * 8 + 4] + srcData[1]);
					lineStart[currX * 8 + 5] = (UInt8)(nnlastLineStart[currX * 8 + 5] + srcData[0]);
					a0 = lineStart[currX * 8 + 6] = (UInt8)(nnlastLineStart[currX * 8 + 6] + srcData[7]);
					a1 = lineStart[currX * 8 + 7] = (UInt8)(nnlastLineStart[currX * 8 + 7] + srcData[6]);
					if (a0 != 0xff || a1 != 0xff)
					{
						semiTr = true;
					}
					srcData += 8;
					currX += xAdd;
				}
			}
			else
			{
				while (currX < maxX)
				{
					lineStart[currX * 8 + 0] = srcData[5];
					lineStart[currX * 8 + 1] = srcData[4];
					lineStart[currX * 8 + 2] = srcData[3];
					lineStart[currX * 8 + 3] = srcData[2];
					lineStart[currX * 8 + 4] = srcData[1];
					lineStart[currX * 8 + 5] = srcData[0];
					a0 = lineStart[currX * 8 + 6] = srcData[7];
					a1 = lineStart[currX * 8 + 7] = srcData[6];
					if (a0 != 0xff || a1 != 0xff)
					{
						semiTr = true;
					}
					srcData += 8;
					currX += xAdd;
				}
			}
		}
		else if (px == 3)
		{
			if (lastLineStart.SetTo(nnlastLineStart))
			{
				lastPx[0] = 0;
				lastPx[1] = 0;
				lastPx[2] = 0;
				lastPx[3] = 0;
				lastPx[4] = 0;
				lastPx[5] = 0;
				lastPx[6] = 0;
				lastPx[7] = 0;
				while (currX < maxX)
				{
					lastPx[0] = (UInt8)(((lastPx[0] + nnlastLineStart[currX * 8 + 0]) >> 1) + srcData[5]);
					lastPx[1] = (UInt8)(((lastPx[1] + nnlastLineStart[currX * 8 + 1]) >> 1) + srcData[4]);
					lastPx[2] = (UInt8)(((lastPx[2] + nnlastLineStart[currX * 8 + 2]) >> 1) + srcData[3]);
					lastPx[3] = (UInt8)(((lastPx[3] + nnlastLineStart[currX * 8 + 3]) >> 1) + srcData[2]);
					lastPx[4] = (UInt8)(((lastPx[4] + nnlastLineStart[currX * 8 + 4]) >> 1) + srcData[1]);
					lastPx[5] = (UInt8)(((lastPx[5] + nnlastLineStart[currX * 8 + 5]) >> 1) + srcData[0]);
					lastPx[6] = (UInt8)(((lastPx[6] + nnlastLineStart[currX * 8 + 6]) >> 1) + srcData[7]);
					lastPx[7] = (UInt8)(((lastPx[7] + nnlastLineStart[currX * 8 + 7]) >> 1) + srcData[6]);
					lineStart[currX * 8 + 0] = lastPx[0];
					lineStart[currX * 8 + 1] = lastPx[1];
					lineStart[currX * 8 + 2] = lastPx[2];
					lineStart[currX * 8 + 3] = lastPx[3];
					lineStart[currX * 8 + 4] = lastPx[4];
					lineStart[currX * 8 + 5] = lastPx[5];
					lineStart[currX * 8 + 6] = lastPx[6];
					lineStart[currX * 8 + 7] = lastPx[7];
					if (lastPx[6] != 0xff || lastPx[7] != 0xff)
					{
						semiTr = true;
					}
					srcData += 8;
					currX += xAdd;
				}
			}
			else
			{
				lastPx[0] = 0;
				lastPx[1] = 0;
				lastPx[2] = 0;
				lastPx[3] = 0;
				lastPx[4] = 0;
				lastPx[5] = 0;
				lastPx[6] = 0;
				lastPx[7] = 0;
				while (currX < maxX)
				{
					lastPx[0] = (UInt8)((lastPx[0] >> 1) + srcData[5]);
					lastPx[1] = (UInt8)((lastPx[1] >> 1) + srcData[4]);
					lastPx[2] = (UInt8)((lastPx[2] >> 1) + srcData[3]);
					lastPx[3] = (UInt8)((lastPx[3] >> 1) + srcData[2]);
					lastPx[4] = (UInt8)((lastPx[4] >> 1) + srcData[1]);
					lastPx[5] = (UInt8)((lastPx[5] >> 1) + srcData[0]);
					lastPx[6] = (UInt8)((lastPx[6] >> 1) + srcData[7]);
					lastPx[7] = (UInt8)((lastPx[7] >> 1) + srcData[6]);
					lineStart[currX * 8 + 0] = lastPx[0];
					lineStart[currX * 8 + 1] = lastPx[1];
					lineStart[currX * 8 + 2] = lastPx[2];
					lineStart[currX * 8 + 3] = lastPx[3];
					lineStart[currX * 8 + 4] = lastPx[4];
					lineStart[currX * 8 + 5] = lastPx[5];
					lineStart[currX * 8 + 6] = lastPx[6];
					lineStart[currX * 8 + 7] = lastPx[7];
					if (lastPx[6] != 0xff || lastPx[7] != 0xff)
					{
						semiTr = true;
					}
					srcData += 8;
					currX += xAdd;
				}
			}
		}
		else if (px == 4)
		{
			if (lastLineStart.SetTo(nnlastLineStart))
			{
				lastPx[0] = 0;
				lastPx[1] = 0;
				lastPx[2] = 0;
				lastPx[3] = 0;
				lastPx[4] = 0;
				lastPx[5] = 0;
				lastPx[6] = 0;
				lastPx[7] = 0;
				while (currX < maxX)
				{
					if (currX < xAdd)
					{
						lastPx[0] = (UInt8)(nnlastLineStart[currX * 8 + 0] + srcData[5]);
						lastPx[1] = (UInt8)(nnlastLineStart[currX * 8 + 1] + srcData[4]);
						lastPx[2] = (UInt8)(nnlastLineStart[currX * 8 + 2] + srcData[3]);
						lastPx[3] = (UInt8)(nnlastLineStart[currX * 8 + 3] + srcData[2]);
						lastPx[4] = (UInt8)(nnlastLineStart[currX * 8 + 4] + srcData[1]);
						lastPx[5] = (UInt8)(nnlastLineStart[currX * 8 + 5] + srcData[0]);
						lastPx[6] = (UInt8)(nnlastLineStart[currX * 8 + 6] + srcData[7]);
						lastPx[7] = (UInt8)(nnlastLineStart[currX * 8 + 7] + srcData[6]);
					}
					else
					{
						lastPx[0] = (UInt8)(PNGParser_PaethPredictor(lastPx[0], nnlastLineStart[currX * 8 + 0], nnlastLineStart[(currX - xAdd) * 8 + 0]) + srcData[5]);
						lastPx[1] = (UInt8)(PNGParser_PaethPredictor(lastPx[1], nnlastLineStart[currX * 8 + 1], nnlastLineStart[(currX - xAdd) * 8 + 1]) + srcData[4]);
						lastPx[2] = (UInt8)(PNGParser_PaethPredictor(lastPx[2], nnlastLineStart[currX * 8 + 2], nnlastLineStart[(currX - xAdd) * 8 + 2]) + srcData[3]);
						lastPx[3] = (UInt8)(PNGParser_PaethPredictor(lastPx[3], nnlastLineStart[currX * 8 + 3], nnlastLineStart[(currX - xAdd) * 8 + 3]) + srcData[2]);
						lastPx[4] = (UInt8)(PNGParser_PaethPredictor(lastPx[4], nnlastLineStart[currX * 8 + 4], nnlastLineStart[(currX - xAdd) * 8 + 4]) + srcData[1]);
						lastPx[5] = (UInt8)(PNGParser_PaethPredictor(lastPx[5], nnlastLineStart[currX * 8 + 5], nnlastLineStart[(currX - xAdd) * 8 + 5]) + srcData[0]);
						lastPx[6] = (UInt8)(PNGParser_PaethPredictor(lastPx[6], nnlastLineStart[currX * 8 + 6], nnlastLineStart[(currX - xAdd) * 8 + 6]) + srcData[7]);
						lastPx[7] = (UInt8)(PNGParser_PaethPredictor(lastPx[7], nnlastLineStart[currX * 8 + 7], nnlastLineStart[(currX - xAdd) * 8 + 7]) + srcData[6]);
					}
					lineStart[currX * 8 + 0] = lastPx[0];
					lineStart[currX * 8 + 1] = lastPx[1];
					lineStart[currX * 8 + 2] = lastPx[2];
					lineStart[currX * 8 + 3] = lastPx[3];
					lineStart[currX * 8 + 4] = lastPx[4];
					lineStart[currX * 8 + 5] = lastPx[5];
					lineStart[currX * 8 + 6] = lastPx[6];
					lineStart[currX * 8 + 7] = lastPx[7];
					if (lastPx[6] != 0xff || lastPx[7] != 0xff)
					{
						semiTr = true;
					}
					srcData += 8;
					currX += xAdd;
				}
			}
			else
			{
				lastPx[0] = 0;
				lastPx[1] = 0;
				lastPx[2] = 0;
				lastPx[3] = 0;
				lastPx[4] = 0;
				lastPx[5] = 0;
				lastPx[6] = 0;
				lastPx[7] = 0;
				while (currX < maxX)
				{
					if (currX < xAdd)
					{
						lastPx[0] = srcData[5];
						lastPx[1] = srcData[4];
						lastPx[2] = srcData[3];
						lastPx[3] = srcData[2];
						lastPx[4] = srcData[1];
						lastPx[5] = srcData[0];
						lastPx[6] = srcData[7];
						lastPx[7] = srcData[6];
					}
					else
					{
						lastPx[0] = (UInt8)(lastPx[0] + srcData[5]);
						lastPx[1] = (UInt8)(lastPx[1] + srcData[4]);
						lastPx[2] = (UInt8)(lastPx[2] + srcData[3]);
						lastPx[3] = (UInt8)(lastPx[3] + srcData[2]);
						lastPx[4] = (UInt8)(lastPx[4] + srcData[1]);
						lastPx[5] = (UInt8)(lastPx[5] + srcData[0]);
						lastPx[6] = (UInt8)(lastPx[6] + srcData[7]);
						lastPx[7] = (UInt8)(lastPx[7] + srcData[6]);
					}
					lineStart[currX * 8 + 0] = lastPx[0];
					lineStart[currX * 8 + 1] = lastPx[1];
					lineStart[currX * 8 + 2] = lastPx[2];
					lineStart[currX * 8 + 3] = lastPx[3];
					lineStart[currX * 8 + 4] = lastPx[4];
					lineStart[currX * 8 + 5] = lastPx[5];
					lineStart[currX * 8 + 6] = lastPx[6];
					lineStart[currX * 8 + 7] = lastPx[7];
					if (lastPx[6] != 0xff || lastPx[7] != 0xff)
					{
						semiTr = true;
					}
					srcData += 8;
					currX += xAdd;
				}
			}
		}
		else
		{
			while (currX < maxX)
			{
				lineStart[currX * 8 + 0] = srcData[5];
				lineStart[currX * 8 + 1] = srcData[4];
				lineStart[currX * 8 + 2] = srcData[3];
				lineStart[currX * 8 + 3] = srcData[2];
				lineStart[currX * 8 + 4] = srcData[1];
				lineStart[currX * 8 + 5] = srcData[0];
				a0 = lineStart[currX * 8 + 6] = srcData[7];
				a1 = lineStart[currX * 8 + 7] = srcData[6];
				if (a0 != 0xff || a1 != 0xff)
				{
					semiTr = true;
				}
				srcData += 8;
				currX += xAdd;
			}
		}
		currY += yAdd;
		lastLineStart = lineStart;
		lineStart += bpl * yAdd;
	}
	if (semiTr)
	{
		*alphaFound = true;
	}
	return srcData;
}

UnsafeArray<UInt8> PNGParser_ParsePixelsAW16(UnsafeArray<UInt8> srcData, UnsafeArray<UInt8> destBuff, UOSInt bpl, UOSInt initX, UOSInt initY, UOSInt maxX, UOSInt maxY, UOSInt xAdd, UOSInt yAdd, Bool *alphaFound)
{
	UnsafeArray<UInt8> lineStart = destBuff + initY * bpl;
	UOSInt currX;
	UOSInt currY;
	UInt8 px;
	UnsafeArrayOpt<UInt8> lastLineStart;
	UnsafeArray<UInt8> nnlastLineStart;
	UInt8 lastPx[2];
	UInt8 a;
	Bool semiTr = false;
	lastLineStart = 0;
	currY = initY;
	while (currY < maxY)
	{
		currX = initX;
		px = *srcData++;
		if (px == 0)
		{
			while (currX < maxX)
			{
				lineStart[currX * 2 + 0] = srcData[0];
				a = lineStart[currX * 2 + 1] = srcData[1];
				if (a != 0xff)
				{
					semiTr = true;
				}
				srcData += 2;
				currX += xAdd;
			}
		}
		else if (px == 1)
		{
			lastPx[0] = 0;
			lastPx[1] = 0;
			while (currX < maxX)
			{
				lastPx[0] = (UInt8)(lastPx[0] + srcData[0]);
				lastPx[1] = (UInt8)(lastPx[1] + srcData[1]);
				lineStart[currX * 2 + 0] = lastPx[0];
				lineStart[currX * 2 + 1] = lastPx[1];
				if (lastPx[1] != 0xff)
				{
					semiTr = true;
				}
				srcData += 2;
				currX += xAdd;
			}
		}
		else if (px == 2)
		{
			if (lastLineStart.SetTo(nnlastLineStart))
			{
				while (currX < maxX)
				{
					lineStart[currX * 2 + 0] = (UInt8)(nnlastLineStart[currX * 2 + 0] + srcData[0]);
					a = lineStart[currX * 2 + 1] = (UInt8)(nnlastLineStart[currX * 2 + 1] + srcData[1]);
					if (a != 0xff)
					{
						semiTr = true;
					}
					srcData += 2;
					currX += xAdd;
				}
			}
			else
			{
				while (currX < maxX)
				{
					lineStart[currX * 2 + 0] = srcData[0];
					a = lineStart[currX * 2 + 1] = srcData[1];
					if (a != 0xff)
					{
						semiTr = true;
					}
					srcData += 2;
					currX += xAdd;
				}
			}
		}
		else if (px == 3)
		{
			if (lastLineStart.SetTo(nnlastLineStart))
			{
				lastPx[0] = 0;
				lastPx[1] = 0;
				while (currX < maxX)
				{
					lastPx[0] = (UInt8)(((lastPx[0] + nnlastLineStart[currX * 2 + 0]) >> 1) + srcData[0]);
					lastPx[1] = (UInt8)(((lastPx[1] + nnlastLineStart[currX * 2 + 1]) >> 1) + srcData[1]);
					lineStart[currX * 2 + 0] = lastPx[0];
					lineStart[currX * 2 + 1] = lastPx[1];
					if (lastPx[1] != 0xff)
					{
						semiTr = true;
					}
					srcData += 2;
					currX += xAdd;
				}
			}
			else
			{
				lastPx[0] = 0;
				lastPx[1] = 0;
				while (currX < maxX)
				{
					lastPx[0] = (UInt8)((lastPx[0] >> 1) + srcData[0]);
					lastPx[1] = (UInt8)((lastPx[1] >> 1) + srcData[1]);
					lineStart[currX * 2 + 0] = lastPx[0];
					lineStart[currX * 2 + 1] = lastPx[1];
					if (lastPx[1] != 0xff)
					{
						semiTr = true;
					}
					srcData += 2;
					currX += xAdd;
				}
			}
		}
		else if (px == 4)
		{
			if (lastLineStart.SetTo(nnlastLineStart))
			{
				lastPx[0] = 0;
				lastPx[1] = 0;
				while (currX < maxX)
				{
					if (currX < xAdd)
					{
						lastPx[0] = (UInt8)(nnlastLineStart[currX * 2 + 0] + srcData[0]);
						lastPx[1] = (UInt8)(nnlastLineStart[currX * 2 + 1] + srcData[1]);
					}
					else
					{
						lastPx[0] = (UInt8)(PNGParser_PaethPredictor(lastPx[0], nnlastLineStart[currX * 2 + 0], nnlastLineStart[(currX - xAdd) * 2 + 0]) + srcData[0]);
						lastPx[1] = (UInt8)(PNGParser_PaethPredictor(lastPx[1], nnlastLineStart[currX * 2 + 1], nnlastLineStart[(currX - xAdd) * 2 + 1]) + srcData[1]);
					}
					lineStart[currX * 2 + 0] = lastPx[0];
					lineStart[currX * 2 + 1] = lastPx[1];
					if (lastPx[1] != 0xff)
					{
						semiTr = true;
					}
					srcData += 2;
					currX += xAdd;
				}
			}
			else
			{
				lastPx[0] = 0;
				lastPx[1] = 0;
				while (currX < maxX)
				{
					if (currX < xAdd)
					{
						lastPx[0] = srcData[0];
						lastPx[1] = srcData[1];
					}
					else
					{
						lastPx[0] = (UInt8)(lastPx[0] + srcData[0]);
						lastPx[1] = (UInt8)(lastPx[1] + srcData[1]);
					}
					lineStart[currX * 2 + 0] = lastPx[0];
					lineStart[currX * 2 + 1] = lastPx[1];
					if (lastPx[1] != 0xff)
					{
						semiTr = true;
					}
					srcData += 2;
					currX += xAdd;
				}
			}
		}
		else
		{
			while (currX < maxX)
			{
				lineStart[currX * 2 + 0] = srcData[0];
				a = lineStart[currX * 2 + 1] = srcData[1];
				if (a != 0xff)
				{
					semiTr = true;
				}
				srcData += 2;
				currX += xAdd;
			}
		}
		currY += yAdd;
		lastLineStart = lineStart;
		lineStart += bpl * yAdd;
	}
	if (semiTr)
	{
		*alphaFound = true;
	}
	return srcData;
}

UnsafeArray<UInt8> PNGParser_ParsePixelsAW32(UnsafeArray<UInt8> srcData, UnsafeArray<UInt8> destBuff, UOSInt bpl, UOSInt initX, UOSInt initY, UOSInt maxX, UOSInt maxY, UOSInt xAdd, UOSInt yAdd, Bool *alphaFound)
{
	UnsafeArray<UInt8> lineStart = destBuff + initY * bpl;
	UOSInt currX;
	UOSInt currY;
	UInt8 px;
	UnsafeArrayOpt<UInt8> lastLineStart;
	UnsafeArray<UInt8> nnlastLineStart;
	UInt8 lastPx[4];
	UInt8 a0;
	UInt8 a1;
	Bool semiTr = false;
	lastLineStart = 0;
	currY = initY;
	while (currY < maxY)
	{
		currX = initX;
		px = *srcData++;
		if (px == 0)
		{
			while (currX < maxX)
			{
				lineStart[currX * 4 + 0] = srcData[1];
				lineStart[currX * 4 + 1] = srcData[0];
				a0 = lineStart[currX * 4 + 2] = srcData[3];
				a1 = lineStart[currX * 4 + 3] = srcData[2];
				if (a0 != 0xff || a1 != 0xff)
				{
					semiTr = true;
				}
				srcData += 4;
				currX += xAdd;
			}
		}
		else if (px == 1)
		{
			lastPx[0] = 0;
			lastPx[1] = 0;
			lastPx[2] = 0;
			lastPx[3] = 0;
			while (currX < maxX)
			{
				lastPx[0] = (UInt8)(lastPx[0] + srcData[1]);
				lastPx[1] = (UInt8)(lastPx[1] + srcData[0]);
				lastPx[2] = (UInt8)(lastPx[2] + srcData[3]);
				lastPx[3] = (UInt8)(lastPx[3] + srcData[2]);
				lineStart[currX * 4 + 0] = lastPx[0];
				lineStart[currX * 4 + 1] = lastPx[1];
				lineStart[currX * 4 + 2] = lastPx[2];
				lineStart[currX * 4 + 3] = lastPx[3];
				if (lastPx[2] != 0xff || lastPx[3] != 0xff)
				{
					semiTr = true;
				}
				srcData += 4;
				currX += xAdd;
			}
		}
		else if (px == 2)
		{
			if (lastLineStart.SetTo(nnlastLineStart))
			{
				while (currX < maxX)
				{
					lineStart[currX * 4 + 0] = (UInt8)(nnlastLineStart[currX * 4 + 0] + srcData[1]);
					lineStart[currX * 4 + 1] = (UInt8)(nnlastLineStart[currX * 4 + 1] + srcData[0]);
					a0 = lineStart[currX * 4 + 2] = (UInt8)(nnlastLineStart[currX * 4 + 2] + srcData[3]);
					a1 = lineStart[currX * 4 + 3] = (UInt8)(nnlastLineStart[currX * 4 + 3] + srcData[2]);
					if (a0 != 0xff || a1 != 0xff)
					{
						semiTr = true;
					}
					srcData += 4;
					currX += xAdd;
				}
			}
			else
			{
				while (currX < maxX)
				{
					lineStart[currX * 4 + 0] = srcData[1];
					lineStart[currX * 4 + 1] = srcData[0];
					a0 = lineStart[currX * 4 + 2] = srcData[3];
					a1 = lineStart[currX * 4 + 3] = srcData[2];
					if (a0 != 0xff || a1 != 0xff)
					{
						semiTr = true;
					}
					srcData += 4;
					currX += xAdd;
				}
			}
		}
		else if (px == 3)
		{
			if (lastLineStart.SetTo(nnlastLineStart))
			{
				lastPx[0] = 0;
				lastPx[1] = 0;
				lastPx[2] = 0;
				lastPx[3] = 0;
				while (currX < maxX)
				{
					lastPx[0] = (UInt8)(((lastPx[0] + nnlastLineStart[currX * 4 + 0]) >> 1) + srcData[1]);
					lastPx[1] = (UInt8)(((lastPx[1] + nnlastLineStart[currX * 4 + 1]) >> 1) + srcData[0]);
					lastPx[2] = (UInt8)(((lastPx[2] + nnlastLineStart[currX * 4 + 2]) >> 1) + srcData[3]);
					lastPx[3] = (UInt8)(((lastPx[3] + nnlastLineStart[currX * 4 + 3]) >> 1) + srcData[2]);
					lineStart[currX * 4 + 0] = lastPx[0];
					lineStart[currX * 4 + 1] = lastPx[1];
					lineStart[currX * 4 + 2] = lastPx[2];
					lineStart[currX * 4 + 3] = lastPx[3];
					if (lastPx[2] != 0xff || lastPx[3] != 0xff)
					{
						semiTr = true;
					}
					srcData += 4;
					currX += xAdd;
				}
			}
			else
			{
				lastPx[0] = 0;
				lastPx[1] = 0;
				lastPx[2] = 0;
				lastPx[3] = 0;
				while (currX < maxX)
				{
					lastPx[0] = (UInt8)((lastPx[0] >> 1) + srcData[1]);
					lastPx[1] = (UInt8)((lastPx[1] >> 1) + srcData[0]);
					lastPx[2] = (UInt8)((lastPx[2] >> 1) + srcData[3]);
					lastPx[3] = (UInt8)((lastPx[3] >> 1) + srcData[2]);
					lineStart[currX * 4 + 0] = lastPx[0];
					lineStart[currX * 4 + 1] = lastPx[1];
					lineStart[currX * 4 + 2] = lastPx[2];
					lineStart[currX * 4 + 3] = lastPx[3];
					if (lastPx[2] != 0xff || lastPx[3] != 0xff)
					{
						semiTr = true;
					}
					srcData += 4;
					currX += xAdd;
				}
			}
		}
		else if (px == 4)
		{
			if (lastLineStart.SetTo(nnlastLineStart))
			{
				lastPx[0] = 0;
				lastPx[1] = 0;
				lastPx[2] = 0;
				lastPx[3] = 0;
				while (currX < maxX)
				{
					if (currX < xAdd)
					{
						lastPx[0] = (UInt8)(nnlastLineStart[currX * 4 + 0] + srcData[1]);
						lastPx[1] = (UInt8)(nnlastLineStart[currX * 4 + 1] + srcData[0]);
						lastPx[2] = (UInt8)(nnlastLineStart[currX * 4 + 2] + srcData[3]);
						lastPx[3] = (UInt8)(nnlastLineStart[currX * 4 + 3] + srcData[2]);
					}
					else
					{
						lastPx[0] = (UInt8)(PNGParser_PaethPredictor(lastPx[0], nnlastLineStart[currX * 4 + 0], nnlastLineStart[(currX - xAdd) * 4 + 0]) + srcData[1]);
						lastPx[1] = (UInt8)(PNGParser_PaethPredictor(lastPx[1], nnlastLineStart[currX * 4 + 1], nnlastLineStart[(currX - xAdd) * 4 + 1]) + srcData[0]);
						lastPx[2] = (UInt8)(PNGParser_PaethPredictor(lastPx[2], nnlastLineStart[currX * 4 + 2], nnlastLineStart[(currX - xAdd) * 4 + 2]) + srcData[3]);
						lastPx[3] = (UInt8)(PNGParser_PaethPredictor(lastPx[3], nnlastLineStart[currX * 4 + 3], nnlastLineStart[(currX - xAdd) * 4 + 3]) + srcData[2]);
					}
					lineStart[currX * 4 + 0] = lastPx[0];
					lineStart[currX * 4 + 1] = lastPx[1];
					lineStart[currX * 4 + 2] = lastPx[2];
					lineStart[currX * 4 + 3] = lastPx[3];
					if (lastPx[2] != 0xff || lastPx[3] != 0xff)
					{
						semiTr = true;
					}
					srcData += 4;
					currX += xAdd;
				}
			}
			else
			{
				lastPx[0] = 0;
				lastPx[1] = 0;
				lastPx[2] = 0;
				lastPx[3] = 0;
				while (currX < maxX)
				{
					if (currX < xAdd)
					{
						lastPx[0] = srcData[1];
						lastPx[1] = srcData[0];
						lastPx[2] = srcData[3];
						lastPx[3] = srcData[2];
					}
					else
					{
						lastPx[0] = (UInt8)(lastPx[0] + srcData[1]);
						lastPx[1] = (UInt8)(lastPx[1] + srcData[0]);
						lastPx[2] = (UInt8)(lastPx[2] + srcData[3]);
						lastPx[3] = (UInt8)(lastPx[3] + srcData[2]);
					}
					lineStart[currX * 4 + 0] = lastPx[0];
					lineStart[currX * 4 + 1] = lastPx[1];
					lineStart[currX * 4 + 2] = lastPx[2];
					lineStart[currX * 4 + 3] = lastPx[3];
					if (lastPx[2] != 0xff || lastPx[3] != 0xff)
					{
						semiTr = true;
					}
					srcData += 4;
					currX += xAdd;
				}
			}
		}
		else
		{
			while (currX < maxX)
			{
				lineStart[currX * 4 + 0] = srcData[1];
				lineStart[currX * 4 + 1] = srcData[0];
				a0 = lineStart[currX * 4 + 2] = srcData[3];
				a1 = lineStart[currX * 4 + 3] = srcData[2];
				if (a0 != 0xff || a1 != 0xff)
				{
					semiTr = true;
				}
				srcData += 4;
				currX += xAdd;
			}
		}
		currY += yAdd;
		lastLineStart = lineStart;
		lineStart += bpl * yAdd;
	}
	if (semiTr)
	{
		*alphaFound = true;
	}
	return srcData;
}

void Parser::FileParser::PNGParser::ParseImage(UInt8 bitDepth, UInt8 colorType, UnsafeArray<UInt8> dataBuff, NN<Media::FrameInfo> info, NN<Media::ImageList> imgList, UInt32 imgDelay, UInt32 imgX, UInt32 imgY, UInt32 imgW, UInt32 imgH, UInt8 interlaceMeth, UnsafeArrayOpt<UInt8> palette, Bool palHasAlpha)
{
	NN<Media::StaticImage> simg;
	UnsafeArray<UInt8> spal;
	UnsafeArray<UInt8> nnpal;
	switch (colorType)
	{
	case 0: //Grayscale
		if (bitDepth < 8)
		{
			UOSInt storeWidth = ((info->dispSize.x + 15) >> 4) << 4;
			UInt8 *tmpData = MemAllocA(UInt8, storeWidth * info->dispSize.y);
			UnsafeArray<UInt8> lineStart;
			OSInt pxMask;
			OSInt pxAMask;
			OSInt pxShift;
			if (bitDepth == 1)
			{
				pxMask = 7;
				pxAMask = 1;
				pxShift = 1;
			}
			else if (bitDepth == 2)
			{
				pxMask = 3;
				pxAMask = 3;
				pxShift = 2;
			}
			else if (bitDepth == 4)
			{
				pxMask = 1;
				pxAMask = 15;
				pxShift = 4;
			}
			else
			{
				pxMask = 1;
				pxAMask = 15;
				pxShift = 4;
			}

			MemClearAC(tmpData, storeWidth * info->dispSize.y);
				
			if (interlaceMeth == 1)
			{
				//Pass1
				dataBuff = PNGParser_ParsePixelsBits(dataBuff, tmpData + imgY * storeWidth + imgX, storeWidth, 0, 0, imgW, imgH, 8, 8, pxMask, pxAMask, pxShift);
				//Pass2
				dataBuff = PNGParser_ParsePixelsBits(dataBuff, tmpData + imgY * storeWidth + imgX, storeWidth, 4, 0, imgW, imgH, 8, 8, pxMask, pxAMask, pxShift);
				//Pass3
				dataBuff = PNGParser_ParsePixelsBits(dataBuff, tmpData + imgY * storeWidth + imgX, storeWidth, 0, 4, imgW, imgH, 4, 8, pxMask, pxAMask, pxShift);
				//Pass4
				dataBuff = PNGParser_ParsePixelsBits(dataBuff, tmpData + imgY * storeWidth + imgX, storeWidth, 2, 0, imgW, imgH, 4, 4, pxMask, pxAMask, pxShift);
				//Pass5
				dataBuff = PNGParser_ParsePixelsBits(dataBuff, tmpData + imgY * storeWidth + imgX, storeWidth, 0, 2, imgW, imgH, 2, 4, pxMask, pxAMask, pxShift);
				//Pass6
				dataBuff = PNGParser_ParsePixelsBits(dataBuff, tmpData + imgY * storeWidth + imgX, storeWidth, 1, 0, imgW, imgH, 2, 2, pxMask, pxAMask, pxShift);
				//Pass7
				dataBuff = PNGParser_ParsePixelsBits(dataBuff, tmpData + imgY * storeWidth + imgX, storeWidth, 0, 1, imgW, imgH, 1, 2, pxMask, pxAMask, pxShift);
			}
			else
			{
				dataBuff = PNGParser_ParsePixelsBits(dataBuff, tmpData + imgY * storeWidth + imgX, storeWidth, 0, 0, imgW, imgH, 1, 1, pxMask, pxAMask, pxShift);
			}

			info->atype = Media::AT_ALPHA_ALL_FF;
			info->storeSize.x = storeWidth;
			UOSInt byteCnt;
			if (bitDepth == 1)
			{
				info->storeBPP = 1;
				info->pf = Media::PF_PAL_W1;
				info->byteSize = storeWidth * info->storeSize.y >> 3;
				NEW_CLASSNN(simg, Media::StaticImage(info));
				if (simg->pal.SetTo(spal))
				{
					WriteUInt32(&spal[0], 0xff000000);
					WriteUInt32(&spal[4], 0xffffffff);
				}

				byteCnt = info->byteSize;
				dataBuff = tmpData;
				lineStart = simg->data;
				while (byteCnt-- > 0)
				{
					*lineStart++ = (UInt8)((dataBuff[0] << 7) | (dataBuff[1] << 6) | (dataBuff[2] << 5) | (dataBuff[3] << 4) | (dataBuff[4] << 3) | (dataBuff[5] << 2) | (dataBuff[6] << 1) | dataBuff[7]);
					dataBuff += 8;
				}

				imgList->AddImage(simg, imgDelay);
			}
			else if (bitDepth == 2)
			{
				info->storeBPP = 2;
				info->pf = Media::PF_PAL_W2;
				info->byteSize = storeWidth * info->storeSize.y >> 2;
				NEW_CLASSNN(simg, Media::StaticImage(info));
				if (simg->pal.SetTo(spal))
				{
					WriteUInt32(&spal[0], 0xff000000);
					WriteUInt32(&spal[4], 0xff555555);
					WriteUInt32(&spal[8], 0xffaaaaaa);
					WriteUInt32(&spal[12], 0xffffffff);
				}

				byteCnt = info->byteSize;
				dataBuff = tmpData;
				lineStart = simg->data;
				while (byteCnt-- > 0)
				{
					*lineStart++ = (UInt8)((dataBuff[0] << 6) | (dataBuff[1] << 4) | (dataBuff[2] << 2) | dataBuff[3]);
					dataBuff += 4;
				}

				imgList->AddImage(simg, imgDelay);
			}
			else if (bitDepth == 4)
			{
				info->storeBPP = 4;
				info->pf = Media::PF_PAL_W4;
				info->byteSize = storeWidth * info->storeSize.y >> 1;
				NEW_CLASSNN(simg, Media::StaticImage(info));
				if (simg->pal.SetTo(spal))
				{
					WriteUInt32(&spal[0], 0xff000000);
					WriteUInt32(&spal[4], 0xff111111);
					WriteUInt32(&spal[8], 0xff222222);
					WriteUInt32(&spal[12], 0xff333333);
					WriteUInt32(&spal[16], 0xff444444);
					WriteUInt32(&spal[20], 0xff555555);
					WriteUInt32(&spal[24], 0xff666666);
					WriteUInt32(&spal[28], 0xff777777);
					WriteUInt32(&spal[32], 0xff888888);
					WriteUInt32(&spal[36], 0xff999999);
					WriteUInt32(&spal[40], 0xffaaaaaa);
					WriteUInt32(&spal[44], 0xffbbbbbb);
					WriteUInt32(&spal[48], 0xffcccccc);
					WriteUInt32(&spal[52], 0xffdddddd);
					WriteUInt32(&spal[56], 0xffeeeeee);
					WriteUInt32(&spal[60], 0xffffffff);
				}

				byteCnt = info->byteSize;
				dataBuff = tmpData;
				lineStart = simg->data;
				while (byteCnt-- > 0)
				{
					*lineStart++ = (UInt8)((dataBuff[0] << 4) | dataBuff[1]);
					dataBuff += 2;
				}

				imgList->AddImage(simg, imgDelay);
			}

			MemFreeA(tmpData);
		}
		else if (bitDepth == 8)
		{
			UInt32 pxId;

			info->atype = Media::AT_ALPHA_ALL_FF;
			info->storeSize.x = info->dispSize.x;
			info->storeBPP = 8;
			info->pf = Media::PF_PAL_W8;
			info->byteSize = info->storeSize.CalcArea();
			NEW_CLASSNN(simg, Media::StaticImage(info));
			if (simg->pal.SetTo(spal))
			{
				pxId = 0;
				while (pxId < 256)
				{
					WriteUInt32(&spal[pxId << 2], (0xff000000 | (pxId << 16) | (pxId << 8) | pxId));
					pxId++;
				}
			}
			if (info->dispSize.x != imgW || info->dispSize.y != imgH)
			{
				MemClearAC(simg->data.Ptr(), info->byteSize);
			}

			if (interlaceMeth == 1)
			{
				//Pass1
				dataBuff = PNGParser_ParsePixelsByte(dataBuff, simg->data + imgY * info->storeSize.x + imgX, info->storeSize.x, 0, 0, imgW, imgH, 8, 8);
				//Pass2
				dataBuff = PNGParser_ParsePixelsByte(dataBuff, simg->data + imgY * info->storeSize.x + imgX, info->storeSize.x, 4, 0, imgW, imgH, 8, 8);
				//Pass3
				dataBuff = PNGParser_ParsePixelsByte(dataBuff, simg->data + imgY * info->storeSize.x + imgX, info->storeSize.x, 0, 4, imgW, imgH, 4, 8);
				//Pass4
				dataBuff = PNGParser_ParsePixelsByte(dataBuff, simg->data + imgY * info->storeSize.x + imgX, info->storeSize.x, 2, 0, imgW, imgH, 4, 4);
				//Pass5
				dataBuff = PNGParser_ParsePixelsByte(dataBuff, simg->data + imgY * info->storeSize.x + imgX, info->storeSize.x, 0, 2, imgW, imgH, 2, 4);
				//Pass6
				dataBuff = PNGParser_ParsePixelsByte(dataBuff, simg->data + imgY * info->storeSize.x + imgX, info->storeSize.x, 1, 0, imgW, imgH, 2, 2);
				//Pass7
				dataBuff = PNGParser_ParsePixelsByte(dataBuff, simg->data + imgY * info->storeSize.x + imgX, info->storeSize.x, 0, 1, imgW, imgH, 1, 2);
			}
			else
			{
				dataBuff = PNGParser_ParsePixelsByte(dataBuff, simg->data + imgY * info->storeSize.x + imgX, info->storeSize.x, 0, 0, imgW, imgH, 1, 1);
			}

			imgList->AddImage(simg, imgDelay);
		}
		else if (bitDepth == 16)
		{
			UOSInt lineAdd;

			info->atype = Media::AT_ALPHA_ALL_FF;
			info->storeBPP = 16;
			info->pf = Media::PF_LE_W16;
			info->byteSize = info->storeSize.CalcArea() * 2;
			lineAdd = imgList->GetCount();
			if (lineAdd > 0 && Optional<Media::StaticImage>::ConvertFrom(imgList->GetImage(lineAdd - 1, 0)).SetTo(simg))
			{
				simg = NN<Media::StaticImage>::ConvertFrom(simg->Clone());
			}
			else
			{
				NEW_CLASSNN(simg, Media::StaticImage(info));
			}

			UOSInt bpl = simg->GetDataBpl();
			if (interlaceMeth == 1)
			{
				//Pass1
				dataBuff = PNGParser_ParsePixelsWord(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 2, bpl, 0, 0, imgW, imgH, 8, 8);
				//Pass2
				dataBuff = PNGParser_ParsePixelsWord(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 2, bpl, 4, 0, imgW, imgH, 8, 8);
				//Pass3
				dataBuff = PNGParser_ParsePixelsWord(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 2, bpl, 0, 4, imgW, imgH, 4, 8);
				//Pass4
				dataBuff = PNGParser_ParsePixelsWord(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 2, bpl, 2, 0, imgW, imgH, 4, 4);
				//Pass5
				dataBuff = PNGParser_ParsePixelsWord(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 2, bpl, 0, 2, imgW, imgH, 2, 4);
				//Pass6
				dataBuff = PNGParser_ParsePixelsWord(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 2, bpl, 1, 0, imgW, imgH, 2, 2);
				//Pass7
				dataBuff = PNGParser_ParsePixelsWord(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 2, bpl, 0, 1, imgW, imgH, 1, 2);
			}
			else
			{
				dataBuff = PNGParser_ParsePixelsWord(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 2, bpl, 0, 0, imgW, imgH, 1, 1);
			}
			imgList->AddImage(simg, imgDelay);
		}
		break;
	case 2: //RGB
		if (bitDepth == 8)
		{
			UOSInt lineAdd;

			info->atype = Media::AT_ALPHA_ALL_FF;
			info->storeBPP = 24;
			info->pf = Media::PF_B8G8R8;
			info->byteSize = info->storeSize.CalcArea() * 3;
			lineAdd = imgList->GetCount();
			if (lineAdd > 0 && Optional<Media::StaticImage>::ConvertFrom(imgList->GetImage(lineAdd - 1, 0)).SetTo(simg))
			{
				simg = NN<Media::StaticImage>::ConvertFrom(simg->Clone());
			}
			else
			{
				NEW_CLASSNN(simg, Media::StaticImage(info));
			}
			UOSInt bpl = simg->GetDataBpl();
			if (interlaceMeth == 1)
			{
				//Pass1
				dataBuff = PNGParser_ParsePixelsRGB24(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 3, bpl, 0, 0, imgW, imgH, 8, 8);
				//Pass2
				dataBuff = PNGParser_ParsePixelsRGB24(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 3, bpl, 4, 0, imgW, imgH, 8, 8);
				//Pass3
				dataBuff = PNGParser_ParsePixelsRGB24(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 3, bpl, 0, 4, imgW, imgH, 4, 8);
				//Pass4
				dataBuff = PNGParser_ParsePixelsRGB24(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 3, bpl, 2, 0, imgW, imgH, 4, 4);
				//Pass5
				dataBuff = PNGParser_ParsePixelsRGB24(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 3, bpl, 0, 2, imgW, imgH, 2, 4);
				//Pass6
				dataBuff = PNGParser_ParsePixelsRGB24(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 3, bpl, 1, 0, imgW, imgH, 2, 2);
				//Pass7
				dataBuff = PNGParser_ParsePixelsRGB24(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 3, bpl, 0, 1, imgW, imgH, 1, 2);
			}
			else
			{
				dataBuff = PNGParser_ParsePixelsRGB24(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 3, bpl, 0, 0, imgW, imgH, 1, 1);
			}
			imgList->AddImage(simg, imgDelay);
		}
		else if (bitDepth == 16)
		{
			UOSInt lineAdd;

			info->atype = Media::AT_ALPHA_ALL_FF;
			info->storeBPP = 48;
			info->pf = Media::PF_LE_B16G16R16;
			info->byteSize = info->storeSize.CalcArea() * 3;
			lineAdd = imgList->GetCount();
			if (lineAdd > 0 && Optional<Media::StaticImage>::ConvertFrom(imgList->GetImage(lineAdd - 1, 0)).SetTo(simg))
			{
				simg = NN<Media::StaticImage>::ConvertFrom(simg->Clone());
			}
			else
			{
				NEW_CLASSNN(simg, Media::StaticImage(info));
			}
			UOSInt bpl = simg->GetDataBpl();
			if (interlaceMeth == 1)
			{
				//Pass1
				dataBuff = PNGParser_ParsePixelsRGB48(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 6, bpl, 0, 0, imgW, imgH, 8, 8);
				//Pass2
				dataBuff = PNGParser_ParsePixelsRGB48(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 6, bpl, 4, 0, imgW, imgH, 8, 8);
				//Pass3
				dataBuff = PNGParser_ParsePixelsRGB48(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 6, bpl, 0, 4, imgW, imgH, 4, 8);
				//Pass4
				dataBuff = PNGParser_ParsePixelsRGB48(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 6, bpl, 2, 0, imgW, imgH, 4, 4);
				//Pass5
				dataBuff = PNGParser_ParsePixelsRGB48(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 6, bpl, 0, 2, imgW, imgH, 2, 4);
				//Pass6
				dataBuff = PNGParser_ParsePixelsRGB48(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 6, bpl, 1, 0, imgW, imgH, 2, 2);
				//Pass7
				dataBuff = PNGParser_ParsePixelsRGB48(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 6, bpl, 0, 1, imgW, imgH, 1, 2);
			}
			else
			{
				dataBuff = PNGParser_ParsePixelsRGB48(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 6, bpl, 0, 0, imgW, imgH, 1, 1);
			}
			imgList->AddImage(simg, imgDelay);
		}
		break;
	case 3: //Indexed
		if (palette.SetTo(nnpal))
		{
			if (bitDepth < 8)
			{
				UOSInt storeWidth = ((info->dispSize.x + 15) >> 4) << 4;
				UnsafeArray<UInt8> lineStart;

				info->atype = palHasAlpha?Media::AT_ALPHA:Media::AT_ALPHA_ALL_FF;
				info->storeSize.x = storeWidth;
				if (bitDepth == 1)
				{
					info->storeBPP = 1;
					info->pf = Media::PF_PAL_1;
					info->byteSize = storeWidth * info->storeSize.y >> 3;
					NEW_CLASSNN(simg, Media::StaticImage(info));
					if (simg->pal.SetTo(spal))
					{
						MemCopyNO(spal.Ptr(), nnpal.Ptr(), 8);
					}

					lineStart = simg->data;

					MemClearAC(lineStart.Ptr(), info->byteSize);
					if (interlaceMeth == 1)
					{
						//Pass1
						dataBuff = PNGParser_ParsePixelsBits1(dataBuff, lineStart + ((imgY * storeWidth + imgX) >> 3), storeWidth >> 3, 0, 0, imgW, imgH, 8, 8);
						//Pass2
						dataBuff = PNGParser_ParsePixelsBits1(dataBuff, lineStart + ((imgY * storeWidth + imgX) >> 3), storeWidth >> 3, 4, 0, imgW, imgH, 8, 8);
						//Pass3
						dataBuff = PNGParser_ParsePixelsBits1(dataBuff, lineStart + ((imgY * storeWidth + imgX) >> 3), storeWidth >> 3, 0, 4, imgW, imgH, 4, 8);
						//Pass4
						dataBuff = PNGParser_ParsePixelsBits1(dataBuff, lineStart + ((imgY * storeWidth + imgX) >> 3), storeWidth >> 3, 2, 0, imgW, imgH, 4, 4);
						//Pass5
						dataBuff = PNGParser_ParsePixelsBits1(dataBuff, lineStart + ((imgY * storeWidth + imgX) >> 3), storeWidth >> 3, 0, 2, imgW, imgH, 2, 4);
						//Pass6
						dataBuff = PNGParser_ParsePixelsBits1(dataBuff, lineStart + ((imgY * storeWidth + imgX) >> 3), storeWidth >> 3, 1, 0, imgW, imgH, 2, 2);
						//Pass7
						dataBuff = PNGParser_ParsePixelsBits1(dataBuff, lineStart + ((imgY * storeWidth + imgX) >> 3), storeWidth >> 3, 0, 1, imgW, imgH, 1, 2);
					}
					else
					{
						dataBuff = PNGParser_ParsePixelsBits1(dataBuff, lineStart + ((imgY * storeWidth + imgX) >> 3), storeWidth >> 3, 0, 0, imgW, imgH, 1, 1);
					}

					imgList->AddImage(simg, imgDelay);
				}
				else if (bitDepth == 2)
				{
					info->storeBPP = 2;
					info->pf = Media::PF_PAL_2;
					info->byteSize = storeWidth * info->storeSize.y >> 2;
					NEW_CLASSNN(simg, Media::StaticImage(info));
					if (simg->pal.SetTo(spal))
					{
						MemCopyNO(spal.Ptr(), nnpal.Ptr(), 16);
					}

					lineStart = simg->data;
					MemClearAC(lineStart.Ptr(), info->byteSize);
					if (interlaceMeth == 1)
					{
						//Pass1
						dataBuff = PNGParser_ParsePixelsBits2(dataBuff, lineStart + ((imgY * storeWidth + imgX) >> 2), storeWidth >> 2, 0, 0, imgW, imgH, 8, 8);
						//Pass2
						dataBuff = PNGParser_ParsePixelsBits2(dataBuff, lineStart + ((imgY * storeWidth + imgX) >> 2), storeWidth >> 2, 4, 0, imgW, imgH, 8, 8);
						//Pass3
						dataBuff = PNGParser_ParsePixelsBits2(dataBuff, lineStart + ((imgY * storeWidth + imgX) >> 2), storeWidth >> 2, 0, 4, imgW, imgH, 4, 8);
						//Pass4
						dataBuff = PNGParser_ParsePixelsBits2(dataBuff, lineStart + ((imgY * storeWidth + imgX) >> 2), storeWidth >> 2, 2, 0, imgW, imgH, 4, 4);
						//Pass5
						dataBuff = PNGParser_ParsePixelsBits2(dataBuff, lineStart + ((imgY * storeWidth + imgX) >> 2), storeWidth >> 2, 0, 2, imgW, imgH, 2, 4);
						//Pass6
						dataBuff = PNGParser_ParsePixelsBits2(dataBuff, lineStart + ((imgY * storeWidth + imgX) >> 2), storeWidth >> 2, 1, 0, imgW, imgH, 2, 2);
						//Pass7
						dataBuff = PNGParser_ParsePixelsBits2(dataBuff, lineStart + ((imgY * storeWidth + imgX) >> 2), storeWidth >> 2, 0, 1, imgW, imgH, 1, 2);
					}
					else
					{
						dataBuff = PNGParser_ParsePixelsBits2(dataBuff, lineStart + ((imgY * storeWidth + imgX) >> 2), storeWidth >> 2, 0, 0, imgW, imgH, 1, 1);
					}

					imgList->AddImage(simg, imgDelay);
				}
				else if (bitDepth == 4)
				{
					info->storeBPP = 4;
					info->pf = Media::PF_PAL_4;
					info->byteSize = storeWidth * info->storeSize.y >> 1;
					NEW_CLASSNN(simg, Media::StaticImage(info));
					if (simg->pal.SetTo(spal))
					{
						MemCopyNO(spal.Ptr(), nnpal.Ptr(), 64);
					}

					lineStart = simg->data;
					MemClearAC(lineStart.Ptr(), info->byteSize);
					if (interlaceMeth == 1)
					{
						//Pass1
						dataBuff = PNGParser_ParsePixelsBits4(dataBuff, lineStart + ((imgY * storeWidth + imgX) >> 1), storeWidth >> 1, 0, 0, imgW, imgH, 8, 8);
						//Pass2
						dataBuff = PNGParser_ParsePixelsBits4(dataBuff, lineStart + ((imgY * storeWidth + imgX) >> 1), storeWidth >> 1, 4, 0, imgW, imgH, 8, 8);
						//Pass3
						dataBuff = PNGParser_ParsePixelsBits4(dataBuff, lineStart + ((imgY * storeWidth + imgX) >> 1), storeWidth >> 1, 0, 4, imgW, imgH, 4, 8);
						//Pass4
						dataBuff = PNGParser_ParsePixelsBits4(dataBuff, lineStart + ((imgY * storeWidth + imgX) >> 1), storeWidth >> 1, 2, 0, imgW, imgH, 4, 4);
						//Pass5
						dataBuff = PNGParser_ParsePixelsBits4(dataBuff, lineStart + ((imgY * storeWidth + imgX) >> 1), storeWidth >> 1, 0, 2, imgW, imgH, 2, 4);
						//Pass6
						dataBuff = PNGParser_ParsePixelsBits4(dataBuff, lineStart + ((imgY * storeWidth + imgX) >> 1), storeWidth >> 1, 1, 0, imgW, imgH, 2, 2);
						//Pass7
						dataBuff = PNGParser_ParsePixelsBits4(dataBuff, lineStart + ((imgY * storeWidth + imgX) >> 1), storeWidth >> 1, 0, 1, imgW, imgH, 1, 2);
					}
					else
					{
						dataBuff = PNGParser_ParsePixelsBits4(dataBuff, lineStart + ((imgY * storeWidth + imgX) >> 1), storeWidth >> 1, 0, 0, imgW, imgH, 1, 1);
					}

					imgList->AddImage(simg, imgDelay);
				}
			}
			else if (bitDepth == 8)
			{
				info->atype = Media::AT_ALPHA;
				info->storeSize.x = info->dispSize.x;
				info->storeBPP = 8;
				info->pf = Media::PF_PAL_8;
				info->byteSize = info->storeSize.CalcArea();
				NEW_CLASSNN(simg, Media::StaticImage(info));
				if (simg->pal.SetTo(spal))
				{
					MemCopyNO(spal.Ptr(), nnpal.Ptr(), 1024);
				}

				if (interlaceMeth == 1)
				{
					//Pass1
					dataBuff = PNGParser_ParsePixelsByte(dataBuff, simg->data + imgY * info->storeSize.x + imgX, info->storeSize.x, 0, 0, imgW, imgH, 8, 8);
					//Pass2
					dataBuff = PNGParser_ParsePixelsByte(dataBuff, simg->data + imgY * info->storeSize.x + imgX, info->storeSize.x, 4, 0, imgW, imgH, 8, 8);
					//Pass3
					dataBuff = PNGParser_ParsePixelsByte(dataBuff, simg->data + imgY * info->storeSize.x + imgX, info->storeSize.x, 0, 4, imgW, imgH, 4, 8);
					//Pass4
					dataBuff = PNGParser_ParsePixelsByte(dataBuff, simg->data + imgY * info->storeSize.x + imgX, info->storeSize.x, 2, 0, imgW, imgH, 4, 4);
					//Pass5
					dataBuff = PNGParser_ParsePixelsByte(dataBuff, simg->data + imgY * info->storeSize.x + imgX, info->storeSize.x, 0, 2, imgW, imgH, 2, 4);
					//Pass6
					dataBuff = PNGParser_ParsePixelsByte(dataBuff, simg->data + imgY * info->storeSize.x + imgX, info->storeSize.x, 1, 0, imgW, imgH, 2, 2);
					//Pass7
					dataBuff = PNGParser_ParsePixelsByte(dataBuff, simg->data + imgY * info->storeSize.x + imgX, info->storeSize.x, 0, 1, imgW, imgH, 1, 2);
				}
				else
				{
					dataBuff = PNGParser_ParsePixelsByte(dataBuff, simg->data + imgY * info->storeSize.x + imgX, info->storeSize.x, 0, 0, imgW, imgH, 1, 1);
				}

				imgList->AddImage(simg, imgDelay);
			}
		}
		break;
	case 4: //Alpha + White
		{
			Bool semiTr = false;
			if (bitDepth == 8)
			{
				UOSInt lineAdd;

				info->atype = Media::AT_ALPHA;
				info->storeBPP = 16;
				info->pf = Media::PF_W8A8;
				info->byteSize = info->storeSize.CalcArea() * 2;
				lineAdd = imgList->GetCount();
				if (lineAdd > 0 && Optional<Media::StaticImage>::ConvertFrom(imgList->GetImage(lineAdd - 1, 0)).SetTo(simg))
				{
					simg = NN<Media::StaticImage>::ConvertFrom(simg->Clone());
					simg->FillColor(0);
				}
				else
				{
					NEW_CLASSNN(simg, Media::StaticImage(info));
				}
				UOSInt bpl = simg->GetDataBpl();
				if (interlaceMeth == 1)
				{
					//Pass1
					dataBuff = PNGParser_ParsePixelsAW16(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 2, bpl, 0, 0, imgW, imgH, 8, 8, &semiTr);
					//Pass2
					dataBuff = PNGParser_ParsePixelsAW16(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 2, bpl, 4, 0, imgW, imgH, 8, 8, &semiTr);
					//Pass3
					dataBuff = PNGParser_ParsePixelsAW16(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 2, bpl, 0, 4, imgW, imgH, 4, 8, &semiTr);
					//Pass4
					dataBuff = PNGParser_ParsePixelsAW16(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 2, bpl, 2, 0, imgW, imgH, 4, 4, &semiTr);
					//Pass5
					dataBuff = PNGParser_ParsePixelsAW16(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 2, bpl, 0, 2, imgW, imgH, 2, 4, &semiTr);
					//Pass6
					dataBuff = PNGParser_ParsePixelsAW16(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 2, bpl, 1, 0, imgW, imgH, 2, 2, &semiTr);
					//Pass7
					dataBuff = PNGParser_ParsePixelsAW16(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 2, bpl, 0, 1, imgW, imgH, 1, 2, &semiTr);
				}
				else
				{
					dataBuff = PNGParser_ParsePixelsAW16(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 2, bpl, 0, 0, imgW, imgH, 1, 1, &semiTr);
				}
				if (!semiTr)
				{
					simg->info.atype = Media::AT_ALPHA_ALL_FF;
				}
				imgList->AddImage(simg, imgDelay);
			}
			else if (bitDepth == 16)
			{
				UOSInt lineAdd;

				info->atype = Media::AT_ALPHA;
				info->storeBPP = 32;
				info->pf = Media::PF_LE_W16A16;
				info->byteSize = info->storeSize.CalcArea() * 4;
				lineAdd = imgList->GetCount();
				if (lineAdd > 0 && Optional<Media::StaticImage>::ConvertFrom(imgList->GetImage(lineAdd - 1, 0)).SetTo(simg))
				{
					simg = NN<Media::StaticImage>::ConvertFrom(simg->Clone());
					simg->FillColor(0);
				}
				else
				{
					NEW_CLASSNN(simg, Media::StaticImage(info));
				}
				UOSInt bpl = simg->GetDataBpl();
				if (interlaceMeth == 1)
				{
					//Pass1
					dataBuff = PNGParser_ParsePixelsAW32(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 4, bpl, 0, 0, imgW, imgH, 8, 8, &semiTr);
					//Pass2
					dataBuff = PNGParser_ParsePixelsAW32(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 4, bpl, 4, 0, imgW, imgH, 8, 8, &semiTr);
					//Pass3
					dataBuff = PNGParser_ParsePixelsAW32(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 4, bpl, 0, 4, imgW, imgH, 4, 8, &semiTr);
					//Pass4
					dataBuff = PNGParser_ParsePixelsAW32(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 4, bpl, 2, 0, imgW, imgH, 4, 4, &semiTr);
					//Pass5
					dataBuff = PNGParser_ParsePixelsAW32(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 4, bpl, 0, 2, imgW, imgH, 2, 4, &semiTr);
					//Pass6
					dataBuff = PNGParser_ParsePixelsAW32(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 4, bpl, 1, 0, imgW, imgH, 2, 2, &semiTr);
					//Pass7
					dataBuff = PNGParser_ParsePixelsAW32(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 4, bpl, 0, 1, imgW, imgH, 1, 2, &semiTr);
				}
				else
				{
					dataBuff = PNGParser_ParsePixelsAW32(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 4, bpl, 0, 0, imgW, imgH, 1, 1, &semiTr);
				}
				if (!semiTr)
				{
					simg->info.atype = Media::AT_ALPHA_ALL_FF;
				}
				imgList->AddImage(simg, imgDelay);
			}
			break;
		}
	case 6: //ARGB
		{
			Bool semiTr = false;
			if (bitDepth == 8)
			{
				UOSInt lineAdd;

				info->atype = Media::AT_ALPHA;
				info->storeBPP = 32;
				info->pf = Media::PF_B8G8R8A8;
				info->byteSize = info->storeSize.CalcArea() * 4;
				lineAdd = imgList->GetCount();
				if (lineAdd > 0 && Optional<Media::StaticImage>::ConvertFrom(imgList->GetImage(lineAdd - 1, 0)).SetTo(simg))
				{
					simg = NN<Media::StaticImage>::ConvertFrom(simg->Clone());
					simg->FillColor(0);
				}
				else
				{
					NEW_CLASSNN(simg, Media::StaticImage(info));
				}
				UOSInt bpl = simg->GetDataBpl();
				if (interlaceMeth == 1)
				{
					//Pass1
					dataBuff = PNGParser_ParsePixelsARGB32(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 4, bpl, 0, 0, imgW, imgH, 8, 8, &semiTr);
					//Pass2
					dataBuff = PNGParser_ParsePixelsARGB32(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 4, bpl, 4, 0, imgW, imgH, 8, 8, &semiTr);
					//Pass3
					dataBuff = PNGParser_ParsePixelsARGB32(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 4, bpl, 0, 4, imgW, imgH, 4, 8, &semiTr);
					//Pass4
					dataBuff = PNGParser_ParsePixelsARGB32(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 4, bpl, 2, 0, imgW, imgH, 4, 4, &semiTr);
					//Pass5
					dataBuff = PNGParser_ParsePixelsARGB32(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 4, bpl, 0, 2, imgW, imgH, 2, 4, &semiTr);
					//Pass6
					dataBuff = PNGParser_ParsePixelsARGB32(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 4, bpl, 1, 0, imgW, imgH, 2, 2, &semiTr);
					//Pass7
					dataBuff = PNGParser_ParsePixelsARGB32(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 4, bpl, 0, 1, imgW, imgH, 1, 2, &semiTr);
				}
				else
				{
					dataBuff = PNGParser_ParsePixelsARGB32(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 4, bpl, 0, 0, imgW, imgH, 1, 1, &semiTr);
				}
				if (!semiTr)
				{
					simg->info.atype = Media::AT_ALPHA_ALL_FF;
				}
				imgList->AddImage(simg, imgDelay);
			}
			else if (bitDepth == 16)
			{
				UOSInt lineAdd;

				info->atype = Media::AT_ALPHA;
				info->storeBPP = 64;
				info->pf = Media::PF_LE_B16G16R16A16;
				info->byteSize = info->storeSize.CalcArea() * 8;
				lineAdd = imgList->GetCount();
				if (lineAdd > 0 && Optional<Media::StaticImage>::ConvertFrom(imgList->GetImage(lineAdd - 1, 0)).SetTo(simg))
				{
					simg = NN<Media::StaticImage>::ConvertFrom(simg->Clone());
					simg->FillColor(0);
				}
				else
				{
					NEW_CLASSNN(simg, Media::StaticImage(info));
				}
				UOSInt bpl = simg->GetDataBpl();
				if (interlaceMeth == 1)
				{
					//Pass1
					dataBuff = PNGParser_ParsePixelsARGB64(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 8, bpl, 0, 0, imgW, imgH, 8, 8, &semiTr);
					//Pass2
					dataBuff = PNGParser_ParsePixelsARGB64(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 8, bpl, 4, 0, imgW, imgH, 8, 8, &semiTr);
					//Pass3
					dataBuff = PNGParser_ParsePixelsARGB64(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 8, bpl, 0, 4, imgW, imgH, 4, 8, &semiTr);
					//Pass4
					dataBuff = PNGParser_ParsePixelsARGB64(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 8, bpl, 2, 0, imgW, imgH, 4, 4, &semiTr);
					//Pass5
					dataBuff = PNGParser_ParsePixelsARGB64(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 8, bpl, 0, 2, imgW, imgH, 2, 4, &semiTr);
					//Pass6
					dataBuff = PNGParser_ParsePixelsARGB64(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 8, bpl, 1, 0, imgW, imgH, 2, 2, &semiTr);
					//Pass7
					dataBuff = PNGParser_ParsePixelsARGB64(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 8, bpl, 0, 1, imgW, imgH, 1, 2, &semiTr);
				}
				else
				{
					dataBuff = PNGParser_ParsePixelsARGB64(dataBuff, simg->data + imgY * bpl + (UOSInt)imgX * 8, bpl, 0, 0, imgW, imgH, 1, 1, &semiTr);
				}
				if (!semiTr)
				{
					simg->info.atype = Media::AT_ALPHA_ALL_FF;
				}
				imgList->AddImage(simg, imgDelay);
			}
			break;
		}
	}
}

Parser::FileParser::PNGParser::PNGParser()
{
}

Parser::FileParser::PNGParser::~PNGParser()
{
}

Int32 Parser::FileParser::PNGParser::GetName()
{
	return *(Int32*)"PNGP";
}

void Parser::FileParser::PNGParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::ImageList)
	{
		selector->AddFilter(CSTR("*.png"), CSTR("Portable Network Graphic File"));
	}
}

IO::ParserType Parser::FileParser::PNGParser::GetParserType()
{
	return IO::ParserType::ImageList;
}

Optional<IO::ParsedObject> Parser::FileParser::PNGParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	UInt64 ofst;
	if (hdr[0] != 0x89 && hdr[1] != 0x50 && hdr[2] != 0x4e && hdr[3] != 0x47 && hdr[4] != 0x0d && hdr[5] != 0x0a && hdr[6] != 0x1a && hdr[7] != 0x0a)
	{
		return 0;
	}
	Media::FrameInfo info;
	NN<Media::ImageList> imgList;
	Bool ihdrFound = false;
	Bool iccpFound = false;
	Bool srgbFound = false;
	UInt32 size;
	UInt8 bitDepth = 0;
	UInt8 colorType = 0;
//	UInt8 compMeth;
//	UInt8 filterMeth;
	UInt8 interlaceMeth = 0;
	Optional<IO::MemoryStream> mstm = 0;
	Optional<Data::Compress::Inflater> cstm = 0;
	IO::WriteCacheStream *wcstm = 0;
	UOSInt imgSize = 0;
	UInt32 imgDelay = 0;
	UInt32 imgX = 0;
	UInt32 imgY = 0;
	UInt32 imgW = 0;
	UInt32 imgH = 0;
	UInt8 *palette = 0;
	Bool palHasAlpha = false;
	UInt8 buff[8];

	NEW_CLASSNN(imgList, Media::ImageList(fd->GetFullFileName()));

	ofst = 8;
	while (true)
	{
		if (fd->GetRealData(ofst, 8, BYTEARR(buff)) != 8)
		{
			break;
		}
		size = ReadMUInt32(&buff[0]);
		if (*(Int32*)&buff[4] == *(Int32*)"IHDR")
		{
			if (size >= 13)
			{
				Data::ByteBuffer chunkData(size);
				if (fd->GetRealData(ofst + 8, size, chunkData) == size)
				{
					info.dispSize.x = ReadMUInt32(&chunkData[0]);
					info.dispSize.y = ReadMUInt32(&chunkData[4]);
					info.storeSize = info.dispSize;
					imgW = (UInt32)info.dispSize.x;
					imgH = (UInt32)info.dispSize.y;
					bitDepth = chunkData[8];
					colorType = chunkData[9];
//					compMeth = chunkData[10];
//					filterMeth = chunkData[11];
					interlaceMeth = chunkData[12];

					info.par2 = 1.0;
					info.hdpi = 72.0;
					info.vdpi = 72.0;
					info.fourcc = 0;
					info.ftype = Media::FT_NON_INTERLACE;
					info.ycOfst = Media::YCOFST_C_CENTER_LEFT;
					info.rotateType = Media::RotateType::None;
					
					ihdrFound = true;
				}
			}
		}
		else if (*(Int32*)&buff[4] == *(Int32*)"pHYs")
		{
			if (size >= 9)
			{
				Data::ByteBuffer chunkData(size);
				if (fd->GetRealData(ofst + 8, size, chunkData) == size)
				{
					if (chunkData[8] == 1)
					{
						info.hdpi = 1.0 / Math::Unit::Distance::Convert(Math::Unit::Distance::DU_METER, Math::Unit::Distance::DU_INCH, 1.0 / ReadMInt32(&chunkData[0]));
						info.par2 = info.hdpi * Math::Unit::Distance::Convert(Math::Unit::Distance::DU_METER, Math::Unit::Distance::DU_INCH, 1.0 / ReadMInt32(&chunkData[4]));
						info.vdpi = info.hdpi / info.par2;
					}
					else if (chunkData[8] == 0)
					{
						Int32 hdpi = ReadMInt32(&chunkData[0]);
						Int32 vdpi = ReadMInt32(&chunkData[4]);
						info.par2 = hdpi / (Double)vdpi;
						if (info.par2 > 1)
						{
							info.vdpi = 300.0 / info.par2;
							info.hdpi = 300.0;
						}
						else
						{
							info.hdpi = 300.0 * info.par2;
							info.vdpi = 300.0;
						}
					}
				}
			}
		}
		else if (*(Int32*)&buff[4] == *(Int32*)"PLTE")
		{
			if (size <= 768 && (size % 3) == 0)
			{
				Data::ByteBuffer chunkData(size);
				if (fd->GetRealData(ofst + 8, size, chunkData) == size)
				{
					if (palette == 0)
					{
						palette = MemAlloc(UInt8, 1024);
					}
					UInt8 *palPtr = palette;
					UnsafeArray<UInt8> dataBuff = chunkData.Arr();
					UOSInt sizeLeft = size;
					while (sizeLeft > 0)
					{
						palPtr[0] = dataBuff[2];
						palPtr[1] = dataBuff[1];
						palPtr[2] = dataBuff[0];
						palPtr[3] = 0xff;
						palPtr += 4;
						dataBuff += 3;
						sizeLeft -= 3;
					}
					sizeLeft = 768 - size;
					while (sizeLeft > 0)
					{
						WriteNInt32(palPtr, 0);
						palPtr += 4;
						sizeLeft -= 3;
					}
				}
			}
		}
		else if (*(Int32*)&buff[4] == *(Int32*)"tRNS")
		{
			if (palette != 0 && size <= 256)
			{
				Data::ByteBuffer chunkData(size);
				if (fd->GetRealData(ofst + 8, size, chunkData) == size)
				{
					UOSInt i = 0;
					while (i < size)
					{
						palette[i * 4 + 3] = chunkData[i];
						if (chunkData[i] != 0xff)
						{
							palHasAlpha = true;
						}
						i++;
					}
				}
			}
		}
		else if (*(Int32*)&buff[4] == *(Int32*)"iCCP")
		{
			Data::ByteBuffer chunkData(size);
			if (fd->GetRealData(ofst + 8, size, chunkData) == size)
			{
				UOSInt i = Text::StrCharCnt(chunkData.Arr()) + 1;
				if (chunkData[i] == 0)
				{
					IO::MemoryStream mstm;
					Data::Compress::Inflater cstm(mstm, false);
					if (cstm.Write(Data::ByteArrayR(&chunkData[i + 3], size - i - 7)) == (size - i - 7))
					{
						Data::ByteArray iccBuff = mstm.GetArray();
						NN<Media::ICCProfile> icc;
						if (Media::ICCProfile::Parse(iccBuff).SetTo(icc))
						{
							icc->SetToColorProfile(info.color);
							icc.Delete();
							iccpFound = true;
						}
					}
				}
			}
		}
		else if (*(Int32*)&buff[4] == *(Int32*)"sRGB")
		{
			if (!iccpFound)
			{
				info.color.SetCommonProfile(Media::ColorProfile::CPT_SRGB);
				srgbFound = true;
			}
		}
		else if (*(Int32*)&buff[4] == *(Int32*)"gAMA")
		{
			if (!iccpFound && !srgbFound && size >= 4)
			{
				Data::ByteBuffer chunkData(size);
				if (fd->GetRealData(ofst + 8, size, chunkData) == size)
				{
					Double g = (Double)ReadMInt32(&chunkData[0]);
					if (g != 0)
					{
						g = 100000 / g;
						info.color.rtransfer.Set(Media::CS::TRANT_GAMMA, g);
						info.color.gtransfer.Set(Media::CS::TRANT_GAMMA, g);
						info.color.btransfer.Set(Media::CS::TRANT_GAMMA, g);
					}
				}
			}
		}
		else if (*(Int32*)&buff[4] == *(Int32*)"cHRM")
		{
			if (!iccpFound && !srgbFound && size >= 32)
			{
				Data::ByteBuffer chunkData(size);
				if (fd->GetRealData(ofst + 8, size, chunkData) == size)
				{
					info.color.primaries.SetColorType(Media::ColorProfile::CT_CUSTOM);
					info.color.primaries.w.x = ReadMInt32(&chunkData[0]) / 100000.0;
					info.color.primaries.w.y = ReadMInt32(&chunkData[4]) / 100000.0;
					info.color.primaries.r.x = ReadMInt32(&chunkData[8]) / 100000.0;
					info.color.primaries.r.y = ReadMInt32(&chunkData[12]) / 100000.0;
					info.color.primaries.g.x = ReadMInt32(&chunkData[16]) / 100000.0;
					info.color.primaries.g.y = ReadMInt32(&chunkData[20]) / 100000.0;
					info.color.primaries.b.x = ReadMInt32(&chunkData[24]) / 100000.0;
					info.color.primaries.b.y = ReadMInt32(&chunkData[28]) / 100000.0;
					if (info.color.primaries.r.x == info.color.primaries.g.x && info.color.primaries.r.y == info.color.primaries.g.y)
					{
						info.color.primaries.SetColorType(Media::ColorProfile::CT_SRGB);
					}
				}
			}
		}
		else if (*(Int32*)&buff[4] == *(Int32*)"fcTL")
		{
			if (size >= 26)
			{
				NN<IO::MemoryStream> nnmstm;
				if (mstm.SetTo(nnmstm))
				{
					UOSInt dataSize;
					UnsafeArray<UInt8> dataBuff = nnmstm->GetBuff(dataSize);
					if (dataSize == imgSize || imgSize != 0)
					{
						ParseImage(bitDepth, colorType, dataBuff, info, imgList, imgDelay, imgX, imgY, imgW, imgH, interlaceMeth, palette, palHasAlpha);
					}
					DEL_CLASS(wcstm);
					wcstm = 0;
					cstm.Delete();
					mstm.Delete();
				}

				Data::ByteBuffer chunkData(size);
				if (fd->GetRealData(ofst + 8, size, chunkData) == size)
				{
					imgDelay = 1000 * (UInt32)ReadMUInt16(&chunkData[20]) / ReadMUInt16(&chunkData[22]);
					imgX = ReadMUInt32(&chunkData[12]);
					imgY = ReadMUInt32(&chunkData[16]);
					imgW = ReadMUInt32(&chunkData[4]);
					imgH = ReadMUInt32(&chunkData[8]);
				}
			}
		}
		else if (*(Int32*)&buff[4] == *(Int32*)"sBIT")
		{
		}
		else if (*(Int32*)&buff[4] == *(Int32*)"IDAT")
		{
			if (ihdrFound)
			{
				Data::ByteBuffer chunkData(size);
				if (fd->GetRealData(ofst + 8, size, chunkData) == size)
				{
					if (mstm.IsNull())
					{
						imgSize = CalcImageSize(imgW, imgH, bitDepth, colorType, interlaceMeth);
						NN<IO::MemoryStream> nnmstm;
						if (imgSize)
						{
							NEW_CLASSNN(nnmstm, IO::MemoryStream(imgSize));
						}
						else
						{
							NEW_CLASSNN(nnmstm, IO::MemoryStream());
						}
						mstm = nnmstm;
						NN<Data::Compress::Inflater> nncstm;
						NEW_CLASSNN(nncstm, Data::Compress::Inflater(nnmstm, false));
						cstm = nncstm;
						NEW_CLASS(wcstm, IO::WriteCacheStream(nncstm));
						wcstm->Write(chunkData.WithSize(size) + 2);
					}
					else
					{
						wcstm->Write(chunkData.WithSize(size));
					}
				}
			}
		}
		else if (*(Int32*)&buff[4] == *(Int32*)"fdAT")
		{
			if (ihdrFound)
			{
				Data::ByteBuffer chunkData(size);
				if (fd->GetRealData(ofst + 8, size, chunkData) == size)
				{
					if (mstm.IsNull())
					{
						if (colorType == 0)
						{
							imgSize = ((((UOSInt)imgW * bitDepth) >> 3) + 1) * imgH;
						}
						else if (colorType == 2)
						{
							imgSize = (imgW * (UOSInt)(bitDepth >> 3) * 3 + 1) * imgH;
						}
						else if (colorType == 3)
						{
							imgSize = ((((UOSInt)imgW * bitDepth) >> 3) + 1) * imgH;
						}
						else if (colorType == 4)
						{
							imgSize = (imgW * (UOSInt)(bitDepth >> 3) * 2 + 1) * imgH;
						}
						else if (colorType == 6)
						{
							imgSize = (imgW * (UOSInt)(bitDepth >> 3) * 4 + 1) * imgH;
						}
						else
						{
							imgSize = 0;
						}
						NN<IO::MemoryStream> nnmstm;
						if (imgSize)
						{
							NEW_CLASSNN(nnmstm, IO::MemoryStream(imgSize));
						}
						else
						{
							NEW_CLASSNN(nnmstm, IO::MemoryStream());
						}
						mstm = nnmstm;
						NN<Data::Compress::Inflater> nncstm;
						NEW_CLASSNN(nncstm, Data::Compress::Inflater(nnmstm, false));
						cstm = nncstm;
						NEW_CLASS(wcstm, IO::WriteCacheStream(nncstm));
						wcstm->Write(Data::ByteArrayR(&chunkData[6], size - 6));
					}
					else
					{
						wcstm->Write(Data::ByteArrayR(&chunkData[4], size - 4));
					}
				}
			}
		}
		else if (*(Int32*)&buff[4] == *(Int32*)"IEND")
		{
			NN<IO::MemoryStream> nnmstm;
			if (mstm.SetTo(nnmstm))
			{
				wcstm->Flush();
				UOSInt dataSize;
				UnsafeArray<UInt8> dataBuff = nnmstm->GetBuff(dataSize);
				if (dataSize == imgSize || imgSize != 0)
				{
					ParseImage(bitDepth, colorType, dataBuff, info, imgList, imgDelay, imgX, imgY, imgW, imgH, interlaceMeth, palette, palHasAlpha);
				}
				DEL_CLASS(wcstm);
				wcstm = 0;
				cstm.Delete();
				mstm.Delete();
			}
		}
		ofst += (UInt64)size + 12;
	}
	NN<IO::MemoryStream> nnmstm;
	if (mstm.SetTo(nnmstm))
	{
		wcstm->Flush();
		UOSInt dataSize;
		UnsafeArray<UInt8> dataBuff = nnmstm->GetBuff(dataSize);
		if (dataSize == imgSize || imgSize != 0)
		{
			ParseImage(bitDepth, colorType, dataBuff, info, imgList, imgDelay, imgX, imgY, imgW, imgH, interlaceMeth, palette, palHasAlpha);
		}
		DEL_CLASS(wcstm);
		wcstm = 0;
		cstm.Delete();
		mstm.Delete();
	}
	if (palette)
	{
		MemFree(palette);
		palette = 0;
	}
	SDEL_CLASS(wcstm);
	cstm.Delete();
	mstm.Delete();
	if (imgList->GetCount() <= 0)
	{
		imgList.Delete();
		return 0;
	}
	else
	{
		return imgList;
	}
}

UOSInt Parser::FileParser::PNGParser::CalcImageSize(UInt32 imgW, UInt32 imgH, UInt8 bitDepth, UInt8 colorType, UInt8 interlaceMeth)
{
	UOSInt byteDepth = (UOSInt)bitDepth >> 3;
	UOSInt imgSize;
	if (colorType == 0)
	{
		if (interlaceMeth == 1 && bitDepth <= 8)
		{
			UOSInt linePxCnt;
			UOSInt lineCnt;
			UOSInt andMask;
			UOSInt shiftCnt;
			imgSize = 0;

			if (bitDepth == 1)
			{
				andMask = 7;
				shiftCnt = 3;
			}
			else if (bitDepth == 2)
			{
				andMask = 3;
				shiftCnt = 2;
			}
			else if (bitDepth == 4)
			{
				andMask = 1;
				shiftCnt = 1;
			}
			else if (bitDepth == 8)
			{
				andMask = 0;
				shiftCnt = 0;
			}
			else
			{
				andMask = 0;
				shiftCnt = 0;
			}

			//Pass1
			linePxCnt = (imgW + 7) >> 3;
			lineCnt = (imgH + 7) >> 3;
			if (linePxCnt & andMask)
			{
				imgSize += (2 + (linePxCnt >> shiftCnt)) * lineCnt;
			}
			else
			{
				imgSize += (1 + (linePxCnt >> shiftCnt)) * lineCnt;
			}

			//Pass2
			linePxCnt = (imgW + 3) >> 3;
			lineCnt = (imgH + 7) >> 3;
			if (linePxCnt & andMask)
			{
				imgSize += (2 + (linePxCnt >> shiftCnt)) * lineCnt;
			}
			else
			{
				imgSize += (1 + (linePxCnt >> shiftCnt)) * lineCnt;
			}

			//Pass3
			linePxCnt = (imgW + 3) >> 2;
			lineCnt = (imgH + 3) >> 3;
			if (linePxCnt & andMask)
			{
				imgSize += (2 + (linePxCnt >> shiftCnt)) * lineCnt;
			}
			else
			{
				imgSize += (1 + (linePxCnt >> shiftCnt)) * lineCnt;
			}

			//Pass4
			linePxCnt = (imgW + 1) >> 2;
			lineCnt = (imgH + 3) >> 2;
			if (linePxCnt & andMask)
			{
				imgSize += (2 + (linePxCnt >> shiftCnt)) * lineCnt;
			}
			else
			{
				imgSize += (1 + (linePxCnt >> shiftCnt)) * lineCnt;
			}

			//Pass5
			linePxCnt = (imgW + 1) >> 1;
			lineCnt = (imgH + 1) >> 2;
			if (linePxCnt & andMask)
			{
				imgSize += (2 + (linePxCnt >> shiftCnt)) * lineCnt;
			}
			else
			{
				imgSize += (1 + (linePxCnt >> shiftCnt)) * lineCnt;
			}

			//Pass6
			linePxCnt = (imgW + 0) >> 1;
			lineCnt = (imgH + 1) >> 1;
			if (linePxCnt & andMask)
			{
				imgSize += (2 + (linePxCnt >> shiftCnt)) * lineCnt;
			}
			else
			{
				imgSize += (1 + (linePxCnt >> shiftCnt)) * lineCnt;
			}

			//Pass7
			linePxCnt = imgW;
			lineCnt = (imgH + 0) >> 1;
			if (linePxCnt & andMask)
			{
				imgSize += (2 + (linePxCnt >> shiftCnt)) * lineCnt;
			}
			else
			{
				imgSize += (1 + (linePxCnt >> shiftCnt)) * lineCnt;
			}
		}
		else if (interlaceMeth == 1 && bitDepth == 16)
		{
			UOSInt linePxCnt;
			UOSInt lineCnt;
			imgSize = 0;

			//Pass1
			linePxCnt = (imgW + 7) >> 3;
			lineCnt = (imgH + 7) >> 3;
			imgSize += (1 + (linePxCnt << 1)) * lineCnt;

			//Pass2
			linePxCnt = (imgW + 3) >> 3;
			lineCnt = (imgH + 7) >> 3;
			imgSize += (1 + (linePxCnt << 1)) * lineCnt;

			//Pass3
			linePxCnt = (imgW + 3) >> 2;
			lineCnt = (imgH + 3) >> 3;
			imgSize += (1 + (linePxCnt << 1)) * lineCnt;

			//Pass4
			linePxCnt = (imgW + 1) >> 2;
			lineCnt = (imgH + 3) >> 2;
			imgSize += (1 + (linePxCnt << 1)) * lineCnt;

			//Pass5
			linePxCnt = (imgW + 1) >> 1;
			lineCnt = (imgH + 1) >> 2;
			imgSize += (1 + (linePxCnt << 1)) * lineCnt;

			//Pass6
			linePxCnt = (imgW + 0) >> 1;
			lineCnt = (imgH + 1) >> 1;
			imgSize += (1 + (linePxCnt << 1)) * lineCnt;

			//Pass7
			linePxCnt = imgW;
			lineCnt = (imgH + 0) >> 1;
			imgSize += (1 + (linePxCnt << 1)) * lineCnt;
		}
		else
		{
			imgSize = ((((UOSInt)imgW * bitDepth) >> 3) + 1) * imgH;
		}
	}
	else if (colorType == 2)
	{
		if (interlaceMeth == 1)
		{
			UOSInt linePxCnt;
			UOSInt lineCnt;
			imgSize = 0;

			//Pass1
			linePxCnt = (imgW + 7) >> 3;
			lineCnt = (imgH + 7) >> 3;
			imgSize += (1 + (linePxCnt * byteDepth * 3)) * lineCnt;

			//Pass2
			linePxCnt = (imgW + 3) >> 3;
			lineCnt = (imgH + 7) >> 3;
			imgSize += (1 + (linePxCnt * byteDepth * 3)) * lineCnt;

			//Pass3
			linePxCnt = (imgW + 3) >> 2;
			lineCnt = (imgH + 3) >> 3;
			imgSize += (1 + (linePxCnt * byteDepth * 3)) * lineCnt;

			//Pass4
			linePxCnt = (imgW + 1) >> 2;
			lineCnt = (imgH + 3) >> 2;
			imgSize += (1 + (linePxCnt * byteDepth * 3)) * lineCnt;

			//Pass5
			linePxCnt = (imgW + 1) >> 1;
			lineCnt = (imgH + 1) >> 2;
			imgSize += (1 + (linePxCnt * byteDepth * 3)) * lineCnt;

			//Pass6
			linePxCnt = (imgW + 0) >> 1;
			lineCnt = (imgH + 1) >> 1;
			imgSize += (1 + (linePxCnt * byteDepth * 3)) * lineCnt;

			//Pass7
			linePxCnt = imgW;
			lineCnt = (imgH + 0) >> 1;
			imgSize += (1 + (linePxCnt * byteDepth * 3)) * lineCnt;
		}
		else
		{
			imgSize = (imgW * byteDepth * 3 + 1) * imgH;
		}
	}
	else if (colorType == 3)
	{
		UOSInt andMask;
		UOSInt shiftCnt;
		if (bitDepth == 1)
		{
			andMask = 7;
			shiftCnt = 3;
		}
		else if (bitDepth == 2)
		{
			andMask = 3;
			shiftCnt = 2;
		}
		else if (bitDepth == 4)
		{
			andMask = 1;
			shiftCnt = 1;
		}
		else if (bitDepth == 8)
		{
			andMask = 0;
			shiftCnt = 0;
		}
		else
		{
			andMask = 0;
			shiftCnt = 0;
		}

		if (interlaceMeth == 1 && bitDepth <= 8)
		{
			UOSInt linePxCnt;
			UOSInt lineCnt;
			imgSize = 0;


			//Pass1
			linePxCnt = (imgW + 7) >> 3;
			lineCnt = (imgH + 7) >> 3;
			if (linePxCnt > 0 && lineCnt > 0)
			{
				if (linePxCnt & andMask)
				{
					imgSize += (2 + (linePxCnt >> shiftCnt)) * lineCnt;
				}
				else
				{
					imgSize += (1 + (linePxCnt >> shiftCnt)) * lineCnt;
				}
			}

			//Pass2
			linePxCnt = (imgW + 3) >> 3;
			lineCnt = (imgH + 7) >> 3;
			if (linePxCnt > 0 && lineCnt > 0)
			{
				if (linePxCnt & andMask)
				{
					imgSize += (2 + (linePxCnt >> shiftCnt)) * lineCnt;
				}
				else
				{
					imgSize += (1 + (linePxCnt >> shiftCnt)) * lineCnt;
				}
			}

			//Pass3
			linePxCnt = (imgW + 3) >> 2;
			lineCnt = (imgH + 3) >> 3;
			if (linePxCnt > 0 && lineCnt > 0)
			{
				if (linePxCnt & andMask)
				{
					imgSize += (2 + (linePxCnt >> shiftCnt)) * lineCnt;
				}
				else
				{
					imgSize += (1 + (linePxCnt >> shiftCnt)) * lineCnt;
				}
			}

			//Pass4
			linePxCnt = (imgW + 1) >> 2;
			lineCnt = (imgH + 3) >> 2;
			if (linePxCnt > 0 && lineCnt > 0)
			{
				if (linePxCnt & andMask)
				{
					imgSize += (2 + (linePxCnt >> shiftCnt)) * lineCnt;
				}
				else
				{
					imgSize += (1 + (linePxCnt >> shiftCnt)) * lineCnt;
				}
			}

			//Pass5
			linePxCnt = (imgW + 1) >> 1;
			lineCnt = (imgH + 1) >> 2;
			if (linePxCnt > 0 && lineCnt > 0)
			{
				if (linePxCnt & andMask)
				{
					imgSize += (2 + (linePxCnt >> shiftCnt)) * lineCnt;
				}
				else
				{
					imgSize += (1 + (linePxCnt >> shiftCnt)) * lineCnt;
				}
			}

			//Pass6
			linePxCnt = (imgW + 0) >> 1;
			lineCnt = (imgH + 1) >> 1;
			if (linePxCnt > 0 && lineCnt > 0)
			{
				if (linePxCnt & andMask)
				{
					imgSize += (2 + (linePxCnt >> shiftCnt)) * lineCnt;
				}
				else
				{
					imgSize += (1 + (linePxCnt >> shiftCnt)) * lineCnt;
				}
			}

			//Pass7
			linePxCnt = imgW;
			lineCnt = (imgH + 0) >> 1;
			if (linePxCnt > 0 && lineCnt > 0)
			{
				if (linePxCnt & andMask)
				{
					imgSize += (2 + (linePxCnt >> shiftCnt)) * lineCnt;
				}
				else
				{
					imgSize += (1 + (linePxCnt >> shiftCnt)) * lineCnt;
				}
			}
		}
		else if (bitDepth <= 8)
		{
			if (imgW & andMask)
			{
				imgSize = (2 + ((UOSInt)imgW >> shiftCnt)) * (UOSInt)imgH;
			}
			else
			{
				imgSize = (1 + ((UOSInt)imgW >> shiftCnt)) * (UOSInt)imgH;
			}
		}
		else
		{
			imgSize = ((((UOSInt)imgW * bitDepth) >> 3) + 1) * (UOSInt)imgH;
		}
	}
	else if (colorType == 4)
	{
		if (interlaceMeth == 1)
		{
			UOSInt linePxCnt;
			UOSInt lineCnt;
			imgSize = 0;

			//Pass1
			linePxCnt = (imgW + 7) >> 3;
			lineCnt = (imgH + 7) >> 3;
			if (linePxCnt > 0)
				imgSize += (1 + (linePxCnt * byteDepth * 2)) * lineCnt;

			//Pass2
			linePxCnt = (imgW + 3) >> 3;
			lineCnt = (imgH + 7) >> 3;
			if (linePxCnt > 0)
				imgSize += (1 + (linePxCnt * byteDepth * 2)) * lineCnt;

			//Pass3
			linePxCnt = (imgW + 3) >> 2;
			lineCnt = (imgH + 3) >> 3;
			if (linePxCnt > 0)
				imgSize += (1 + (linePxCnt * byteDepth * 2)) * lineCnt;

			//Pass4
			linePxCnt = (imgW + 1) >> 2;
			lineCnt = (imgH + 3) >> 2;
			if (linePxCnt > 0)
				imgSize += (1 + (linePxCnt * byteDepth * 2)) * lineCnt;

			//Pass5
			linePxCnt = (imgW + 1) >> 1;
			lineCnt = (imgH + 1) >> 2;
			if (linePxCnt > 0)
				imgSize += (1 + (linePxCnt * byteDepth * 2)) * lineCnt;

			//Pass6
			linePxCnt = (imgW + 0) >> 1;
			lineCnt = (imgH + 1) >> 1;
			if (linePxCnt > 0)
				imgSize += (1 + (linePxCnt * byteDepth * 2)) * lineCnt;

			//Pass7
			linePxCnt = imgW;
			lineCnt = (imgH + 0) >> 1;
			if (linePxCnt > 0)
				imgSize += (1 + (linePxCnt * byteDepth * 2)) * lineCnt;
		}
		else
		{
			imgSize = (imgW * byteDepth * 2 + 1) * imgH;
		}
	}
	else if (colorType == 6)
	{
		if (interlaceMeth == 1)
		{
			UOSInt linePxCnt;
			UOSInt lineCnt;
			imgSize = 0;

			//Pass1
			linePxCnt = (imgW + 7) >> 3;
			lineCnt = (imgH + 7) >> 3;
			if (linePxCnt > 0)
				imgSize += (1 + (linePxCnt * byteDepth * 4)) * lineCnt;

			//Pass2
			linePxCnt = (imgW + 3) >> 3;
			lineCnt = (imgH + 7) >> 3;
			if (linePxCnt > 0)
				imgSize += (1 + (linePxCnt * byteDepth * 4)) * lineCnt;

			//Pass3
			linePxCnt = (imgW + 3) >> 2;
			lineCnt = (imgH + 3) >> 3;
			if (linePxCnt > 0)
				imgSize += (1 + (linePxCnt * byteDepth * 4)) * lineCnt;

			//Pass4
			linePxCnt = (imgW + 1) >> 2;
			lineCnt = (imgH + 3) >> 2;
			if (linePxCnt > 0)
				imgSize += (1 + (linePxCnt * byteDepth * 4)) * lineCnt;

			//Pass5
			linePxCnt = (imgW + 1) >> 1;
			lineCnt = (imgH + 1) >> 2;
			if (linePxCnt > 0)
				imgSize += (1 + (linePxCnt * byteDepth * 4)) * lineCnt;

			//Pass6
			linePxCnt = (imgW + 0) >> 1;
			lineCnt = (imgH + 1) >> 1;
			if (linePxCnt > 0)
				imgSize += (1 + (linePxCnt * byteDepth * 4)) * lineCnt;

			//Pass7
			linePxCnt = imgW;
			lineCnt = (imgH + 0) >> 1;
			if (linePxCnt > 0)
				imgSize += (1 + (linePxCnt * byteDepth * 4)) * lineCnt;
		}
		else
		{
			imgSize = (imgW * byteDepth * 4 + 1) * imgH;
		}
	}
	else
	{
		imgSize = 0;
	}
	return imgSize;
}