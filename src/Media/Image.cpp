#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/IImgResizer.h"
#include "Media/FrameInfo.h"
#include "Media/Image.h"
#include "Media/StaticImage.h"

Media::Image::Image(UOSInt dispWidth, UOSInt dispHeight, UOSInt storeWidth, UOSInt storeHeight, UInt32 fourcc, UInt32 bpp, Media::PixelFormat pf, UOSInt maxSize, const Media::ColorProfile *color, Media::ColorProfile::YUVType yuvType, Media::AlphaType atype, Media::YCOffset ycOfst)
{
	this->exif = 0;
	this->hasHotSpot = false;
	this->hotSpotX = 0;
	this->hotSpotY = 0;
	NEW_CLASS(this->info, Media::FrameInfo());
	info->dispWidth = dispWidth;
	info->dispHeight = dispHeight;
	info->storeWidth = storeWidth;
	info->storeHeight = storeHeight;
	info->fourcc = fourcc;
	info->storeBPP = bpp;
	info->pf = pf;
	info->byteSize = maxSize;
	info->par2 = 1;
	info->hdpi = 96;
	info->vdpi = 96;
	if (color == 0)
	{
		info->color->SetCommonProfile(Media::ColorProfile::CPT_PUNKNOWN);
	}
	else
	{
		info->color->Set(color);
	}
	info->atype = atype;
	info->yuvType = yuvType;
	info->ycOfst = ycOfst;
	info->ftype = Media::FT_NON_INTERLACE;

	if (pf == Media::PF_PAL_1_A1)
	{
		info->storeWidth = dispHeight;
		info->storeHeight = dispWidth;
		info->byteSize = ((dispWidth + 7) >> 3) * dispHeight * 2;
	}
	else if (pf == Media::PF_PAL_2_A1)
	{
		info->storeWidth = dispHeight;
		info->storeHeight = dispWidth;
		info->byteSize = (((dispWidth + 7) >> 3) + ((dispWidth + 3) >> 2)) * dispHeight;
	}
	else if (pf == Media::PF_PAL_4_A1)
	{
		info->storeWidth = dispHeight;
		info->storeHeight = dispWidth;
		info->byteSize = (((dispWidth + 7) >> 3) + ((dispWidth + 1) >> 1)) * dispHeight;
	}
	else if (pf == Media::PF_PAL_8_A1)
	{
		info->storeWidth = dispHeight;
		info->storeHeight = dispWidth;
		info->byteSize = (((dispWidth + 7) >> 3) + dispWidth) * dispHeight;
	}
	else
	{
		if (storeWidth == 0)
		{
			storeWidth = dispWidth;
			storeHeight = dispHeight;
			info->storeHeight = storeHeight;
			info->storeWidth = storeWidth;
			if (pf == Media::PF_PAL_1 || pf == Media::PF_PAL_W1)
			{
				if (dispWidth & 7)
				{
					storeWidth = dispWidth + 8 - (dispWidth & 7);
					info->storeWidth = storeWidth;
				}
			}
			else if (pf == Media::PF_PAL_2 || pf == Media::PF_PAL_W2 || pf == Media::PF_PAL_2_A1)
			{
				if (dispWidth & 3)
				{
					storeWidth = dispWidth + 4 - (dispWidth & 3);
					info->storeWidth = storeWidth;
				}
			}
			else if (pf == Media::PF_PAL_4 || pf == Media::PF_PAL_W4 || pf == Media::PF_PAL_4_A1)
			{
				if (dispWidth & 1)
				{
					storeWidth = dispWidth + 2 - (dispWidth & 1);
					info->storeWidth = storeWidth;
				}
			}
		}
		if (bpp)
		{
			info->byteSize = storeWidth * bpp;
			if (info->byteSize & 7)
				info->byteSize += 8 - (info->byteSize & 7);
			info->byteSize = storeHeight * (info->byteSize >> 3);
		}
	}
	this->pal = 0;
	if (info->fourcc == 0)
	{
		if (pf == Media::PF_PAL_1_A1)
		{
			this->pal = MemAlloc(UInt8, 4 << 1);
		}
		else if (pf == Media::PF_PAL_2_A1)
		{
			this->pal = MemAlloc(UInt8, 4 << 2);
		}
		else if (pf == Media::PF_PAL_4_A1)
		{
			this->pal = MemAlloc(UInt8, 4 << 4);
		}
		else if (pf == Media::PF_PAL_8_A1)
		{
			this->pal = MemAlloc(UInt8, 4 << 8);
		}
		else if (bpp <= 8)
		{
			UOSInt palSize = (UOSInt)(4 << bpp);
			this->pal = MemAlloc(UInt8, palSize);
		}
	}
}

Media::Image::~Image()
{
	if (this->pal)
	{
		MemFree(pal);
	}
	if (this->exif)
	{
		DEL_CLASS(this->exif);
	}
	DEL_CLASS(this->info);
}

UOSInt Media::Image::GetDataBpl()
{
	if (this->info->fourcc == *(UInt32*)"LRGB")
	{
		return (this->info->storeWidth << 3);
	}
	if (this->info->fourcc != 0 && this->info->fourcc != *(UInt32*)"DIBS")
		return 0;
	if (this->info->pf == Media::PF_PAL_1_A1)
	{
		return ((this->info->dispWidth + 7) >> 3) * 2;
	}
	else if (this->info->pf == Media::PF_PAL_2_A1)
	{
		return (((this->info->dispWidth + 7) >> 3) + ((this->info->dispWidth + 3) >> 2));
	}
	else if (this->info->pf == Media::PF_PAL_4_A1)
	{
		return (((this->info->dispWidth + 7) >> 3) + ((this->info->dispWidth + 1) >> 1));
	}
	else if (this->info->pf == Media::PF_PAL_8_A1)
	{
		return (((this->info->dispWidth + 7) >> 3) + this->info->dispWidth);
	}
	else if (this->info->storeBPP <= 0)
		return (this->info->storeWidth * (this->info->storeBPP >> 3));
	else if (this->info->storeBPP == 4)
		return ((this->info->storeWidth >> 1) + (this->info->storeWidth & 1));
	else
		return ((this->info->storeWidth * this->info->storeBPP) >> 3);
}

Bool Media::Image::IsUpsideDown()
{
	return false;
}

void Media::Image::SetHotSpot(OSInt hotSpotX, OSInt hotSpotY)
{
	this->hotSpotX = hotSpotX;
	this->hotSpotY = hotSpotY;
	this->hasHotSpot = true;
}

Bool Media::Image::HasHotSpot()
{
	return this->hasHotSpot;
}

OSInt Media::Image::GetHotSpotX()
{
	return this->hotSpotX;
}

OSInt Media::Image::GetHotSpotY()
{
	return this->hotSpotY;
}

Media::StaticImage *Media::Image::CreateStaticImage()
{
	Media::StaticImage *outImg;
	NEW_CLASS(outImg, Media::StaticImage(this->info));
	if (this->exif)
	{
		outImg->exif = this->exif->Clone();
	}
	this->GetImageData(outImg->data, 0, 0, this->info->dispWidth, this->info->dispHeight, this->GetDataBpl(), false);
	if (this->pal)
	{
		UOSInt size;
		if (this->info->pf == Media::PF_PAL_1_A1 || this->info->pf == Media::PF_PAL_2_A1 || this->info->pf == Media::PF_PAL_4_A1 || this->info->pf == Media::PF_PAL_8_A1)
		{
			size = (UOSInt)(4 << (this->info->storeBPP - 1));
		}
		else
		{
			size = (UOSInt)(4 << this->info->storeBPP);
		}
		MemCopyNO(outImg->pal, this->pal, size);
	}
	return outImg;
}

Media::EXIFData *Media::Image::SetEXIFData(Media::EXIFData *exif)
{
	Media::EXIFData *oldExif = this->exif;
	this->exif = exif;
	return oldExif;
}

void Media::Image::ToString(Text::StringBuilderUTF *sb)
{
	this->info->ToString(sb);
	if (this->HasHotSpot())
	{
		sb->Append((const UTF8Char*)"\r\nHot Spot: (");
		sb->AppendOSInt(this->GetHotSpotX());
		sb->Append((const UTF8Char*)", ");
		sb->AppendOSInt(this->GetHotSpotY());
		sb->Append((const UTF8Char*)")");
	}

	if (this->exif)
	{
		sb->Append((const UTF8Char*)"\r\n");
		this->exif->ToString(sb, 0);
	}
}

const UTF8Char *Media::Image::AlphaTypeGetName(AlphaType atype)
{
	switch (atype)
	{
	case AT_NO_ALPHA:
		return (const UTF8Char*)"No Alpha";
	case AT_ALPHA:
		return (const UTF8Char*)"Standard Alpha";
	case AT_PREMUL_ALPHA:
		return (const UTF8Char*)"Premul Alpha";
	default:
		return (const UTF8Char*)"Unknown";
	}
}
