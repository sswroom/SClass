#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Math/Unit/Distance.h"
#include "Media/BitmapUtil.h"

Media::StaticImage *Media::BitmapUtil::ParseDIBBuffer(const UInt8 *dataBuff, OSInt dataSize)
{
	OSInt imgWidth;
	OSInt imgHeight;
	Int32 bpp;
	Double hdpi = 96.0;
	Double vdpi = 96.0;
	OSInt imgPos;
	Media::StaticImage *outImg = 0;

	if (ReadInt32(&dataBuff[0]) == 12)
	{
		imgWidth = ReadInt16(&dataBuff[4]);
		imgHeight = ReadInt16(&dataBuff[6]);
		bpp = ReadUInt16(&dataBuff[10]);
		imgPos = 12;
	}
	else if (ReadInt32(&dataBuff[0]) == 40)
	{
		imgWidth = ReadInt32(&dataBuff[4]);
		imgHeight = ReadInt32(&dataBuff[8]);
		bpp = ReadUInt16(&dataBuff[14]);
		imgPos = 40;
		hdpi = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, ReadInt32(&dataBuff[24]));
		vdpi = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, ReadInt32(&dataBuff[28]));
	}
	else
	{
		return 0;
	}

	Bool inv = true;
	if (imgHeight < 0)
	{
		inv = false;
		imgHeight = -imgHeight;
	}
	OSInt palSize = 0;
	if (bpp <= 8)
	{
		palSize = 4 << bpp;
	}

	NEW_CLASS(outImg, Media::StaticImage(imgWidth, imgHeight, 0, bpp, Media::FrameInfo::GetDefPixelFormat(0, bpp), 0, 0, Media::ColorProfile::YUVT_UNKNOWN, (bpp == 32)?Media::AT_ALPHA:Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
	outImg->info->hdpi = hdpi;
	outImg->info->vdpi = vdpi;
	UInt8 *pBits = (UInt8*)outImg->data;
	OSInt lineW;
	OSInt lineW2;
	OSInt currOfst;
	Int32 i;
	if (inv)
	{
		switch (bpp)
		{
		case 4:
 			MemCopyNO(outImg->pal, &dataBuff[imgPos], palSize);
			imgPos += palSize;
			i = 16;
			while (i-- > 0)
			{
				outImg->pal[(i << 2) + 3] = 0xff;
			}
			lineW = (imgWidth >> 1) + (imgWidth & 1);
			lineW2 = lineW;
			if (lineW & 3)
			{
				lineW = lineW + 4 - (lineW & 3);
			}
			currOfst = (lineW * imgHeight) + imgPos;
			if (currOfst > dataSize)
			{
				DEL_CLASS(outImg);
				outImg = 0;
			}
			else
			{
				while (imgHeight-- > 0)
				{
					currOfst -= lineW;
					MemCopyNO(pBits, &dataBuff[currOfst], lineW2);
					pBits += lineW2;
				}
			}
			break;
		case 8:
 			MemCopyNO(outImg->pal, &dataBuff[imgPos], palSize);
			imgPos += palSize;
			i = 256;
			while (i-- > 0)
			{
				outImg->pal[(i << 2) + 3] = 0xff;
			}
			lineW = imgWidth;
			if (lineW & 3)
			{
				lineW = lineW + 4 - (lineW & 3);
			}
			currOfst = (lineW * imgHeight) + imgPos;
			while (imgHeight-- > 0)
			{
				currOfst -= lineW;
				MemCopyNO(pBits, &dataBuff[currOfst], imgWidth);
				pBits += imgWidth;
			}
			break;
		case 16:
			lineW = imgWidth << 1;
			if (lineW & 3)
			{
				lineW = lineW + 4 - (lineW & 3);
			}
			lineW2 = imgWidth << 1;
			currOfst = (lineW * imgHeight) + imgPos;
			while (imgHeight-- > 0)
			{
				currOfst -= lineW;
				MemCopyNO(pBits, &dataBuff[currOfst], lineW2);
				pBits += lineW2;
			}
			break;
		case 24:
			lineW = imgWidth * 3;
			if (lineW & 3)
			{
				lineW = lineW + 4 - (lineW & 3);
			}
			lineW2 = imgWidth * 3;
			currOfst = (lineW * imgHeight) + imgPos;
			while (imgHeight-- > 0)
			{
				currOfst -= lineW;
				MemCopyNO(pBits, &dataBuff[currOfst], lineW2);
				pBits += lineW2;
			}
			break;
		case 32:
			currOfst = imgPos;
			pBits = pBits + imgWidth * imgHeight * 4;
			while (imgHeight-- > 0)
			{
				pBits -= imgWidth << 2;
				MemCopyNO(pBits, &dataBuff[currOfst], imgWidth << 2);
				currOfst += imgWidth << 2;
			}
			break;
		default:
			DEL_CLASS(outImg);
			outImg = 0;
			break;
		};
	}
	else
	{
		currOfst = imgPos;
		lineW = (imgWidth * bpp >> 3);
		if (lineW & 3)
		{
			lineW2 = lineW + 4 - (lineW & 3);
			while (imgHeight-- > 0)
			{
				MemCopyNO(pBits, &dataBuff[currOfst], lineW);
				pBits += lineW;
				currOfst += lineW2;
			}
		}
		else
		{
			MemCopyNO(pBits, &dataBuff[currOfst], imgHeight * lineW);
		}
	}
	return outImg;
}