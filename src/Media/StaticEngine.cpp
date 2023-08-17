#include "Stdafx.h"
#include "MyMemory.h"
#include "Exporter/GIFExporter.h"
#include "IO/StmData/FileData.h"
#include "Math/Math.h"
#include "Media/IImgResizer.h"
#include "Media/ImageCopyC.h"
#include "Media/ImageList.h"
#include "Media/ImageUtil.h"
#include "Media/StaticEngine.h"
#include "Media/StaticImage.h"
#include "Media/ABlend/AlphaBlend8_C8.h"
#include "Text/MyStringW.h"

Media::StaticEngine::StaticEngine(Parser::ParserList *parsers)
{
	this->parsers = parsers;
	NEW_CLASS(this->iab32, Media::ABlend::AlphaBlend8_C8(0, false));
}

Media::StaticEngine::~StaticEngine()
{
	DEL_CLASS(this->iab32);
}

Media::DrawImage *Media::StaticEngine::CreateImage32(Math::Size2D<UOSInt> size, Media::AlphaType atype)
{
//	Media::StaticDrawImage *simg;
//	Media::ColorProfile color(Media::ColorProfile::CPT_PUNKNOWN);
//	NEW_CLASS(simg, Media::StaticDrawImage(width, height, 0, 32, Media::PF_B8G8R8A8, width * height * 4, &color, Media::ColorProfile::YUVT_UNKNOWN, atype, Media::YCOFST_C_CENTER_LEFT));
//	return simg;
	return 0;
}

Media::DrawImage *Media::StaticEngine::LoadImage(Text::CStringNN fileName)
{
	Media::ImageList *imgList = 0;
	{
		IO::StmData::FileData fd(fileName, false);
		if (this->parsers)
		{
			imgList = (Media::ImageList*)this->parsers->ParseFileType(fd, IO::ParserType::ImageList);
		}
	}

	Media::StaticDrawImage *simg = 0;
	if (imgList)
	{
		simg = (Media::StaticDrawImage*)this->ConvImage(imgList->GetImage(0, 0));
		DEL_CLASS(imgList);
	}
	return simg;
}

Media::DrawImage *Media::StaticEngine::LoadImageW(const WChar *fileName)
{
	Media::ImageList *imgList = 0;
	NotNullPtr<Text::String> s = Text::String::NewNotNull(fileName);
	{
		IO::StmData::FileData fd(s, false);
		s->Release();
		if (this->parsers)
		{
			imgList = (Media::ImageList*)this->parsers->ParseFileType(fd, IO::ParserType::ImageList);
		}
	}

	Media::StaticDrawImage *simg = 0;
	if (imgList)
	{
		simg = (Media::StaticDrawImage*)this->ConvImage(imgList->GetImage(0, 0));
		DEL_CLASS(imgList);
	}
	return simg;
}

Media::DrawImage *Media::StaticEngine::LoadImageStream(NotNullPtr<IO::SeekableStream> stm)
{
	return 0;
}

Media::DrawImage *Media::StaticEngine::ConvImage(Media::Image *img)
{
//	Media::StaticImage *simg = img->CreateStaticImage();
//	simg->SetEngine(this);
//	return simg;
	return 0;
}

Media::DrawImage *Media::StaticEngine::CloneImage(DrawImage *img)
{
	return this->ConvImage((Media::StaticDrawImage*)img);
}

Bool Media::StaticEngine::DeleteImage(DrawImage *img)
{
	Media::StaticDrawImage *simg = (Media::StaticDrawImage*)img;
	DEL_CLASS(simg);
	return true;
}

Media::StaticBrush::StaticBrush(Int32 color)
{
	this->color = color;
}

Media::StaticBrush::~StaticBrush()
{
}

Media::StaticPen::StaticPen(Int32 color, Double thick, const UInt8 *pattern, OSInt nPattern)
{
	this->color = color;
	this->thick = thick;
	if (pattern)
	{
		this->pattern = MemAlloc(UInt8, nPattern);
		MemCopyNO(this->pattern, pattern, nPattern);
		this->nPattern = nPattern;
	}
	else
	{
		this->pattern = 0;
		this->nPattern = 0;
	}
}

Media::StaticPen::~StaticPen()
{
	if (this->pattern)
	{
		MemFree(this->pattern);
	}
}

Double Media::StaticPen::GetThick()
{
	return this->thick;
}

Media::StaticDrawImage::StaticDrawImage(StaticEngine *eng, Math::Size2D<UOSInt> dispSize, Int32 fourcc, Int32 bpp, Media::PixelFormat pf, OSInt maxSize, const Media::ColorProfile *color, Media::ColorProfile::YUVType yuvType, Media::AlphaType atype, Media::YCOffset ycOfst) : Media::StaticImage(dispSize, fourcc, bpp, pf, maxSize, color, yuvType, atype, ycOfst)
{
	this->eng = eng;
}


Media::StaticDrawImage::~StaticDrawImage()
{
}

UOSInt Media::StaticDrawImage::GetWidth()
{
	return this->info.dispSize.x;
}

UOSInt Media::StaticDrawImage::GetHeight()
{
	return this->info.dispSize.y;
}

UInt32 Media::StaticDrawImage::GetBitCount()
{
	return this->info.storeBPP;
}

Media::ColorProfile *Media::StaticDrawImage::GetColorProfile()
{
	return this->info.color;
}

Media::AlphaType Media::StaticDrawImage::GetAlphaType()
{
	return this->info.atype;
}

Double Media::StaticDrawImage::GetHDPI()
{
	return this->info.hdpi;
}

Double Media::StaticDrawImage::GetVDPI()
{
	return this->info.vdpi;
}

void Media::StaticDrawImage::SetHDPI(Double dpi)
{
	if (dpi > 0)
	{
		this->info.hdpi = dpi;
	}
}

void Media::StaticDrawImage::SetVDPI(Double dpi)
{
	if (dpi > 0)
	{
		this->info.vdpi = dpi;
	}
}

UInt8 *Media::StaticDrawImage::GetImgBits(Bool *revOrder)
{
	if (revOrder)
	{
		*revOrder = false;
	}
	return this->data;
}

Bool Media::StaticDrawImage::DrawImagePt(DrawImage *img, Double tlx, Double tly)
{
	return this->DrawImagePt2((Media::StaticImage*)img, tlx, tly);
}

Bool Media::StaticDrawImage::DrawImagePt2(Media::StaticImage *img, Double tlx, Double tly)
{
	if (this->info.fourcc != 0)
	{
		return false;
	}
	if (this->info.pf == Media::PF_B8G8R8A8)
	{
		img->To32bpp();
		if (img->info.atype == Media::AT_NO_ALPHA)
		{
			Int32 x = Double2Int32(tlx);
			Int32 y = Double2Int32(tly);
			Int32 sx = 0;
			Int32 sy = 0;
			OSInt w = img->info.dispSize.x;
			OSInt h = img->info.dispSize.y;
			OSInt bpl = this->info.storeSize.x << 2;
			if (x < 0)
			{
				w += x;
				sx = -x;
				x = 0;
			}
			if (y < 0)
			{
				h += y;
				sy = -y;
				y = 0;
			}
			if (x + w > (OSInt)this->info.dispSize.x)
			{
				w = this->info.dispSize.x - x;
			}
			if (y + h > (OSInt)this->info.dispSize.y)
			{
				h = this->info.dispSize.y - y;
			}
			if (w > 0 && h > 0)
			{
				ImageCopy_ImgCopy(img->data + (sy * img->info.storeSize.x << 2) + (sx << 2), this->data + y * bpl + (x << 2), w << 2, h, img->info.storeSize.x << 2, this->info.storeSize.x << 2);
			}
		}
		else
		{
			OSInt w = img->info.dispSize.x;
			OSInt h = img->info.dispSize.y;
			UInt8 *dbits = this->data;
			UInt8 *sbits = img->data;
			OSInt dbpl = this->info.storeSize.x << 2;
			OSInt sbpl = img->info.storeSize.x << 2;

			if (tlx < 0)
			{
				w += Double2Int32(tlx);
				sbits -= Double2Int32(tlx) << 2;
				tlx = 0;
			}
			if (tly < 0)
			{
				h += Double2Int32(tly);
				sbits -= Double2Int32(tly) * sbpl;
				tly = 0;
			}

			if (tlx + w > this->info.dispSize.x)
			{
				w = this->info.dispSize.x - Double2Int32(tlx);
			}
			if (tly + h > this->info.dispSize.y)
			{
				h = this->info.dispSize.y - Double2Int32(tly);
			}
			if (w > 0 && h > 0)
			{
				this->eng->iab32->SetSourceProfile(img->info.color);
				this->eng->iab32->SetDestProfile(this->info.color);
				this->eng->iab32->SetOutputProfile(this->info.color);
				this->eng->iab32->Blend(dbits + Double2Int32(tly) * dbpl + (Double2Int32(tlx) * 4), dbpl, sbits, sbpl, w, h, img->info.atype);
			}
		}
		return true;
	}
	return false;
}

Bool Media::StaticDrawImage::DrawImagePt3(DrawImage *img, Double destX, Double destY, Double srcX, Double srcY, Double srcW, Double srcH)
{
	Media::StaticDrawImage *simg = (Media::StaticDrawImage *)img;
	if (this->info.fourcc != 0)
	{
		return false;
	}
	if (this->info.pf == Media::PF_B8G8R8A8)
	{
		simg->To32bpp();
		if (simg->info.atype == Media::AT_NO_ALPHA)
		{
			Int32 x = Double2Int32(destX);
			Int32 y = Double2Int32(destY);
			Int32 sx = Double2Int32(srcX);
			Int32 sy = Double2Int32(srcY);
			OSInt w = Double2Int32(srcW);
			OSInt h = Double2Int32(srcH);
			OSInt bpl = this->info.storeSize.x << 2;
			if (x < 0)
			{
				w += x;
				sx -= x;
				x = 0;
			}
			if (y < 0)
			{
				h += y;
				sy -= y;
				y = 0;
			}
			if (x + w > (OSInt)this->info.dispSize.x)
			{
				w = this->info.dispSize.x - x;
			}
			if (y + h > (OSInt)this->info.dispSize.y)
			{
				h = this->info.dispSize.y - y;
			}
			if (w > 0 && h > 0)
			{
				ImageCopy_ImgCopy(simg->data + (sy * simg->info.storeSize.x << 2) + (sx << 2), this->data + y * bpl + (x << 2), w << 2, h, simg->info.storeSize.x << 2, bpl);
			}
		}
		else
		{
			Int32 x = Double2Int32(destX);
			Int32 y = Double2Int32(destY);
			Int32 sx = Double2Int32(srcX);
			Int32 sy = Double2Int32(srcY);
			OSInt w = Double2Int32(srcW);
			OSInt h = Double2Int32(srcH);
			UInt8 *dbits = (UInt8*)this->data;
			UInt8 *sbits = (UInt8*)simg->data;
			OSInt dbpl = this->info.storeSize.x << 2;
			OSInt sbpl = simg->info.storeSize.x << 2;

			if (x < 0)
			{
				w += x;
				sx -= x;
				x = 0;
			}
			if (y < 0)
			{
				h += y;
				sy -= y;
				y = 0;
			}
			if (x + w > (OSInt)this->info.dispSize.x)
			{
				w = this->info.dispSize.x - x;
			}
			if (y + h > (OSInt)this->info.dispSize.y)
			{
				h = this->info.dispSize.y - y;
			}
			if (w > 0 && h > 0)
			{
				this->eng->iab32->SetSourceProfile(simg->info.color);
				this->eng->iab32->SetDestProfile(this->info.color);
				this->eng->iab32->SetOutputProfile(this->info.color);
				this->eng->iab32->Blend(dbits + y * dbpl + (x * 4), dbpl, sbits + sy * sbpl + (sx << 2), sbpl, w, h, simg->info.atype);
			}
		}
		return true;
	}
	return false;
}

Media::DrawPen *Media::StaticDrawImage::NewPenARGB(Int32 color, Double thick, UInt8 *pattern, OSInt nPattern)
{
	Media::StaticPen *p;
	NEW_CLASS(p, Media::StaticPen(color, thick, pattern, nPattern));
	return p;
}

Media::DrawBrush *Media::StaticDrawImage::NewBrushARGB(Int32 color)
{
	Media::StaticBrush *b;
	NEW_CLASS(b, Media::StaticBrush(color));
	return b;
}

void Media::StaticDrawImage::DelPen(DrawPen *p)
{
	Media::StaticPen *pen = (Media::StaticPen*)p;
	DEL_CLASS(pen);
}

void Media::StaticDrawImage::DelBrush(DrawBrush *b)
{
	Media::StaticBrush *brush = (Media::StaticBrush*)b;
	DEL_CLASS(brush);
}

Media::StaticImage *Media::StaticDrawImage::ToStaticImage()
{
	return (Media::StaticImage*)this->Clone();
}

UOSInt Media::StaticDrawImage::SaveGIF(NotNullPtr<IO::SeekableStream> stm)
{
	Media::StaticImage *simg = (Media::StaticImage*)this->Clone();
	if (!simg->ToPal8())
	{
		DEL_CLASS(simg);
		return -1;
	}
	Exporter::GIFExporter exporter;
	Media::ImageList *imgList;
	NEW_CLASS(imgList, Media::ImageList(CSTR("GIFTemp")));
	imgList->AddImage(simg, 0);
	Bool succ = exporter.ExportFile(stm, CSTR("Temp"), imgList, 0);
	DEL_CLASS(imgList);
	return succ?0:-1;
}
