#include "Stdafx.h"
#include "MyMemory.h"
#include "Exporter/GIFExporter.h"
#include "IO/StmData/FileData.h"
#include "Math/Math_C.h"
#include "Media/ImageResizer.h"
#include "Media/ImageCopy_C.h"
#include "Media/ImageList.h"
#include "Media/ImageUtil.h"
#include "Media/StaticEngine.h"
#include "Media/StaticImage.h"
#include "Media/ABlend/AlphaBlend8_C8.h"
#include "Text/MyStringW.h"

Media::StaticEngine::StaticEngine(Optional<Parser::ParserList> parsers)
{
	this->parsers = parsers;
	NEW_CLASSNN(this->iab32, Media::ABlend::AlphaBlend8_C8(0, false));
}

Media::StaticEngine::~StaticEngine()
{
	this->iab32.Delete();
}

Optional<Media::DrawImage> Media::StaticEngine::CreateImage32(Math::Size2D<UOSInt> size, Media::AlphaType atype)
{
//	Media::StaticDrawImage *simg;
//	Media::ColorProfile color(Media::ColorProfile::CPT_PUNKNOWN);
//	NEW_CLASS(simg, Media::StaticDrawImage(width, height, 0, 32, Media::PF_B8G8R8A8, width * height * 4, &color, Media::ColorProfile::YUVT_UNKNOWN, atype, Media::YCOFST_C_CENTER_LEFT));
//	return simg;
	return 0;
}

Optional<Media::DrawImage> Media::StaticEngine::LoadImage(Text::CStringNN fileName)
{
	Optional<Media::ImageList> imgList = 0;
	NN<Media::ImageList> nnimgList;
	{
		IO::StmData::FileData fd(fileName, false);
		NN<Parser::ParserList> parsers;
		if (this->parsers.SetTo(parsers))
		{
			imgList = Optional<Media::ImageList>::ConvertFrom(parsers->ParseFileType(fd, IO::ParserType::ImageList));
		}
	}

	Optional<Media::StaticDrawImage> simg = 0;
	NN<Media::RasterImage> img;
	if (imgList.SetTo(nnimgList))
	{
		if (nnimgList->GetImage(0, 0).SetTo(img))
		{
			simg = Optional<Media::StaticDrawImage>::ConvertFrom(this->ConvImage(img, 0));
		}
		imgList.Delete();
	}
	return simg;
}

Optional<Media::DrawImage> Media::StaticEngine::LoadImageW(UnsafeArray<const WChar> fileName)
{
	Optional<Media::ImageList> imgList = 0;
	NN<Media::ImageList> nnimgList;
	NN<Text::String> s = Text::String::NewNotNull(fileName);
	{
		IO::StmData::FileData fd(s, false);
		s->Release();
		NN<Parser::ParserList> parsers;
		if (this->parsers.SetTo(parsers))
		{
			imgList = Optional<Media::ImageList>::ConvertFrom(parsers->ParseFileType(fd, IO::ParserType::ImageList));
		}
	}

	Optional<Media::StaticDrawImage> simg = 0;
	if (imgList.SetTo(nnimgList))
	{
		NN<Media::RasterImage> img;
		if (nnimgList->GetImage(0, 0).SetTo(img))
		{
			simg = Optional<Media::StaticDrawImage>::ConvertFrom(this->ConvImage(img, 0));
		}
		nnimgList.Delete();
	}
	return simg;
}

Optional<Media::DrawImage> Media::StaticEngine::LoadImageStream(NN<IO::SeekableStream> stm)
{
	return 0;
}

Optional<Media::DrawImage> Media::StaticEngine::ConvImage(NN<Media::RasterImage> img, Optional<Media::ColorSess> colorSess)
{
//	Media::StaticImage *simg = img->CreateStaticImage();
//	simg->SetEngine(this);
//	return simg;
	return 0;
}

Optional<Media::DrawImage> Media::StaticEngine::CloneImage(NN<DrawImage> img)
{
	return this->ConvImage(NN<Media::StaticDrawImage>::ConvertFrom(img), 0);
}

Bool Media::StaticEngine::DeleteImage(NN<DrawImage> img)
{
	NN<Media::StaticDrawImage> simg = NN<Media::StaticDrawImage>::ConvertFrom(img);
	simg.Delete();
	return true;
}

void Media::StaticEngine::EndColorSess(NN<Media::ColorSess> colorSess)
{

}

Media::StaticBrush::StaticBrush(UInt32 color)
{
	this->color = color;
}

Media::StaticBrush::~StaticBrush()
{
}

Media::StaticPen::StaticPen(UInt32 color, Double thick, UnsafeArrayOpt<const UInt8> pattern, UOSInt nPattern)
{
	this->color = color;
	this->thick = thick;
	UnsafeArray<const UInt8> srcPattern;
	UnsafeArray<UInt8> destPattern;
	if (pattern.SetTo(srcPattern))
	{
		this->pattern = destPattern = MemAllocArr(UInt8, nPattern);
		destPattern.CopyFromNO(srcPattern, nPattern);
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
	UnsafeArray<UInt8> nnpattern;
	if (this->pattern.SetTo(nnpattern))
	{
		MemFreeArr(nnpattern);
	}
}

Double Media::StaticPen::GetThick()
{
	return this->thick;
}

Media::StaticDrawImage::StaticDrawImage(NN<StaticEngine> eng, Math::Size2D<UOSInt> dispSize, Int32 fourcc, Int32 bpp, Media::PixelFormat pf, OSInt maxSize, NN<const Media::ColorProfile> color, Media::ColorProfile::YUVType yuvType, Media::AlphaType atype, Media::YCOffset ycOfst) : Media::StaticImage(dispSize, fourcc, bpp, pf, maxSize, color, yuvType, atype, ycOfst)
{
	this->eng = eng;
}


Media::StaticDrawImage::~StaticDrawImage()
{
}

UOSInt Media::StaticDrawImage::GetWidth() const
{
	return this->info.dispSize.x;
}

UOSInt Media::StaticDrawImage::GetHeight() const
{
	return this->info.dispSize.y;
}

UInt32 Media::StaticDrawImage::GetBitCount() const
{
	return this->info.storeBPP;
}

NN<const Media::ColorProfile> Media::StaticDrawImage::GetColorProfile() const
{
	return this->info.color;
}

Media::AlphaType Media::StaticDrawImage::GetAlphaType() const
{
	return this->info.atype;
}

Double Media::StaticDrawImage::GetHDPI() const
{
	return this->info.hdpi;
}

Double Media::StaticDrawImage::GetVDPI() const
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

UnsafeArrayOpt<UInt8> Media::StaticDrawImage::GetImgBits(OutParam<Bool> revOrder)
{
	revOrder.Set(false);
	return this->data;
}

Bool Media::StaticDrawImage::DrawImagePt(NN<DrawImage> img, Math::Coord2DDbl tl)
{
	return this->DrawImagePt2(NN<Media::StaticImage>::ConvertFrom(img), tl);
}

Bool Media::StaticDrawImage::DrawImagePt2(NN<Media::StaticImage> img, Math::Coord2DDbl tl)
{
	if (this->info.fourcc != 0)
	{
		return false;
	}
	if (this->info.pf == Media::PF_B8G8R8A8)
	{
		img->ToB8G8R8A8();
		if (img->info.atype == Media::AT_IGNORE_ALPHA || img->info.atype == Media::AT_ALPHA_ALL_FF)
		{
			Int32 x = Double2Int32(tl.x);
			Int32 y = Double2Int32(tl.y);
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
				ImageCopy_ImgCopy(img->data.Ptr() + (sy * img->info.storeSize.x << 2) + (sx << 2), this->data.Ptr() + y * bpl + (x << 2), w << 2, h, img->info.storeSize.x << 2, this->info.storeSize.x << 2);
			}
		}
		else
		{
			OSInt w = img->info.dispSize.x;
			OSInt h = img->info.dispSize.y;
			UnsafeArray<UInt8> dbits = this->data;
			UnsafeArray<UInt8> sbits = img->data;
			OSInt dbpl = this->info.storeSize.x << 2;
			OSInt sbpl = img->info.storeSize.x << 2;

			if (tl.x < 0)
			{
				w += Double2Int32(tl.x);
				sbits -= Double2Int32(tl.x) << 2;
				tl.x = 0;
			}
			if (tl.y < 0)
			{
				h += Double2Int32(tl.y);
				sbits -= Double2Int32(tl.y) * sbpl;
				tl.y = 0;
			}

			if (tl.x + w > this->info.dispSize.x)
			{
				w = this->info.dispSize.x - Double2Int32(tl.x);
			}
			if (tl.y + h > this->info.dispSize.y)
			{
				h = this->info.dispSize.y - Double2Int32(tl.y);
			}
			if (w > 0 && h > 0)
			{
				this->eng->iab32->SetSourceProfile(img->info.color);
				this->eng->iab32->SetDestProfile(this->info.color);
				this->eng->iab32->SetOutputProfile(this->info.color);
				this->eng->iab32->Blend(dbits + Double2Int32(tl.y) * dbpl + (Double2Int32(tl.x) * 4), dbpl, sbits, sbpl, w, h, img->info.atype);
			}
		}
		return true;
	}
	return false;
}

Bool Media::StaticDrawImage::DrawImagePt3(NN<DrawImage> img, Math::Coord2DDbl destTL, Math::Coord2DDbl srcTL, Math::Size2DDbl srcSize)
{
	Media::StaticDrawImage *simg = (Media::StaticDrawImage *)img.Ptr();
	if (this->info.fourcc != 0)
	{
		return false;
	}
	if (this->info.pf == Media::PF_B8G8R8A8)
	{
		simg->ToB8G8R8A8();
		if (simg->info.atype == Media::AT_IGNORE_ALPHA || simg->info.atype == Media::AT_ALPHA_ALL_FF)
		{
			Int32 x = Double2Int32(destTL.x);
			Int32 y = Double2Int32(destTL.y);
			Int32 sx = Double2Int32(srcTL.x);
			Int32 sy = Double2Int32(srcTL.y);
			OSInt w = Double2Int32(srcSize.x);
			OSInt h = Double2Int32(srcSize.y);
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
				ImageCopy_ImgCopy(simg->data.Ptr() + (sy * simg->info.storeSize.x << 2) + (sx << 2), this->data.Ptr() + y * bpl + (x << 2), w << 2, h, simg->info.storeSize.x << 2, bpl);
			}
		}
		else
		{
			Int32 x = Double2Int32(destTL.x);
			Int32 y = Double2Int32(destTL.y);
			Int32 sx = Double2Int32(srcTL.x);
			Int32 sy = Double2Int32(srcTL.y);
			OSInt w = Double2Int32(srcSize.x);
			OSInt h = Double2Int32(srcSize.y);
			UnsafeArray<UInt8> dbits = this->data;
			UnsafeArray<UInt8> sbits = simg->data;
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

NN<Media::DrawPen> Media::StaticDrawImage::NewPenARGB(UInt32 color, Double thick, UnsafeArrayOpt<UInt8> pattern, UOSInt nPattern)
{
	NN<Media::StaticPen> p;
	NEW_CLASSNN(p, Media::StaticPen(color, thick, pattern, nPattern));
	return p;
}

NN<Media::DrawBrush> Media::StaticDrawImage::NewBrushARGB(UInt32 color)
{
	NN<Media::StaticBrush> b;
	NEW_CLASSNN(b, Media::StaticBrush(color));
	return b;
}

void Media::StaticDrawImage::DelPen(NN<DrawPen> p)
{
	NN<Media::StaticPen> pen = NN<Media::StaticPen>::ConvertFrom(p);
	pen.Delete();
}

void Media::StaticDrawImage::DelBrush(NN<DrawBrush> b)
{
	NN<Media::StaticBrush> brush = NN<Media::StaticBrush>::ConvertFrom(b);
	brush.Delete();
}

Optional<Media::StaticImage> Media::StaticDrawImage::ToStaticImage() const
{
	return NN<Media::StaticImage>::ConvertFrom(this->Clone());
}

UOSInt Media::StaticDrawImage::SaveGIF(NN<IO::SeekableStream> stm)
{
	NN<Media::StaticImage> simg = NN<Media::StaticImage>::ConvertFrom(this->Clone());
	if (!simg->ToPal8())
	{
		simg.Delete();
		return -1;
	}
	Exporter::GIFExporter exporter;
	NN<Media::ImageList> imgList;
	NEW_CLASSNN(imgList, Media::ImageList(CSTR("GIFTemp")));
	imgList->AddImage(simg, 0);
	Bool succ = exporter.ExportFile(stm, CSTR("Temp"), imgList, 0);
	imgList.Delete();
	return succ?0:-1;
}
