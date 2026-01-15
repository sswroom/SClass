#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"

extern "C" void RGBColorFilter_ProcessImagePart(UInt8 *srcPtr, UInt8 *destPtr, IntOS width, IntOS height, IntOS sbpl, IntOS dbpl, UInt8 *lut, Int32 bpp)
{
	IntOS i;
	Int16 rv;
	Int16 gv;
	Int16 bv;
	Int16 maxV;
	Int16 minV;
	if (bpp == 32)
	{
		sbpl -= width * 4;
		dbpl -= width * 4;
		while (height-- > 0)
		{
			i = width;
			while (i-- > 0)
			{
				bv = *(Int16*)&lut[srcPtr[0] * 2];
				maxV = bv;
				minV = bv;
				gv = *(Int16*)&lut[srcPtr[1] * 2];
				if (gv > maxV) maxV = gv;
				if (gv < minV) minV = gv;
				rv = *(Int16*)&lut[srcPtr[2] * 2];
				if (rv > maxV) maxV = rv;
				if (rv < minV) minV = rv;
				destPtr[3] = srcPtr[3];
				if (minV >= 8192)
				{
					destPtr[0] = lut[8704];
					destPtr[1] = lut[8704];
					destPtr[2] = lut[8704];
				}
				else if (maxV <= 8192)
				{
					destPtr[0] = lut[512 + (UInt16)bv];
					destPtr[1] = lut[512 + (UInt16)gv];
					destPtr[2] = lut[512 + (UInt16)rv];
				}
				else
				{
					destPtr[0] = lut[512 + ((minV + (bv - minV) * (8192 - minV) / (maxV - minV)) & 0xffff)];
					destPtr[1] = lut[512 + ((minV + (gv - minV) * (8192 - minV) / (maxV - minV)) & 0xffff)];
					destPtr[2] = lut[512 + ((minV + (rv - minV) * (8192 - minV) / (maxV - minV)) & 0xffff)];
				}
				srcPtr += 4;
				destPtr += 4;
			}
			srcPtr += sbpl;
			destPtr += dbpl;
		}
	}
	else if (bpp == 48)
	{
		sbpl -= width * 6;
		dbpl -= width * 6;
		while (height-- > 0)
		{
			i = width;
			while (i-- > 0)
			{
				bv = *(Int16*)&lut[ReadUInt16(&srcPtr[0]) * 2];
				maxV = bv;
				minV = bv;
				gv = *(Int16*)&lut[ReadUInt16(&srcPtr[2]) * 2];
				if (gv > maxV) maxV = gv;
				if (gv < minV) minV = gv;
				rv = *(Int16*)&lut[ReadUInt16(&srcPtr[4]) * 2];
				if (rv > maxV) maxV = rv;
				if (rv < minV) minV = rv;
				if (minV >= 8192)
				{
					WriteInt16(&destPtr[0], *(Int16*)&lut[147456]);
					WriteInt16(&destPtr[2], *(Int16*)&lut[147456]);
					WriteInt16(&destPtr[4], *(Int16*)&lut[147456]);
				}
				else if (maxV <= 8192)
				{
					WriteInt16(&destPtr[0], *(Int16*)&lut[131072 + 2 * (UInt16)bv]);
					WriteInt16(&destPtr[2], *(Int16*)&lut[131072 + 2 * (UInt16)gv]);
					WriteInt16(&destPtr[4], *(Int16*)&lut[131072 + 2 * (UInt16)rv]);
				}
				else
				{
					WriteInt16(&destPtr[0], *(Int16*)&lut[131072 + 2 * ((minV + (bv - minV) * (8192 - minV) / (maxV - minV)) & 0xffff)]);
					WriteInt16(&destPtr[2], *(Int16*)&lut[131072 + 2 * ((minV + (gv - minV) * (8192 - minV) / (maxV - minV)) & 0xffff)]);
					WriteInt16(&destPtr[4], *(Int16*)&lut[131072 + 2 * ((minV + (rv - minV) * (8192 - minV) / (maxV - minV)) & 0xffff)]);
				}
				srcPtr += 6;
				destPtr += 6;
			}
			srcPtr += sbpl;
			destPtr += dbpl;
		}
	}
}

extern "C" void RGBColorFilter_ProcessImageHDRPart(UInt8 *srcPtr, UInt8 *destPtr, IntOS width, IntOS height, IntOS sbpl, IntOS dbpl, UInt8 *lut, Int32 bpp)
{
}

extern "C" void RGBColorFilter_ProcessImageHDRDPart(UInt8 *srcPtr, UInt8 *destPtr, IntOS width, IntOS height, IntOS sbpl, IntOS dbpl, UInt8 *lut, Int32 bpp)
{
}

extern "C" void RGBColorFilter_ProcessImageHDRDLPart(UInt8 *srcPtr, UInt8 *destPtr, IntOS width, IntOS height, IntOS sbpl, IntOS dbpl, UInt8 *lut, Int32 bpp, Int32 hdrLev)
{
	IntOS i;
	Int32 rv;
	Int32 gv;
	Int32 bv;
	Int32 maxV;
	Int32 minV;
	if (bpp == 32)
	{
		sbpl -= width * 4;
		dbpl -= width * 4;
		while (height-- > 0)
		{
			i = width;
			while (i-- > 0)
			{
				bv = *(Int32*)&lut[66048 + srcPtr[0] * 4];
				maxV = bv;
				minV = bv;
				gv = *(Int32*)&lut[66048 + srcPtr[1] * 4];
				if (gv > maxV) maxV = gv;
				if (gv < minV) minV = gv;
				rv = *(Int32*)&lut[66048 + srcPtr[2] * 4];
				if (rv > maxV) maxV = rv;
				if (rv < minV) minV = rv;
				destPtr[3] = srcPtr[3];
				minV = minV >> hdrLev;
				if (minV >= 8192)
				{
					destPtr[0] = lut[8704];
					destPtr[1] = lut[8704];
					destPtr[2] = lut[8704];
				}
				else if (maxV <= 8192)
				{
					destPtr[0] = lut[512 + (bv & 0xffff)];
					destPtr[1] = lut[512 + (gv & 0xffff)];
					destPtr[2] = lut[512 + (rv & 0xffff)];
				}
				else
				{
					destPtr[0] = lut[512 + ((minV + (bv - minV) * (8192 - minV) / (maxV - minV)) & 0xffff)];
					destPtr[1] = lut[512 + ((minV + (gv - minV) * (8192 - minV) / (maxV - minV)) & 0xffff)];
					destPtr[2] = lut[512 + ((minV + (rv - minV) * (8192 - minV) / (maxV - minV)) & 0xffff)];
				}
				srcPtr += 4;
				destPtr += 4;
			}
			srcPtr += sbpl;
			destPtr += dbpl;
		}
	}
	else if (bpp == 48)
	{
		sbpl -= width * 6;
		dbpl -= width * 6;
		while (height-- > 0)
		{
			i = width;
			while (i-- > 0)
			{
				bv = *(Int32*)&lut[262144 + ReadUInt16(&srcPtr[0]) * 4];
				maxV = bv;
				minV = bv;
				gv = *(Int32*)&lut[262144 + ReadUInt16(&srcPtr[2]) * 4];
				if (gv > maxV) maxV = gv;
				if (gv < minV) minV = gv;
				rv = *(Int32*)&lut[262144 + ReadUInt16(&srcPtr[4]) * 4];
				if (rv > maxV) maxV = rv;
				if (rv < minV) minV = rv;
				minV = minV >> hdrLev;
				if (minV >= 8192)
				{
					WriteInt16(&destPtr[0], *(Int16*)&lut[147456]);
					WriteInt16(&destPtr[2], *(Int16*)&lut[147456]);
					WriteInt16(&destPtr[4], *(Int16*)&lut[147456]);
				}
				else if (maxV <= 8192)
				{
					WriteInt16(&destPtr[0], *(Int16*)&lut[131072 + 2 * (bv & 0xffff)]);
					WriteInt16(&destPtr[2], *(Int16*)&lut[131072 + 2 * (gv & 0xffff)]);
					WriteInt16(&destPtr[4], *(Int16*)&lut[131072 + 2 * (rv & 0xffff)]);
				}
				else
				{
					WriteInt16(&destPtr[0], *(Int16*)&lut[131072 + 2 * ((minV + (bv - minV) * (8192 - minV) / (maxV - minV)) & 0xffff)]);
					WriteInt16(&destPtr[2], *(Int16*)&lut[131072 + 2 * ((minV + (gv - minV) * (8192 - minV) / (maxV - minV)) & 0xffff)]);
					WriteInt16(&destPtr[4], *(Int16*)&lut[131072 + 2 * ((minV + (rv - minV) * (8192 - minV) / (maxV - minV)) & 0xffff)]);
				}
				srcPtr += 6;
				destPtr += 6;
			}
			srcPtr += sbpl;
			destPtr += dbpl;
		}
	}
}
