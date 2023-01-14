#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Math/Math.h"
#include "Media/EDID.h"
#include "Text/MyString.h"

Bool Media::EDID::Parse(const UInt8 *edidBuff, Media::EDID::EDIDInfo *info)
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

	info->rx = OSInt2Double((((OSInt)edidBuff[0x1b]) << 2) | (edidBuff[0x19] >> 6))/ 1024.0;
	info->ry = OSInt2Double((((OSInt)edidBuff[0x1c]) << 2) | ((edidBuff[0x19] >> 4) & 3))/ 1024.0;
	info->gx = OSInt2Double((((OSInt)edidBuff[0x1d]) << 2) | ((edidBuff[0x19] >> 2) & 3))/ 1024.0;
	info->gy = OSInt2Double((((OSInt)edidBuff[0x1e]) << 2) | (edidBuff[0x19] & 3))/ 1024.0;
	info->bx = OSInt2Double((((OSInt)edidBuff[0x1f]) << 2) | (edidBuff[0x1a] >> 6))/ 1024.0;
	info->by = OSInt2Double((((OSInt)edidBuff[0x20]) << 2) | ((edidBuff[0x1a] >> 4) & 3))/ 1024.0;
	info->wx = OSInt2Double((((OSInt)edidBuff[0x21]) << 2) | ((edidBuff[0x1a] >> 2) & 3))/ 1024.0;
	info->wy = OSInt2Double((((OSInt)edidBuff[0x22]) << 2) | (edidBuff[0x1a] & 3))/ 1024.0;
	info->monitorName[0] = 0;
	info->monitorSN[0] = 0;
	info->monitorOther[0] = 0;
	ParseDescriptor(info, &edidBuff[54]);
	ParseDescriptor(info, &edidBuff[72]);
	ParseDescriptor(info, &edidBuff[90]);
	ParseDescriptor(info, &edidBuff[108]);
	return true;
}

void Media::EDID::ParseDescriptor(EDIDInfo *info, const UInt8 *descriptor)
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
				*sptr++ = b;
			}
			*sptr = 0;
		}
	}
	else
	{
		info->pixelW = descriptor[2] + (UInt32)(descriptor[4] & 0xf0) * 16;
		info->pixelH = descriptor[5] + (UInt32)(descriptor[7] & 0xf0) * 16;
		info->dispPhysicalW_mm = descriptor[12] + (UInt32)(descriptor[14] & 0xf0) * 16;
		info->dispPhysicalH_mm = descriptor[13] + (UInt32)(descriptor[14] & 0xf) * 256;
	}
}

Bool Media::EDID::SetColorProfile(EDIDInfo *info, Media::ColorProfile *cp)
{
	if (info->wx != 0 || info->wy != 0)
	{
		cp->primaries.colorType = Media::ColorProfile::CT_CUSTOM;
		cp->primaries.wx = info->wx;
		cp->primaries.wy = info->wy;
		cp->primaries.rx = info->rx;
		cp->primaries.ry = info->ry;
		cp->primaries.gx = info->gx;
		cp->primaries.gy = info->gy;
		cp->primaries.bx = info->bx;
		cp->primaries.by = info->by;
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
