#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/StmData/FileData.h"
#include "Math/Math.h"
#include "Media/GTKDrawEngine.h"
#include "Media/ImageCopyC.h"
#include "Media/ImageList.h"
#include "Media/ImageUtil.h"
#include "Media/JPEGFile.h"
#include "Media/StaticImage.h"
#include "Parser/FileParser/GUIImgParser.h"
#include "Text/MyStringW.h"

#if defined(NO_GDK)
#include <cairo/cairo.h>
#else
#include <gdk/gdk.h>
#endif
#include <stdio.h>

Media::GTKDrawEngine::GTKDrawEngine() : iab(0, true)
{
}

Media::GTKDrawEngine::~GTKDrawEngine()
{
}

Optional<Media::DrawImage> Media::GTKDrawEngine::CreateImage32(Math::Size2D<UOSInt> size, Media::AlphaType atype)
{
	NN<Media::GTKDrawImage> dimg;
	cairo_surface_t *surface;
	cairo_t *cr;
	surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, (int)size.x, (int)size.y);
	cr = cairo_create(surface);
	if (atype == Media::AT_ALPHA)
	{
		atype = Media::AT_PREMUL_ALPHA;
	}
	NEW_CLASSNN(dimg, Media::GTKDrawImage(*this, surface, cr, Math::Coord2D<OSInt>(0, 0), size, 32, atype));
	return dimg;
}

NN<Media::DrawImage> Media::GTKDrawEngine::CreateImageScn(void *cr, Math::Coord2D<OSInt> tl, Math::Coord2D<OSInt> br)
{
	NN<Media::GTKDrawImage> dimg;
	NEW_CLASSNN(dimg, Media::GTKDrawImage(*this, 0, cr, tl, Math::Size2D<UOSInt>((UOSInt)(br.x - tl.x), (UOSInt)(br.y - tl.y)), 32, Media::AT_NO_ALPHA));
	return dimg;
}

Optional<Media::DrawImage> Media::GTKDrawEngine::LoadImage(Text::CStringNN fileName)
{
	Optional<Media::ImageList> imgList = 0;
	NN<Media::ImageList> nnimgList;
	{
		IO::StmData::FileData fd(fileName, false);
		if (fd.IsError())
		{
			return 0;
		}
		if (imgList.IsNull())
		{
			Parser::FileParser::GUIImgParser parser;
			imgList = Optional<Media::ImageList>::ConvertFrom(parser.ParseFile(fd, 0, IO::ParserType::ImageList));
		}
	}

	if (!imgList.SetTo(nnimgList))
	{
//		printf("GTKDrawEngine: Error in loading image %s\r\n", fileName.v);
		return 0;
	}

	Optional<Media::DrawImage> dimg = 0;
	NN<Media::RasterImage> img;
	if (nnimgList->GetImage(0, 0).SetTo(img))
	{
		dimg = this->ConvImage(img);
	}
	nnimgList.Delete();
	return dimg;
}

Optional<Media::DrawImage> Media::GTKDrawEngine::LoadImageStream(NN<IO::SeekableStream> stm)
{
	return 0;
}

Optional<Media::DrawImage> Media::GTKDrawEngine::ConvImage(NN<Media::RasterImage> img)
{
	if (img->info.fourcc != 0)
	{
		return 0; 
	}
	NN<Media::GTKDrawImage> gimg;;
	if (!Optional<Media::GTKDrawImage>::ConvertFrom(this->CreateImage32(img->info.dispSize, img->info.atype)).SetTo(gimg))
		return 0;
	gimg->SetHDPI(img->info.hdpi);
	gimg->SetVDPI(img->info.vdpi);
	gimg->info.color.Set(img->info.color);
	if (img->GetImageType() == Media::RasterImage::ImageType::Static)
	{
		NN<Media::StaticImage> simg = NN<Media::StaticImage>::ConvertFrom(img);
		if (simg->To32bpp())
		{
			cairo_surface_flush((cairo_surface_t*)gimg->GetSurface());
			UInt8 *sptr = (UInt8*)simg->data;
			UInt8 *dptr = cairo_image_surface_get_data((cairo_surface_t*)gimg->GetSurface());
			OSInt sbpl = (OSInt)simg->info.storeSize.x << 2;
			OSInt dbpl = cairo_image_surface_get_stride((cairo_surface_t*)gimg->GetSurface());
			if (simg->info.atype == Media::AT_ALPHA)
			{
				this->iab.PremulAlpha(dptr, dbpl, sptr, sbpl, simg->info.dispSize.x, simg->info.dispSize.y);
			}
			else
			{
				ImageCopy_ImgCopy(sptr, dptr, simg->info.dispSize.x << 2, simg->info.dispSize.y, sbpl, dbpl);
			}
			cairo_surface_mark_dirty((cairo_surface_t*)gimg->GetSurface());
		}
	}
	else
	{
		NN<Media::StaticImage> simg = img->CreateStaticImage();
		if (simg->To32bpp())
		{
			cairo_surface_flush((cairo_surface_t*)gimg->GetSurface());
			UInt8 *sptr = (UInt8*)simg->data;
			UInt8 *dptr = cairo_image_surface_get_data((cairo_surface_t*)gimg->GetSurface());
			OSInt sbpl = (OSInt)simg->info.storeSize.x << 2;
			OSInt dbpl = cairo_image_surface_get_stride((cairo_surface_t*)gimg->GetSurface());
			if (simg->info.atype == Media::AT_ALPHA)
			{
				this->iab.PremulAlpha(dptr, dbpl, sptr, sbpl, simg->info.dispSize.x, simg->info.dispSize.y);
			}
			else
			{
				ImageCopy_ImgCopy(sptr, dptr, simg->info.dispSize.x << 2, simg->info.dispSize.y, sbpl, dbpl);
			}
			cairo_surface_mark_dirty((cairo_surface_t*)gimg->GetSurface());
		}
		simg.Delete();
	}
	return gimg;
}

Optional<Media::DrawImage> Media::GTKDrawEngine::CloneImage(NN<DrawImage> img)
{
	NN<Media::GTKDrawImage> dimg = NN<Media::GTKDrawImage>::ConvertFrom(img);
	Math::Size2D<UOSInt> size = dimg->GetSize();
	Media::AlphaType atype = dimg->GetAlphaType();
	if (dimg->GetSurface())
	{
		NN<Media::GTKDrawImage> newImg;
		Bool upsideDown;
		UInt8 *dptr;
		if (Optional<GTKDrawImage>::ConvertFrom(this->CreateImage32(size, atype)).SetTo(newImg))
		{
			dptr = newImg->GetImgBits(upsideDown);
			((Media::GTKDrawImage*)img.Ptr())->CopyBits(0, 0, dptr, newImg->GetDataBpl(), size.x, size.y, upsideDown);
			newImg->SetHDPI(img->GetHDPI());
			newImg->SetVDPI(img->GetVDPI());
			return newImg;
		}
	}
	return 0;
}

Bool Media::GTKDrawEngine::DeleteImage(NN<DrawImage> img)
{
	Media::GTKDrawImage *dimg = (Media::GTKDrawImage*)img.Ptr();
	DEL_CLASS(dimg);
	return true;
}

Media::GTKDrawFont::GTKDrawFont(Text::CString fontName, Double fontHeight, Media::DrawEngine::DrawFontStyle drawFontStyle)
{
	Text::CStringNN cstr;
	if (!fontName.SetTo(cstr) || fontName.leng == 0)
	{
		this->fontName = Text::String::New(UTF8STRC("sans-serif"));
	}
	else
	{
		this->fontName = Text::String::New(cstr.v, cstr.leng);
	}
	this->fontHeight = fontHeight;
	if (drawFontStyle & Media::DrawEngine::DFS_BOLD)
	{
		this->fontWeight = CAIRO_FONT_WEIGHT_BOLD;
	}
	else
	{
		this->fontWeight = CAIRO_FONT_WEIGHT_NORMAL;
	}
	if (drawFontStyle & Media::DrawEngine::DFS_ITALIC)
	{
		this->fontSlant = CAIRO_FONT_SLANT_ITALIC;
	}
	else
	{
		this->fontSlant = CAIRO_FONT_SLANT_NORMAL;
	}
}

Media::GTKDrawFont::GTKDrawFont(Text::String *fontName, Double fontHeight, OSInt fontSlant, OSInt fontWeight)
{
	if (fontName == 0)
	{
		this->fontName = Text::String::New(UTF8STRC("sans-serif"));
	}
	else
	{
		this->fontName = fontName->Clone();
	}
	this->fontHeight = fontHeight;
	this->fontWeight = fontWeight;
	this->fontSlant = fontSlant;
}

Media::GTKDrawFont::GTKDrawFont(NN<Text::String> fontName, Double fontHeight, OSInt fontSlant, OSInt fontWeight)
{
	this->fontName = fontName->Clone();
	this->fontHeight = fontHeight;
	this->fontWeight = fontWeight;
	this->fontSlant = fontSlant;
}

Media::GTKDrawFont::~GTKDrawFont()
{
	this->fontName->Release();
}

void Media::GTKDrawFont::Init(void *cr)
{
	cairo_select_font_face((cairo_t*)cr, (const Char*)this->fontName->v.Ptr(), (cairo_font_slant_t)this->fontSlant, (cairo_font_weight_t)this->fontWeight);
	cairo_set_font_size((cairo_t*)cr, this->fontHeight);
}

NN<Text::String> Media::GTKDrawFont::GetFontName()
{
	return this->fontName;
}

Double Media::GTKDrawFont::GetHeight()
{
	return this->fontHeight;
}

OSInt Media::GTKDrawFont::GetFontWeight()
{
	return this->fontWeight;
}

OSInt Media::GTKDrawFont::GetFontSlant()
{
	return this->fontSlant;
}

Media::GTKDrawPen::GTKDrawPen(UInt32 oriColor, Double thick)
{
	this->oriColor = oriColor;
	this->bV = (oriColor & 255) / 255.0;
	this->gV = ((oriColor >> 8) & 255) / 255.0;
	this->rV = ((oriColor >> 16) & 255) / 255.0;
	this->aV = ((oriColor >> 24) & 255) / 255.0;
	this->thick = thick;
}

Media::GTKDrawPen::~GTKDrawPen()
{
}

Double Media::GTKDrawPen::GetThick()
{
	return this->thick;
}

void Media::GTKDrawPen::Init(void *cr)
{
	cairo_set_source_rgba((cairo_t*)cr, this->rV, this->gV, this->bV, this->aV);
	cairo_set_line_width((cairo_t*)cr, this->thick);
}

Media::GTKDrawBrush::GTKDrawBrush(UInt32 oriColor)
{
	this->oriColor = oriColor;
	this->bV = (oriColor & 255) / 255.0;
	this->gV = ((oriColor >> 8) & 255) / 255.0;
	this->rV = ((oriColor >> 16) & 255) / 255.0;
	this->aV = ((oriColor >> 24) & 255) / 255.0;
}

Media::GTKDrawBrush::~GTKDrawBrush()
{
}

void Media::GTKDrawBrush::Init(void *cr)
{
	cairo_set_source_rgba((cairo_t*)cr, this->rV, this->gV, this->bV, this->aV);
}

UInt32 Media::GTKDrawBrush::GetOriColor()
{
	return this->oriColor;
}

Media::GTKDrawImage::GTKDrawImage(NN<GTKDrawEngine> eng, void *surface, void *cr, Math::Coord2D<OSInt> tl, Math::Size2D<UOSInt> size, UInt32 bitCount, Media::AlphaType atype) : Media::RasterImage(size, Math::Size2D<UOSInt>(0, 0), 0, bitCount, Media::PixelFormatGetDef(0, bitCount), 0, ColorProfile(), Media::ColorProfile::YUVT_BT601, atype, Media::YCOFST_C_CENTER_LEFT)
{
	this->eng = eng;
	this->surface = surface;
	this->cr = cr;
	this->tl = tl;
	this->info.hdpi = 96.0;
	this->info.vdpi = 96.0;
}

Media::GTKDrawImage::~GTKDrawImage()
{
	if (this->surface)
	{
		cairo_surface_destroy((cairo_surface_t*)this->surface);
		cairo_destroy((cairo_t*)this->cr);
	}
}

UOSInt Media::GTKDrawImage::GetWidth() const
{
	return this->info.dispSize.x;
}

UOSInt Media::GTKDrawImage::GetHeight() const
{
	return this->info.dispSize.y;
}

Math::Size2D<UOSInt> Media::GTKDrawImage::GetSize() const
{
	return this->info.dispSize;
}

UInt32 Media::GTKDrawImage::GetBitCount() const
{
	return this->info.storeBPP;
}

NN<const Media::ColorProfile> Media::GTKDrawImage::GetColorProfile() const
{
	return this->info.color;
}

void Media::GTKDrawImage::SetColorProfile(NN<const ColorProfile> color)
{
	this->info.color.Set(color);
}

Media::AlphaType Media::GTKDrawImage::GetAlphaType() const
{
	return this->info.atype;
}

void Media::GTKDrawImage::SetAlphaType(Media::AlphaType atype)
{
	this->info.atype = atype;
}

Double Media::GTKDrawImage::GetHDPI() const
{
	return this->info.hdpi;
}

Double Media::GTKDrawImage::GetVDPI() const
{
	return this->info.vdpi;
}

void Media::GTKDrawImage::SetHDPI(Double dpi)
{
	this->info.hdpi = dpi;
}

void Media::GTKDrawImage::SetVDPI(Double dpi)
{
	this->info.vdpi = dpi;
}

UInt8 *Media::GTKDrawImage::GetImgBits(OutParam<Bool> revOrder)
{
	cairo_surface_flush((cairo_surface_t*)this->surface);
	revOrder.Set(false);
	return cairo_image_surface_get_data((cairo_surface_t*)this->surface);
}

void Media::GTKDrawImage::GetImgBitsEnd(Bool modified)
{
	if (modified)
	{
		cairo_surface_mark_dirty((cairo_surface_t*)this->surface);
	}
}

UOSInt Media::GTKDrawImage::GetImgBpl() const
{
	return (this->info.storeSize.x * this->info.storeBPP) >> 3;
}

Optional<Media::EXIFData> Media::GTKDrawImage::GetEXIF() const
{
	return this->exif;
}

Media::PixelFormat Media::GTKDrawImage::GetPixelFormat() const
{
	return this->info.pf;
}

Bool Media::GTKDrawImage::DrawLine(Double x1, Double y1, Double x2, Double y2, NN<DrawPen> p)
{
	NN<GTKDrawPen> pen = NN<GTKDrawPen>::ConvertFrom(p);
	pen->Init(this->cr);
	cairo_move_to((cairo_t*)this->cr, x1 + OSInt2Double(this->tl.x), y1 + OSInt2Double(this->tl.y));
	cairo_line_to((cairo_t*)this->cr, x2 + OSInt2Double(this->tl.x), y2 + OSInt2Double(this->tl.y));
	cairo_stroke((cairo_t*)this->cr);
	return true;
}

Bool Media::GTKDrawImage::DrawPolylineI(const Int32 *points, UOSInt nPoints, NN<DrawPen> p)
{
	printf("GTK: Draw PolylineI (Not support)\r\n");
	return false;
}

Bool Media::GTKDrawImage::DrawPolygonI(const Int32 *points, UOSInt nPoints, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	printf("GTK: Draw PolygonI (Not support)\r\n");
	return false;
}

Bool Media::GTKDrawImage::DrawPolyPolygonI(const Int32 *points, const UInt32 *pointCnt, UOSInt nPointCnt, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	printf("GTK: Draw PolyPolygonI (Not support)\r\n");
	return false;
}

Bool Media::GTKDrawImage::DrawPolyline(const Math::Coord2DDbl *points, UOSInt nPoints, NN<DrawPen> p)
{
	NN<GTKDrawPen> pen = NN<GTKDrawPen>::ConvertFrom(p);
	if (nPoints >= 2)
	{
		pen->Init(this->cr);
		cairo_move_to((cairo_t*)this->cr, points->x + OSInt2Double(this->tl.x), points->y + OSInt2Double(this->tl.y));
		points++;
		while (nPoints-- > 1)
		{
			cairo_line_to((cairo_t*)this->cr, points->x + OSInt2Double(this->tl.x), points->y + OSInt2Double(this->tl.y));
			points++;
		}
		cairo_stroke((cairo_t*)this->cr);
		return true;
	}
	else
	{
		return false;
	}
}

Bool Media::GTKDrawImage::DrawPolygon(const Math::Coord2DDbl *points, UOSInt nPoints, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	NN<GTKDrawPen> pen;
	NN<DrawPen> nnp;
	NN<DrawBrush> nnb;
	NN<GTKDrawBrush> brush;
	if (nPoints >= 2)
	{
		cairo_move_to((cairo_t*)this->cr, points->x + OSInt2Double(this->tl.x), points->y + OSInt2Double(this->tl.y));
		points++;
		while (nPoints-- > 1)
		{
			cairo_line_to((cairo_t*)this->cr, points->x + OSInt2Double(this->tl.x), points->y + OSInt2Double(this->tl.y));
			points++;
		}
		cairo_close_path((cairo_t*)this->cr);
		if (p.SetTo(nnp) && b.SetTo(nnb))
		{
			brush = NN<GTKDrawBrush>::ConvertFrom(nnb);
			pen = NN<GTKDrawPen>::ConvertFrom(nnp);
			pen->Init(this->cr);
			cairo_stroke_preserve((cairo_t*)this->cr);
			brush->Init(this->cr);
			cairo_fill((cairo_t*)this->cr);
		}
		else if (p.SetTo(nnp))
		{
			pen = NN<GTKDrawPen>::ConvertFrom(nnp);
			pen->Init(this->cr);
			cairo_stroke((cairo_t*)this->cr);
		}
		else if (b.SetTo(nnb))
		{
			brush = NN<GTKDrawBrush>::ConvertFrom(nnb);
			brush->Init(this->cr);
			cairo_fill((cairo_t*)this->cr);
		}
		else
		{
			cairo_new_path((cairo_t*)this->cr);
		}
		return true;
	}
	else
	{
		return false;
	}
}

Bool Media::GTKDrawImage::DrawPolyPolygon(const Math::Coord2DDbl *points, const UInt32 *pointCnt, UOSInt nPointCnt, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	NN<GTKDrawPen> pen;
	NN<DrawPen> nnp;
	NN<DrawBrush> nnb;
	NN<GTKDrawBrush> brush;
	UOSInt i;
	if (nPointCnt > 0)
	{
		while (nPointCnt-- > 0)
		{
			i = *pointCnt++;
			if (i > 0)
			{
				cairo_move_to((cairo_t*)this->cr, points->x + OSInt2Double(this->tl.x), points->y + OSInt2Double(this->tl.y));
				points++;
				while (i-- > 1)
				{
					cairo_line_to((cairo_t*)this->cr, points->x + OSInt2Double(this->tl.x), points->y + OSInt2Double(this->tl.y));
					points++;
				}
				cairo_close_path((cairo_t*)this->cr);
			}
		}

		if (p.SetTo(nnp) && b.SetTo(nnb))
		{
			brush = NN<GTKDrawBrush>::ConvertFrom(nnb);
			pen = NN<GTKDrawPen>::ConvertFrom(nnp);
			pen->Init(this->cr);
			cairo_stroke_preserve((cairo_t*)this->cr);
			brush->Init(this->cr);
			cairo_fill((cairo_t*)this->cr);
		}
		else if (p.SetTo(nnp))
		{
			pen = NN<GTKDrawPen>::ConvertFrom(nnp);
			pen->Init(this->cr);
			cairo_stroke((cairo_t*)this->cr);
		}
		else if (b.SetTo(nnb))
		{
			brush = NN<GTKDrawBrush>::ConvertFrom(nnb);
			brush->Init(this->cr);
			cairo_fill((cairo_t*)this->cr);
		}
		else
		{
			cairo_new_path((cairo_t*)this->cr);
		}
		return true;
	}
	else
	{
		return false;
	}
}

Bool Media::GTKDrawImage::DrawRect(Math::Coord2DDbl tl, Math::Size2DDbl size, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	NN<DrawBrush> nnb;
	NN<DrawPen> nnp;
	if (b.SetTo(nnb))
	{
		NN<GTKDrawBrush> brush = NN<GTKDrawBrush>::ConvertFrom(nnb);
		brush->Init(this->cr);
		cairo_rectangle((cairo_t*)this->cr, tl.x + OSInt2Double(this->tl.x), tl.y + OSInt2Double(this->tl.y), size.x, size.y);
		cairo_fill((cairo_t*)this->cr);
	}
	if (p.SetTo(nnp))
	{
		NN<GTKDrawPen> pen = NN<GTKDrawPen>::ConvertFrom(nnp);
		pen->Init(this->cr);
		cairo_rectangle((cairo_t*)this->cr, tl.x + OSInt2Double(this->tl.x), tl.y + OSInt2Double(this->tl.y), size.x, size.y);
		cairo_stroke((cairo_t*)this->cr);
	}
	return true;
}

Bool Media::GTKDrawImage::DrawEllipse(Math::Coord2DDbl tl, Math::Size2DDbl size, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	NN<GTKDrawPen> pen;
	NN<DrawPen> nnp;
	NN<DrawBrush> nnb;
	NN<GTKDrawBrush> brush;
	cairo_t *cr = (cairo_t *)this->cr;
	cairo_save(cr);
	cairo_translate(cr, tl.x + size.x * 0.5, tl.y + size.y * 0.5);
	cairo_scale(cr, 1, size.y / size.x);
	cairo_translate(cr, -tl.x - size.x * 0.5, -tl.y - size.y * 0.5);
	cairo_arc(cr, tl.x + size.x * 0.5, tl.y + size.y * 0.5, size.x * 0.5, 0, 2 * Math::PI);
	cairo_close_path(cr);
	if (p.SetTo(nnp) && b.SetTo(nnb))
	{
		brush = NN<GTKDrawBrush>::ConvertFrom(nnb);
		pen = NN<GTKDrawPen>::ConvertFrom(nnp);
		pen->Init(this->cr);
		cairo_stroke_preserve((cairo_t*)this->cr);
		brush->Init(this->cr);
		cairo_fill((cairo_t*)this->cr);
	}
	else if (p.SetTo(nnp))
	{
		pen = NN<GTKDrawPen>::ConvertFrom(nnp);
		pen->Init(this->cr);
		cairo_stroke((cairo_t*)this->cr);
	}
	else if (b.SetTo(nnb))
	{
		brush = NN<GTKDrawBrush>::ConvertFrom(nnb);
		brush->Init(this->cr);
		cairo_fill((cairo_t*)this->cr);
	}
	else
	{
		cairo_new_path((cairo_t*)this->cr);
	}
	cairo_restore(cr);
	return true;
}

Bool Media::GTKDrawImage::DrawString(Math::Coord2DDbl tl, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b)
{
	return DrawString(tl, str->ToCString(), f, b);
}

Bool Media::GTKDrawImage::DrawString(Math::Coord2DDbl tl, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b)
{
	NN<GTKDrawFont> font = NN<GTKDrawFont>::ConvertFrom(f);
	NN<GTKDrawBrush> brush = NN<GTKDrawBrush>::ConvertFrom(b);
	font->Init(this->cr);
	brush->Init(this->cr);
	cairo_move_to((cairo_t *)this->cr, tl.x + OSInt2Double(this->tl.x), tl.y + font->GetHeight() * 0.8 + 1 + OSInt2Double(this->tl.y));
	cairo_show_text((cairo_t *)this->cr, (const Char*)str.v.Ptr());
	return true;
}

Bool Media::GTKDrawImage::DrawStringRot(Math::Coord2DDbl center, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegree)
{
	return DrawStringRot(center, str->ToCString(), f, b, angleDegree);
}

Bool Media::GTKDrawImage::DrawStringRot(Math::Coord2DDbl center, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegree)
{
	NN<GTKDrawFont> font = NN<GTKDrawFont>::ConvertFrom(f);
	NN<GTKDrawBrush> brush = NN<GTKDrawBrush>::ConvertFrom(b);
	Double angleR = -angleDegree * Math::PI / 180;
	cairo_text_extents_t extents;
	font->Init(this->cr);
	brush->Init(this->cr);
	Double cVal = Math_Cos(angleR);
	Double sVal = Math_Sin(angleR);
	Double dist = font->GetHeight() * 0.8 + 1;
	cairo_text_extents((cairo_t *)this->cr, (const Char*)str.v.Ptr(), &extents);
	cairo_move_to((cairo_t *)this->cr, center.x - dist * sVal + OSInt2Double(this->tl.x), center.y + dist * cVal + OSInt2Double(this->tl.y));
	cairo_rotate((cairo_t *)this->cr, angleR);
	cairo_show_text((cairo_t *)this->cr, (const Char*)str.v.Ptr());
	cairo_rotate((cairo_t *)this->cr, -angleR);
	return true;
}

Bool Media::GTKDrawImage::DrawStringB(Math::Coord2DDbl tl, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b, UOSInt buffSize)
{
	return DrawStringB(tl, str->ToCString(), f, b, buffSize);
}

Bool Media::GTKDrawImage::DrawStringB(Math::Coord2DDbl tl, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b, UOSInt buffSize)
{
	OSInt px = Double2OSInt(tl.x);
	OSInt py = Double2OSInt(tl.y);
	NN<GTKDrawFont> font = NN<GTKDrawFont>::ConvertFrom(f);
	NN<GTKDrawBrush> brush = NN<GTKDrawBrush>::ConvertFrom(b);

	UInt32 sz[2];
	NN<Media::GTKDrawImage> gimg;
	OSInt swidth;
	OSInt sheight;
	OSInt sx;
	OSInt sy;
	OSInt dwidth;
	OSInt dheight;

	cairo_text_extents_t extents;
	font->Init(this->cr);
	cairo_text_extents((cairo_t *)this->cr, (const Char*)str.v.Ptr(), &extents);
	sz[0] = (UInt32)Double2Int32(extents.width + 2);
	sz[1] = (UInt32)Double2Int32(extents.height + 2);
	dwidth = (OSInt)this->info.dispSize.x - px;
	dheight = (OSInt)this->info.dispSize.y - py;

	if (dwidth < 0)
	{
	}
	else if (dheight < 0)
	{
	}
	else
	{
		swidth = (OSInt)(sz[0] + (buffSize << 1));
		sheight = (OSInt)(sz[1] + (buffSize << 1));
		if (!Optional<Media::GTKDrawImage>::ConvertFrom(eng->CreateImage32(Math::Size2D<UOSInt>((UOSInt)swidth, (UOSInt)sheight), Media::AT_ALPHA)).SetTo(gimg))
		{
			return false;
		}

		if (px < (OSInt)buffSize)
		{
			sx = -px + (OSInt)buffSize;
			swidth += px;
			px = 0;
		}
		else
		{
			sx = 0;
			px -= (OSInt)buffSize;
		}
		if (py < (OSInt)buffSize)
		{
			sy = -py + (OSInt)buffSize;
			sheight = (sheight + py);
			py = 0;
		}
		else
		{
			sy = 0;
			py -= (OSInt)buffSize;
		}
		if (((OSInt)gimg->GetHeight() - sheight) < sy)
		{
			sheight = (OSInt)gimg->GetHeight() - sy;
		}
		if (((OSInt)gimg->GetWidth() - swidth) < sx)
		{
			swidth = (OSInt)gimg->GetWidth() - sx;
		}
		if (dwidth + (OSInt)buffSize < swidth)
		{
			swidth = dwidth + (OSInt)buffSize;
		}
		if (dheight + (OSInt)buffSize < sheight)
		{
			sheight = dheight + (OSInt)buffSize;
		}
		if (swidth <= 0 || sheight <= 0 || sz[0] <= 0 || sz[1] <= 0)
		{

		}
		else
		{
			NN<Media::DrawBrush> whiteB = gimg->NewBrushARGB(0xffffffff);
			gimg->DrawString(Math::Coord2DDbl(OSInt2Double(sx) + tl.x - OSInt2Double(px),
				OSInt2Double(sy) + tl.y - OSInt2Double(py)), str, f, whiteB);
			gimg->DelBrush(whiteB);

			UOSInt bpl = (sz[0] + (buffSize << 1)) << 2;
			UOSInt dbpl = this->info.dispSize.x << 2;
			UInt32 color = brush->GetOriColor();
			cairo_surface_flush((cairo_surface_t*)gimg->surface);
			UInt8 *pbits = cairo_image_surface_get_data((cairo_surface_t*)gimg->surface);
			UInt8 *dbits = cairo_image_surface_get_data((cairo_surface_t*)this->surface);
			ImageUtil_ImageColorBuffer32(pbits + bpl * buffSize + buffSize * 4, sz[0], sz[1], bpl, buffSize);
			if (py + sheight > (OSInt)this->info.dispSize.y)
			{
				sheight = (OSInt)this->info.dispSize.y - py;
			}
			if (px + swidth > (OSInt)this->info.dispSize.x)
			{
				swidth = (OSInt)this->info.dispSize.x - px;
			}
			if (swidth > 0 && sheight > 0)
			{
				if (dbits)
				{
					pbits = ((UInt8*)pbits) + (sy * (OSInt)bpl) + (sx << 2);
					dbits = ((UInt8*)dbits) + (py * (OSInt)dbpl) + (px << 2);

					if ((color & 0xff000000) == 0xff000000)
					{
						ImageUtil_ImageColorReplace32((UInt8*)pbits, (UInt8*)dbits, (UOSInt)swidth, (UOSInt)sheight, (OSInt)bpl, (OSInt)dbpl, color);
					}
					else
					{
						ImageUtil_ImageMaskABlend32((UInt8*)pbits, (UInt8*)dbits, (UOSInt)swidth, (UOSInt)sheight, (OSInt)bpl, (OSInt)dbpl, color);
					}
					cairo_surface_mark_dirty((cairo_surface_t*)this->surface);
				}
				else
				{
					pbits = ((UInt8*)pbits) + (sy * (OSInt)bpl) + (sx << 2);

					ImageUtil_ImageColorReplace32((UInt8*)pbits, (UInt8*)pbits, (UOSInt)swidth, (UOSInt)sheight, (OSInt)bpl, (OSInt)bpl, color);
					gimg->SetHDPI(this->GetHDPI());
					gimg->SetVDPI(this->GetVDPI());
					this->DrawImagePt(gimg, Math::Coord2DDbl(OSInt2Double(px - sx), OSInt2Double(py - sy)));
				}
			}
		}
		eng->DeleteImage(gimg);
	}

	return true;
}

Bool Media::GTKDrawImage::DrawStringRotB(Math::Coord2DDbl center, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegree, UOSInt buffSize)
{
	return DrawStringRotB(center, str->ToCString(), f, b, angleDegree, buffSize);
}

Bool Media::GTKDrawImage::DrawStringRotB(Math::Coord2DDbl center, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegree, UOSInt buffSize)
{
	printf("GTK: Draw StringRotBUTF8 (Not support)\r\n");
	return false;
}

Bool Media::GTKDrawImage::DrawImagePt(NN<DrawImage> img, Math::Coord2DDbl tl)
{
	Media::GTKDrawImage *gimg = (GTKDrawImage*)img.Ptr();
	if (gimg->surface == 0)
	{
		return false;
	}
	if (this->surface == 0)
	{
		cairo_surface_flush((cairo_surface_t*)gimg->surface);
		if (gimg->info.atype == Media::AT_NO_ALPHA)
		{
			Bool revOrder;
			ImageUtil_ImageFillAlpha32(gimg->GetImgBits(revOrder), gimg->GetWidth(), gimg->GetHeight(), gimg->GetImgBpl(), 0xFF);
			gimg->GetImgBitsEnd(true);
		}
		cairo_save((cairo_t*)this->cr);
		cairo_translate((cairo_t*)this->cr, tl.x + OSInt2Double(this->tl.x), tl.y + OSInt2Double(this->tl.y));
		cairo_scale((cairo_t*)this->cr, this->info.hdpi / img->GetHDPI(), this->info.vdpi / img->GetVDPI());
		cairo_set_source_surface((cairo_t*)this->cr, (cairo_surface_t*)gimg->surface, 0, 0);
		cairo_paint((cairo_t*)this->cr);
		cairo_restore((cairo_t*)this->cr);
		return true;
	}
	else
	{
		if (gimg->info.storeBPP != 32 || this->info.storeBPP != 32)
		{
			cairo_save((cairo_t*)this->cr);
			cairo_translate((cairo_t*)this->cr, tl.x + OSInt2Double(this->tl.x), tl.y + OSInt2Double(this->tl.y));
			cairo_scale((cairo_t*)this->cr, 1, 1);
			cairo_set_source_surface((cairo_t*)this->cr, (cairo_surface_t*)gimg->surface, 0, 0);
			cairo_paint((cairo_t*)this->cr);
			cairo_restore((cairo_t*)this->cr);
			return true;
		}
		cairo_surface_flush((cairo_surface_t*)this->surface);
		cairo_surface_flush((cairo_surface_t*)gimg->surface);
		UInt8 *dimgPtr = cairo_image_surface_get_data((cairo_surface_t*)this->surface);
		OSInt dbpl = cairo_image_surface_get_stride((cairo_surface_t*)this->surface);
		UInt8 *simgPtr = cairo_image_surface_get_data((cairo_surface_t*)gimg->surface);
		OSInt sbpl = cairo_image_surface_get_stride((cairo_surface_t*)gimg->surface);

		OSInt ixPos = Double2Int32(tl.x);
		OSInt iyPos = Double2Int32(tl.y);
		OSInt ixPos2 = ixPos + (OSInt)gimg->info.dispSize.x;
		OSInt iyPos2 = iyPos + (OSInt)gimg->info.dispSize.y;
		if ((OSInt)this->info.dispSize.x < ixPos2)
		{
			ixPos2 = (OSInt)this->info.dispSize.x;
		}
		if (ixPos < 0)
		{
			simgPtr += -ixPos * 4;
			ixPos = 0;
		}

		if ((OSInt)this->info.dispSize.y < iyPos2)
		{
			iyPos2 = (OSInt)this->info.dispSize.y;
		}
		if (iyPos < 0)
		{
			simgPtr += -iyPos * sbpl;
			iyPos = 0;
		}
		if (ixPos >= ixPos2 || iyPos >= iyPos2)
		{
			return true;
		}

		if (gimg->info.atype == Media::AT_NO_ALPHA)
		{
			ImageCopy_ImgCopy(simgPtr, dimgPtr + ixPos * 4 + iyPos * dbpl, (UOSInt)(ixPos2 - ixPos) * 4, (UOSInt)(iyPos2 - iyPos), sbpl, dbpl);
			cairo_surface_mark_dirty((cairo_surface_t*)this->surface);
			return true;
		}
		else
		{
			this->eng->iab.SetSourceProfile(gimg->info.color);
			this->eng->iab.SetDestProfile(this->info.color);
			this->eng->iab.SetOutputProfile(this->info.color);
			this->eng->iab.Blend(dimgPtr + ixPos * 4 + iyPos * dbpl, dbpl, simgPtr, sbpl, (UOSInt)(ixPos2 - ixPos), (UOSInt)(iyPos2 - iyPos), gimg->info.atype);
			cairo_surface_mark_dirty((cairo_surface_t*)this->surface);
			return true;
		}
		return false;
	}
}

Bool Media::GTKDrawImage::DrawImagePt2(NN<Media::StaticImage> img, Math::Coord2DDbl tl)
{
	if (this->surface == 0)
	{
		NN<Media::DrawImage> dimg;
		if (!this->eng->ConvImage(img).SetTo(dimg))
		{
			return false;
		}
		this->DrawImagePt(dimg, tl);
		this->eng->DeleteImage(dimg);
		return true;
	}
	img->To32bpp();
	if (img->info.storeBPP != 32)
	{
		return false;
	}
	cairo_surface_flush((cairo_surface_t*)this->surface);
	UInt8 *dimgPtr = cairo_image_surface_get_data((cairo_surface_t*)this->surface);
	OSInt dbpl = cairo_image_surface_get_stride((cairo_surface_t*)this->surface);
	UInt8 *simgPtr = img->data;
	OSInt sbpl = (OSInt)img->info.storeSize.x * 4;

	Int32 ixPos = Double2Int32(tl.x);
	Int32 iyPos = Double2Int32(tl.y);
	Int32 ixPos2 = ixPos + (Int32)img->info.dispSize.x;
	Int32 iyPos2 = iyPos + (Int32)img->info.dispSize.y;
	if ((OSInt)this->info.dispSize.x < ixPos2)
	{
		ixPos2 = (Int32)this->info.dispSize.x;
	}
	if (ixPos < 0)
	{
		simgPtr += -ixPos * 4;
		ixPos = 0;
	}

	if ((OSInt)this->info.dispSize.y < iyPos2)
	{
		iyPos2 = (Int32)this->info.dispSize.y;
	}
	if (iyPos < 0)
	{
		simgPtr += -iyPos * sbpl;
		iyPos = 0;
	}
	if (ixPos >= ixPos2 || iyPos >= iyPos2)
	{
		return true;
	}

	if (img->info.atype == Media::AT_NO_ALPHA)
	{
		ImageCopy_ImgCopy(simgPtr, dimgPtr + ixPos * 4 + iyPos * dbpl, (UOSInt)(ixPos2 - ixPos) * 4, (UOSInt)(iyPos2 - iyPos), sbpl, dbpl);
		cairo_surface_mark_dirty((cairo_surface_t*)this->surface);
		return true;
	}
	else //////////////////////////////////////////
	{
		this->eng->iab.SetSourceProfile(img->info.color);
		this->eng->iab.SetDestProfile(this->info.color);
		this->eng->iab.SetOutputProfile(this->info.color);
		this->eng->iab.Blend(dimgPtr + ixPos * 4 + iyPos * dbpl, dbpl, simgPtr, sbpl, (UOSInt)(ixPos2 - ixPos), (UOSInt)(iyPos2 - iyPos), img->info.atype);
		cairo_surface_mark_dirty((cairo_surface_t*)this->surface);
		return true;
	}
	return false;
}

Bool Media::GTKDrawImage::DrawImagePt3(NN<DrawImage> img, Math::Coord2DDbl destTL, Math::Coord2DDbl srcTL, Math::Size2DDbl srcSize)
{
	Media::GTKDrawImage *gimg = (GTKDrawImage*)img.Ptr();
	if (gimg->surface == 0)
	{
		return false;
	}
	if (this->surface == 0)
	{
		cairo_surface_flush((cairo_surface_t*)gimg->surface);
		if (gimg->info.atype == Media::AT_NO_ALPHA)
		{
			Bool revOrder;
			ImageUtil_ImageFillAlpha32(gimg->GetImgBits(revOrder), gimg->GetWidth(), gimg->GetHeight(), gimg->GetImgBpl(), 0xFF);
			gimg->GetImgBitsEnd(true);
		}
		cairo_save((cairo_t*)this->cr);
		cairo_rectangle((cairo_t*)this->cr, destTL.x + OSInt2Double(this->tl.x), destTL.y + OSInt2Double(this->tl.y), srcSize.x, srcSize.y);
		cairo_clip((cairo_t*)this->cr);
		cairo_translate((cairo_t*)this->cr, destTL.x + OSInt2Double(this->tl.x), destTL.y + OSInt2Double(this->tl.y));
		cairo_scale((cairo_t*)this->cr, this->info.hdpi / img->GetHDPI(), this->info.vdpi / img->GetVDPI());
		cairo_set_source_surface((cairo_t*)this->cr, (cairo_surface_t*)gimg->surface, Double2Int32(srcTL.x), Double2Int32(srcTL.y));
		cairo_paint((cairo_t*)this->cr);
		cairo_restore((cairo_t*)this->cr);
		return true;
	}
	else
	{
		if (gimg->info.storeBPP != 32 || this->info.storeBPP != 32)
		{
			cairo_save((cairo_t*)this->cr);
			cairo_rectangle((cairo_t*)this->cr, destTL.x + OSInt2Double(this->tl.x), destTL.y + OSInt2Double(this->tl.y), srcSize.x, srcSize.y);
			cairo_clip((cairo_t*)this->cr);
			cairo_translate((cairo_t*)this->cr, destTL.x + OSInt2Double(this->tl.x), destTL.y + OSInt2Double(this->tl.y));
			cairo_scale((cairo_t*)this->cr, 1, 1);
			cairo_set_source_surface((cairo_t*)this->cr, (cairo_surface_t*)gimg->surface, Double2Int32(-srcTL.x), Double2Int32(-srcTL.y));
			cairo_paint((cairo_t*)this->cr);
			cairo_restore((cairo_t*)this->cr);
			return true;
		}
		cairo_surface_flush((cairo_surface_t*)this->surface);
		cairo_surface_flush((cairo_surface_t*)gimg->surface);
		UInt8 *dimgPtr = cairo_image_surface_get_data((cairo_surface_t*)this->surface);
		OSInt dbpl = cairo_image_surface_get_stride((cairo_surface_t*)this->surface);
		UInt8 *simgPtr = cairo_image_surface_get_data((cairo_surface_t*)gimg->surface);
		OSInt sbpl = cairo_image_surface_get_stride((cairo_surface_t*)gimg->surface);

		OSInt ixPos = Double2Int32(destTL.x);
		OSInt iyPos = Double2Int32(destTL.y);
		OSInt ixPos2 = ixPos + (OSInt)srcSize.x;
		OSInt iyPos2 = iyPos + (OSInt)srcSize.y;
		if ((OSInt)this->info.dispSize.x < ixPos2)
		{
			ixPos2 = (OSInt)this->info.dispSize.x;
		}
		if (ixPos < 0)
		{
			simgPtr += -ixPos * 4;
			ixPos = 0;
		}

		if ((OSInt)this->info.dispSize.y < iyPos2)
		{
			iyPos2 = (OSInt)this->info.dispSize.y;
		}
		if (iyPos < 0)
		{
			simgPtr += -iyPos * sbpl;
			iyPos = 0;
		}
		if (ixPos >= ixPos2 || iyPos >= iyPos2)
		{
			return true;
		}

		if (gimg->info.atype == Media::AT_NO_ALPHA)
		{
			ImageCopy_ImgCopy(simgPtr + Double2OSInt(srcTL.y) * sbpl + Double2OSInt(srcTL.x) * 4, dimgPtr + ixPos * 4 + iyPos * dbpl, (UOSInt)(ixPos2 - ixPos) * 4, (UOSInt)(iyPos2 - iyPos), sbpl, dbpl);
			cairo_surface_mark_dirty((cairo_surface_t*)this->surface);
			return true;
		}
		else
		{
			this->eng->iab.SetSourceProfile(gimg->info.color);
			this->eng->iab.SetDestProfile(this->info.color);
			this->eng->iab.SetOutputProfile(this->info.color);
			this->eng->iab.Blend(dimgPtr + ixPos * 4 + iyPos * dbpl, dbpl, simgPtr + Double2OSInt(srcTL.y) * sbpl + Double2OSInt(srcTL.x) * 4, sbpl, (UOSInt)(ixPos2 - ixPos), (UOSInt)(iyPos2 - iyPos), gimg->info.atype);
			cairo_surface_mark_dirty((cairo_surface_t*)this->surface);
			return true;
		}
		return false;
	}
}

NN<Media::DrawPen> Media::GTKDrawImage::NewPenARGB(UInt32 color, Double thick, UInt8 *pattern, UOSInt nPattern)
{
	NN<Media::GTKDrawPen> p;
	NEW_CLASSNN(p, Media::GTKDrawPen(color, thick));
	return p;
}

NN<Media::DrawBrush> Media::GTKDrawImage::NewBrushARGB(UInt32 color)
{
	NN<Media::GTKDrawBrush> b;
	NEW_CLASSNN(b, Media::GTKDrawBrush(color));
	return b;
}

NN<Media::DrawFont> Media::GTKDrawImage::NewFontPt(Text::CString name, Double ptSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage)
{
	NN<Media::GTKDrawFont> f;
	NEW_CLASSNN(f, Media::GTKDrawFont(name, ptSize * this->info.hdpi / 72.0, fontStyle));
	return f;
}

NN<Media::DrawFont> Media::GTKDrawImage::NewFontPx(Text::CString name, Double pxSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage)
{
	NN<Media::GTKDrawFont> f;
	NEW_CLASSNN(f, Media::GTKDrawFont(name, pxSize, fontStyle));
	return f;
}

NN<Media::DrawFont> Media::GTKDrawImage::CloneFont(NN<DrawFont> f)
{
	NN<Media::GTKDrawFont> fnt = NN<Media::GTKDrawFont>::ConvertFrom(f);
	NN<Text::String> fname = fnt->GetFontName();
	Double height = fnt->GetHeight();
	OSInt fontWeight = fnt->GetFontWeight();
	OSInt fontSlant = fnt->GetFontSlant();
	NEW_CLASSNN(fnt, Media::GTKDrawFont(fname, height, fontWeight, fontSlant));
	return fnt;
}

void Media::GTKDrawImage::DelPen(NN<DrawPen> p)
{
	NN<GTKDrawPen> pen = NN<GTKDrawPen>::ConvertFrom(p);
	pen.Delete();
}

void Media::GTKDrawImage::DelBrush(NN<DrawBrush> b)
{
	NN<GTKDrawBrush> brush = NN<GTKDrawBrush>::ConvertFrom(b);
	brush.Delete();
}

void Media::GTKDrawImage::DelFont(NN<DrawFont> f)
{
	NN<GTKDrawFont> font = NN<GTKDrawFont>::ConvertFrom(f);
	font.Delete();
}

Math::Size2DDbl Media::GTKDrawImage::GetTextSize(NN<DrawFont> fnt, Text::CStringNN txt)
{
	NN<GTKDrawFont> font = NN<GTKDrawFont>::ConvertFrom(fnt);
	cairo_text_extents_t extents;
	font->Init(this->cr);
	cairo_text_extents((cairo_t *)this->cr, (const Char*)txt.v.Ptr(), &extents);
	return Math::Size2DDbl(extents.width + 2, font->GetHeight() + 2);
}

void Media::GTKDrawImage::SetTextAlign(DrawEngine::DrawPos pos)
{
}

void Media::GTKDrawImage::GetStringBound(Int32 *pos, OSInt centX, OSInt centY, const UTF8Char *str, NN<DrawFont> f, OutParam<OSInt> drawX, OutParam<OSInt> drawY)
{
}

void Media::GTKDrawImage::GetStringBoundRot(Int32 *pos, Double centX, Double centY, const UTF8Char *str, NN<DrawFont> f, Double angleDegree, OutParam<OSInt> drawX, OutParam<OSInt> drawY)
{
}

void Media::GTKDrawImage::CopyBits(OSInt x, OSInt y, void *imgPtr, UOSInt bpl, UOSInt width, UOSInt height, Bool upsideDown) const
{
	if (this->surface)
	{
		if (x < 0)
		{
			width = (UOSInt)((OSInt)width + x);
			x = 0;
		}
		if (y < 0)
		{
			height = (UOSInt)((OSInt)height + y);
			y = 0;
		}
		cairo_surface_flush((cairo_surface_t*)this->surface);
		UInt8 *srcData = cairo_image_surface_get_data((cairo_surface_t*)this->surface);
		if (srcData)
		{
			ImageCopy_ImgCopyR(srcData + x * 4 + y * (OSInt)this->info.storeSize.x * 4, (UInt8*)imgPtr, width * 4, height, this->info.storeSize.x * 4, bpl, upsideDown);
		}
	}
}

Media::StaticImage *Media::GTKDrawImage::ToStaticImage() const
{
	if (this->surface)
	{
		Media::StaticImage *simg = 0;
		UInt8 *srcData;
		cairo_surface_flush((cairo_surface_t*)this->surface);
		srcData = cairo_image_surface_get_data((cairo_surface_t*)this->surface);
		if (srcData == 0)
			return 0;
		NEW_CLASS(simg, Media::StaticImage(this->info));
		MemCopyNO(simg->data, srcData, this->info.storeSize.CalcArea() * 4);
		return simg;
	}
	return 0;
}

UOSInt Media::GTKDrawImage::SavePng(NN<IO::SeekableStream> stm)
{
#if defined(NO_GDK)
	return 0;
#else
	gchar *buff = 0;
	gsize buffSize;
	if (this->surface)
	{
		cairo_surface_flush((cairo_surface_t*)this->surface);
	}
	GdkPixbuf *pixbuf;
	pixbuf = gdk_pixbuf_get_from_surface((cairo_surface_t*)this->surface, 0, 0, (gint)this->info.dispSize.x, (gint)this->info.dispSize.y);
	if (pixbuf == 0)
	{
		return 0;
	}
	gdk_pixbuf_save_to_buffer(pixbuf, &buff, &buffSize, "png", 0, (void*)0);
	if (buff)
	{
		buffSize = stm->Write((UInt8*)buff, buffSize);
		g_free(buff);
		g_object_unref(pixbuf);
		return buffSize;
	}
	else
	{
		g_object_unref(pixbuf);
		return 0;
	}
#endif
}

UOSInt Media::GTKDrawImage::SaveGIF(NN<IO::SeekableStream> stm)
{
	return 0;
}

UOSInt Media::GTKDrawImage::SaveJPG(NN<IO::SeekableStream> stm)
{
#if defined(NO_GDK)
	return 0;
#else
	gchar *buff = 0;
	gsize buffSize;
	if (this->surface)
	{
		cairo_surface_flush((cairo_surface_t*)this->surface);
	}
	if (this->GetBitCount() == 32)
	{
		Bool revOrder;
		UInt8 *imgPtr = this->GetImgBits(revOrder);
		ImageUtil_ImageFillAlpha32(imgPtr, this->GetWidth(), this->GetHeight(), this->GetDataBpl(), 0xff);
		this->GetImgBitsEnd(true);
	}
	GdkPixbuf *pixbuf;
	pixbuf = gdk_pixbuf_get_from_surface((cairo_surface_t*)this->surface, 0, 0, (gint)this->info.dispSize.x, (gint)this->info.dispSize.y);
	if (pixbuf == 0)
	{
		return 0;
	}
	gdk_pixbuf_save_to_buffer(pixbuf, &buff, &buffSize, "jpeg", 0, (void*)0);
	if (buff)
	{
		Media::JPEGFile::WriteJPGBuffer(stm, (const UInt8*)buff, buffSize, this);
		g_free(buff);
		g_object_unref(pixbuf);
		return buffSize;
	}
	else
	{
		g_object_unref(pixbuf);
		return 0;
	}
#endif
}

NN<Media::RasterImage> Media::GTKDrawImage::Clone() const
{
	NN<Media::GTKDrawImage> dimg;
	NEW_CLASSNN(dimg, Media::GTKDrawImage(this->eng, this->surface, this->cr, this->tl, this->info.dispSize, this->info.storeBPP, this->info.atype));
	return dimg;
}

Media::RasterImage::ImageType Media::GTKDrawImage::GetImageType() const
{
	return Media::RasterImage::ImageType::GUIImage;
}

void Media::GTKDrawImage::GetRasterData(UInt8 *destBuff, OSInt left, OSInt top, UOSInt width, UOSInt height, UOSInt destBpl, Bool upsideDown, Media::RotateType destRotate) const
{
	this->CopyBits(left, top, destBuff, destBpl, width, height, upsideDown);
}

Int32 Media::GTKDrawImage::GetPixel32(OSInt x, OSInt y) const
{
	return 0;
}

void *Media::GTKDrawImage::GetSurface() const
{
	return this->surface;
}

void *Media::GTKDrawImage::GetCairo() const
{
	return this->cr;
}

