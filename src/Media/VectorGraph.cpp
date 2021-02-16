#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Ellipse.h"
#include "Math/Math.h"
#include "Math/Polyline.h"
#include "Math/VectorImage.h"
#include "Math/VectorString.h"
#include "Math/Unit/Distance.h"
#include "Media/ImageList.h"
#include "Media/SharedImage.h"
#include "Media/StaticImage.h"
#include "Media/VectorGraph.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Media::VectorGraph::VectorPenStyle::VectorPenStyle(OSInt index, Int32 color, Double thick, UInt8 *pattern, OSInt nPattern)
{
	this->index = index;
	this->color = color;
	this->thick = thick;
	this->nPattern = nPattern;
	if (this->nPattern > 0)
	{
		this->pattern = MemAlloc(UInt8, this->nPattern);
		MemCopyNO(this->pattern, pattern, this->nPattern);
	}
	else
	{
		this->pattern = 0;
	}
}
Media::VectorGraph::VectorPenStyle::~VectorPenStyle()
{
	if (this->pattern)
		MemFree(this->pattern);
}

Double Media::VectorGraph::VectorPenStyle::GetThick()
{
	return this->thick;
}

Bool Media::VectorGraph::VectorPenStyle::IsSame(Int32 color, Double thick, UInt8 *pattern, OSInt nPattern)
{
	OSInt i;
	if (this->color != color)
		return false;
	if (this->thick != thick)
		return false;
	if (this->nPattern != nPattern)
		return false;
	i = this->nPattern;
	while (i-- > 0)
	{
		if (pattern[i] != this->pattern[i])
			return false;
	}
	return true;
}

OSInt Media::VectorGraph::VectorPenStyle::GetIndex()
{
	return this->index;
}

Media::DrawPen *Media::VectorGraph::VectorPenStyle::CreateDrawPen(Double oriDPI, Media::DrawImage *dimg)
{
	Double thick = this->thick * dimg->GetHDPI() / oriDPI;
	return dimg->NewPenARGB(this->color, thick, this->pattern, this->nPattern);
}

Media::VectorGraph::VectorFontStyle::VectorFontStyle(OSInt index, const UTF8Char *name, Double height, Media::DrawEngine::DrawFontStyle fontStyle, Int32 codePage)
{
	this->index = index;
	this->name = Text::StrCopyNew(name);
	this->height = height;
	this->fontStyle = fontStyle;
	this->codePage = codePage;
}

Media::VectorGraph::VectorFontStyle::~VectorFontStyle()
{
	Text::StrDelNew(this->name);
}

Bool Media::VectorGraph::VectorFontStyle::IsSame(const UTF8Char *name, Double height, Media::DrawEngine::DrawFontStyle fontStyle, Int32 codePage)
{
	if (!Text::StrEquals(this->name, name))
		return false;
	if (this->height != height)
		return false;
	if (this->fontStyle != fontStyle)
		return false;
	if (this->codePage != codePage)
		return false;
	return true;
}

OSInt Media::VectorGraph::VectorFontStyle::GetIndex()
{
	return this->index;
}

Double Media::VectorGraph::VectorFontStyle::GetHeight()
{
	return this->height;
}

const UTF8Char *Media::VectorGraph::VectorFontStyle::GetName()
{
	return this->name;
}

Media::DrawEngine::DrawFontStyle Media::VectorGraph::VectorFontStyle::GetStyle()
{
	return this->fontStyle;
}

Int32 Media::VectorGraph::VectorFontStyle::GetCodePage()
{
	return this->codePage;
}

Media::DrawFont *Media::VectorGraph::VectorFontStyle::CreateDrawFont(Double oriDPI, Media::DrawImage *dimg)
{
	return dimg->NewFontH(this->name, this->height * dimg->GetHDPI() / oriDPI, this->fontStyle, this->codePage);
}

Media::VectorGraph::VectorBrushStyle::VectorBrushStyle(OSInt index, Int32 color)
{
	this->index = index;
	this->color = color;
}

Media::VectorGraph::VectorBrushStyle::~VectorBrushStyle()
{
}

Bool Media::VectorGraph::VectorBrushStyle::IsSame(Int32 color)
{
	if (this->color != color)
		return false;
	return true;
}

OSInt Media::VectorGraph::VectorBrushStyle::GetIndex()
{
	return this->index;
}

Media::DrawBrush *Media::VectorGraph::VectorBrushStyle::CreateDrawBrush(Double oriDPI, Media::DrawImage *dimg)
{
	return dimg->NewBrushARGB(this->color);
}

Media::VectorGraph::VectorGraph(Int32 srid, Double width, Double height, Math::Unit::Distance::DistanceUnit unit, Media::DrawEngine *refEng, Media::ColorProfile *colorProfile)
{
	this->width = width;
	this->height = height;
	this->unit = unit;
	this->align = Media::DrawEngine::DRAW_POS_TOPLEFT;
	this->refEng = refEng;
	if (colorProfile)
	{
		NEW_CLASS(this->colorProfile, Media::ColorProfile(colorProfile));
	}
	else
	{
		NEW_CLASS(this->colorProfile, Media::ColorProfile(Media::ColorProfile::CPT_SRGB));
	}
	NEW_CLASS(this->penStyles, Data::ArrayList<Media::VectorGraph::VectorPenStyle*>());
	NEW_CLASS(this->fontStyles, Data::ArrayList<Media::VectorGraph::VectorFontStyle*>());
	NEW_CLASS(this->brushStyles, Data::ArrayList<VectorBrushStyle*>());
	NEW_CLASS(this->itemStyle, Data::ArrayList<VectorStyles*>());
	NEW_CLASS(this->items, Data::ArrayList<Math::Vector2D*>());
}

Media::VectorGraph::~VectorGraph()
{
	Media::VectorGraph::VectorPenStyle *pen;
	Media::VectorGraph::VectorFontStyle *font;
	VectorBrushStyle *brush;
	Math::Vector2D *vec;
	OSInt i;
	i = this->penStyles->GetCount();
	while (i-- > 0)
	{
		pen = this->penStyles->GetItem(i);
		DEL_CLASS(pen);
	}
	DEL_CLASS(this->penStyles);
	i = this->fontStyles->GetCount();
	while (i-- > 0)
	{
		font = this->fontStyles->GetItem(i);
		DEL_CLASS(font);
	}
	DEL_CLASS(this->fontStyles);
	i = this->brushStyles->GetCount();
	while (i-- > 0)
	{
		brush = this->brushStyles->GetItem(i);
		DEL_CLASS(brush);
	}
	DEL_CLASS(this->brushStyles);
	i = this->itemStyle->GetCount();
	while (i-- > 0)
	{
		MemFree(this->itemStyle->GetItem(i));
	}
	DEL_CLASS(this->itemStyle);
	i = this->items->GetCount();
	while (i-- > 0)
	{
		vec = this->items->GetItem(i);
		DEL_CLASS(vec);
	}
	DEL_CLASS(this->items);
	DEL_CLASS(this->colorProfile);
}

UOSInt Media::VectorGraph::GetWidth()
{
	return Math::Double2Int32(this->width);
}

UOSInt Media::VectorGraph::GetHeight()
{
	return Math::Double2Int32(this->height);
}

UInt32 Media::VectorGraph::GetBitCount()
{
	return 32;
}

Media::ColorProfile *Media::VectorGraph::GetColorProfile()
{
	return this->colorProfile;
}

void Media::VectorGraph::SetColorProfile(const Media::ColorProfile *color)
{
	this->colorProfile->Set(color);
}

Media::AlphaType Media::VectorGraph::GetAlphaType()
{
	return Media::AT_ALPHA;
}

void Media::VectorGraph::SetAlphaType(Media::AlphaType atype)
{
}

Double Media::VectorGraph::GetHDPI()
{
	return Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, this->unit, 1);
}

Double Media::VectorGraph::GetVDPI()
{
	return Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, this->unit, 1);
}

void Media::VectorGraph::SetHDPI(Double dpi)
{
}

void Media::VectorGraph::SetVDPI(Double dpi)
{
}

UInt8 *Media::VectorGraph::GetImgBits(Bool *revOrder)
{
	return 0;
}

void Media::VectorGraph::GetImgBitsEnd(Bool modified)
{
}

Media::EXIFData *Media::VectorGraph::GetEXIF()
{
	return 0;
}

Media::PixelFormat Media::VectorGraph::GetPixelFormat()
{
	return Media::PF_UNKNOWN;
}

OSInt Media::VectorGraph::GetImgBpl()
{
	return 0;
}

Bool Media::VectorGraph::DrawLine(Double x1, Double y1, Double x2, Double y2, DrawPen *p)
{
	Double pt[4];
	pt[0] = x1;
	pt[1] = y1;
	pt[2] = x2;
	pt[3] = y2;
	Math::Polyline *pl;
	VectorStyles *style;
	NEW_CLASS(pl, Math::Polyline(this->srid, pt, 2));
	style = MemAlloc(VectorStyles, 1);
	style->pen = (Media::VectorGraph::VectorPenStyle*)p;
	style->brush = 0;
	style->font = 0;
	this->items->Add(pl);
	this->itemStyle->Add(style);
	return true;
}

Bool Media::VectorGraph::DrawPolylineI(Int32 *points, UOSInt nPoints, DrawPen *p)
{
	Double *dPoints = MemAlloc(Double, nPoints * 2);
	Math_Int32Arr2DblArr(dPoints, points, nPoints * 2);
	Math::Polyline *pl;
	VectorStyles *style;
	NEW_CLASS(pl, Math::Polyline(this->srid, dPoints, nPoints));
	style = MemAlloc(VectorStyles, 1);
	style->pen = (VectorPenStyle*)p;
	style->brush = 0;
	style->font = 0;
	this->items->Add(pl);
	this->itemStyle->Add(style);
	MemFree(dPoints);
	return true;
}

Bool Media::VectorGraph::DrawPolygonI(Int32 *points, UOSInt nPoints, DrawPen *p, DrawBrush *b)
{
	/////////////////////////////////
	return false;
}

Bool Media::VectorGraph::DrawPolyPolygonI(Int32 *points, UInt32 *pointCnt, UOSInt nPointCnt, DrawPen *p, DrawBrush *b)
{
	/////////////////////////////////
	return false;
}

Bool Media::VectorGraph::DrawPolyline(Double *points, UOSInt nPoints, DrawPen *p)
{
	Math::Polyline *pl;
	VectorStyles *style;
	NEW_CLASS(pl, Math::Polyline(this->srid, points, nPoints));
	style = MemAlloc(VectorStyles, 1);
	style->pen = (VectorPenStyle*)p;
	style->brush = 0;
	style->font = 0;
	this->items->Add(pl);
	this->itemStyle->Add(style);
	return true;
}

Bool Media::VectorGraph::DrawPolygon(Double *points, UOSInt nPoints, DrawPen *p, DrawBrush *b)
{
	/////////////////////////////////
	return false;
}

Bool Media::VectorGraph::DrawPolyPolygon(Double *points, UInt32 *pointCnt, UOSInt nPointCnt, DrawPen *p, DrawBrush *b)
{
	/////////////////////////////////
	return false;
}

Bool Media::VectorGraph::DrawRect(Double x, Double y, Double w, Double h, DrawPen *p, DrawBrush *b)
{
	/////////////////////////////////
	return false;
}

Bool Media::VectorGraph::DrawEllipse(Double tlx, Double tly, Double w, Double h, DrawPen *p, DrawBrush *b)
{
	VectorStyles *style;
	Math::Ellipse *vstr;
	NEW_CLASS(vstr, Math::Ellipse(this->srid, tlx, tly, w, h));
	style = MemAlloc(VectorStyles, 1);
	style->pen = (Media::VectorGraph::VectorPenStyle*)p;
	style->brush = (VectorBrushStyle*)b;
	style->font = 0;
	this->items->Add(vstr);
	this->itemStyle->Add(style);
	return true;
}

Bool Media::VectorGraph::DrawString(Double tlx, Double tly, const UTF8Char *str, DrawFont *f, DrawBrush *b)
{
	VectorStyles *style;
	Math::VectorString *vstr;
	NEW_CLASS(vstr, Math::VectorString(this->srid, str, tlx, tly, 0, 0, this->align));
	style = MemAlloc(VectorStyles, 1);
	style->pen = 0;
	style->brush = (VectorBrushStyle*)b;
	style->font = (VectorFontStyle*)f;
	this->items->Add(vstr);
	this->itemStyle->Add(style);
	return true;
}

Bool Media::VectorGraph::DrawStringRot(Double centX, Double centY, const UTF8Char *str, DrawFont *f, DrawBrush *b, Double angleDegree)
{
	VectorStyles *style;
	Math::VectorString *vstr;
	NEW_CLASS(vstr, Math::VectorString(this->srid, str, centX, centY, angleDegree, 0, this->align));
	style = MemAlloc(VectorStyles, 1);
	style->pen = 0;
	style->brush = (VectorBrushStyle*)b;
	style->font = (VectorFontStyle*)f;
	this->items->Add(vstr);
	this->itemStyle->Add(style);
	return true;
}

Bool Media::VectorGraph::DrawStringB(Double tlx, Double tly, const UTF8Char *str, DrawFont *f, DrawBrush *b, OSInt buffSize)
{
	VectorStyles *style;
	Math::VectorString *vstr;
	NEW_CLASS(vstr, Math::VectorString(this->srid, str, tlx, tly, 0, Math::OSInt2Double(buffSize), this->align));
	style = MemAlloc(VectorStyles, 1);
	style->pen = 0;
	style->brush = (VectorBrushStyle*)b;
	style->font = (VectorFontStyle*)f;
	this->items->Add(vstr);
	this->itemStyle->Add(style);
	return true;
}

Bool Media::VectorGraph::DrawStringRotB(Double centX, Double centY, const UTF8Char *str, DrawFont *f, DrawBrush *b, Double angleDegree, OSInt buffSize)
{
	VectorStyles *style;
	Math::VectorString *vstr;
	NEW_CLASS(vstr, Math::VectorString(this->srid, str, centX, centY, angleDegree, Math::OSInt2Double(buffSize), this->align));
	style = MemAlloc(VectorStyles, 1);
	style->pen = 0;
	style->brush = (VectorBrushStyle*)b;
	style->font = (VectorFontStyle*)f;
	this->items->Add(vstr);
	this->itemStyle->Add(style);
	return true;
}

Bool Media::VectorGraph::DrawImagePt(DrawImage *img, Double tlx, Double tly)
{
	VectorStyles *style;
	Media::SharedImage *simg;
	Media::StaticImage *stImg;
	Math::VectorImage *vimg;
	Media::ImageList *imgList;
	NEW_CLASS(imgList, Media::ImageList((const UTF8Char*)"VectorGraphImage"));
	imgList->AddImage(stImg = img->ToStaticImage(), 0);
	NEW_CLASS(simg, Media::SharedImage(imgList, false));
	NEW_CLASS(vimg, Math::VectorImage(this->srid, simg, tlx, tly, tlx + stImg->info->dispWidth * this->GetHDPI() / stImg->info->hdpi, tly + stImg->info->dispHeight * stImg->info->par2 * this->GetVDPI() / stImg->info->vdpi, true, 0, 0, 0));
	style = MemAlloc(VectorStyles, 1);
	style->pen = 0;
	style->brush = 0;
	style->font = 0;
	this->items->Add(vimg);
	this->itemStyle->Add(style);
	DEL_CLASS(simg);
	return true;
}

Bool Media::VectorGraph::DrawImagePt2(Media::StaticImage *img, Double tlx, Double tly)
{
	VectorStyles *style;
	Media::SharedImage *simg;
	Math::VectorImage *vimg;
	Media::ImageList *imgList;
	NEW_CLASS(imgList, Media::ImageList((const UTF8Char*)"VectorGraphImage"));
	imgList->AddImage(img->CreateStaticImage(), 0);
	NEW_CLASS(simg, Media::SharedImage(imgList, false));
	NEW_CLASS(vimg, Math::VectorImage(this->srid, simg, tlx, tly, tlx + img->info->dispWidth * this->GetHDPI() / img->info->hdpi, tly + img->info->dispHeight * img->info->par2 * this->GetVDPI() / img->info->vdpi, true, 0, 0, 0));
	style = MemAlloc(VectorStyles, 1);
	style->pen = 0;
	style->brush = 0;
	style->font = 0;
	this->items->Add(vimg);
	this->itemStyle->Add(style);
	DEL_CLASS(simg);
	return true;
}

Bool Media::VectorGraph::DrawImagePt3(DrawImage *img, Double destX, Double destY, Double srcX, Double srcY, Double srcW, Double srcH)
{
	VectorStyles *style;
	Media::StaticImage *stImg;
	Media::SharedImage *simg;
	Math::VectorImage *vimg;
	Media::ImageList *imgList;
	NEW_CLASS(imgList, Media::ImageList((const UTF8Char*)"VectorGraphImage"));
	imgList->AddImage(stImg = img->ToStaticImage(), 0);
	NEW_CLASS(simg, Media::SharedImage(imgList, false));
	NEW_CLASS(vimg, Math::VectorImage(this->srid, simg, destX, destY, destX + stImg->info->dispWidth * this->GetHDPI() / stImg->info->hdpi, destY + stImg->info->dispHeight * stImg->info->par2 * this->GetVDPI() / stImg->info->vdpi, true, 0, 0, 0));
	style = MemAlloc(VectorStyles, 1);
	style->pen = 0;
	style->brush = 0;
	style->font = 0;
	this->items->Add(vimg);
	this->itemStyle->Add(style);
	DEL_CLASS(simg);
	return true;
}

Media::DrawPen *Media::VectorGraph::NewPenARGB(Int32 color, Double thick, UInt8 *pattern, OSInt nPattern)
{
	Media::VectorGraph::VectorPenStyle *pen;
	OSInt i;
	OSInt j;
	i = 0;
	j = this->penStyles->GetCount();
	while (i < j)
	{
		pen = this->penStyles->GetItem(i);
		if (pen->IsSame(color, thick, pattern, nPattern))
			return pen;
		i++;
	}
	NEW_CLASS(pen, Media::VectorGraph::VectorPenStyle(this->penStyles->GetCount(), color, thick, pattern, nPattern));
	this->penStyles->Add(pen);
	return pen;
}

Media::DrawBrush *Media::VectorGraph::NewBrushARGB(Int32 color)
{
	Media::VectorGraph::VectorBrushStyle *brush;
	OSInt i;
	OSInt j;
	i = 0;
	j = this->brushStyles->GetCount();
	while (i < j)
	{
		brush = this->brushStyles->GetItem(i);
		if (brush->IsSame(color))
			return brush;
		i++;
	}
	NEW_CLASS(brush, Media::VectorGraph::VectorBrushStyle(this->brushStyles->GetCount(), color));
	this->brushStyles->Add(brush);
	return brush;
}

Media::DrawFont *Media::VectorGraph::NewFont(const UTF8Char *name, Int16 pxSize, Media::DrawEngine::DrawFontStyle fontStyle)
{
	Media::VectorGraph::VectorFontStyle *font;
	OSInt i;
	OSInt j;
	i = 0;
	j = this->fontStyles->GetCount();
	while (i < j)
	{
		font = this->fontStyles->GetItem(i);
		if (font->IsSame(name, pxSize, fontStyle, 0))
			return font;
		i++;
	}
	NEW_CLASS(font, Media::VectorGraph::VectorFontStyle(this->fontStyles->GetCount(), name, pxSize, fontStyle, 0));
	this->fontStyles->Add(font);
	return font;
}

Media::DrawFont *Media::VectorGraph::NewFontH(const UTF8Char *name, Double height, Media::DrawEngine::DrawFontStyle fontStyle, Int32 codePage)
{
	Media::VectorGraph::VectorFontStyle *font;
	OSInt i;
	OSInt j;
	i = 0;
	j = this->fontStyles->GetCount();
	while (i < j)
	{
		font = this->fontStyles->GetItem(i);
		if (font->IsSame(name, height, fontStyle, codePage))
			return font;
		i++;
	}
	NEW_CLASS(font, Media::VectorGraph::VectorFontStyle(this->fontStyles->GetCount(), name, height, fontStyle, codePage));
	this->fontStyles->Add(font);
	return font;
}

Media::DrawFont *Media::VectorGraph::CloneFont(Media::DrawFont *f)
{
	Media::VectorGraph::VectorFontStyle *font = (Media::VectorGraph::VectorFontStyle*)f;
	NEW_CLASS(font, Media::VectorGraph::VectorFontStyle(this->fontStyles->GetCount(), font->GetName(), font->GetHeight(), font->GetStyle(), font->GetCodePage()));
	this->fontStyles->Add(font);
	return font;
}

void Media::VectorGraph::DelPen(DrawPen *p)
{
}

void Media::VectorGraph::DelBrush(DrawBrush *b)
{
}

void Media::VectorGraph::DelFont(DrawFont *f)
{
}

Bool Media::VectorGraph::GetTextSize(DrawFont *fnt, const UTF8Char *txt, OSInt txtLen, Double *sz)
{
	Media::DrawImage *tmpImg = this->refEng->CreateImage32(16, 16, Media::AT_NO_ALPHA);
	Media::DrawFont *f;
	Media::VectorGraph::VectorFontStyle *fntStyle = (Media::VectorGraph::VectorFontStyle*)fnt;
	Double fntSize = fntStyle->GetHeight();
	if (fntSize < 100)
	{
		f = tmpImg->NewFontH(fntStyle->GetName(), 100, fntStyle->GetStyle(), fntStyle->GetCodePage());
		tmpImg->GetTextSize(f, txt, txtLen, sz);
		tmpImg->DelFont(f);
		sz[0] *= fntSize / 100.0;
		sz[1] *= fntSize / 100.0;
	}
	else
	{
		f = tmpImg->NewFontH(fntStyle->GetName(), fntSize, fntStyle->GetStyle(), fntStyle->GetCodePage());
		tmpImg->GetTextSize(f, txt, txtLen, sz);
		tmpImg->DelFont(f);
	}
	this->refEng->DeleteImage(tmpImg);
	return true;
}

void Media::VectorGraph::SetTextAlign(Media::DrawEngine::DrawPos pos)
{
	this->align = pos;
}

void Media::VectorGraph::GetStringBound(Int32 *pos, OSInt centX, OSInt centY, const UTF8Char *str, DrawFont *f, OSInt *drawX, OSInt *drawY)
{
	////////////////////////////////////////
}

void Media::VectorGraph::GetStringBoundRot(Int32 *pos, Double centX, Double centY, const UTF8Char *str, DrawFont *f, Double angleDegree, OSInt *drawX, OSInt *drawY)
{
	////////////////////////////////////////
}

void Media::VectorGraph::CopyBits(OSInt x, OSInt y, void *imgPtr, OSInt bpl, OSInt width, OSInt height)
{
}

Media::StaticImage *Media::VectorGraph::ToStaticImage()
{
	return 0;
}

Int32 Media::VectorGraph::SavePng(IO::SeekableStream *stm)
{
	return 0;
}

Int32 Media::VectorGraph::SaveGIF(IO::SeekableStream *stm)
{
	return 0;
}

Int32 Media::VectorGraph::SaveJPG(IO::SeekableStream *stm)
{
	return 0;
}

Double Media::VectorGraph::GetVisibleWidthMM()
{
	return Math::Unit::Distance::Convert(this->unit, Math::Unit::Distance::DU_MILLIMETER, this->width);
}

Double Media::VectorGraph::GetVisibleHeightMM()
{
	return Math::Unit::Distance::Convert(this->unit, Math::Unit::Distance::DU_MILLIMETER, this->height);
}

void Media::VectorGraph::DrawTo(Media::DrawImage *dimg, Int32 *imgDurMS)
{
	Int32 imgTimeMS = 0;
	Double scale = (dimg->GetWidth() / this->width + dimg->GetHeight() / this->height) * 0.5;
	Double dpi = this->GetHDPI();
	Media::DrawEngine::DrawPos currAlign = Media::DrawEngine::DRAW_POS_TOPLEFT;
	Media::DrawEngine::DrawPos align;
	dimg->SetTextAlign(currAlign);
	OSInt i;
	OSInt j;
	OSInt k;
	Data::ArrayList<Media::DrawFont*> *dfonts;
	Data::ArrayList<Media::DrawBrush*> *dbrushes;
	Data::ArrayList<Media::DrawPen*> *dpens;
	VectorPenStyle *pen;
	VectorFontStyle *font;
	VectorBrushStyle *brush;
	VectorStyles *styles;
	Math::Vector2D *vec;
	Media::DrawPen *p;
	Media::DrawBrush *b;
	Media::DrawFont *f;
	NEW_CLASS(dfonts, Data::ArrayList<Media::DrawFont*>());
	NEW_CLASS(dbrushes, Data::ArrayList<Media::DrawBrush*>());
	NEW_CLASS(dpens, Data::ArrayList<Media::DrawPen*>());
	i = 0;
	j = this->fontStyles->GetCount();
	while (i < j)
	{
		font = this->fontStyles->GetItem(i);
		f = font->CreateDrawFont(dpi, dimg);
		dfonts->Add(f);
		i++;
	}
	i = 0;
	j = this->penStyles->GetCount();
	while (i < j)
	{
		pen = this->penStyles->GetItem(i);
		p = pen->CreateDrawPen(dpi, dimg);
		dpens->Add(p);
		i++;
	}
	i = 0;
	j = this->brushStyles->GetCount();
	while (i < j)
	{
		brush = this->brushStyles->GetItem(i);
		b = brush->CreateDrawBrush(dpi, dimg);
		dbrushes->Add(b);
		i++;
	}


	i = 0;
	j = this->items->GetCount();
	while (i < j)
	{
		vec = this->items->GetItem(i);
		styles = this->itemStyle->GetItem(i);
		if (styles->pen)
		{
			p = dpens->GetItem(styles->pen->GetIndex());
		}
		else
		{
			p = 0;
		}
		if (styles->font)
		{
			f = dfonts->GetItem(styles->font->GetIndex());
		}
		else
		{
			f = 0;
		}
		if (styles->brush)
		{
			b = dbrushes->GetItem(styles->brush->GetIndex());
		}
		else
		{
			b = 0;
		}

		if (vec->GetVectorType() == Math::Vector2D::VT_POLYLINE)
		{
			Math::Polyline *pl = (Math::Polyline*)vec;
//			OSInt nParts;
			UOSInt nPoints;
//			Int32 *parts;
			Double *points;
			Double *dpoints;
//			parts = pl->GetPartList(&nParts);
			dpoints = pl->GetPointList(&nPoints);
			points = MemAlloc(Double, nPoints * 2);
			k = nPoints * 2;
			while (k-- > 0)
			{
				points[k] = dpoints[k] * scale;
			}
			dimg->DrawPolyline(points, nPoints, p);
			MemFree(points);
		}
		else if (vec->GetVectorType() == Math::Vector2D::VT_STRING)
		{
			Double x;
			Double y;
			Math::VectorString *vstr = (Math::VectorString*)vec;
			vstr->GetCenter(&x, &y);
			align = vstr->GetTextAlign();
			if (align != currAlign)
			{
				currAlign = align;
				dimg->SetTextAlign(currAlign);
			}
			if (vstr->GetAngleDegree() == 0)
			{
				if (vstr->GetBuffSize() == 0)
				{
					dimg->DrawString(x * scale, y * scale, vstr->GetString(), f, b);
				}
				else
				{
					dimg->DrawStringB(x * scale, y * scale, vstr->GetString(), f, b, Math::Double2Int32(vstr->GetBuffSize() * scale));
				}
			}
			else
			{
				if (vstr->GetBuffSize() == 0)
				{
					dimg->DrawStringRot(x * scale, y * scale, vstr->GetString(), f, b, Math::Double2Int32(vstr->GetAngleDegree()));
				}
				else
				{
					dimg->DrawStringRotB(x * scale, y * scale, vstr->GetString(), f, b, Math::Double2Int32(vstr->GetAngleDegree()), Math::Double2Int32(vstr->GetBuffSize() * scale));
				}
			}
		}
		else if (vec->GetVectorType() == Math::Vector2D::VT_IMAGE)
		{
			Math::VectorImage *vimg = (Math::VectorImage*)vec;
			Double minX;
			Double minY;
			Double maxX;
			Double maxY;
			Int32 thisTimeMS;
			vimg->GetBounds(&minX, &minY, &maxX, &maxY);
			Media::StaticImage *simg = vimg->GetImage(&thisTimeMS);
			dimg->DrawImagePt2(simg, minX * scale, minY * scale);
			if (imgTimeMS == 0)
			{
				imgTimeMS = thisTimeMS;
			}
			else if (thisTimeMS == 0)
			{
			}
			else if (imgTimeMS > thisTimeMS)
			{
				imgTimeMS = thisTimeMS;
			}
		}
		else if (vec->GetVectorType() == Math::Vector2D::VT_ELLIPSE)
		{
			Math::Ellipse *ellipse = (Math::Ellipse*)vec;
			Double minX;
			Double minY;
			Double maxX;
			Double maxY;
			ellipse->GetBounds(&minX, &minY, &maxX, &maxY);
			dimg->DrawEllipse(minX * scale, minY * scale, (maxX - minX) * scale, (maxY - minY) * scale, p, b);
		}
		else
		{
			///////////////////////////////////
		}
		i++;
	}

	i = dpens->GetCount();
	while (i-- > 0)
	{
		p = dpens->GetItem(i);
		dimg->DelPen(p);
	}
	DEL_CLASS(dpens);
	i = dbrushes->GetCount();
	while (i-- > 0)
	{
		b = dbrushes->GetItem(i);
		dimg->DelBrush(b);
	}
	DEL_CLASS(dbrushes);
	i = dfonts->GetCount();
	while (i-- > 0)
	{
		f = dfonts->GetItem(i);
		dimg->DelFont(f);
	}
	DEL_CLASS(dfonts);

	if (imgDurMS)
	{
		*imgDurMS = imgTimeMS;
	}
}
