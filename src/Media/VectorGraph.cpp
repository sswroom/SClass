#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Math/Geometry/Ellipse.h"
#include "Math/Geometry/Polyline.h"
#include "Math/Geometry/VectorImage.h"
#include "Math/Geometry/VectorString.h"
#include "Math/Unit/Distance.h"
#include "Media/ImageList.h"
#include "Media/SharedImage.h"
#include "Media/StaticImage.h"
#include "Media/VectorGraph.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Media::VectorGraph::VectorPenStyle::VectorPenStyle(UOSInt index, UInt32 color, Double thick, UInt8 *pattern, UOSInt nPattern)
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

Bool Media::VectorGraph::VectorPenStyle::IsSame(UInt32 color, Double thick, UInt8 *pattern, UOSInt nPattern)
{
	UOSInt i;
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

UOSInt Media::VectorGraph::VectorPenStyle::GetIndex()
{
	return this->index;
}

Media::DrawPen *Media::VectorGraph::VectorPenStyle::CreateDrawPen(Double oriDPI, Media::DrawImage *dimg)
{
	Double thick = this->thick * dimg->GetHDPI() / oriDPI;
	return dimg->NewPenARGB(this->color, thick, this->pattern, this->nPattern);
}

Media::VectorGraph::VectorFontStyle::VectorFontStyle(UOSInt index, Text::CString name, Double heightPt, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage)
{
	this->index = index;
	this->name = Text::String::New(name);
	this->heightPt = heightPt;
	this->fontStyle = fontStyle;
	this->codePage = codePage;
}

Media::VectorGraph::VectorFontStyle::~VectorFontStyle()
{
	this->name->Release();
}

Bool Media::VectorGraph::VectorFontStyle::IsSame(Text::CString name, Double heightPt, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage)
{
	if (!this->name->Equals(name.v, name.leng))
		return false;
	if (this->heightPt != heightPt)
		return false;
	if (this->fontStyle != fontStyle)
		return false;
	if (this->codePage != codePage)
		return false;
	return true;
}

UOSInt Media::VectorGraph::VectorFontStyle::GetIndex()
{
	return this->index;
}

Double Media::VectorGraph::VectorFontStyle::GetHeightPt()
{
	return this->heightPt;
}

Text::String *Media::VectorGraph::VectorFontStyle::GetName()
{
	return this->name;
}

Media::DrawEngine::DrawFontStyle Media::VectorGraph::VectorFontStyle::GetStyle()
{
	return this->fontStyle;
}

UInt32 Media::VectorGraph::VectorFontStyle::GetCodePage()
{
	return this->codePage;
}

Media::DrawFont *Media::VectorGraph::VectorFontStyle::CreateDrawFont(Double oriDPI, Media::DrawImage *dimg)
{
	return dimg->NewFontPt(this->name->ToCString(), this->heightPt, this->fontStyle, this->codePage);
}

Media::VectorGraph::VectorBrushStyle::VectorBrushStyle(UOSInt index, UInt32 color)
{
	this->index = index;
	this->color = color;
}

Media::VectorGraph::VectorBrushStyle::~VectorBrushStyle()
{
}

Bool Media::VectorGraph::VectorBrushStyle::IsSame(UInt32 color)
{
	if (this->color != color)
		return false;
	return true;
}

UOSInt Media::VectorGraph::VectorBrushStyle::GetIndex()
{
	return this->index;
}

Media::DrawBrush *Media::VectorGraph::VectorBrushStyle::CreateDrawBrush(Double oriDPI, Media::DrawImage *dimg)
{
	return dimg->NewBrushARGB(this->color);
}

Media::VectorGraph::VectorGraph(UInt32 srid, Double width, Double height, Math::Unit::Distance::DistanceUnit unit, Media::DrawEngine *refEng, Media::ColorProfile *colorProfile)
{
	this->size = Math::Size2D<Double>(width, height);
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
	NEW_CLASS(this->items, Data::ArrayList<Math::Geometry::Vector2D*>());
}

Media::VectorGraph::~VectorGraph()
{
	Media::VectorGraph::VectorPenStyle *pen;
	Media::VectorGraph::VectorFontStyle *font;
	VectorBrushStyle *brush;
	Math::Geometry::Vector2D *vec;
	UOSInt i;
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

Math::Size2D<Double> Media::VectorGraph::GetSize() const
{
	return this->size;
}

UOSInt Media::VectorGraph::GetWidth() const
{
	return (UOSInt)Double2Int32(this->size.width);
}

UOSInt Media::VectorGraph::GetHeight() const
{
	return (UOSInt)Double2Int32(this->size.height);
}

UInt32 Media::VectorGraph::GetBitCount() const
{
	return 32;
}

Media::ColorProfile *Media::VectorGraph::GetColorProfile() const
{
	return this->colorProfile;
}

void Media::VectorGraph::SetColorProfile(const Media::ColorProfile *color)
{
	this->colorProfile->Set(color);
}

Media::AlphaType Media::VectorGraph::GetAlphaType() const
{
	return Media::AT_ALPHA;
}

void Media::VectorGraph::SetAlphaType(Media::AlphaType atype)
{
}

Double Media::VectorGraph::GetHDPI() const
{
	return Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, this->unit, 1);
}

Double Media::VectorGraph::GetVDPI() const
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

Media::EXIFData *Media::VectorGraph::GetEXIF() const
{
	return 0;
}

Media::PixelFormat Media::VectorGraph::GetPixelFormat() const
{
	return Media::PF_UNKNOWN;
}

UOSInt Media::VectorGraph::GetImgBpl() const
{
	return 0;
}

Bool Media::VectorGraph::DrawLine(Double x1, Double y1, Double x2, Double y2, DrawPen *p)
{
	Math::Coord2DDbl pt[2];
	pt[0] = Math::Coord2DDbl(x1, y1);
	pt[1] = Math::Coord2DDbl(x2, y2);
	Math::Geometry::Polyline *pl;
	VectorStyles *style;
	NEW_CLASS(pl, Math::Geometry::Polyline(this->srid, pt, 2, false, false));
	style = MemAlloc(VectorStyles, 1);
	style->pen = (Media::VectorGraph::VectorPenStyle*)p;
	style->brush = 0;
	style->font = 0;
	this->items->Add(pl);
	this->itemStyle->Add(style);
	return true;
}

Bool Media::VectorGraph::DrawPolylineI(const Int32 *points, UOSInt nPoints, DrawPen *p)
{
	Double *dPoints = MemAlloc(Double, nPoints * 2);
	Math_Int32Arr2DblArr(dPoints, points, nPoints * 2);
	Math::Geometry::Polyline *pl;
	VectorStyles *style;
	NEW_CLASS(pl, Math::Geometry::Polyline(this->srid, (Math::Coord2DDbl*)dPoints, nPoints, false, false));
	style = MemAlloc(VectorStyles, 1);
	style->pen = (VectorPenStyle*)p;
	style->brush = 0;
	style->font = 0;
	this->items->Add(pl);
	this->itemStyle->Add(style);
	MemFree(dPoints);
	return true;
}

Bool Media::VectorGraph::DrawPolygonI(const Int32 *points, UOSInt nPoints, DrawPen *p, DrawBrush *b)
{
	/////////////////////////////////
	return false;
}

Bool Media::VectorGraph::DrawPolyPolygonI(const Int32 *points, const UInt32 *pointCnt, UOSInt nPointCnt, DrawPen *p, DrawBrush *b)
{
	/////////////////////////////////
	return false;
}

Bool Media::VectorGraph::DrawPolyline(const Math::Coord2DDbl *points, UOSInt nPoints, DrawPen *p)
{
	Math::Geometry::Polyline *pl;
	VectorStyles *style;
	NEW_CLASS(pl, Math::Geometry::Polyline(this->srid, points, nPoints, false, false));
	style = MemAlloc(VectorStyles, 1);
	style->pen = (VectorPenStyle*)p;
	style->brush = 0;
	style->font = 0;
	this->items->Add(pl);
	this->itemStyle->Add(style);
	return true;
}

Bool Media::VectorGraph::DrawPolygon(const Math::Coord2DDbl *points, UOSInt nPoints, DrawPen *p, DrawBrush *b)
{
	/////////////////////////////////
	return false;
}

Bool Media::VectorGraph::DrawPolyPolygon(const Math::Coord2DDbl *points, const UInt32 *pointCnt, UOSInt nPointCnt, DrawPen *p, DrawBrush *b)
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
	Math::Geometry::Ellipse *vstr;
	NEW_CLASS(vstr, Math::Geometry::Ellipse(this->srid, tlx, tly, w, h));
	style = MemAlloc(VectorStyles, 1);
	style->pen = (Media::VectorGraph::VectorPenStyle*)p;
	style->brush = (VectorBrushStyle*)b;
	style->font = 0;
	this->items->Add(vstr);
	this->itemStyle->Add(style);
	return true;
}

Bool Media::VectorGraph::DrawString(Double tlx, Double tly, Text::String *str, DrawFont *f, DrawBrush *b)
{
	VectorStyles *style;
	Math::Geometry::VectorString *vstr;
	NEW_CLASS(vstr, Math::Geometry::VectorString(this->srid, str, Math::Coord2DDbl(tlx, tly), 0, 0, this->align));
	style = MemAlloc(VectorStyles, 1);
	style->pen = 0;
	style->brush = (VectorBrushStyle*)b;
	style->font = (VectorFontStyle*)f;
	this->items->Add(vstr);
	this->itemStyle->Add(style);
	return true;
}

Bool Media::VectorGraph::DrawString(Double tlx, Double tly, Text::CString str, DrawFont *f, DrawBrush *b)
{
	VectorStyles *style;
	Math::Geometry::VectorString *vstr;
	NEW_CLASS(vstr, Math::Geometry::VectorString(this->srid, str, Math::Coord2DDbl(tlx, tly), 0, 0, this->align));
	style = MemAlloc(VectorStyles, 1);
	style->pen = 0;
	style->brush = (VectorBrushStyle*)b;
	style->font = (VectorFontStyle*)f;
	this->items->Add(vstr);
	this->itemStyle->Add(style);
	return true;
}

Bool Media::VectorGraph::DrawStringRot(Double centX, Double centY, Text::String *str, DrawFont *f, DrawBrush *b, Double angleDegree)
{
	VectorStyles *style;
	Math::Geometry::VectorString *vstr;
	NEW_CLASS(vstr, Math::Geometry::VectorString(this->srid, str, Math::Coord2DDbl(centX, centY), angleDegree, 0, this->align));
	style = MemAlloc(VectorStyles, 1);
	style->pen = 0;
	style->brush = (VectorBrushStyle*)b;
	style->font = (VectorFontStyle*)f;
	this->items->Add(vstr);
	this->itemStyle->Add(style);
	return true;
}

Bool Media::VectorGraph::DrawStringRot(Double centX, Double centY, Text::CString str, DrawFont *f, DrawBrush *b, Double angleDegree)
{
	VectorStyles *style;
	Math::Geometry::VectorString *vstr;
	NEW_CLASS(vstr, Math::Geometry::VectorString(this->srid, str, Math::Coord2DDbl(centX, centY), angleDegree, 0, this->align));
	style = MemAlloc(VectorStyles, 1);
	style->pen = 0;
	style->brush = (VectorBrushStyle*)b;
	style->font = (VectorFontStyle*)f;
	this->items->Add(vstr);
	this->itemStyle->Add(style);
	return true;
}

Bool Media::VectorGraph::DrawStringB(Double tlx, Double tly, Text::String *str, DrawFont *f, DrawBrush *b, UOSInt buffSize)
{
	VectorStyles *style;
	Math::Geometry::VectorString *vstr;
	NEW_CLASS(vstr, Math::Geometry::VectorString(this->srid, str, Math::Coord2DDbl(tlx, tly), 0, UOSInt2Double(buffSize), this->align));
	style = MemAlloc(VectorStyles, 1);
	style->pen = 0;
	style->brush = (VectorBrushStyle*)b;
	style->font = (VectorFontStyle*)f;
	this->items->Add(vstr);
	this->itemStyle->Add(style);
	return true;
}

Bool Media::VectorGraph::DrawStringB(Double tlx, Double tly, Text::CString str, DrawFont *f, DrawBrush *b, UOSInt buffSize)
{
	VectorStyles *style;
	Math::Geometry::VectorString *vstr;
	NEW_CLASS(vstr, Math::Geometry::VectorString(this->srid, str, Math::Coord2DDbl(tlx, tly), 0, UOSInt2Double(buffSize), this->align));
	style = MemAlloc(VectorStyles, 1);
	style->pen = 0;
	style->brush = (VectorBrushStyle*)b;
	style->font = (VectorFontStyle*)f;
	this->items->Add(vstr);
	this->itemStyle->Add(style);
	return true;
}

Bool Media::VectorGraph::DrawStringRotB(Double centX, Double centY, Text::String *str, DrawFont *f, DrawBrush *b, Double angleDegree, UOSInt buffSize)
{
	VectorStyles *style;
	Math::Geometry::VectorString *vstr;
	NEW_CLASS(vstr, Math::Geometry::VectorString(this->srid, str, Math::Coord2DDbl(centX, centY), angleDegree, UOSInt2Double(buffSize), this->align));
	style = MemAlloc(VectorStyles, 1);
	style->pen = 0;
	style->brush = (VectorBrushStyle*)b;
	style->font = (VectorFontStyle*)f;
	this->items->Add(vstr);
	this->itemStyle->Add(style);
	return true;
}

Bool Media::VectorGraph::DrawStringRotB(Double centX, Double centY, Text::CString str, DrawFont *f, DrawBrush *b, Double angleDegree, UOSInt buffSize)
{
	VectorStyles *style;
	Math::Geometry::VectorString *vstr;
	NEW_CLASS(vstr, Math::Geometry::VectorString(this->srid, str, Math::Coord2DDbl(centX, centY), angleDegree, UOSInt2Double(buffSize), this->align));
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
	Math::Geometry::VectorImage *vimg;
	Media::ImageList *imgList;
	NEW_CLASS(imgList, Media::ImageList(CSTR("VectorGraphImage")));
	imgList->AddImage(stImg = img->ToStaticImage(), 0);
	NEW_CLASS(simg, Media::SharedImage(imgList, false));
	NEW_CLASS(vimg, Math::Geometry::VectorImage(this->srid, simg, Math::Coord2DDbl(tlx, tly), Math::Coord2DDbl(tlx + UOSInt2Double(stImg->info.dispWidth) * this->GetHDPI() / stImg->info.hdpi, tly + UOSInt2Double(stImg->info.dispHeight) * stImg->info.par2 * this->GetVDPI() / stImg->info.vdpi), true, CSTR_NULL, 0, 0));
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
	Math::Geometry::VectorImage *vimg;
	Media::ImageList *imgList;
	NEW_CLASS(imgList, Media::ImageList(CSTR("VectorGraphImage")));
	imgList->AddImage(img->CreateStaticImage(), 0);
	NEW_CLASS(simg, Media::SharedImage(imgList, false));
	NEW_CLASS(vimg, Math::Geometry::VectorImage(this->srid, simg, Math::Coord2DDbl(tlx, tly), Math::Coord2DDbl(tlx + UOSInt2Double(img->info.dispWidth) * this->GetHDPI() / img->info.hdpi, tly + UOSInt2Double(img->info.dispHeight) * img->info.par2 * this->GetVDPI() / img->info.vdpi), true, CSTR_NULL, 0, 0));
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
	Math::Geometry::VectorImage *vimg;
	Media::ImageList *imgList;
	NEW_CLASS(imgList, Media::ImageList(CSTR("VectorGraphImage")));
	imgList->AddImage(stImg = img->ToStaticImage(), 0);
	NEW_CLASS(simg, Media::SharedImage(imgList, false));
	NEW_CLASS(vimg, Math::Geometry::VectorImage(this->srid, simg, Math::Coord2DDbl(destX, destY), Math::Coord2DDbl(destX + UOSInt2Double(stImg->info.dispWidth) * this->GetHDPI() / stImg->info.hdpi, destY + UOSInt2Double(stImg->info.dispHeight) * stImg->info.par2 * this->GetVDPI() / stImg->info.vdpi), true, CSTR_NULL, 0, 0));
	style = MemAlloc(VectorStyles, 1);
	style->pen = 0;
	style->brush = 0;
	style->font = 0;
	this->items->Add(vimg);
	this->itemStyle->Add(style);
	DEL_CLASS(simg);
	return true;
}

Media::DrawPen *Media::VectorGraph::NewPenARGB(UInt32 color, Double thick, UInt8 *pattern, UOSInt nPattern)
{
	Media::VectorGraph::VectorPenStyle *pen;
	UOSInt i;
	UOSInt j;
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

Media::DrawBrush *Media::VectorGraph::NewBrushARGB(UInt32 color)
{
	Media::VectorGraph::VectorBrushStyle *brush;
	UOSInt i;
	UOSInt j;
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

Media::DrawFont *Media::VectorGraph::NewFontPt(Text::CString name, Double ptSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage)
{
	Media::VectorGraph::VectorFontStyle *font;
	UOSInt i;
	UOSInt j;
	i = 0;
	j = this->fontStyles->GetCount();
	while (i < j)
	{
		font = this->fontStyles->GetItem(i);
		if (font->IsSame(name, ptSize, fontStyle, 0))
			return font;
		i++;
	}
	NEW_CLASS(font, Media::VectorGraph::VectorFontStyle(this->fontStyles->GetCount(), name, ptSize, fontStyle, 0));
	this->fontStyles->Add(font);
	return font;
}

Media::DrawFont *Media::VectorGraph::NewFontPx(Text::CString name, Double pxSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage)
{
	Media::VectorGraph::VectorFontStyle *font;
	Double ptSize = pxSize * 96.0 / 72.0;
	UOSInt i;
	UOSInt j;
	i = 0;
	j = this->fontStyles->GetCount();
	while (i < j)
	{
		font = this->fontStyles->GetItem(i);
		if (font->IsSame(name, ptSize, fontStyle, codePage))
			return font;
		i++;
	}
	NEW_CLASS(font, Media::VectorGraph::VectorFontStyle(this->fontStyles->GetCount(), name, ptSize, fontStyle, codePage));
	this->fontStyles->Add(font);
	return font;
}

Media::DrawFont *Media::VectorGraph::CloneFont(Media::DrawFont *f)
{
	Media::VectorGraph::VectorFontStyle *font = (Media::VectorGraph::VectorFontStyle*)f;
	Text::String *fontName = font->GetName();
	NEW_CLASS(font, Media::VectorGraph::VectorFontStyle(this->fontStyles->GetCount(), fontName->ToCString(), font->GetHeightPt(), font->GetStyle(), font->GetCodePage()));
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

Bool Media::VectorGraph::GetTextSize(DrawFont *fnt, Text::CString txt, Double *sz)
{
	Media::DrawImage *tmpImg = this->refEng->CreateImage32(16, 16, Media::AT_NO_ALPHA);
	tmpImg->SetHDPI(this->GetHDPI());
	tmpImg->SetVDPI(this->GetVDPI());
	Media::DrawFont *f;
	Media::VectorGraph::VectorFontStyle *fntStyle = (Media::VectorGraph::VectorFontStyle*)fnt;
	Double fntSizePt = fntStyle->GetHeightPt();
	if (fntSizePt < 100)
	{
		Text::String *fontName = fntStyle->GetName();
		f = tmpImg->NewFontPt(fontName->ToCString(), 100, fntStyle->GetStyle(), fntStyle->GetCodePage());
		tmpImg->GetTextSize(f, txt, sz);
		tmpImg->DelFont(f);
		sz[0] *= fntSizePt / 100.0;
		sz[1] *= fntSizePt / 100.0;
	}
	else
	{
		Text::String *fontName = fntStyle->GetName();
		f = tmpImg->NewFontPt(fontName->ToCString(), fntSizePt, fntStyle->GetStyle(), fntStyle->GetCodePage());
		tmpImg->GetTextSize(f, txt, sz);
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

void Media::VectorGraph::CopyBits(OSInt x, OSInt y, void *imgPtr, UOSInt bpl, UOSInt width, UOSInt height, Bool upsideDown) const
{
}

Media::StaticImage *Media::VectorGraph::ToStaticImage() const
{
	return 0;
}

UOSInt Media::VectorGraph::SavePng(IO::SeekableStream *stm)
{
	return 0;
}

UOSInt Media::VectorGraph::SaveGIF(IO::SeekableStream *stm)
{
	return 0;
}

UOSInt Media::VectorGraph::SaveJPG(IO::SeekableStream *stm)
{
	return 0;
}

Double Media::VectorGraph::GetVisibleWidthMM() const
{
	return Math::Unit::Distance::Convert(this->unit, Math::Unit::Distance::DU_MILLIMETER, this->size.width);
}

Double Media::VectorGraph::GetVisibleHeightMM() const
{
	return Math::Unit::Distance::Convert(this->unit, Math::Unit::Distance::DU_MILLIMETER, this->size.height);
}

void Media::VectorGraph::DrawTo(Media::DrawImage *dimg, UInt32 *imgDurMS)
{
	UInt32 imgTimeMS = 0;
	Double scale = (UOSInt2Double(dimg->GetWidth()) / this->size.width + UOSInt2Double(dimg->GetHeight()) / this->size.height) * 0.5;
	Double dpi = this->GetHDPI();
	Media::DrawEngine::DrawPos currAlign = Media::DrawEngine::DRAW_POS_TOPLEFT;
	Media::DrawEngine::DrawPos align;
	dimg->SetTextAlign(currAlign);
	UOSInt i;
	UOSInt j;
	UOSInt k;
	Data::ArrayList<Media::DrawFont*> *dfonts;
	Data::ArrayList<Media::DrawBrush*> *dbrushes;
	Data::ArrayList<Media::DrawPen*> *dpens;
	VectorPenStyle *pen;
	VectorFontStyle *font;
	VectorBrushStyle *brush;
	VectorStyles *styles;
	Math::Geometry::Vector2D *vec;
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

		if (vec->GetVectorType() == Math::Geometry::Vector2D::VectorType::Polyline)
		{
			Math::Geometry::Polyline *pl = (Math::Geometry::Polyline*)vec;
//			OSInt nParts;
			UOSInt nPoints;
//			Int32 *parts;
			Math::Coord2DDbl *points;
			Math::Coord2DDbl *dpoints;
//			parts = pl->GetPartList(&nParts);
			dpoints = pl->GetPointList(&nPoints);
			points = MemAllocA(Math::Coord2DDbl, nPoints);
			Math::Coord2DDbl dScale = Math::Coord2DDbl(scale, scale);
			k = nPoints;
			while (k-- > 0)
			{
				points[k] = dpoints[k] * dScale;
			}
			dimg->DrawPolyline(points, nPoints, p);
			MemFreeA(points);
		}
		else if (vec->GetVectorType() == Math::Geometry::Vector2D::VectorType::String)
		{
			Math::Geometry::VectorString *vstr = (Math::Geometry::VectorString*)vec;
			Math::Coord2DDbl coord = vstr->GetCenter();
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
					dimg->DrawString(coord.x * scale, coord.y * scale, vstr->GetString(), f, b);
				}
				else
				{
					dimg->DrawStringB(coord.x * scale, coord.y * scale, vstr->GetString(), f, b, (UOSInt)Double2Int32(vstr->GetBuffSize() * scale));
				}
			}
			else
			{
				if (vstr->GetBuffSize() == 0)
				{
					dimg->DrawStringRot(coord.x * scale, coord.y * scale, vstr->GetString(), f, b, Double2Int32(vstr->GetAngleDegree()));
				}
				else
				{
					dimg->DrawStringRotB(coord.x * scale, coord.y * scale, vstr->GetString(), f, b, Double2Int32(vstr->GetAngleDegree()), (UOSInt)Double2Int32(vstr->GetBuffSize() * scale));
				}
			}
		}
		else if (vec->GetVectorType() == Math::Geometry::Vector2D::VectorType::Image)
		{
			Math::Geometry::VectorImage *vimg = (Math::Geometry::VectorImage*)vec;
			Math::RectAreaDbl bounds;
			UInt32 thisTimeMS;
			vimg->GetBounds(&bounds);
			Media::StaticImage *simg = vimg->GetImage(&thisTimeMS);
			dimg->DrawImagePt2(simg, bounds.tl.x * scale, bounds.tl.y * scale);
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
		else if (vec->GetVectorType() == Math::Geometry::Vector2D::VectorType::Ellipse)
		{
			Math::Geometry::Ellipse *ellipse = (Math::Geometry::Ellipse*)vec;
			Math::RectAreaDbl bounds;
			ellipse->GetBounds(&bounds);
			bounds = bounds * scale;
			dimg->DrawEllipse(bounds.tl.x, bounds.tl.y, bounds.GetWidth(), bounds.GetHeight(), p, b);
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
