#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Math/Math.h"
#include "Media/EDID.h"
#include "Text/MyString.h"

Bool Media::EDID::Parse(UnsafeArray<const UInt8> edidBuff, NN<Media::EDID::EDIDInfo> info)
{
	if (edidBuff[0] != 0 || edidBuff[1] != 0xff || edidBuff[2] != 0xff || edidBuff[3] != 0xff ||edidBuff[4] != 0xff || edidBuff[5] != 0xff || edidBuff[6] != 0xff || edidBuff[7] != 0)
	{
		return false;
	}

	info->vendorName[0] = (UTF8Char)(0x40 + ((edidBuff[8] >> 2) & 0x1f));
	info->vendorName[1] = (UTF8Char)(0x40 + ((edidBuff[9] >> 5) | ((edidBuff[8] << 3) & 0x1f)));
	info->vendorName[2] = (UTF8Char)(0x40 + (edidBuff[9] & 0x1f));
	info->vendorName[3] = 0;
	info->productCode = ReadUInt16(&edidBuff[0xa]);
	info->sn = ReadUInt32(&edidBuff[0x0c]);
	info->weekOfManu = edidBuff[0x10];
	info->yearOfManu = edidBuff[0x11] + 1990;
	info->edidVer = edidBuff[0x12];
	info->edidRev = edidBuff[0x13];
	info->pixelW = 0;
	info->pixelH = 0;
	info->bitPerColor = 0;
	if (edidBuff[0x14] & 0x80)
	{
		switch ((edidBuff[0x14] >> 4) & 7)
		{
		case 1:
			info->bitPerColor = 6;
			break;
		case 2:
			info->bitPerColor = 8;
			break;
		case 3:
			info->bitPerColor = 10;
			break;
		case 4:
			info->bitPerColor = 12;
			break;
		case 5:
			info->bitPerColor = 14;
			break;
		case 6:
			info->bitPerColor = 16;
			break;
		}
	}
	info->dispPhysicalW_mm = (UInt32)edidBuff[0x15] * 10;
	info->dispPhysicalH_mm = (UInt32)edidBuff[0x16] * 10;
	info->gamma = (edidBuff[0x17] + 100) * 0.01;

	info->r.x = OSInt2Double((((OSInt)edidBuff[0x1b]) << 2) | (edidBuff[0x19] >> 6))/ 1024.0;
	info->r.y = OSInt2Double((((OSInt)edidBuff[0x1c]) << 2) | ((edidBuff[0x19] >> 4) & 3))/ 1024.0;
	info->g.x = OSInt2Double((((OSInt)edidBuff[0x1d]) << 2) | ((edidBuff[0x19] >> 2) & 3))/ 1024.0;
	info->g.y = OSInt2Double((((OSInt)edidBuff[0x1e]) << 2) | (edidBuff[0x19] & 3))/ 1024.0;
	info->b.x = OSInt2Double((((OSInt)edidBuff[0x1f]) << 2) | (edidBuff[0x1a] >> 6))/ 1024.0;
	info->b.y = OSInt2Double((((OSInt)edidBuff[0x20]) << 2) | ((edidBuff[0x1a] >> 4) & 3))/ 1024.0;
	info->w.x = OSInt2Double((((OSInt)edidBuff[0x21]) << 2) | ((edidBuff[0x1a] >> 2) & 3))/ 1024.0;
	info->w.y = OSInt2Double((((OSInt)edidBuff[0x22]) << 2) | (edidBuff[0x1a] & 3))/ 1024.0;
	info->monitorName[0] = 0;
	info->monitorSN[0] = 0;
	info->monitorOther[0] = 0;
	ParseDescriptor(info, &edidBuff[54]);
	ParseDescriptor(info, &edidBuff[72]);
	ParseDescriptor(info, &edidBuff[90]);
	ParseDescriptor(info, &edidBuff[108]);
	return true;
}

void Media::EDID::ParseDescriptor(NN<EDIDInfo> info, const UInt8 *descriptor)
{
	OSInt i;
	UTF8Char *sptr;
	const UInt8 *buffPtr;
	UInt8 b;
	if (descriptor[0] == 0 && descriptor[1] == 0)
	{
		if (descriptor[3] == 0xfc)
		{
			sptr = info->monitorName;
		}
		else if (descriptor[3] == 0xfe)
		{
			sptr = info->monitorOther;
		}
		else if (descriptor[3] == 0xff)
		{
			sptr = info->monitorSN;
		}
		else
		{
			sptr = 0;
		}
		if (sptr)
		{
			i = 13;
			buffPtr = &descriptor[5];
			while (i-- > 0)
			{
				b = *buffPtr++;
				if (b == 0 || b == 10)
				{
					break;
				}
				if (b & 0x80)
				{
					*sptr++ = '-';
				}
				else
				{
					*sptr++ = b;
				}
			}
			*sptr = 0;
		}
	}
	else
	{
		info->pixelW = descriptor[2] + (UInt32)(descriptor[4] & 0xf0) * 16;
		info->pixelH = descriptor[5] + (UInt32)(descriptor[7] & 0xf0) * 16;
		UInt32 newW = descriptor[12] + (UInt32)(descriptor[14] & 0xf0) * 16;
		UInt32 newH = descriptor[13] + (UInt32)(descriptor[14] & 0xf) * 256;
		if (info->dispPhysicalW_mm > newW - 10 && info->dispPhysicalW_mm < newW + 10)
		{
			info->dispPhysicalW_mm = newW;
		}
		if (info->dispPhysicalH_mm > newH - 10 && info->dispPhysicalH_mm < newH + 10)
		{
			info->dispPhysicalH_mm = newH;
		}
	}
}

Bool Media::EDID::SetColorProfile(NN<EDIDInfo> info, NN<Media::ColorProfile> cp)
{
	if (info->w.x != 0 || info->w.y != 0)
	{
		cp->primaries.colorType = Media::ColorProfile::CT_CUSTOM;
		cp->primaries.w = info->w;
		cp->primaries.r = info->r;
		cp->primaries.g = info->g;
		cp->primaries.b = info->b;
		if (info->gamma >= 3.5)
		{
			cp->rtransfer.Set(Media::CS::TRANT_sRGB, 2.2);
			cp->gtransfer.Set(Media::CS::TRANT_sRGB, 2.2);
			cp->btransfer.Set(Media::CS::TRANT_sRGB, 2.2);
		}
		else
		{
			cp->rtransfer.Set(Media::CS::TRANT_GAMMA, info->gamma);
			cp->gtransfer.Set(Media::CS::TRANT_GAMMA, info->gamma);
			cp->btransfer.Set(Media::CS::TRANT_GAMMA, info->gamma);
		}
		return true;
	}
	return false;
}
