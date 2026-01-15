#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "Math/Unit/Distance.h"
#include "Media/BitmapUtil.h"

Optional<Media::StaticImage> Media::BitmapUtil::ParseDIBBuffer(const UInt8 *dataBuff, UIntOS dataSize)
{
	IntOS imgWidth;
	IntOS imgHeight;
	UInt32 bpp;
	Double hdpi = 96.0;
	Double vdpi = 96.0;
	UIntOS imgPos;
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
		return nullptr;
	}

	Bool inv = true;
	if (imgHeight < 0)
	{
		inv = false;
		imgHeight = -imgHeight;
	}
	UIntOS palSize = 0;
	if (bpp <= 8)
	{
		palSize = (UIntOS)(4 << bpp);
	}

	NEW_CLASS(outImg, Media::StaticImage(Math::Size2D<UIntOS>((UIntOS)imgWidth, (UIntOS)imgHeight), 0, bpp, Media::PixelFormatGetDef(0, bpp), 0, Media::ColorProfile(), Media::ColorProfile::YUVT_UNKNOWN, (bpp == 32)?Media::AT_ALPHA:Media::AT_IGNORE_ALPHA, Media::YCOFST_C_CENTER_LEFT));
	outImg->info.hdpi = hdpi;
	outImg->info.vdpi = vdpi;
	UnsafeArray<UInt8> pBits = outImg->data;
	UIntOS lineW;
	UIntOS lineW2;
	UIntOS currOfst;
	UnsafeArray<UInt8> pal;
	Int32 i;
	if (inv)
	{
		switch (bpp)
		{
		case 4:
			if (outImg->pal.SetTo(pal))
			{
	 			MemCopyNO(pal.Ptr(), &dataBuff[imgPos], palSize);
			}
			imgPos += palSize;
			if (outImg->pal.SetTo(pal))
			{
				i = 16;
				while (i-- > 0)
				{
					pal[(i << 2) + 3] = 0xff;
				}
			}
			lineW = (UIntOS)((imgWidth >> 1) + (imgWidth & 1));
			lineW2 = lineW;
			if (lineW & 3)
			{
				lineW = lineW + 4 - (lineW & 3);
			}
			currOfst = (lineW * (UIntOS)imgHeight) + imgPos;
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
					MemCopyNO(pBits.Ptr(), &dataBuff[currOfst], lineW2);
					pBits += lineW2;
				}
			}
			break;
		case 8:
			if (outImg->pal.SetTo(pal))
			{
	 			MemCopyNO(pal.Ptr(), &dataBuff[imgPos], palSize);
			}
			imgPos += palSize;
			if (outImg->pal.SetTo(pal))
			{
				i = 256;
				while (i-- > 0)
				{
					pal[(i << 2) + 3] = 0xff;
				}
			}
			lineW = (UIntOS)imgWidth;
			if (lineW & 3)
			{
				lineW = lineW + 4 - (lineW & 3);
			}
			currOfst = (lineW * (UIntOS)imgHeight) + imgPos;
			while (imgHeight-- > 0)
			{
				currOfst -= lineW;
				MemCopyNO(pBits.Ptr(), &dataBuff[currOfst], (UIntOS)imgWidth);
				pBits += imgWidth;
			}
			break;
		case 16:
			lineW = (UIntOS)imgWidth << 1;
			if (lineW & 3)
			{
				lineW = lineW + 4 - (lineW & 3);
			}
			lineW2 = (UIntOS)imgWidth << 1;
			currOfst = (lineW * (UIntOS)imgHeight) + imgPos;
			while (imgHeight-- > 0)
			{
				currOfst -= lineW;
				MemCopyNO(pBits.Ptr(), &dataBuff[currOfst], lineW2);
				pBits += lineW2;
			}
			break;
		case 24:
			lineW = (UIntOS)imgWidth * 3;
			if (lineW & 3)
			{
				lineW = lineW + 4 - (lineW & 3);
			}
			lineW2 = (UIntOS)imgWidth * 3;
			currOfst = (lineW * (UIntOS)imgHeight) + imgPos;
			while (imgHeight-- > 0)
			{
				currOfst -= lineW;
				MemCopyNO(pBits.Ptr(), &dataBuff[currOfst], lineW2);
				pBits += lineW2;
			}
			break;
		case 32:
			currOfst = imgPos;
			pBits = pBits + imgWidth * imgHeight * 4;
			while (imgHeight-- > 0)
			{
				pBits -= imgWidth << 2;
				MemCopyNO(pBits.Ptr(), &dataBuff[currOfst], (UIntOS)imgWidth << 2);
				currOfst += (UIntOS)imgWidth << 2;
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
		lineW = ((UIntOS)imgWidth * bpp >> 3);
		if (lineW & 3)
		{
			lineW2 = lineW + 4 - (lineW & 3);
			while (imgHeight-- > 0)
			{
				MemCopyNO(pBits.Ptr(), &dataBuff[currOfst], lineW);
				pBits += lineW;
				currOfst += lineW2;
			}
		}
		else
		{
			MemCopyNO(pBits.Ptr(), &dataBuff[currOfst], (UIntOS)imgHeight * lineW);
		}
	}
	return outImg;
}