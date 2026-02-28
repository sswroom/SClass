#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/ImageResizer.h"
#include "Media/FrameInfo.h"
#include "Media/RasterImage.h"
#include "Media/StaticImage.h"

Media::RasterImage::RasterImage(Math::Size2D<UIntOS> dispSize)
{
	this->exif = nullptr;
	this->hasHotSpot = false;
	this->hotSpotX = 0;
	this->hotSpotY = 0;
	this->info.dispSize = dispSize;
	this->info.storeSize = dispSize;
	this->info.fourcc = 0;
	this->info.storeBPP = 32;
	this->info.pf = Media::PF_B8G8R8A8;
	this->info.byteSize = dispSize.CalcArea() * 4;
	this->info.hdpi = 96;
	this->info.vdpi = 96;
	this->info.color.SetCommonProfile(Media::ColorProfile::CPT_PUNKNOWN);
	this->info.atype = Media::AT_ALPHA;
	this->info.yuvType = Media::ColorProfile::YUVT_UNKNOWN;
	this->info.rotateType = Media::RotateType::None;
	this->info.ycOfst = Media::YCOFST_C_CENTER_LEFT;
	this->info.ftype = Media::FT_NON_INTERLACE;
	this->pal = nullptr;
}

Media::RasterImage::RasterImage(Math::Size2D<UIntOS> dispSize, Math::Size2D<UIntOS> storeSize, UInt32 fourcc, UInt32 bpp, Media::PixelFormat pf, UIntOS maxSize, NN<const Media::ColorProfile> color, Media::ColorProfile::YUVType yuvType, Media::AlphaType atype, Media::YCOffset ycOfst)
{
	this->exif = nullptr;
	this->hasHotSpot = false;
	this->hotSpotX = 0;
	this->hotSpotY = 0;
	this->info.dispSize = dispSize;
	this->info.storeSize = storeSize;
	this->info.fourcc = fourcc;
	this->info.storeBPP = bpp;
	this->info.pf = pf;
	this->info.byteSize = maxSize;
	this->info.hdpi = 96;
	this->info.vdpi = 96;
	this->info.color.Set(color);
	this->info.atype = atype;
	this->info.yuvType = yuvType;
	this->info.ycOfst = ycOfst;
	this->info.ftype = Media::FT_NON_INTERLACE;
	this->info.rotateType = Media::RotateType::None;

	if (pf == Media::PF_PAL_1_A1)
	{
		this->info.storeSize = dispSize;
		this->info.byteSize = ((dispSize.x + 7) >> 3) * dispSize.y * 2;
	}
	else if (pf == Media::PF_PAL_2_A1)
	{
		this->info.storeSize = dispSize;
		this->info.byteSize = (((dispSize.x + 7) >> 3) + ((dispSize.x + 3) >> 2)) * dispSize.y;
	}
	else if (pf == Media::PF_PAL_4_A1)
	{
		this->info.storeSize = dispSize;
		this->info.byteSize = (((dispSize.x + 7) >> 3) + ((dispSize.x + 1) >> 1)) * dispSize.y;
	}
	else if (pf == Media::PF_PAL_8_A1)
	{
		this->info.storeSize = dispSize;
		this->info.byteSize = (((dispSize.x + 7) >> 3) + dispSize.x) * dispSize.y;
	}
	else
	{
		if (storeSize.x == 0)
		{
			storeSize = dispSize;
			this->info.storeSize = storeSize;
			if (pf == Media::PF_PAL_1 || pf == Media::PF_PAL_W1)
			{
				if (dispSize.x & 7)
				{
					storeSize.x = dispSize.x + 8 - (dispSize.x & 7);
					this->info.storeSize.x = storeSize.x;
				}
			}
			else if (pf == Media::PF_PAL_2 || pf == Media::PF_PAL_W2 || pf == Media::PF_PAL_2_A1)
			{
				if (dispSize.x & 3)
				{
					storeSize.x = dispSize.x + 4 - (dispSize.x & 3);
					this->info.storeSize.x = storeSize.x;
				}
			}
			else if (pf == Media::PF_PAL_4 || pf == Media::PF_PAL_W4 || pf == Media::PF_PAL_4_A1)
			{
				if (dispSize.x & 1)
				{
					storeSize.x = dispSize.x + 2 - (dispSize.x & 1);
					this->info.storeSize.x = storeSize.x;
				}
			}
		}
		if (bpp)
		{
			this->info.byteSize = storeSize.x * bpp;
			if (this->info.byteSize & 7)
				this->info.byteSize += 8 - (this->info.byteSize & 7);
			this->info.byteSize = storeSize.y * (this->info.byteSize >> 3);
		}
	}
	this->pal = nullptr;
	if (this->info.fourcc == 0)
	{
		if (pf == Media::PF_PAL_1_A1)
		{
			this->pal = MemAllocArr(UInt8, 4 << 1);
		}
		else if (pf == Media::PF_PAL_2_A1)
		{
			this->pal = MemAllocArr(UInt8, 4 << 2);
		}
		else if (pf == Media::PF_PAL_4_A1)
		{
			this->pal = MemAllocArr(UInt8, 4 << 4);
		}
		else if (pf == Media::PF_PAL_8_A1)
		{
			this->pal = MemAllocArr(UInt8, 4 << 8);
		}
		else if (bpp <= 8)
		{
			UIntOS palSize = ((UIntOS)4 << bpp);
			this->pal = MemAllocArr(UInt8, palSize);
		}
	}
}

Media::RasterImage::~RasterImage()
{
	UnsafeArray<UInt8> pal;
	if (this->pal.SetTo(pal))
	{
		MemFreeArr(pal);
	}
	this->exif.Delete();
}

void Media::RasterImage::InitGrayPal()
{
	UnsafeArray<UInt8> pal;
	if (this->pal.SetTo(pal))
	{
		if (this->info.storeBPP == 1)
		{
			WriteUInt32(&pal[0], 0xff000000);
			WriteUInt32(&pal[4], 0xffffffff);
		}
		else if (this->info.storeBPP == 2)
		{
			WriteUInt32(&pal[0], 0xff000000);
			WriteUInt32(&pal[4], 0xff555555);
			WriteUInt32(&pal[8], 0xffaaaaaa);
			WriteUInt32(&pal[12], 0xffffffff);
		}
		else if (this->info.storeBPP == 4)
		{
			WriteUInt32(&pal[0], 0xff000000);
			WriteUInt32(&pal[4], 0xff111111);
			WriteUInt32(&pal[8], 0xff222222);
			WriteUInt32(&pal[12], 0xff333333);
			WriteUInt32(&pal[16], 0xff444444);
			WriteUInt32(&pal[20], 0xff555555);
			WriteUInt32(&pal[24], 0xff666666);
			WriteUInt32(&pal[28], 0xff777777);
			WriteUInt32(&pal[32], 0xff888888);
			WriteUInt32(&pal[36], 0xff999999);
			WriteUInt32(&pal[40], 0xffaaaaaa);
			WriteUInt32(&pal[44], 0xffbbbbbb);
			WriteUInt32(&pal[48], 0xffcccccc);
			WriteUInt32(&pal[52], 0xffdddddd);
			WriteUInt32(&pal[56], 0xffeeeeee);
			WriteUInt32(&pal[60], 0xffffffff);
		}
		else if (this->info.storeBPP == 8)
		{
			UnsafeArray<UInt8> ptr = pal;
			UIntOS i = 0;
			UIntOS j = 256;
			while (i < j)
			{
				UInt32 c = (UInt32)(i | (i << 8) | (i << 16) | 0xff000000);
				WriteUInt32(&ptr[0], c);
				ptr += 4;
				i++;
			}
		}
	}
}

UIntOS Media::RasterImage::GetDataBpl() const
{
	if (this->info.fourcc == *(UInt32*)"LRGB")
	{
		return (this->info.storeSize.x << 3);
	}
	if (this->info.fourcc != 0 && this->info.fourcc != *(UInt32*)"DIBS")
		return 0;
	if (this->info.pf == Media::PF_PAL_1_A1)
	{
		return ((this->info.dispSize.x + 7) >> 3) * 2;
	}
	else if (this->info.pf == Media::PF_PAL_2_A1)
	{
		return (((this->info.dispSize.x + 7) >> 3) + ((this->info.dispSize.x + 3) >> 2));
	}
	else if (this->info.pf == Media::PF_PAL_4_A1)
	{
		return (((this->info.dispSize.x + 7) >> 3) + ((this->info.dispSize.x + 1) >> 1));
	}
	else if (this->info.pf == Media::PF_PAL_8_A1)
	{
		return (((this->info.dispSize.x + 7) >> 3) + this->info.dispSize.x);
	}
	else if (this->info.storeBPP <= 0)
		return (this->info.storeSize.x * (this->info.storeBPP >> 3));
	else if (this->info.storeBPP == 4)
		return ((this->info.storeSize.x >> 1) + (this->info.storeSize.x & 1));
	else
		return ((this->info.storeSize.x * this->info.storeBPP) >> 3);
}

Bool Media::RasterImage::IsUpsideDown() const
{
	return false;
}

void Media::RasterImage::SetHotSpot(IntOS hotSpotX, IntOS hotSpotY)
{
	this->hotSpotX = hotSpotX;
	this->hotSpotY = hotSpotY;
	this->hasHotSpot = true;
}

Bool Media::RasterImage::HasHotSpot() const
{
	return this->hasHotSpot;
}

IntOS Media::RasterImage::GetHotSpotX() const
{
	return this->hotSpotX;
}

IntOS Media::RasterImage::GetHotSpotY() const
{
	return this->hotSpotY;
}

NN<Media::StaticImage> Media::RasterImage::CreateStaticImage() const
{
	NN<Media::StaticImage> outImg;
	NN<Media::EXIFData> exif;
	NEW_CLASSNN(outImg, Media::StaticImage(this->info));
	if (this->exif.SetTo(exif))
	{
		outImg->exif = exif->Clone();
	}
	this->GetRasterData(outImg->data, 0, 0, this->info.dispSize.x, this->info.dispSize.y, this->GetDataBpl(), false, outImg->info.rotateType);
	UnsafeArray<UInt8> pal;
	UnsafeArray<UInt8> outPal;
	if (this->pal.SetTo(pal) && outImg->pal.SetTo(outPal))
	{
		UIntOS size;
		if (this->info.pf == Media::PF_PAL_1_A1 || this->info.pf == Media::PF_PAL_2_A1 || this->info.pf == Media::PF_PAL_4_A1 || this->info.pf == Media::PF_PAL_8_A1)
		{
			size = ((UIntOS)4 << (this->info.storeBPP - 1));
		}
		else
		{
			size = ((UIntOS)4 << this->info.storeBPP);
		}
		MemCopyNO(&outPal[0], &pal[0], size);
	}
	return outImg;
}

NN<Media::StaticImage> Media::RasterImage::CreateSubImage(Math::RectArea<IntOS> area) const
{
	Media::FrameInfo frameInfo;
	frameInfo.Set(this->info);
	frameInfo.dispSize = Math::Size2D<UIntOS>((UIntOS)area.GetWidth(), (UIntOS)area.GetHeight());
	frameInfo.storeSize = frameInfo.dispSize;
	frameInfo.byteSize = frameInfo.storeSize.CalcArea() * (frameInfo.storeBPP >> 3);
	NN<Media::StaticImage> outImg;
	NN<Media::EXIFData> exif;
	NEW_CLASSNN(outImg, Media::StaticImage(frameInfo));
	if (this->exif.SetTo(exif))
	{
		outImg->exif = exif->Clone();
	}
	this->GetRasterData(outImg->data, area.min.x, area.min.y, frameInfo.dispSize.x, frameInfo.dispSize.y, outImg->GetDataBpl(), false, outImg->info.rotateType);
	UnsafeArray<UInt8> pal;
	UnsafeArray<UInt8> outPal;
	if (this->pal.SetTo(pal) && outImg->pal.SetTo(outPal))
	{
		UIntOS size;
		if (this->info.pf == Media::PF_PAL_1_A1 || this->info.pf == Media::PF_PAL_2_A1 || this->info.pf == Media::PF_PAL_4_A1 || this->info.pf == Media::PF_PAL_8_A1)
		{
			size = ((UIntOS)4 << (this->info.storeBPP - 1));
		}
		else
		{
			size = ((UIntOS)4 << this->info.storeBPP);
		}
		MemCopyNO(&outPal[0], &pal[0], size);
	}
	return outImg;
}

Optional<Media::EXIFData> Media::RasterImage::SetEXIFData(Optional<Media::EXIFData> exif)
{
	Optional<Media::EXIFData> oldExif = this->exif;
	this->exif = exif;
	return oldExif;
}

void Media::RasterImage::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	this->info.ToString(sb);
	if (this->HasHotSpot())
	{
		sb->AppendC(UTF8STRC("\r\nHot Spot: ("));
		sb->AppendIntOS(this->GetHotSpotX());
		sb->AppendC(UTF8STRC(", "));
		sb->AppendIntOS(this->GetHotSpotY());
		sb->AppendC(UTF8STRC(")"));
	}

	NN<Media::EXIFData> exif;
	if (this->exif.SetTo(exif))
	{
		sb->AppendC(UTF8STRC("\r\n"));
		exif->ToString(sb, nullptr);
	}
}

Bool Media::RasterImage::IsDispSize(Math::Size2D<UIntOS> dispSize)
{
	return this->info.dispSize == dispSize;
}
