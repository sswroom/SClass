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

Media::GTKDrawEngine::GTKDrawEngine()
{
	NEW_CLASS(this->iab, Media::ABlend::AlphaBlend8_C8(0, true));
}

Media::GTKDrawEngine::~GTKDrawEngine()
{
	DEL_CLASS(this->iab);
}

Media::DrawImage *Media::GTKDrawEngine::CreateImage32(UOSInt width, UOSInt height, Media::AlphaType atype)
{
	Media::GTKDrawImage *dimg;
	cairo_surface_t *surface;
	cairo_t *cr;
	surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, (int)width, (int)height);
	cr = cairo_create(surface);
	if (atype == Media::AT_ALPHA)
	{
		atype = Media::AT_PREMUL_ALPHA;
	}
	NEW_CLASS(dimg, Media::GTKDrawImage(this, surface, cr, 0, 0, width, height, 32, atype));
	return dimg;
}

Media::DrawImage *Media::GTKDrawEngine::CreateImageScn(void *cr, OSInt left, OSInt top, OSInt right, OSInt bottom)
{
	Media::GTKDrawImage *dimg;
	NEW_CLASS(dimg, Media::GTKDrawImage(this, 0, cr, left, top, (UOSInt)(right - left), (UOSInt)(bottom - top), 32, Media::AT_NO_ALPHA));
	return dimg;
}

Media::DrawImage *Media::GTKDrawEngine::LoadImage(Text::CString fileName)
{
	Media::ImageList *imgList = 0;
	IO::StmData::FileData *fd;

	NEW_CLASS(fd, IO::StmData::FileData(fileName, false));
	if (imgList == 0)
	{
		Parser::FileParser::GUIImgParser parser;
		imgList = (Media::ImageList*)parser.ParseFile(fd, 0, IO::ParserType::ImageList);
	}
	DEL_CLASS(fd);

	if (imgList == 0)
	{
		return 0;
	}

	Media::Image *img = imgList->GetImage(0, 0);
	Media::DrawImage *dimg = 0;
	if (img)
	{
		dimg = this->ConvImage(img);
	}
	DEL_CLASS(imgList);
	return dimg;
}

Media::DrawImage *Media::GTKDrawEngine::LoadImageStream(IO::SeekableStream *stm)
{
	return 0;
}

Media::DrawImage *Media::GTKDrawEngine::ConvImage(Media::Image *img)
{
	if (img == 0)
	{
		return 0;
	}
	if (img->info->fourcc != 0)
	{
		return 0; 
	}
	Media::GTKDrawImage *gimg = (Media::GTKDrawImage*)this->CreateImage32(img->info->dispWidth, img->info->dispHeight, img->info->atype);
	if (gimg == 0)
		return 0;
	gimg->SetHDPI(img->info->hdpi);
	gimg->SetVDPI(img->info->vdpi);
	gimg->info->color->Set(img->info->color);
	if (img->GetImageType() == Media::Image::IT_STATIC)
	{
		Media::StaticImage *simg = (Media::StaticImage*)img;
		if (simg->To32bpp())
		{
			cairo_surface_flush((cairo_surface_t*)gimg->GetSurface());
			UInt8 *sptr = (UInt8*)simg->data;
			UInt8 *dptr = cairo_image_surface_get_data((cairo_surface_t*)gimg->GetSurface());
			OSInt sbpl = (OSInt)simg->info->storeWidth << 2;
			OSInt dbpl = cairo_image_surface_get_stride((cairo_surface_t*)gimg->GetSurface());
			if (simg->info->atype == Media::AT_ALPHA)
			{
				this->iab->PremulAlpha(dptr, dbpl, sptr, sbpl, simg->info->dispWidth, simg->info->dispHeight);
			}
			else
			{
				ImageCopy_ImgCopy(sptr, dptr, simg->info->dispWidth << 2, simg->info->dispHeight, sbpl, dbpl);
			}
			cairo_surface_mark_dirty((cairo_surface_t*)gimg->GetSurface());
		}
	}
	else
	{
		Media::StaticImage *simg = img->CreateStaticImage();
		if (simg->To32bpp())
		{
			cairo_surface_flush((cairo_surface_t*)gimg->GetSurface());
			UInt8 *sptr = (UInt8*)simg->data;
			UInt8 *dptr = cairo_image_surface_get_data((cairo_surface_t*)gimg->GetSurface());
			OSInt sbpl = (OSInt)simg->info->storeWidth << 2;
			OSInt dbpl = cairo_image_surface_get_stride((cairo_surface_t*)gimg->GetSurface());
			if (simg->info->atype == Media::AT_ALPHA)
			{
				this->iab->PremulAlpha(dptr, dbpl, sptr, sbpl, simg->info->dispWidth, simg->info->dispHeight);
			}
			else
			{
				ImageCopy_ImgCopy(sptr, dptr, simg->info->dispWidth << 2, simg->info->dispHeight, sbpl, dbpl);
			}
			cairo_surface_mark_dirty((cairo_surface_t*)gimg->GetSurface());
		}
		DEL_CLASS(simg);
	}
	return gimg;
}

Media::DrawImage *Media::GTKDrawEngine::CloneImage(DrawImage *img)
{
	Media::GTKDrawImage *dimg = (Media::GTKDrawImage*)img;
	UOSInt width = dimg->GetWidth();
	UOSInt height = dimg->GetHeight();
	Media::AlphaType atype = dimg->GetAlphaType();
	Media::GTKDrawImage *newImg = 0;
	if (dimg->GetSurface())
	{
		Bool upsideDown;
		UInt8 *dptr;
		newImg = (GTKDrawImage*)this->CreateImage32(width, height, atype);
		dptr = newImg->GetImgBits(&upsideDown);
		((Media::GTKDrawImage*)img)->CopyBits(0, 0, dptr, newImg->GetDataBpl(), width, height, upsideDown);
	}
	return newImg;
}

Bool Media::GTKDrawEngine::DeleteImage(DrawImage *img)
{
	Media::GTKDrawImage *dimg = (Media::GTKDrawImage*)img;
	DEL_CLASS(dimg);
	return true;
}

Media::GTKDrawFont::GTKDrawFont(Text::CString fontName, Double fontHeight, Media::DrawEngine::DrawFontStyle drawFontStyle)
{
	if (fontName.leng == 0)
	{
		this->fontName = Text::String::New(UTF8STRC("sans-serif"));
	}
	else
	{
		this->fontName = Text::String::New(fontName.v, fontName.leng);
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

Media::GTKDrawFont::~GTKDrawFont()
{
	this->fontName->Release();
}

void Media::GTKDrawFont::Init(void *cr)
{
	cairo_select_font_face((cairo_t*)cr, (const Char*)this->fontName->v, (cairo_font_slant_t)this->fontSlant, (cairo_font_weight_t)this->fontWeight);
	cairo_set_font_size((cairo_t*)cr, this->fontHeight);
}

Text::String *Media::GTKDrawFont::GetFontName()
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

Media::GTKDrawImage::GTKDrawImage(GTKDrawEngine *eng, void *surface, void *cr, OSInt left, OSInt top, UOSInt width, UOSInt height, UInt32 bitCount, Media::AlphaType atype) : Media::Image(width, height, 0, 0, 0, bitCount, Media::PixelFormatGetDef(0, bitCount), 0, 0, Media::ColorProfile::YUVT_BT601, atype, Media::YCOFST_C_CENTER_LEFT)
{
	this->eng = eng;
	this->surface = surface;
	this->cr = cr;
	this->left = left;
	this->top = top;
	this->info->hdpi = 96.0;
	this->info->vdpi = 96.0;
}

Media::GTKDrawImage::~GTKDrawImage()
{
	if (this->surface)
	{
		cairo_surface_destroy((cairo_surface_t*)this->surface);
		cairo_destroy((cairo_t*)this->cr);
	}
}

UOSInt Media::GTKDrawImage::GetWidth()
{
	return this->info->dispWidth;
}

UOSInt Media::GTKDrawImage::GetHeight()
{
	return this->info->dispHeight;
}

UInt32 Media::GTKDrawImage::GetBitCount()
{
	return this->info->storeBPP;
}

Media::ColorProfile *Media::GTKDrawImage::GetColorProfile()
{
	return this->info->color;
}

void Media::GTKDrawImage::SetColorProfile(const ColorProfile *color)
{
	this->info->color->Set(color);
}

Media::AlphaType Media::GTKDrawImage::GetAlphaType()
{
	return this->info->atype;
}

void Media::GTKDrawImage::SetAlphaType(Media::AlphaType atype)
{
	this->info->atype = atype;
}

Double Media::GTKDrawImage::GetHDPI()
{
	return this->info->hdpi;
}

Double Media::GTKDrawImage::GetVDPI()
{
	return this->info->vdpi;
}

void Media::GTKDrawImage::SetHDPI(Double dpi)
{
	this->info->hdpi = dpi;
}

void Media::GTKDrawImage::SetVDPI(Double dpi)
{
	this->info->vdpi = dpi;
}

UInt8 *Media::GTKDrawImage::GetImgBits(Bool *revOrder)
{
	cairo_surface_flush((cairo_surface_t*)this->surface);
	if (revOrder) *revOrder = false;
	return cairo_image_surface_get_data((cairo_surface_t*)this->surface);
}

void Media::GTKDrawImage::GetImgBitsEnd(Bool modified)
{
	if (modified)
	{
		cairo_surface_mark_dirty((cairo_surface_t*)this->surface);
	}
}

UOSInt Media::GTKDrawImage::GetImgBpl()
{
	return (this->info->storeWidth * this->info->storeBPP) >> 3;
}

Media::EXIFData *Media::GTKDrawImage::GetEXIF()
{
	return this->exif;
}

Media::PixelFormat Media::GTKDrawImage::GetPixelFormat()
{
	return this->info->pf;
}

Bool Media::GTKDrawImage::DrawLine(Double x1, Double y1, Double x2, Double y2, DrawPen *p)
{
	GTKDrawPen *pen = (GTKDrawPen*)p;
	pen->Init(this->cr);
	cairo_move_to((cairo_t*)this->cr, x1 + OSInt2Double(this->left), y1 + OSInt2Double(this->top));
	cairo_line_to((cairo_t*)this->cr, x2 + OSInt2Double(this->left), y2 + OSInt2Double(this->top));
	cairo_stroke((cairo_t*)this->cr);
	return true;
}

Bool Media::GTKDrawImage::DrawPolylineI(Int32 *points, UOSInt nPoints, DrawPen *p)
{
	printf("GTK: Draw PolylineI (Not support)\r\n");
	return false;
}

Bool Media::GTKDrawImage::DrawPolygonI(Int32 *points, UOSInt nPoints, DrawPen *p, DrawBrush *b)
{
	printf("GTK: Draw PolygonI (Not support)\r\n");
	return false;
}

Bool Media::GTKDrawImage::DrawPolyPolygonI(Int32 *points, UInt32 *pointCnt, UOSInt nPointCnt, DrawPen *p, DrawBrush *b)
{
	printf("GTK: Draw PolyPolygonI (Not support)\r\n");
	return false;
}

Bool Media::GTKDrawImage::DrawPolyline(Double *points, UOSInt nPoints, DrawPen *p)
{
	GTKDrawPen *pen = (GTKDrawPen*)p;
	if (nPoints >= 2)
	{
		pen->Init(this->cr);
		cairo_move_to((cairo_t*)this->cr, points[0] + OSInt2Double(this->left), points[1] + OSInt2Double(this->top));
		points += 2;
		while (nPoints-- > 1)
		{
			cairo_line_to((cairo_t*)this->cr, points[0] + OSInt2Double(this->left), points[1] + OSInt2Double(this->top));
			points += 2;
		}
		cairo_stroke((cairo_t*)this->cr);

		return true;
	}
	else
	{
		return false;
	}
}

Bool Media::GTKDrawImage::DrawPolygon(Double *points, UOSInt nPoints, DrawPen *p, DrawBrush *b)
{
	GTKDrawPen *pen = (GTKDrawPen*)p;
	GTKDrawBrush *brush = (GTKDrawBrush*)b;
	if (nPoints >= 2)
	{
		cairo_move_to((cairo_t*)this->cr, points[0] + OSInt2Double(this->left), points[1] + OSInt2Double(this->top));
		points += 2;
		while (nPoints-- > 1)
		{
			cairo_line_to((cairo_t*)this->cr, points[0] + OSInt2Double(this->left), points[1] + OSInt2Double(this->top));
			points += 2;
		}
		cairo_close_path((cairo_t*)this->cr);
		if (pen && brush)
		{
			pen->Init(this->cr);
			cairo_stroke_preserve((cairo_t*)this->cr);
			brush->Init(this->cr);
			cairo_fill((cairo_t*)this->cr);
		}
		else if (pen)
		{
			pen->Init(this->cr);
			cairo_stroke((cairo_t*)this->cr);
		}
		else if (brush)
		{
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

Bool Media::GTKDrawImage::DrawPolyPolygon(Double *points, UInt32 *pointCnt, UOSInt nPointCnt, DrawPen *p, DrawBrush *b)
{
	GTKDrawPen *pen = (GTKDrawPen*)p;
	GTKDrawBrush *brush = (GTKDrawBrush*)b;
	UOSInt i;
	if (nPointCnt > 0)
	{
		while (nPointCnt-- > 0)
		{
			i = *pointCnt++;
			if (i > 0)
			{
				cairo_move_to((cairo_t*)this->cr, points[0] + OSInt2Double(this->left), points[1] + OSInt2Double(this->top));
				points += 2;
				while (i-- > 1)
				{
					cairo_line_to((cairo_t*)this->cr, points[0] + OSInt2Double(this->left), points[1] + OSInt2Double(this->top));
					points += 2;
				}
				cairo_close_path((cairo_t*)this->cr);
			}
		}

		if (pen && brush)
		{
			pen->Init(this->cr);
			cairo_stroke_preserve((cairo_t*)this->cr);
			brush->Init(this->cr);
			cairo_fill((cairo_t*)this->cr);
		}
		else if (pen)
		{
			pen->Init(this->cr);
			cairo_stroke((cairo_t*)this->cr);
		}
		else if (brush)
		{
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

Bool Media::GTKDrawImage::DrawRect(Double x, Double y, Double w, Double h, DrawPen *p, DrawBrush *b)
{
	if (b)
	{
		GTKDrawBrush *brush = (GTKDrawBrush*)b;
		brush->Init(this->cr);
		cairo_rectangle((cairo_t*)this->cr, x + OSInt2Double(this->left), y + OSInt2Double(this->top), w, h);
		cairo_fill((cairo_t*)this->cr);
	}
	if (p)
	{
		GTKDrawPen *pen = (GTKDrawPen*)p;
		pen->Init(this->cr);
		cairo_rectangle((cairo_t*)this->cr, x + OSInt2Double(this->left), y + OSInt2Double(this->top), w, h);
		cairo_stroke((cairo_t*)this->cr);
	}
	return true;
}

Bool Media::GTKDrawImage::DrawEllipse(Double tlx, Double tly, Double w, Double h, DrawPen *p, DrawBrush *b)
{
	GTKDrawPen *pen = (GTKDrawPen*)p;
	GTKDrawBrush *brush = (GTKDrawBrush*)b;
	cairo_t *cr = (cairo_t *)this->cr;
	cairo_save(cr);
	cairo_translate(cr, tlx + w * 0.5, tly + h * 0.5);
	cairo_scale(cr, 1, h / w);
	cairo_translate(cr, -tlx - w * 0.5, -tly - h * 0.5);
	cairo_arc(cr, tlx + w * 0.5, tly + h * 0.5, w, 0, 2 * Math::PI);
	cairo_close_path(cr);
	if (pen && brush)
	{
		pen->Init(this->cr);
		cairo_stroke_preserve((cairo_t*)this->cr);
		brush->Init(this->cr);
		cairo_fill((cairo_t*)this->cr);
	}
	else if (pen)
	{
		pen->Init(this->cr);
		cairo_stroke((cairo_t*)this->cr);
	}
	else if (brush)
	{
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

Bool Media::GTKDrawImage::DrawString(Double tlx, Double tly, Text::String *str, DrawFont *f, DrawBrush *b)
{
	return DrawString(tlx, tly, str->ToCString(), f, b);
}

Bool Media::GTKDrawImage::DrawString(Double tlx, Double tly, Text::CString str, DrawFont *f, DrawBrush *b)
{
	GTKDrawFont *font = (GTKDrawFont*)f;
	GTKDrawBrush *brush = (GTKDrawBrush*)b;
	font->Init(this->cr);
	brush->Init(this->cr);
	cairo_move_to((cairo_t *)this->cr, tlx + OSInt2Double(this->left), tly + font->GetHeight() * 0.8 + 1 + OSInt2Double(this->top));
	cairo_show_text((cairo_t *)this->cr, (const Char*)str.v);
	return true;
}

Bool Media::GTKDrawImage::DrawStringRot(Double centX, Double centY, Text::String *str, DrawFont *f, DrawBrush *b, Double angleDegree)
{
	return DrawStringRot(centX, centY, str->ToCString(), f, b, angleDegree);
}

Bool Media::GTKDrawImage::DrawStringRot(Double centX, Double centY, Text::CString str, DrawFont *f, DrawBrush *b, Double angleDegree)
{
	GTKDrawFont *font = (GTKDrawFont*)f;
	GTKDrawBrush *brush = (GTKDrawBrush*)b;
	Double angleR = angleDegree * Math::PI / 180;
	cairo_text_extents_t extents;
	font->Init(this->cr);
	brush->Init(this->cr);
	Double cVal = Math_Cos(angleR);
	Double sVal = Math_Sin(angleR);
	Double dist = font->GetHeight() * 0.8 + 1;
	cairo_text_extents((cairo_t *)this->cr, (const Char*)str.v, &extents);
	cairo_move_to((cairo_t *)this->cr, centX - dist * sVal + OSInt2Double(this->left), centY + dist * cVal + OSInt2Double(this->top));
	cairo_rotate((cairo_t *)this->cr, angleR);
	cairo_show_text((cairo_t *)this->cr, (const Char*)str.v);
	cairo_rotate((cairo_t *)this->cr, -angleR);
	return true;
}

Bool Media::GTKDrawImage::DrawStringB(Double tlx, Double tly, Text::String *str, DrawFont *f, DrawBrush *b, UOSInt buffSize)
{
	return DrawStringB(tlx, tly, str->ToCString(), f, b, buffSize);
}

Bool Media::GTKDrawImage::DrawStringB(Double tlx, Double tly, Text::CString str, DrawFont *f, DrawBrush *b, UOSInt buffSize)
{
	OSInt px = Double2Int32(tlx);
	OSInt py = Double2Int32(tly);
	GTKDrawFont *font = (GTKDrawFont*)f;
	GTKDrawBrush *brush = (GTKDrawBrush*)b;

	UInt32 sz[2];
	Media::GTKDrawImage *gimg;
	UOSInt swidth;
	UOSInt sheight;
	OSInt sx;
	OSInt sy;
	OSInt dwidth;
	OSInt dheight;

	cairo_text_extents_t extents;
	font->Init(this->cr);
	cairo_text_extents((cairo_t *)this->cr, (const Char*)str.v, &extents);
	sz[0] = (UInt32)Double2Int32(extents.width + 2);
	sz[1] = (UInt32)Double2Int32(extents.height + 2);
	dwidth = (OSInt)this->info->dispWidth - px;
	dheight = (OSInt)this->info->dispHeight - py;

	if (dwidth < 0)
	{
	}
	else if (dheight < 0)
	{
	}
	else
	{
		gimg = (Media::GTKDrawImage*)eng->CreateImage32(swidth = sz[0] + (buffSize << 1), sheight = sz[1] + (buffSize << 1), Media::AT_ALPHA);
		if (gimg == 0)
		{
			return false;
		}

		if (px < (OSInt)buffSize)
		{
			sx = -px + (OSInt)buffSize;
			swidth += (UOSInt)px;
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
			sheight = (UOSInt)((OSInt)sheight + py);
			py = 0;
		}
		else
		{
			sy = 0;
			py -= (OSInt)buffSize;
		}
		if ((OSInt)(gimg->GetHeight() - sheight) < sy)
		{
			sheight = gimg->GetHeight() - (UOSInt)sy;
		}
		if ((OSInt)(gimg->GetWidth() - swidth) < sx)
		{
			swidth = gimg->GetWidth() - (UOSInt)sx;
		}
		if ((UOSInt)dwidth + buffSize < swidth)
		{
			swidth = (UOSInt)dwidth + buffSize;
		}
		if ((UOSInt)dheight + buffSize < sheight)
		{
			sheight = (UOSInt)dheight + buffSize;
		}
		if (swidth <= 0 || sheight <= 0 || sz[0] <= 0 || sz[1] <= 0)
		{

		}
		else
		{
			Media::DrawBrush *whiteB = gimg->NewBrushARGB(0xffffffff);
			//gimg->SetTextAlign(this->strAlign);
			gimg->DrawString(OSInt2Double(sx) + tlx - OSInt2Double(px),
				OSInt2Double(sy) + tly - OSInt2Double(py), str, f, whiteB);

			UOSInt bpl = (sz[0] + (buffSize << 1)) << 2;
			UOSInt dbpl = this->info->dispWidth << 2;
			UInt32 color = brush->GetOriColor();
			cairo_surface_flush((cairo_surface_t*)gimg->surface);
			UInt8 *pbits = cairo_image_surface_get_data((cairo_surface_t*)gimg->surface);
			UInt8 *dbits = cairo_image_surface_get_data((cairo_surface_t*)this->surface);
			ImageUtil_ImageColorBuffer32(pbits + bpl * buffSize + buffSize * 4, sz[0], sz[1], bpl, buffSize);
			if (py + (OSInt)sheight > (OSInt)this->info->dispHeight)
			{
				sheight = this->info->dispHeight - (UOSInt)py;
			}
			if (px + (OSInt)swidth > (OSInt)this->info->dispWidth)
			{
				swidth = this->info->dispWidth - (UOSInt)px;
			}
			if ((OSInt)swidth > 0 && (OSInt)sheight > 0)
			{
				if (dbits)
				{
					pbits = ((UInt8*)pbits) + (sy * (OSInt)bpl) + (sx << 2);
					dbits = ((UInt8*)dbits) + (py * (OSInt)dbpl) + (px << 2);

					if ((color & 0xff000000) == 0xff000000)
					{
						ImageUtil_ImageColorReplace32((UInt8*)pbits, (UInt8*)dbits, swidth, sheight, (OSInt)bpl, (OSInt)dbpl, color);
					}
					else
					{
						ImageUtil_ImageMaskABlend32((UInt8*)pbits, (UInt8*)dbits, swidth, sheight, (OSInt)bpl, (OSInt)dbpl, color);
					}
					cairo_surface_mark_dirty((cairo_surface_t*)this->surface);
				}
				else
				{
					pbits = ((UInt8*)pbits) + (sy * (OSInt)bpl) + (sx << 2);

					ImageUtil_ImageColorReplace32((UInt8*)pbits, (UInt8*)pbits, swidth, sheight, (OSInt)bpl, (OSInt)bpl, color);
					gimg->SetHDPI(this->GetHDPI());
					gimg->SetVDPI(this->GetVDPI());
					this->DrawImagePt(gimg, OSInt2Double(px - sx), OSInt2Double(py - sy));
				}
			}
		}
		eng->DeleteImage(gimg);
	}

	return true;
}

Bool Media::GTKDrawImage::DrawStringRotB(Double centX, Double centY, Text::String *str, DrawFont *f, DrawBrush *b, Double angleDegree, UOSInt buffSize)
{
	return DrawStringRotB(centX, centY, str->ToCString(), f, b, angleDegree, buffSize);
}

Bool Media::GTKDrawImage::DrawStringRotB(Double centX, Double centY, Text::CString str, DrawFont *f, DrawBrush *b, Double angleDegree, UOSInt buffSize)
{
	printf("GTK: Draw StringRotBUTF8 (Not support)\r\n");
	return false;
}

Bool Media::GTKDrawImage::DrawImagePt(DrawImage *img, Double tlx, Double tly)
{
	Media::GTKDrawImage *gimg = (GTKDrawImage*)img;
	if (gimg->surface == 0)
	{
		return false;
	}
	if (this->surface == 0)
	{
		cairo_surface_flush((cairo_surface_t*)gimg->surface);
		if (gimg->info->atype == Media::AT_NO_ALPHA)
		{
			Bool revOrder;
			ImageUtil_ImageFillAlpha32(gimg->GetImgBits(&revOrder), gimg->GetWidth(), gimg->GetHeight(), gimg->GetImgBpl(), 0xFF);
			gimg->GetImgBitsEnd(true);
		}
		cairo_save((cairo_t*)this->cr);
		cairo_translate((cairo_t*)this->cr, tlx + OSInt2Double(this->left), tly + OSInt2Double(this->top));
		cairo_scale((cairo_t*)this->cr, 1, 1);
		cairo_set_source_surface((cairo_t*)this->cr, (cairo_surface_t*)gimg->surface, 0, 0);
		cairo_paint((cairo_t*)this->cr);
		cairo_restore((cairo_t*)this->cr);
		return true;
	}
	else
	{
		if (gimg->info->storeBPP != 32 || this->info->storeBPP != 32)
		{
			cairo_save((cairo_t*)this->cr);
			cairo_set_source_surface((cairo_t*)this->cr, (cairo_surface_t*)gimg->surface, 0, 0);
			cairo_translate((cairo_t*)this->cr, tlx + OSInt2Double(this->left), tly + OSInt2Double(this->top));
			cairo_scale((cairo_t*)this->cr, 1, 1);
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

		OSInt ixPos = Double2Int32(tlx);
		OSInt iyPos = Double2Int32(tly);
		OSInt ixPos2 = ixPos + (OSInt)gimg->info->dispWidth;
		OSInt iyPos2 = iyPos + (OSInt)gimg->info->dispHeight;
		if ((OSInt)this->info->dispWidth < ixPos2)
		{
			ixPos2 = (OSInt)this->info->dispWidth;
		}
		if (ixPos < 0)
		{
			simgPtr += -ixPos * 4;
			ixPos = 0;
		}

		if ((OSInt)this->info->dispHeight < iyPos2)
		{
			iyPos2 = (OSInt)this->info->dispHeight;
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

		if (gimg->info->atype == Media::AT_NO_ALPHA)
		{
			ImageCopy_ImgCopy(simgPtr, dimgPtr + ixPos * 4 + iyPos * dbpl, (UOSInt)(ixPos2 - ixPos) * 4, (UOSInt)(iyPos2 - iyPos), sbpl, dbpl);
			cairo_surface_mark_dirty((cairo_surface_t*)this->surface);
			return true;
		}
		else
		{
			this->eng->iab->SetSourceProfile(gimg->info->color);
			this->eng->iab->SetDestProfile(this->info->color);
			this->eng->iab->SetOutputProfile(this->info->color);
			this->eng->iab->Blend(dimgPtr + ixPos * 4 + iyPos * dbpl, dbpl, simgPtr, sbpl, (UOSInt)(ixPos2 - ixPos), (UOSInt)(iyPos2 - iyPos), gimg->info->atype);
			cairo_surface_mark_dirty((cairo_surface_t*)this->surface);
			return true;
		}
		return false;
	}
}

Bool Media::GTKDrawImage::DrawImagePt2(Media::StaticImage *img, Double tlx, Double tly)
{
	if (this->surface == 0)
	{
		return false;
	}
	img->To32bpp();
	if (img->info->storeBPP != 32)
	{
		return false;
	}
	cairo_surface_flush((cairo_surface_t*)this->surface);
	UInt8 *dimgPtr = cairo_image_surface_get_data((cairo_surface_t*)this->surface);
	OSInt dbpl = cairo_image_surface_get_stride((cairo_surface_t*)this->surface);
	UInt8 *simgPtr = img->data;
	OSInt sbpl = (OSInt)img->info->storeWidth * 4;

	Int32 ixPos = Double2Int32(tlx);
	Int32 iyPos = Double2Int32(tly);
	Int32 ixPos2 = ixPos + (Int32)img->info->dispWidth;
	Int32 iyPos2 = iyPos + (Int32)img->info->dispHeight;
	if ((OSInt)this->info->dispWidth < ixPos2)
	{
		ixPos2 = (Int32)this->info->dispWidth;
	}
	if (ixPos < 0)
	{
		simgPtr += -ixPos * 4;
		ixPos = 0;
	}

	if ((OSInt)this->info->dispHeight < iyPos2)
	{
		iyPos2 = (Int32)this->info->dispHeight;
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

	if (img->info->atype == Media::AT_NO_ALPHA)
	{
		ImageCopy_ImgCopy(simgPtr, dimgPtr + ixPos * 4 + iyPos * dbpl, (UOSInt)(ixPos2 - ixPos) * 4, (UOSInt)(iyPos2 - iyPos), sbpl, dbpl);
		cairo_surface_mark_dirty((cairo_surface_t*)this->surface);
		return true;
	}
	else //////////////////////////////////////////
	{
		this->eng->iab->SetSourceProfile(img->info->color);
		this->eng->iab->SetDestProfile(this->info->color);
		this->eng->iab->SetOutputProfile(this->info->color);
		this->eng->iab->Blend(dimgPtr + ixPos * 4 + iyPos * dbpl, dbpl, simgPtr, sbpl, (UOSInt)(ixPos2 - ixPos), (UOSInt)(iyPos2 - iyPos), img->info->atype);
		cairo_surface_mark_dirty((cairo_surface_t*)this->surface);
		return true;
	}
	return false;
}

Bool Media::GTKDrawImage::DrawImagePt3(DrawImage *img, Double destX, Double destY, Double srcX, Double srcY, Double srcW, Double srcH)
{
	return false;
}

Media::DrawPen *Media::GTKDrawImage::NewPenARGB(UInt32 color, Double thick, UInt8 *pattern, UOSInt nPattern)
{
	Media::GTKDrawPen *p;
	NEW_CLASS(p, Media::GTKDrawPen(color, thick));
	return p;
}

Media::DrawBrush *Media::GTKDrawImage::NewBrushARGB(UInt32 color)
{
	Media::GTKDrawBrush *b;
	NEW_CLASS(b, Media::GTKDrawBrush(color));
	return b;
}

Media::DrawFont *Media::GTKDrawImage::NewFontPt(Text::CString name, Double ptSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage)
{
	Media::GTKDrawFont *f;
	NEW_CLASS(f, Media::GTKDrawFont(name, ptSize * this->info->hdpi / 72.0, fontStyle));
	return f;
}

Media::DrawFont *Media::GTKDrawImage::NewFontPx(Text::CString name, Double pxSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage)
{
	Media::GTKDrawFont *f;
	NEW_CLASS(f, Media::GTKDrawFont(name, pxSize, fontStyle));
	return f;
}

Media::DrawFont *Media::GTKDrawImage::CloneFont(DrawFont *f)
{
	Media::GTKDrawFont *fnt = (Media::GTKDrawFont*)f;
	Text::String *fname = fnt->GetFontName();
	Double height = fnt->GetHeight();
	OSInt fontWeight = fnt->GetFontWeight();
	OSInt fontSlant = fnt->GetFontSlant();
	NEW_CLASS(fnt, Media::GTKDrawFont(fname, height, fontWeight, fontSlant));
	return fnt;
}

void Media::GTKDrawImage::DelPen(DrawPen *p)
{
	GTKDrawPen *pen = (GTKDrawPen*)p;
	DEL_CLASS(pen);
}

void Media::GTKDrawImage::DelBrush(DrawBrush *b)
{
	GTKDrawBrush *brush = (GTKDrawBrush*)b;
	DEL_CLASS(brush);
}

void Media::GTKDrawImage::DelFont(DrawFont *f)
{
	GTKDrawFont *font = (GTKDrawFont*)f;
	DEL_CLASS(font);
}

Bool Media::GTKDrawImage::GetTextSize(DrawFont *fnt, Text::CString txt, Double *sz)
{
	GTKDrawFont *font = (GTKDrawFont*)fnt;
	cairo_text_extents_t extents;
	font->Init(this->cr);
	cairo_text_extents((cairo_t *)this->cr, (const Char*)txt.v, &extents);
	sz[0] = extents.width + 2;
	sz[1] = font->GetHeight() + 2;
	return true;
}

void Media::GTKDrawImage::SetTextAlign(DrawEngine::DrawPos pos)
{
}

void Media::GTKDrawImage::GetStringBound(Int32 *pos, OSInt centX, OSInt centY, const UTF8Char *str, DrawFont *f, OSInt *drawX, OSInt *drawY)
{
}

void Media::GTKDrawImage::GetStringBoundRot(Int32 *pos, Double centX, Double centY, const UTF8Char *str, DrawFont *f, Double angleDegree, OSInt *drawX, OSInt *drawY)
{
}

void Media::GTKDrawImage::CopyBits(OSInt x, OSInt y, void *imgPtr, UOSInt bpl, UOSInt width, UOSInt height, Bool upsideDown)
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
			ImageCopy_ImgCopyR(srcData + x * 4 + y * (OSInt)this->info->storeWidth * 4, (UInt8*)imgPtr, width * 4, height, this->info->storeWidth * 4, bpl, upsideDown);
		}
	}
}

Media::StaticImage *Media::GTKDrawImage::ToStaticImage()
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
		MemCopyNO(simg->data, srcData, this->info->storeWidth * this->info->storeHeight * 4);
		return simg;
	}
	return 0;
}

UOSInt Media::GTKDrawImage::SavePng(IO::SeekableStream *stm)
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
	pixbuf = gdk_pixbuf_get_from_surface((cairo_surface_t*)this->surface, 0, 0, (gint)this->info->dispWidth, (gint)this->info->dispHeight);
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

UOSInt Media::GTKDrawImage::SaveGIF(IO::SeekableStream *stm)
{
	return 0;
}

UOSInt Media::GTKDrawImage::SaveJPG(IO::SeekableStream *stm)
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
		UInt8 *imgPtr = this->GetImgBits(&revOrder);
		ImageUtil_ImageFillAlpha32(imgPtr, this->GetWidth(), this->GetHeight(), this->GetDataBpl(), 0xff);
	}
	GdkPixbuf *pixbuf;
	pixbuf = gdk_pixbuf_get_from_surface((cairo_surface_t*)this->surface, 0, 0, (gint)this->info->dispWidth, (gint)this->info->dispHeight);
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

Media::Image *Media::GTKDrawImage::Clone()
{
	return 0;
}

Media::Image::ImageType Media::GTKDrawImage::GetImageType()
{
	return Media::Image::IT_GUIIMAGE;
}

void Media::GTKDrawImage::GetImageData(UInt8 *destBuff, OSInt left, OSInt top, UOSInt width, UOSInt height, UOSInt destBpl, Bool upsideDown)
{
	this->CopyBits(left, top, destBuff, destBpl, width, height, upsideDown);
}

Int32 Media::GTKDrawImage::GetPixel32(OSInt x, OSInt y)
{
	return 0;
}

void *Media::GTKDrawImage::GetSurface()
{
	return this->surface;
}

void *Media::GTKDrawImage::GetCairo()
{
	return this->cr;
}

