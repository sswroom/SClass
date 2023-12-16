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

NotNullPtr<Media::DrawPen> Media::VectorGraph::VectorPenStyle::CreateDrawPen(Double oriDPI, NotNullPtr<Media::DrawImage> dimg)
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

NotNullPtr<Text::String> Media::VectorGraph::VectorFontStyle::GetName() const
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

NotNullPtr<Media::DrawFont> Media::VectorGraph::VectorFontStyle::CreateDrawFont(Double oriDPI, NotNullPtr<Media::DrawImage> dimg)
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

NotNullPtr<Media::DrawBrush> Media::VectorGraph::VectorBrushStyle::CreateDrawBrush(Double oriDPI, NotNullPtr<Media::DrawImage> dimg)
{
	return dimg->NewBrushARGB(this->color);
}

Media::VectorGraph::VectorGraph(UInt32 srid, Double width, Double height, Math::Unit::Distance::DistanceUnit unit, NotNullPtr<Media::DrawEngine> refEng, NotNullPtr<const Media::ColorProfile> colorProfile) : colorProfile(colorProfile)
{
	this->size = Math::Size2DDbl(width, height);
	this->unit = unit;
	this->align = Media::DrawEngine::DRAW_POS_TOPLEFT;
	this->refEng = refEng;
}

Media::VectorGraph::~VectorGraph()
{
	Math::Geometry::Vector2D *vec;
	UOSInt i;
	this->penStyles.DeleteAll();
	this->fontStyles.DeleteAll();
	this->brushStyles.DeleteAll();
	i = this->itemStyle.GetCount();
	while (i-- > 0)
	{
		MemFree(this->itemStyle.GetItem(i));
	}
	i = this->items.GetCount();
	while (i-- > 0)
	{
		vec = this->items.GetItem(i);
		DEL_CLASS(vec);
	}
}

Math::Size2DDbl Media::VectorGraph::GetSizeDbl() const
{
	return this->size;
}

UOSInt Media::VectorGraph::GetWidth() const
{
	return (UOSInt)Double2OSInt(this->size.x);
}

UOSInt Media::VectorGraph::GetHeight() const
{
	return (UOSInt)Double2OSInt(this->size.y);
}

Math::Size2D<UOSInt> Media::VectorGraph::GetSize() const
{
	return Math::Size2D<UOSInt>::UOSIntFromDouble(this->size);
}

UInt32 Media::VectorGraph::GetBitCount() const
{
	return 32;
}

NotNullPtr<const Media::ColorProfile> Media::VectorGraph::GetColorProfile() const
{
	return this->colorProfile;
}

void Media::VectorGraph::SetColorProfile(NotNullPtr<const Media::ColorProfile> color)
{
	this->colorProfile.Set(color);
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

UInt8 *Media::VectorGraph::GetImgBits(OutParam<Bool> revOrder)
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

Bool Media::VectorGraph::DrawLine(Double x1, Double y1, Double x2, Double y2, NotNullPtr<DrawPen> p)
{
	Math::Coord2DDbl pt[2];
	pt[0] = Math::Coord2DDbl(x1, y1);
	pt[1] = Math::Coord2DDbl(x2, y2);
	Math::Geometry::LineString *pl;
	VectorStyles *style;
	NEW_CLASS(pl, Math::Geometry::LineString(this->srid, pt, 2, 0, 0));
	style = MemAlloc(VectorStyles, 1);
	style->pen = (Media::VectorGraph::VectorPenStyle*)p.Ptr();
	style->brush = 0;
	style->font = 0;
	this->items.Add(pl);
	this->itemStyle.Add(style);
	return true;
}

Bool Media::VectorGraph::DrawPolylineI(const Int32 *points, UOSInt nPoints, NotNullPtr<DrawPen> p)
{
	Double *dPoints = MemAlloc(Double, nPoints * 2);
	Math_Int32Arr2DblArr(dPoints, points, nPoints * 2);
	Math::Geometry::LineString *pl;
	VectorStyles *style;
	NEW_CLASS(pl, Math::Geometry::LineString(this->srid, (Math::Coord2DDbl*)dPoints, nPoints, 0, 0));
	style = MemAlloc(VectorStyles, 1);
	style->pen = (VectorPenStyle*)p.Ptr();
	style->brush = 0;
	style->font = 0;
	this->items.Add(pl);
	this->itemStyle.Add(style);
	MemFree(dPoints);
	return true;
}

Bool Media::VectorGraph::DrawPolygonI(const Int32 *points, UOSInt nPoints, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	/////////////////////////////////
	return false;
}

Bool Media::VectorGraph::DrawPolyPolygonI(const Int32 *points, const UInt32 *pointCnt, UOSInt nPointCnt, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	/////////////////////////////////
	return false;
}

Bool Media::VectorGraph::DrawPolyline(const Math::Coord2DDbl *points, UOSInt nPoints, NotNullPtr<DrawPen> p)
{
	Math::Geometry::LineString *pl;
	VectorStyles *style;
	NEW_CLASS(pl, Math::Geometry::LineString(this->srid, points, nPoints, 0, 0));
	style = MemAlloc(VectorStyles, 1);
	style->pen = (VectorPenStyle*)p.Ptr();
	style->brush = 0;
	style->font = 0;
	this->items.Add(pl);
	this->itemStyle.Add(style);
	return true;
}

Bool Media::VectorGraph::DrawPolygon(const Math::Coord2DDbl *points, UOSInt nPoints, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	/////////////////////////////////
	return false;
}

Bool Media::VectorGraph::DrawPolyPolygon(const Math::Coord2DDbl *points, const UInt32 *pointCnt, UOSInt nPointCnt, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	/////////////////////////////////
	return false;
}

Bool Media::VectorGraph::DrawRect(Math::Coord2DDbl tl, Math::Size2DDbl size, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	/////////////////////////////////
	return false;
}

Bool Media::VectorGraph::DrawEllipse(Math::Coord2DDbl tl, Math::Size2DDbl size, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	VectorStyles *style;
	Math::Geometry::Ellipse *vstr;
	NEW_CLASS(vstr, Math::Geometry::Ellipse(this->srid, tl, size));
	style = MemAlloc(VectorStyles, 1);
	style->pen = Optional<Media::VectorGraph::VectorPenStyle>::ConvertFrom(p);
	style->brush = Optional<VectorBrushStyle>::ConvertFrom(b);
	style->font = 0;
	this->items.Add(vstr);
	this->itemStyle.Add(style);
	return true;
}

Bool Media::VectorGraph::DrawString(Math::Coord2DDbl tl, NotNullPtr<Text::String> str, NotNullPtr<DrawFont> f, NotNullPtr<DrawBrush> b)
{
	VectorStyles *style;
	Math::Geometry::VectorString *vstr;
	NEW_CLASS(vstr, Math::Geometry::VectorString(this->srid, str, tl, 0, 0, this->align));
	style = MemAlloc(VectorStyles, 1);
	style->pen = 0;
	style->brush = NotNullPtr<VectorBrushStyle>::ConvertFrom(b);
	style->font = NotNullPtr<VectorFontStyle>::ConvertFrom(f);
	this->items.Add(vstr);
	this->itemStyle.Add(style);
	return true;
}

Bool Media::VectorGraph::DrawString(Math::Coord2DDbl tl, Text::CStringNN str, NotNullPtr<DrawFont> f, NotNullPtr<DrawBrush> b)
{
	VectorStyles *style;
	Math::Geometry::VectorString *vstr;
	NEW_CLASS(vstr, Math::Geometry::VectorString(this->srid, str, tl, 0, 0, this->align));
	style = MemAlloc(VectorStyles, 1);
	style->pen = 0;
	style->brush = NotNullPtr<VectorBrushStyle>::ConvertFrom(b);
	style->font = NotNullPtr<VectorFontStyle>::ConvertFrom(f);
	this->items.Add(vstr);
	this->itemStyle.Add(style);
	return true;
}

Bool Media::VectorGraph::DrawStringRot(Math::Coord2DDbl center, NotNullPtr<Text::String> str, NotNullPtr<DrawFont> f, NotNullPtr<DrawBrush> b, Double angleDegree)
{
	VectorStyles *style;
	Math::Geometry::VectorString *vstr;
	NEW_CLASS(vstr, Math::Geometry::VectorString(this->srid, str, center, angleDegree, 0, this->align));
	style = MemAlloc(VectorStyles, 1);
	style->pen = 0;
	style->brush = NotNullPtr<VectorBrushStyle>::ConvertFrom(b);
	style->font = NotNullPtr<VectorFontStyle>::ConvertFrom(f);
	this->items.Add(vstr);
	this->itemStyle.Add(style);
	return true;
}

Bool Media::VectorGraph::DrawStringRot(Math::Coord2DDbl center, Text::CStringNN str, NotNullPtr<DrawFont> f, NotNullPtr<DrawBrush> b, Double angleDegree)
{
	VectorStyles *style;
	Math::Geometry::VectorString *vstr;
	NEW_CLASS(vstr, Math::Geometry::VectorString(this->srid, str, center, angleDegree, 0, this->align));
	style = MemAlloc(VectorStyles, 1);
	style->pen = 0;
	style->brush = NotNullPtr<VectorBrushStyle>::ConvertFrom(b);
	style->font = NotNullPtr<VectorFontStyle>::ConvertFrom(f);
	this->items.Add(vstr);
	this->itemStyle.Add(style);
	return true;
}

Bool Media::VectorGraph::DrawStringB(Math::Coord2DDbl tl, NotNullPtr<Text::String> str, NotNullPtr<DrawFont> f, NotNullPtr<DrawBrush> b, UOSInt buffSize)
{
	VectorStyles *style;
	Math::Geometry::VectorString *vstr;
	NEW_CLASS(vstr, Math::Geometry::VectorString(this->srid, str, tl, 0, UOSInt2Double(buffSize), this->align));
	style = MemAlloc(VectorStyles, 1);
	style->pen = 0;
	style->brush = NotNullPtr<VectorBrushStyle>::ConvertFrom(b);
	style->font = NotNullPtr<VectorFontStyle>::ConvertFrom(f);
	this->items.Add(vstr);
	this->itemStyle.Add(style);
	return true;
}

Bool Media::VectorGraph::DrawStringB(Math::Coord2DDbl tl, Text::CStringNN str, NotNullPtr<DrawFont> f, NotNullPtr<DrawBrush> b, UOSInt buffSize)
{
	VectorStyles *style;
	Math::Geometry::VectorString *vstr;
	NEW_CLASS(vstr, Math::Geometry::VectorString(this->srid, str, tl, 0, UOSInt2Double(buffSize), this->align));
	style = MemAlloc(VectorStyles, 1);
	style->pen = 0;
	style->brush = NotNullPtr<VectorBrushStyle>::ConvertFrom(b);
	style->font = NotNullPtr<VectorFontStyle>::ConvertFrom(f);
	this->items.Add(vstr);
	this->itemStyle.Add(style);
	return true;
}

Bool Media::VectorGraph::DrawStringRotB(Math::Coord2DDbl center, NotNullPtr<Text::String> str, NotNullPtr<DrawFont> f, NotNullPtr<DrawBrush> b, Double angleDegree, UOSInt buffSize)
{
	VectorStyles *style;
	Math::Geometry::VectorString *vstr;
	NEW_CLASS(vstr, Math::Geometry::VectorString(this->srid, str, center, angleDegree, UOSInt2Double(buffSize), this->align));
	style = MemAlloc(VectorStyles, 1);
	style->pen = 0;
	style->brush = NotNullPtr<VectorBrushStyle>::ConvertFrom(b);
	style->font = NotNullPtr<VectorFontStyle>::ConvertFrom(f);
	this->items.Add(vstr);
	this->itemStyle.Add(style);
	return true;
}

Bool Media::VectorGraph::DrawStringRotB(Math::Coord2DDbl center, Text::CStringNN str, NotNullPtr<DrawFont> f, NotNullPtr<DrawBrush> b, Double angleDegree, UOSInt buffSize)
{
	VectorStyles *style;
	Math::Geometry::VectorString *vstr;
	NEW_CLASS(vstr, Math::Geometry::VectorString(this->srid, str, center, angleDegree, UOSInt2Double(buffSize), this->align));
	style = MemAlloc(VectorStyles, 1);
	style->pen = 0;
	style->brush = NotNullPtr<VectorBrushStyle>::ConvertFrom(b);
	style->font = NotNullPtr<VectorFontStyle>::ConvertFrom(f);
	this->items.Add(vstr);
	this->itemStyle.Add(style);
	return true;
}

Bool Media::VectorGraph::DrawImagePt(NotNullPtr<DrawImage> img, Math::Coord2DDbl tl)
{
	VectorStyles *style;
	Media::SharedImage *simg;
	Media::StaticImage *stImg;
	Math::Geometry::VectorImage *vimg;
	Media::ImageList *imgList;
	NEW_CLASS(imgList, Media::ImageList(CSTR("VectorGraphImage")));
	imgList->AddImage(stImg = img->ToStaticImage(), 0);
	NEW_CLASS(simg, Media::SharedImage(imgList, false));
	NEW_CLASS(vimg, Math::Geometry::VectorImage(this->srid, simg, tl, Math::Coord2DDbl(tl.x + UOSInt2Double(stImg->info.dispSize.x) * this->GetHDPI() / stImg->info.hdpi, tl.y + UOSInt2Double(stImg->info.dispSize.y) * stImg->info.par2 * this->GetVDPI() / stImg->info.vdpi), true, CSTR_NULL, 0, 0));
	style = MemAlloc(VectorStyles, 1);
	style->pen = 0;
	style->brush = 0;
	style->font = 0;
	this->items.Add(vimg);
	this->itemStyle.Add(style);
	DEL_CLASS(simg);
	return true;
}

Bool Media::VectorGraph::DrawImagePt2(NotNullPtr<Media::StaticImage> img, Math::Coord2DDbl tl)
{
	VectorStyles *style;
	Media::SharedImage *simg;
	Math::Geometry::VectorImage *vimg;
	Media::ImageList *imgList;
	NEW_CLASS(imgList, Media::ImageList(CSTR("VectorGraphImage")));
	imgList->AddImage(img->CreateStaticImage(), 0);
	NEW_CLASS(simg, Media::SharedImage(imgList, false));
	NEW_CLASS(vimg, Math::Geometry::VectorImage(this->srid, simg, tl, Math::Coord2DDbl(tl.x + UOSInt2Double(img->info.dispSize.x) * this->GetHDPI() / img->info.hdpi, tl.y + UOSInt2Double(img->info.dispSize.y) * img->info.par2 * this->GetVDPI() / img->info.vdpi), true, CSTR_NULL, 0, 0));
	style = MemAlloc(VectorStyles, 1);
	style->pen = 0;
	style->brush = 0;
	style->font = 0;
	this->items.Add(vimg);
	this->itemStyle.Add(style);
	DEL_CLASS(simg);
	return true;
}

Bool Media::VectorGraph::DrawImagePt3(NotNullPtr<DrawImage> img, Math::Coord2DDbl destTL, Math::Coord2DDbl srcTL, Math::Size2DDbl srcSize)
{
	VectorStyles *style;
	Media::StaticImage *stImg;
	Media::SharedImage *simg;
	Math::Geometry::VectorImage *vimg;
	Media::ImageList *imgList;
	NEW_CLASS(imgList, Media::ImageList(CSTR("VectorGraphImage")));
	imgList->AddImage(stImg = img->ToStaticImage(), 0);
	NEW_CLASS(simg, Media::SharedImage(imgList, false));
	NEW_CLASS(vimg, Math::Geometry::VectorImage(this->srid, simg, destTL, Math::Coord2DDbl(destTL.x + UOSInt2Double(stImg->info.dispSize.x) * this->GetHDPI() / stImg->info.hdpi, destTL.y + UOSInt2Double(stImg->info.dispSize.y) * stImg->info.par2 * this->GetVDPI() / stImg->info.vdpi), true, CSTR_NULL, 0, 0));
	style = MemAlloc(VectorStyles, 1);
	style->pen = 0;
	style->brush = 0;
	style->font = 0;
	this->items.Add(vimg);
	this->itemStyle.Add(style);
	DEL_CLASS(simg);
	return true;
}

NotNullPtr<Media::DrawPen> Media::VectorGraph::NewPenARGB(UInt32 color, Double thick, UInt8 *pattern, UOSInt nPattern)
{
	NotNullPtr<Media::VectorGraph::VectorPenStyle> pen;
	Data::ArrayIterator<NotNullPtr<Media::VectorGraph::VectorPenStyle>> it = this->penStyles.Iterator();
	while (it.HasNext())
	{
		pen = it.Next();
		if (pen->IsSame(color, thick, pattern, nPattern))
			return pen;
	}
	NEW_CLASSNN(pen, Media::VectorGraph::VectorPenStyle(this->penStyles.GetCount(), color, thick, pattern, nPattern));
	this->penStyles.Add(pen);
	return pen;
}

NotNullPtr<Media::DrawBrush> Media::VectorGraph::NewBrushARGB(UInt32 color)
{
	NotNullPtr<Media::VectorGraph::VectorBrushStyle> brush;
	Data::ArrayIterator<NotNullPtr<Media::VectorGraph::VectorBrushStyle>> it = this->brushStyles.Iterator();
	while (it.HasNext())
	{
		brush = it.Next();
		if (brush->IsSame(color))
			return brush;
	}
	NEW_CLASSNN(brush, Media::VectorGraph::VectorBrushStyle(this->brushStyles.GetCount(), color));
	this->brushStyles.Add(brush);
	return brush;
}

NotNullPtr<Media::DrawFont> Media::VectorGraph::NewFontPt(Text::CString name, Double ptSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage)
{
	NotNullPtr<Media::VectorGraph::VectorFontStyle> font;
	Data::ArrayIterator<NotNullPtr<VectorFontStyle>> it = this->fontStyles.Iterator();
	while (it.HasNext())
	{
		font = it.Next();
		if (font->IsSame(name, ptSize, fontStyle, 0))
			return font;
	}
	NEW_CLASSNN(font, Media::VectorGraph::VectorFontStyle(this->fontStyles.GetCount(), name, ptSize, fontStyle, 0));
	this->fontStyles.Add(font);
	return font;
}

NotNullPtr<Media::DrawFont> Media::VectorGraph::NewFontPx(Text::CString name, Double pxSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage)
{
	NotNullPtr<Media::VectorGraph::VectorFontStyle> font;
	Double ptSize = pxSize * 96.0 / 72.0;
	Data::ArrayIterator<NotNullPtr<VectorFontStyle>> it = this->fontStyles.Iterator();
	while (it.HasNext())
	{
		font = it.Next();
		if (font->IsSame(name, ptSize, fontStyle, codePage))
			return font;
	}
	NEW_CLASSNN(font, Media::VectorGraph::VectorFontStyle(this->fontStyles.GetCount(), name, ptSize, fontStyle, codePage));
	this->fontStyles.Add(font);
	return font;
}

NotNullPtr<Media::DrawFont> Media::VectorGraph::CloneFont(NotNullPtr<Media::DrawFont> f)
{
	NotNullPtr<Media::VectorGraph::VectorFontStyle> font = NotNullPtr<Media::VectorGraph::VectorFontStyle>::ConvertFrom(f);
	NotNullPtr<Text::String> fontName = font->GetName();
	NEW_CLASSNN(font, Media::VectorGraph::VectorFontStyle(this->fontStyles.GetCount(), fontName->ToCString(), font->GetHeightPt(), font->GetStyle(), font->GetCodePage()));
	this->fontStyles.Add(font);
	return font;
}

void Media::VectorGraph::DelPen(NotNullPtr<DrawPen> p)
{
}

void Media::VectorGraph::DelBrush(NotNullPtr<DrawBrush> b)
{
}

void Media::VectorGraph::DelFont(NotNullPtr<DrawFont> f)
{
}

Math::Size2DDbl Media::VectorGraph::GetTextSize(NotNullPtr<DrawFont> fnt, Text::CStringNN txt)
{
	NotNullPtr<Media::DrawImage> tmpImg;
	if (tmpImg.Set(this->refEng->CreateImage32(Math::Size2D<UOSInt>(16, 16), Media::AT_NO_ALPHA)))
	{
		tmpImg->SetHDPI(this->GetHDPI());
		tmpImg->SetVDPI(this->GetVDPI());
		Math::Size2DDbl sz;
		NotNullPtr<Media::DrawFont> f;
		NotNullPtr<Media::VectorGraph::VectorFontStyle> fntStyle = NotNullPtr<Media::VectorGraph::VectorFontStyle>::ConvertFrom(fnt);
		Double fntSizePt = fntStyle->GetHeightPt();
		if (fntSizePt < 100)
		{
			NotNullPtr<Text::String> fontName = fntStyle->GetName();
			f = tmpImg->NewFontPt(fontName->ToCString(), 100, fntStyle->GetStyle(), fntStyle->GetCodePage());
			sz = tmpImg->GetTextSize(f, txt);
			tmpImg->DelFont(f);
			sz.x *= fntSizePt / 100.0;
			sz.y *= fntSizePt / 100.0;
		}
		else
		{
			NotNullPtr<Text::String> fontName = fntStyle->GetName();
			f = tmpImg->NewFontPt(fontName->ToCString(), fntSizePt, fntStyle->GetStyle(), fntStyle->GetCodePage());
			sz = tmpImg->GetTextSize(f, txt);
			tmpImg->DelFont(f);
		}
		this->refEng->DeleteImage(tmpImg);
		return sz;
	}
	else
	{
		NotNullPtr<Media::VectorGraph::VectorFontStyle> fntStyle = NotNullPtr<Media::VectorGraph::VectorFontStyle>::ConvertFrom(fnt);
		Double fntSizePt = fntStyle->GetHeightPt();
		return Math::Size2DDbl(UOSInt2Double(txt.leng) * fntSizePt * 0.5, fntSizePt);
	}
}

void Media::VectorGraph::SetTextAlign(Media::DrawEngine::DrawPos pos)
{
	this->align = pos;
}

void Media::VectorGraph::GetStringBound(Int32 *pos, OSInt centX, OSInt centY, const UTF8Char *str, NotNullPtr<DrawFont> f, OutParam<OSInt> drawX, OutParam<OSInt> drawY)
{
	////////////////////////////////////////
}

void Media::VectorGraph::GetStringBoundRot(Int32 *pos, Double centX, Double centY, const UTF8Char *str, NotNullPtr<DrawFont> f, Double angleDegree, OutParam<OSInt> drawX, OutParam<OSInt> drawY)
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

UOSInt Media::VectorGraph::SavePng(NotNullPtr<IO::SeekableStream> stm)
{
	return 0;
}

UOSInt Media::VectorGraph::SaveGIF(NotNullPtr<IO::SeekableStream> stm)
{
	return 0;
}

UOSInt Media::VectorGraph::SaveJPG(NotNullPtr<IO::SeekableStream> stm)
{
	return 0;
}

Double Media::VectorGraph::GetVisibleWidthMM() const
{
	return Math::Unit::Distance::Convert(this->unit, Math::Unit::Distance::DU_MILLIMETER, this->size.x);
}

Double Media::VectorGraph::GetVisibleHeightMM() const
{
	return Math::Unit::Distance::Convert(this->unit, Math::Unit::Distance::DU_MILLIMETER, this->size.y);
}

void Media::VectorGraph::DrawTo(NotNullPtr<Media::DrawImage> dimg, OptOut<UInt32> imgDurMS)
{
	UInt32 imgTimeMS = 0;
	Double scale = (UOSInt2Double(dimg->GetWidth()) / this->size.x + UOSInt2Double(dimg->GetHeight()) / this->size.y) * 0.5;
	Double dpi = this->GetHDPI();
	Media::DrawEngine::DrawPos currAlign = Media::DrawEngine::DRAW_POS_TOPLEFT;
	Media::DrawEngine::DrawPos align;
	dimg->SetTextAlign(currAlign);
	UOSInt i;
	UOSInt j;
	UOSInt k;
	Data::ArrayListNN<Media::DrawFont> dfonts;
	Data::ArrayListNN<Media::DrawBrush> dbrushes;
	Data::ArrayListNN<Media::DrawPen> dpens;
	NotNullPtr<VectorPenStyle> pen;
	NotNullPtr<VectorFontStyle> font;
	NotNullPtr<VectorBrushStyle> brush;
	VectorStyles *styles;
	Math::Geometry::Vector2D *vec;
	NotNullPtr<Media::DrawPen> p;
	Optional<Media::DrawPen> dp;
	NotNullPtr<Media::DrawBrush> b;
	Optional<Media::DrawBrush> ob;
	NotNullPtr<Media::DrawFont> f;
	Optional<Media::DrawFont> df;
	Data::ArrayIterator<NotNullPtr<VectorFontStyle>> itFont = this->fontStyles.Iterator();
	while (itFont.HasNext())
	{
		font = itFont.Next();
		f = font->CreateDrawFont(dpi, dimg);
		dfonts.Add(f);
	}
	Data::ArrayIterator<NotNullPtr<VectorPenStyle>> itPen = this->penStyles.Iterator();
	while (itPen.HasNext())
	{
		pen = itPen.Next();
		p = pen->CreateDrawPen(dpi, dimg);
		dpens.Add(p);
	}
	Data::ArrayIterator<NotNullPtr<VectorBrushStyle>> itBrush = this->brushStyles.Iterator();
	while (itBrush.HasNext())
	{
		brush = itBrush.Next();
		b = brush->CreateDrawBrush(dpi, dimg);
		dbrushes.Add(b);
	}


	i = 0;
	j = this->items.GetCount();
	while (i < j)
	{
		vec = this->items.GetItem(i);
		styles = this->itemStyle.GetItem(i);
		if (styles->pen.SetTo(pen))
		{
			dp = dpens.GetItem(pen->GetIndex());
		}
		else
		{
			dp = 0;
		}
		if (styles->font.SetTo(font))
		{
			df = dfonts.GetItem(font->GetIndex());
		}
		else
		{
			df = 0;
		}
		NotNullPtr<VectorBrushStyle> brush;
		if (styles->brush.SetTo(brush))
		{
			ob = dbrushes.GetItem(brush->GetIndex());
		}
		else
		{
			ob = 0;
		}

		if (vec->GetVectorType() == Math::Geometry::Vector2D::VectorType::Polyline)
		{
			if (dp.SetTo(p))
			{
				Math::Geometry::Polyline *pl = (Math::Geometry::Polyline*)vec;
				UOSInt nPoints;
				Math::Coord2DDbl *points;
				Math::Coord2DDbl *dpoints;
				NotNullPtr<Math::Geometry::LineString> lineString;
				Data::ArrayIterator<NotNullPtr<Math::Geometry::LineString>> it = pl->Iterator();
				while (it.HasNext())
				{
					lineString = it.Next();
					dpoints = lineString->GetPointList(nPoints);
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
			}
		}
		else if (vec->GetVectorType() == Math::Geometry::Vector2D::VectorType::LineString)
		{
			if (dp.SetTo(p))
			{
				Math::Geometry::LineString *lineString = (Math::Geometry::LineString*)vec;
				UOSInt nPoints;
				Math::Coord2DDbl *points;
				Math::Coord2DDbl *dpoints;
				dpoints = lineString->GetPointList(nPoints);
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
		}
		else if (vec->GetVectorType() == Math::Geometry::Vector2D::VectorType::String && df.SetTo(f) && ob.SetTo(b))
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
					dimg->DrawString(coord * scale, vstr->GetString(), f, b);
				}
				else
				{
					dimg->DrawStringB(coord * scale, vstr->GetString(), f, b, (UOSInt)Double2Int32(vstr->GetBuffSize() * scale));
				}
			}
			else
			{
				if (vstr->GetBuffSize() == 0)
				{
					dimg->DrawStringRot(coord * scale, vstr->GetString(), f, b, Double2Int32(vstr->GetAngleDegree()));
				}
				else
				{
					dimg->DrawStringRotB(coord * scale, vstr->GetString(), f, b, Double2Int32(vstr->GetAngleDegree()), (UOSInt)Double2Int32(vstr->GetBuffSize() * scale));
				}
			}
		}
		else if (vec->GetVectorType() == Math::Geometry::Vector2D::VectorType::Image)
		{
			Math::Geometry::VectorImage *vimg = (Math::Geometry::VectorImage*)vec;
			Math::RectAreaDbl bounds;
			UInt32 thisTimeMS;
			bounds = vimg->GetBounds();
			NotNullPtr<Media::StaticImage> simg;
			if (simg.Set(vimg->GetImage(thisTimeMS)))
			{
				dimg->DrawImagePt2(simg, bounds.tl * scale);
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
		}
		else if (vec->GetVectorType() == Math::Geometry::Vector2D::VectorType::Ellipse)
		{
			Math::Geometry::Ellipse *ellipse = (Math::Geometry::Ellipse*)vec;
			Math::RectAreaDbl bounds = ellipse->GetBounds();
			bounds = bounds * scale;
			dimg->DrawEllipse(bounds.tl, bounds.GetSize(), dp, ob);
		}
		else
		{
			///////////////////////////////////
		}
		i++;
	}

	i = dpens.GetCount();
	while (i-- > 0)
	{
		if (dpens.GetItem(i).SetTo(p))
			dimg->DelPen(p);
	}
	i = dbrushes.GetCount();
	while (i-- > 0)
	{
		if (dbrushes.GetItem(i).SetTo(b))
			dimg->DelBrush(b);
	}
	i = dfonts.GetCount();
	while (i-- > 0)
	{
		if (dfonts.GetItem(i).SetTo(f))
			dimg->DelFont(f);
	}
	imgDurMS.Set(imgTimeMS);
}
