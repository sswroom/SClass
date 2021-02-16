#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/StmData/FileData.h"
#include "Math/Math.h"
#include "Media/GTKDrawEngine.h"
#include "Media/ImageCopyC.h"
#include "Media/ImageList.h"
#include "Media/ImageUtil.h"
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
	surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
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
	NEW_CLASS(dimg, Media::GTKDrawImage(this, 0, cr, left, top, right, bottom, 32, Media::AT_NO_ALPHA));
	return dimg;
}

Media::DrawImage *Media::GTKDrawEngine::LoadImage(const UTF8Char *fileName)
{
	Media::ImageList *imgList = 0;
	IO::StmData::FileData *fd;

	NEW_CLASS(fd, IO::StmData::FileData(fileName, false));
	if (imgList == 0)
	{
		Parser::FileParser::GUIImgParser parser;
		imgList = (Media::ImageList*)parser.ParseFile(fd, 0, IO::ParsedObject::PT_IMAGE_LIST_PARSER);
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
	if (img->GetImageType() == Media::Image::IT_STATIC)
	{
		Media::StaticImage *simg = (Media::StaticImage*)img;
		if (simg->To32bpp())
		{
			cairo_surface_flush((cairo_surface_t*)gimg->GetSurface());
			UInt8 *sptr = (UInt8*)simg->data;
			UInt8 *dptr = cairo_image_surface_get_data((cairo_surface_t*)gimg->GetSurface());
			OSInt sbpl = simg->info->storeWidth << 2;
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
			OSInt sbpl = simg->info->storeWidth << 2;
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
	OSInt width = dimg->GetWidth();
	OSInt height = dimg->GetHeight();
	Media::AlphaType atype = dimg->GetAlphaType();
	Media::GTKDrawImage *newImg = 0;
	if (dimg->GetSurface())
	{
		Bool revOrder;
		UInt8 *dptr;
		newImg = (GTKDrawImage*)this->CreateImage32(width, height, atype);
		dptr = newImg->GetImgBits(&revOrder);
		((Media::GTKDrawImage*)img)->CopyBits(0, 0, dptr, newImg->GetDataBpl(), width, height);
	}
	return newImg;
}

Bool Media::GTKDrawEngine::DeleteImage(DrawImage *img)
{
	Media::GTKDrawImage *dimg = (Media::GTKDrawImage*)img;
	DEL_CLASS(dimg);
	return true;
}

Media::GTKDrawFont::GTKDrawFont(const UTF8Char *fontName, Double fontHeight, Media::DrawEngine::DrawFontStyle drawFontStyle)
{
	if (fontName == 0)
	{
		fontName = (const UTF8Char*)"Arial";
	}
	this->fontName = Text::StrCopyNew(fontName);
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

Media::GTKDrawFont::GTKDrawFont(const UTF8Char *fontName, Double fontHeight, OSInt fontSlant, OSInt fontWeight)
{
	if (fontName == 0)
	{
		fontName = (const UTF8Char*)"Arial";
	}
	this->fontName = Text::StrCopyNew(fontName);
	this->fontHeight = fontHeight;
	this->fontWeight = fontWeight;
	this->fontSlant = fontSlant;
}

Media::GTKDrawFont::~GTKDrawFont()
{
	Text::StrDelNew(this->fontName);
}

void Media::GTKDrawFont::Init(void *cr)
{
	cairo_select_font_face((cairo_t*)cr, (const Char*)this->fontName, (cairo_font_slant_t)this->fontSlant, (cairo_font_weight_t)this->fontWeight);
	cairo_set_font_size((cairo_t*)cr, this->fontHeight);
}

const UTF8Char *Media::GTKDrawFont::GetFontName()
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

Media::GTKDrawPen::GTKDrawPen(Int32 oriColor, Double thick)
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

Media::GTKDrawBrush::GTKDrawBrush(Int32 oriColor)
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

Int32 Media::GTKDrawBrush::GetOriColor()
{
	return this->oriColor;
}

Media::GTKDrawImage::GTKDrawImage(GTKDrawEngine *eng, void *surface, void *cr, OSInt left, OSInt top, OSInt width, OSInt height, Int32 bitCount, Media::AlphaType atype) : Media::Image(width, height, 0, 0, 0, bitCount, Media::FrameInfo::GetDefPixelFormat(0, bitCount), 0, 0, Media::ColorProfile::YUVT_BT601, atype, Media::YCOFST_C_CENTER_LEFT)
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
	return cairo_image_surface_get_data((cairo_surface_t*)this->surface);
}

void Media::GTKDrawImage::GetImgBitsEnd(Bool modified)
{
	if (modified)
	{
		cairo_surface_mark_dirty((cairo_surface_t*)this->surface);
	}
}

OSInt Media::GTKDrawImage::GetImgBpl()
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
	cairo_move_to((cairo_t*)this->cr, x1 + this->left, y1 + this->top);
	cairo_line_to((cairo_t*)this->cr, x2 + this->left, y2 + this->top);
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
		cairo_move_to((cairo_t*)this->cr, points[0] + this->left, points[1] + this->top);
		points += 2;
		while (nPoints-- > 1)
		{
			cairo_line_to((cairo_t*)this->cr, points[0] + this->left, points[1] + this->top);
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
		cairo_move_to((cairo_t*)this->cr, points[0] + this->left, points[1] + this->top);
		points += 2;
		while (nPoints-- > 1)
		{
			cairo_line_to((cairo_t*)this->cr, points[0] + this->left, points[1] + this->top);
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
				cairo_move_to((cairo_t*)this->cr, points[0] + this->left, points[1] + this->top);
				points += 2;
				while (i-- > 1)
				{
					cairo_line_to((cairo_t*)this->cr, points[0] + this->left, points[1] + this->top);
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
		cairo_rectangle((cairo_t*)this->cr, x + this->left, y + this->top, w, h);
		cairo_fill((cairo_t*)this->cr);
	}
	if (p)
	{
		GTKDrawPen *pen = (GTKDrawPen*)p;
		pen->Init(this->cr);
		cairo_rectangle((cairo_t*)this->cr, x + this->left, y + this->top, w, h);
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

Bool Media::GTKDrawImage::DrawString(Double tlx, Double tly, const UTF8Char *str, DrawFont *f, DrawBrush *b)
{
//	wprintf(L"GTK: DrawStringUTF8, txt = %s, pos: (%lf, %lf)\r\n", str, tlx, tly);
	GTKDrawFont *font = (GTKDrawFont*)f;
	GTKDrawBrush *brush = (GTKDrawBrush*)b;
	font->Init(this->cr);
	brush->Init(this->cr);
	cairo_move_to((cairo_t *)this->cr, tlx + this->left, tly + font->GetHeight() * 0.8 + 1 + this->top);
	cairo_show_text((cairo_t *)this->cr, (const Char*)str);
	return true;
}

Bool Media::GTKDrawImage::DrawStringRot(Double centX, Double centY, const UTF8Char *str, DrawFont *f, DrawBrush *b, Double angleDegree)
{
//	wprintf(L"GTK: DrawStringRotUTF8, angle = %lf\r\n", angleDegree);
	GTKDrawFont *font = (GTKDrawFont*)f;
	GTKDrawBrush *brush = (GTKDrawBrush*)b;
	Double angleR = angleDegree * Math::PI / 180;
	cairo_text_extents_t extents;
	font->Init(this->cr);
	brush->Init(this->cr);
	Double cVal = Math::Cos(angleR);
	Double sVal = Math::Sin(angleR);
	Double dist = font->GetHeight() * 0.8 + 1;
	cairo_text_extents((cairo_t *)this->cr, (const Char*)str, &extents);
	cairo_move_to((cairo_t *)this->cr, centX - dist * sVal + this->left, centY + dist * cVal + this->top);
	cairo_rotate((cairo_t *)this->cr, angleR);
	cairo_show_text((cairo_t *)this->cr, (const Char*)str);
	cairo_rotate((cairo_t *)this->cr, -angleR);
	return true;
}

Bool Media::GTKDrawImage::DrawStringB(Double tlx, Double tly, const UTF8Char *str, DrawFont *f, DrawBrush *b, OSInt buffSize)
{
	OSInt px = Math::Double2Int32(tlx);
	OSInt py = Math::Double2Int32(tly);
	GTKDrawFont *font = (GTKDrawFont*)f;
	GTKDrawBrush *brush = (GTKDrawBrush*)b;

	Int32 sz[2];
	Media::GTKDrawImage *gimg;
	OSInt swidth;
	OSInt sheight;
	OSInt sx;
	OSInt sy;
	OSInt dwidth;
	OSInt dheight;

	cairo_text_extents_t extents;
	font->Init(this->cr);
	cairo_text_extents((cairo_t *)this->cr, (const Char*)str, &extents);
	sz[0] = extents.width;
	sz[1] = extents.height;
	dwidth = this->info->dispWidth - px;
	dheight = this->info->dispHeight - py;

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

		if (px < buffSize)
		{
			sx = -px + buffSize;
			swidth += px;
			px = 0;
		}
		else
		{
			sx = 0;
			px -= buffSize;
		}
		if (py < buffSize)
		{
			sy = -py + buffSize;
			sheight += py;
			py = 0;
		}
		else
		{
			sy = 0;
			py -= buffSize;
		}
		if ((OSInt)gimg->GetHeight() - sheight < sy)
		{
			sheight = gimg->GetHeight() - sy;
		}
		if ((OSInt)gimg->GetWidth() - swidth < sx)
		{
			swidth = gimg->GetWidth() - sx;
		}
		if (dwidth + buffSize < swidth)
		{
			swidth = dwidth + buffSize;
		}
		if (dheight + buffSize < sheight)
		{
			sheight = dheight + buffSize;
		}
		if (swidth <= 0 || sheight <= 0 || sz[0] <= 0 || sz[1] <= 0)
		{

		}
		else
		{
			Media::DrawBrush *whiteB = gimg->NewBrushARGB(0xffffffff);
			//gimg->SetTextAlign(this->strAlign);
			gimg->DrawString(sx + buffSize, sy + buffSize, str, f, whiteB);

			OSInt bpl = (sz[0] + (buffSize << 1)) << 2;
			OSInt dbpl = this->info->dispWidth << 2;
			Int32 color = brush->GetOriColor();
			UInt8 *pbits = cairo_image_surface_get_data((cairo_surface_t*)gimg->surface);
			UInt8 *dbits = cairo_image_surface_get_data((cairo_surface_t*)this->surface);
			ImageUtil_ImageColorBuffer32(pbits + bpl * buffSize + buffSize * 4, sz[0], sz[1], bpl, buffSize);
			if (py + sheight > (OSInt)this->info->dispHeight)
			{
				sheight = this->info->dispHeight - py;
			}
			if (px + swidth > (OSInt)this->info->dispWidth)
			{
				swidth = this->info->dispWidth - px;
			}
			if (swidth > 0 && sheight > 0)
			{
				if (dbits)
				{
					pbits = ((UInt8*)pbits) + (sy * bpl) + (sx << 2);
					dbits = ((UInt8*)dbits) + (py * dbpl) + (px << 2);

					if ((color & 0xff000000) == 0xff000000)
					{
						ImageUtil_ImageColorReplace32((UInt8*)pbits, (UInt8*)dbits, swidth, sheight, bpl, dbpl, color);
					}
					else
					{
						ImageUtil_ImageMaskABlend32((UInt8*)pbits, (UInt8*)dbits, swidth, sheight, bpl, dbpl, color);
					}
				}
				else
				{
					pbits = ((UInt8*)pbits) + (sy * bpl) + (sx << 2);
					dbits = ((UInt8*)dbits) + (py * dbpl) + (px << 2);

					ImageUtil_ImageColorReplace32((UInt8*)pbits, (UInt8*)pbits, swidth, sheight, bpl, bpl, color);
					gimg->SetHDPI(this->GetHDPI());
					gimg->SetVDPI(this->GetVDPI());
					this->DrawImagePt(gimg, Math::OSInt2Double(px - sx), Math::OSInt2Double(py - sy));
				}
			}
		}
		eng->DeleteImage(gimg);
	}

	return true;
}

Bool Media::GTKDrawImage::DrawStringRotB(Double centX, Double centY, const UTF8Char *str, DrawFont *f, DrawBrush *p, Double angleDegree, OSInt buffSize)
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
		cairo_save((cairo_t*)this->cr);
		cairo_translate((cairo_t*)this->cr, tlx + this->left, tly + this->top);
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
			cairo_translate((cairo_t*)this->cr, tlx + this->left, tly + this->top);
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

		Int32 ixPos = Math::Double2Int32(tlx);
		Int32 iyPos = Math::Double2Int32(tly);
		Int32 ixPos2 = ixPos + gimg->info->dispWidth;
		Int32 iyPos2 = iyPos + gimg->info->dispHeight;
		if ((OSInt)this->info->dispWidth < ixPos2)
		{
			ixPos2 = this->info->dispWidth;
		}
		if (ixPos < 0)
		{
			simgPtr += -ixPos * 4;
			ixPos = 0;
		}

		if ((OSInt)this->info->dispHeight < iyPos2)
		{
			iyPos2 = this->info->dispHeight;
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
			ImageCopy_ImgCopy(simgPtr, dimgPtr + ixPos * 4 + iyPos * dbpl, (ixPos2 - ixPos) * 4, iyPos2 - iyPos, sbpl, dbpl);
			cairo_surface_mark_dirty((cairo_surface_t*)this->surface);
			return true;
		}
		else
		{
			this->eng->iab->SetSourceProfile(gimg->info->color);
			this->eng->iab->SetDestProfile(this->info->color);
			this->eng->iab->SetOutputProfile(this->info->color);
			this->eng->iab->Blend(dimgPtr + ixPos * 4 + iyPos * dbpl, dbpl, simgPtr, sbpl, ixPos2 - ixPos, iyPos2 - iyPos, gimg->info->atype);
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
	OSInt sbpl = img->info->storeWidth * 4;

	Int32 ixPos = Math::Double2Int32(tlx);
	Int32 iyPos = Math::Double2Int32(tly);
	Int32 ixPos2 = ixPos + img->info->dispWidth;
	Int32 iyPos2 = iyPos + img->info->dispHeight;
	if ((OSInt)this->info->dispWidth < ixPos2)
	{
		ixPos2 = this->info->dispWidth;
	}
	if (ixPos < 0)
	{
		simgPtr += -ixPos * 4;
		ixPos = 0;
	}

	if ((OSInt)this->info->dispHeight < iyPos2)
	{
		iyPos2 = this->info->dispHeight;
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
		ImageCopy_ImgCopy(simgPtr, dimgPtr + ixPos * 4 + iyPos * dbpl, (ixPos2 - ixPos) * 4, iyPos2 - iyPos, sbpl, dbpl);
		cairo_surface_mark_dirty((cairo_surface_t*)this->surface);
		return true;
	}
	else //////////////////////////////////////////
	{
		this->eng->iab->SetSourceProfile(img->info->color);
		this->eng->iab->SetDestProfile(this->info->color);
		this->eng->iab->SetOutputProfile(this->info->color);
		this->eng->iab->Blend(dimgPtr + ixPos * 4 + iyPos * dbpl, dbpl, simgPtr, sbpl, ixPos2 - ixPos, iyPos2 - iyPos, img->info->atype);
		cairo_surface_mark_dirty((cairo_surface_t*)this->surface);
		return true;
	}
	return false;
}

Bool Media::GTKDrawImage::DrawImagePt3(DrawImage *img, Double destX, Double destY, Double srcX, Double srcY, Double srcW, Double srcH)
{
	return false;
}

Media::DrawPen *Media::GTKDrawImage::NewPenARGB(Int32 color, Double thick, UInt8 *pattern, OSInt nPattern)
{
	Media::GTKDrawPen *p;
	NEW_CLASS(p, Media::GTKDrawPen(color, thick));
	return p;
}

Media::DrawBrush *Media::GTKDrawImage::NewBrushARGB(Int32 color)
{
	Media::GTKDrawBrush *b;
	NEW_CLASS(b, Media::GTKDrawBrush(color));
	return b;
}

Media::DrawFont *Media::GTKDrawImage::NewFont(const UTF8Char *name, Int16 pxSize, Media::DrawEngine::DrawFontStyle fontStyle)
{
	Media::GTKDrawFont *f;
	NEW_CLASS(f, Media::GTKDrawFont(name, pxSize * 128.0 / this->info->hdpi, fontStyle));
	return f;
}

Media::DrawFont *Media::GTKDrawImage::NewFontH(const UTF8Char *name, Double height, Media::DrawEngine::DrawFontStyle fontStyle, Int32 codePage)
{
	Media::GTKDrawFont *f;
	NEW_CLASS(f, Media::GTKDrawFont(name, height * 96.0 / this->info->hdpi, fontStyle));
	return f;
}

Media::DrawFont *Media::GTKDrawImage::CloneFont(DrawFont *f)
{
	Media::GTKDrawFont *fnt = (Media::GTKDrawFont*)f;
	const UTF8Char *fname = fnt->GetFontName();
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

Bool Media::GTKDrawImage::GetTextSize(DrawFont *fnt, const UTF8Char *txt, OSInt txtLen, Double *sz)
{
	GTKDrawFont *font = (GTKDrawFont*)fnt;
	cairo_text_extents_t extents;
	font->Init(this->cr);
	if (txtLen != -1)
	{
		const UTF8Char *utf8 = Text::StrCopyNewC(txt, txtLen);;
		cairo_text_extents((cairo_t *)this->cr, (const Char*)utf8, &extents);
		Text::StrDelNew(utf8);
	}
	else
	{
		cairo_text_extents((cairo_t *)this->cr, (const Char*)txt, &extents);
	}
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

void Media::GTKDrawImage::CopyBits(OSInt x, OSInt y, void *imgPtr, OSInt bpl, OSInt width, OSInt height)
{
	if (this->surface)
	{
		cairo_surface_flush((cairo_surface_t*)this->surface);
		UInt8 *srcData = cairo_image_surface_get_data((cairo_surface_t*)this->surface);
		if (srcData)
		{
			ImageCopy_ImgCopy(srcData + x * 4 + y * this->info->storeWidth * 4, (UInt8*)imgPtr, width * 4, height, this->info->storeWidth * 4, bpl);
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

Int32 Media::GTKDrawImage::SavePng(IO::SeekableStream *stm)
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
	pixbuf = gdk_pixbuf_get_from_surface((cairo_surface_t*)this->surface, 0, 0, this->info->dispWidth, this->info->dispHeight);
	if (pixbuf == 0)
	{
		return 0;
	}
	gdk_pixbuf_save_to_buffer(pixbuf, &buff, &buffSize, "png", 0, (void*)0);
	if (buff)
	{
		buffSize = stm->Write((UInt8*)buff, (OSInt)buffSize);
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

Int32 Media::GTKDrawImage::SaveGIF(IO::SeekableStream *stm)
{
	return 0;
}

Int32 Media::GTKDrawImage::SaveJPG(IO::SeekableStream *stm)
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
	pixbuf = gdk_pixbuf_get_from_surface((cairo_surface_t*)this->surface, 0, 0, this->info->dispWidth, this->info->dispHeight);
	if (pixbuf == 0)
	{
		return 0;
	}
	gdk_pixbuf_save_to_buffer(pixbuf, &buff, &buffSize, "jpeg", 0, (void*)0);
	if (buff)
	{
		buffSize = stm->Write((UInt8*)buff, (OSInt)buffSize);
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

void Media::GTKDrawImage::GetImageData(UInt8 *destBuff, OSInt left, OSInt top, OSInt width, OSInt height, OSInt destBpl)
{
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

