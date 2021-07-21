#include "Stdafx.h"
#include "SIMD.h"
#include "Data/ByteTool.h"

extern "C" void GRFilter_ProcessLayer32H(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt sbpl, OSInt dbpl, OSInt level, OSInt hOfst, OSInt vOfst)
{
	OSInt targetOfst = 0;
	OSInt loopX = width - 1;
	OSInt loopY = height - 1;
	OSInt sAdd;
	OSInt dAdd;
	OSInt i;
//	OSInt v;
	srcPtr += 4 + sbpl;
	destPtr += 4 + dbpl;
	if (hOfst < 0)
	{
		targetOfst += hOfst * 4;
		loopX += hOfst + 1;
		srcPtr += (-hOfst - 1) * 4;
		destPtr -= 4;
	}
	else
	{
		targetOfst += hOfst * 4;
		loopX -= hOfst;
		destPtr += hOfst * 4;
	}
	if (vOfst < 0)
	{
		targetOfst += vOfst * dbpl;
		loopY += vOfst + 1;
		srcPtr += (-vOfst - 1) * sbpl;
		destPtr -= dbpl;
	}
	else
	{
		targetOfst += vOfst * dbpl;
		loopY -= vOfst;
		destPtr += vOfst * dbpl;
	}
	UInt8x4 zeroU8x4 = PUInt8x4Clear();
	Int16x4 levelx4 = PInt16x4SetA((Int16)level);
	Int16x4 lastSrc;
	Int16x4 thisSrc;
	sAdd = sbpl - loopX * 4;
	dAdd = dbpl - loopX * 4;
	lastSrc = PCONVU16x4_I(PUNPCKUBW4(PLoadUInt8x4(&srcPtr[-4]), zeroU8x4));
	while (loopY-- > 0)
	{
		i = loopX;
		while (i-- > 0)
		{
			thisSrc = PCONVU16x4_I(PUNPCKUBW4(PLoadUInt8x4(&srcPtr[0]), zeroU8x4));
			PStoreUInt8x4(destPtr, PADDSUWB4(PUNPCKUBW4(PLoadUInt8x4(&destPtr[0]), zeroU8x4), PCONVI16x4_U(PMULHW4(PSUBW4(thisSrc, lastSrc), levelx4))));
			lastSrc = thisSrc;
/*			v = destPtr[0] + ((((Int32)srcPtr[0] - (Int32)srcPtr[-4]) * level) >> 8);
			if (v < 0)
				destPtr[0] = 0;
			else if (v > 255)
				destPtr[0] = 255;
			else
				destPtr[0] = (UInt8)v;

			v = destPtr[1] + ((((Int32)srcPtr[1] - (Int32)srcPtr[-3]) * level) >> 8);
			if (v < 0)
				destPtr[1] = 0;
			else if (v > 255)
				destPtr[1] = 255;
			else
				destPtr[1] = (UInt8)v;

			v = destPtr[2] + ((((Int32)srcPtr[2] - (Int32)srcPtr[-2]) * level) >> 8);
			if (v < 0)
				destPtr[2] = 0;
			else if (v > 255)
				destPtr[2] = 255;
			else
				destPtr[2] = (UInt8)v;

			v = destPtr[3] + ((((Int32)srcPtr[3] - (Int32)srcPtr[-1]) * level) >> 8);
			if (v < 0)
				destPtr[3] = 0;
			else if (v > 255)
				destPtr[3] = 255;
			else
				destPtr[3] = (UInt8)v;*/
			srcPtr += 4;
			destPtr += 4;
		}
		srcPtr += sAdd;
		destPtr += dAdd;
	}
}

extern "C" void GRFilter_ProcessLayer32V(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt sbpl, OSInt dbpl, OSInt level, OSInt hOfst, OSInt vOfst)
{
	OSInt targetOfst = 0;
	OSInt loopX = width - 1;
	OSInt loopY = height - 1;
	OSInt sAdd;
	OSInt dAdd;
	OSInt i;
	OSInt v;
	srcPtr += 4 + sbpl;
	destPtr += 4 + dbpl;
	if (hOfst < 0)
	{
		targetOfst += hOfst * 4;
		loopX += hOfst + 1;
		srcPtr += (-hOfst - 1) * 4;
		destPtr -= 4;
	}
	else
	{
		targetOfst += hOfst * 4;
		loopX -= hOfst;
		destPtr += hOfst * 4;
	}
	if (vOfst < 0)
	{
		targetOfst += vOfst * dbpl;
		loopY += vOfst + 1;
		srcPtr += (-vOfst - 1) * sbpl;
		destPtr -= dbpl;
	}
	else
	{
		targetOfst += vOfst * dbpl;
		loopY -= vOfst;
		destPtr += vOfst * dbpl;
	}
	sAdd = sbpl - loopX * 4;
	dAdd = dbpl - loopX * 4;
	while (loopY-- > 0)
	{
		i = loopX;
		while (i-- > 0)
		{
			v = destPtr[0] + ((((Int32)srcPtr[0] - (Int32)srcPtr[-sbpl]) * level) >> 8);
			if (v < 0)
				destPtr[0] = 0;
			else if (v > 255)
				destPtr[0] = 255;
			else
				destPtr[0] = (UInt8)v;

			v = destPtr[1] + ((((Int32)srcPtr[1] - (Int32)srcPtr[-sbpl+1]) * level) >> 8);
			if (v < 0)
				destPtr[1] = 0;
			else if (v > 255)
				destPtr[1] = 255;
			else
				destPtr[1] = (UInt8)v;

			v = destPtr[2] + ((((Int32)srcPtr[2] - (Int32)srcPtr[-sbpl+2]) * level) >> 8);
			if (v < 0)
				destPtr[2] = 0;
			else if (v > 255)
				destPtr[2] = 255;
			else
				destPtr[2] = (UInt8)v;

			v = destPtr[3] + ((((Int32)srcPtr[3] - (Int32)srcPtr[-sbpl+3]) * level) >> 8);
			if (v < 0)
				destPtr[3] = 0;
			else if (v > 255)
				destPtr[3] = 255;
			else
				destPtr[3] = (UInt8)v;
			srcPtr += 4;
			destPtr += 4;
		}
		srcPtr += sAdd;
		destPtr += dAdd;
	}
}

extern "C" void GRFilter_ProcessLayer32HV(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt sbpl, OSInt dbpl, OSInt level, OSInt hOfst, OSInt vOfst)
{
	OSInt targetOfst = 0;
	OSInt loopX = width - 1;
	OSInt loopY = height - 1;
	OSInt sAdd;
	OSInt dAdd;
	OSInt i;
	OSInt v;
	srcPtr += 4 + sbpl;
	destPtr += 4 + dbpl;
	if (hOfst < 0)
	{
		targetOfst += hOfst * 4;
		loopX += hOfst + 1;
		srcPtr += (-hOfst - 1) * 4;
		destPtr -= 4;
	}
	else
	{
		targetOfst += hOfst * 4;
		loopX -= hOfst;
		destPtr += hOfst * 4;
	}
	if (vOfst < 0)
	{
		targetOfst += vOfst * dbpl;
		loopY += vOfst + 1;
		srcPtr += (-vOfst - 1) * sbpl;
		destPtr -= dbpl;
	}
	else
	{
		targetOfst += vOfst * dbpl;
		loopY -= vOfst;
		destPtr += vOfst * dbpl;
	}
	sAdd = sbpl - loopX * 4;
	dAdd = dbpl - loopX * 4;
	while (loopY-- > 0)
	{
		i = loopX;
		while (i-- > 0)
		{
			v = destPtr[0] + ((((Int32)srcPtr[0] + (Int32)srcPtr[0] - (Int32)srcPtr[-sbpl] - (Int32)srcPtr[-4]) * level) >> 8);
			if (v < 0)
				destPtr[0] = 0;
			else if (v > 255)
				destPtr[0] = 255;
			else
				destPtr[0] = (UInt8)v;

			v = destPtr[1] + ((((Int32)srcPtr[1] + (Int32)srcPtr[1] - (Int32)srcPtr[-sbpl+1] - (Int32)srcPtr[-3]) * level) >> 8);
			if (v < 0)
				destPtr[1] = 0;
			else if (v > 255)
				destPtr[1] = 255;
			else
				destPtr[1] = (UInt8)v;

			v = destPtr[2] + ((((Int32)srcPtr[2] + (Int32)srcPtr[2] - (Int32)srcPtr[-sbpl+2] - (Int32)srcPtr[-2]) * level) >> 8);
			if (v < 0)
				destPtr[2] = 0;
			else if (v > 255)
				destPtr[2] = 255;
			else
				destPtr[2] = (UInt8)v;

			v = destPtr[3] + ((((Int32)srcPtr[3] + (Int32)srcPtr[3] - (Int32)srcPtr[-sbpl+3] - (Int32)srcPtr[-1]) * level) >> 8);
			if (v < 0)
				destPtr[3] = 0;
			else if (v > 255)
				destPtr[3] = 255;
			else
				destPtr[3] = (UInt8)v;
			srcPtr += 4;
			destPtr += 4;
		}
		srcPtr += sAdd;
		destPtr += dAdd;
	}
}

extern "C" void GRFilter_ProcessLayer64H(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt sbpl, OSInt dbpl, OSInt level, OSInt hOfst, OSInt vOfst)
{
	OSInt targetOfst = 0;
	OSInt loopX = width - 1;
	OSInt loopY = height - 1;
	OSInt sAdd;
	OSInt dAdd;
	OSInt i;
	OSInt v;
	srcPtr += 8 + sbpl;
	destPtr += 8 + dbpl;
	if (hOfst < 0)
	{
		targetOfst += hOfst * 8;
		loopX += hOfst + 1;
		srcPtr += (-hOfst - 1) * 8;
		destPtr -= 8;
	}
	else
	{
		targetOfst += hOfst * 8;
		loopX -= hOfst;
		destPtr += hOfst * 8;
	}
	if (vOfst < 0)
	{
		targetOfst += vOfst * dbpl;
		loopY += vOfst + 1;
		srcPtr += (-vOfst - 1) * sbpl;
		destPtr -= dbpl;
	}
	else
	{
		targetOfst += vOfst * dbpl;
		loopY -= vOfst;
		destPtr += vOfst * dbpl;
	}
	sAdd = sbpl - loopX * 8;
	dAdd = dbpl - loopX * 8;
	while (loopY-- > 0)
	{
		i = loopX;
		while (i-- > 0)
		{
			v = ReadUInt16(&destPtr[0]) + ((((Int32)ReadUInt16(&srcPtr[0]) - (Int32)ReadUInt16(&srcPtr[-8])) * level) >> 16);
			if (v < 0)
				WriteInt16(&destPtr[0], 0);
			else if (v > 65535)
				WriteInt16(&destPtr[0], 65535);
			else
				WriteInt16(&destPtr[0], v);

			v = ReadUInt16(&destPtr[2]) + ((((Int32)ReadUInt16(&srcPtr[2]) - (Int32)ReadUInt16(&srcPtr[-6])) * level) >> 16);
			if (v < 0)
				WriteInt16(&destPtr[2], 0);
			else if (v > 65535)
				WriteInt16(&destPtr[2], 65535);
			else
				WriteInt16(&destPtr[2], v);

			v = ReadUInt16(&destPtr[4]) + ((((Int32)ReadUInt16(&srcPtr[4]) - (Int32)ReadUInt16(&srcPtr[-4])) * level) >> 16);
			if (v < 0)
				WriteInt16(&destPtr[4], 0);
			else if (v > 65535)
				WriteInt16(&destPtr[4], 65535);
			else
				WriteInt16(&destPtr[4], v);

			v = ReadUInt16(&destPtr[6]) + ((((Int32)ReadUInt16(&srcPtr[6]) - (Int32)ReadUInt16(&srcPtr[-2])) * level) >> 16);
			if (v < 0)
				WriteInt16(&destPtr[6], 0);
			else if (v > 65535)
				WriteInt16(&destPtr[6], 65535);
			else
				WriteInt16(&destPtr[6], v);
			srcPtr += 8;
			destPtr += 8;
		}
		srcPtr += sAdd;
		destPtr += dAdd;
	}
}

extern "C" void GRFilter_ProcessLayer64V(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt sbpl, OSInt dbpl, OSInt level, OSInt hOfst, OSInt vOfst)
{
	OSInt targetOfst = 0;
	OSInt loopX = width - 1;
	OSInt loopY = height - 1;
	OSInt sAdd;
	OSInt dAdd;
	OSInt i;
	OSInt v;
	srcPtr += 8 + sbpl;
	destPtr += 8 + dbpl;
	if (hOfst < 0)
	{
		targetOfst += hOfst * 8;
		loopX += hOfst + 1;
		srcPtr += (-hOfst - 1) * 8;
		destPtr -= 8;
	}
	else
	{
		targetOfst += hOfst * 8;
		loopX -= hOfst;
		destPtr += hOfst * 8;
	}
	if (vOfst < 0)
	{
		targetOfst += vOfst * dbpl;
		loopY += vOfst + 1;
		srcPtr += (-vOfst - 1) * sbpl;
		destPtr -= dbpl;
	}
	else
	{
		targetOfst += vOfst * dbpl;
		loopY -= vOfst;
		destPtr += vOfst * dbpl;
	}
	sAdd = sbpl - loopX * 8;
	dAdd = dbpl - loopX * 8;
	while (loopY-- > 0)
	{
		i = loopX;
		while (i-- > 0)
		{
			v = ReadUInt16(&destPtr[0]) + ((((Int32)ReadUInt16(&srcPtr[0]) - (Int32)ReadUInt16(&srcPtr[-sbpl+0])) * level) >> 16);
			if (v < 0)
				WriteInt16(&destPtr[0], 0);
			else if (v > 65535)
				WriteInt16(&destPtr[0], 65535);
			else
				WriteInt16(&destPtr[0], v);

			v = ReadUInt16(&destPtr[2]) + ((((Int32)ReadUInt16(&srcPtr[2]) - (Int32)ReadUInt16(&srcPtr[-sbpl+2])) * level) >> 16);
			if (v < 0)
				WriteInt16(&destPtr[2], 0);
			else if (v > 65535)
				WriteInt16(&destPtr[2], 65535);
			else
				WriteInt16(&destPtr[2], v);

			v = ReadUInt16(&destPtr[4]) + ((((Int32)ReadUInt16(&srcPtr[4]) - (Int32)ReadUInt16(&srcPtr[-sbpl+4])) * level) >> 16);
			if (v < 0)
				WriteInt16(&destPtr[4], 0);
			else if (v > 65535)
				WriteInt16(&destPtr[4], 65535);
			else
				WriteInt16(&destPtr[4], v);

			v = ReadUInt16(&destPtr[6]) + ((((Int32)ReadUInt16(&srcPtr[6]) - (Int32)ReadUInt16(&srcPtr[-sbpl+6])) * level) >> 16);
			if (v < 0)
				WriteInt16(&destPtr[6], 0);
			else if (v > 65535)
				WriteInt16(&destPtr[6], 65535);
			else
				WriteInt16(&destPtr[6], v);

			srcPtr += 8;
			destPtr += 8;
		}
		srcPtr += sAdd;
		destPtr += dAdd;
	}
}

extern "C" void GRFilter_ProcessLayer64HV(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt sbpl, OSInt dbpl, OSInt level, OSInt hOfst, OSInt vOfst)
{
	OSInt targetOfst = 0;
	OSInt loopX = width - 1;
	OSInt loopY = height - 1;
	OSInt sAdd;
	OSInt dAdd;
	OSInt i;
	OSInt v;
	srcPtr += 8 + sbpl;
	destPtr += 8 + dbpl;
	if (hOfst < 0)
	{
		targetOfst += hOfst * 8;
		loopX += hOfst + 1;
		srcPtr += (-hOfst - 1) * 8;
		destPtr -= 8;
	}
	else
	{
		targetOfst += hOfst * 8;
		loopX -= hOfst;
		destPtr += hOfst * 8;
	}
	if (vOfst < 0)
	{
		targetOfst += vOfst * dbpl;
		loopY += vOfst + 1;
		srcPtr += (-vOfst - 1) * sbpl;
		destPtr -= dbpl;
	}
	else
	{
		targetOfst += vOfst * dbpl;
		loopY -= vOfst;
		destPtr += vOfst * dbpl;
	}
	sAdd = sbpl - loopX * 8;
	dAdd = dbpl - loopX * 8;
	while (loopY-- > 0)
	{
		i = loopX;
		while (i-- > 0)
		{
			v = ReadUInt16(&destPtr[0]) + ((((Int32)ReadUInt16(&srcPtr[0]) + (Int32)ReadUInt16(&srcPtr[0]) - (Int32)ReadUInt16(&srcPtr[-sbpl+0]) - (Int32)ReadUInt16(&srcPtr[-0])) * level) >> 16);
			if (v < 0)
				WriteInt16(&destPtr[0], 0);
			else if (v > 65535)
				WriteInt16(&destPtr[0], 65535);
			else
				WriteInt16(&destPtr[0], v);

			v = ReadUInt16(&destPtr[2]) + ((((Int32)ReadUInt16(&srcPtr[2]) + (Int32)ReadUInt16(&srcPtr[2]) - (Int32)ReadUInt16(&srcPtr[-sbpl+2]) - (Int32)ReadUInt16(&srcPtr[-2])) * level) >> 16);
			if (v < 0)
				WriteInt16(&destPtr[2], 0);
			else if (v > 65535)
				WriteInt16(&destPtr[2], 65535);
			else
				WriteInt16(&destPtr[2], v);

			v = ReadUInt16(&destPtr[4]) + ((((Int32)ReadUInt16(&srcPtr[4]) + (Int32)ReadUInt16(&srcPtr[4]) - (Int32)ReadUInt16(&srcPtr[-sbpl+4]) - (Int32)ReadUInt16(&srcPtr[-4])) * level) >> 16);
			if (v < 0)
				WriteInt16(&destPtr[4], 0);
			else if (v > 65535)
				WriteInt16(&destPtr[4], 65535);
			else
				WriteInt16(&destPtr[4], v);

			v = ReadUInt16(&destPtr[6]) + ((((Int32)ReadUInt16(&srcPtr[6]) + (Int32)ReadUInt16(&srcPtr[6]) - (Int32)ReadUInt16(&srcPtr[-sbpl+6]) - (Int32)ReadUInt16(&srcPtr[-6])) * level) >> 16);
			if (v < 0)
				WriteInt16(&destPtr[6], 0);
			else if (v > 65535)
				WriteInt16(&destPtr[6], 65535);
			else
				WriteInt16(&destPtr[6], v);

			srcPtr += 8;
			destPtr += 8;
		}
		srcPtr += sAdd;
		destPtr += dAdd;
	}
}
