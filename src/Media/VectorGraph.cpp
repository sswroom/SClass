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

Media::VectorGraph::VectorPenStyle::VectorPenStyle(UOSInt index, UInt32 color, Double thick, UnsafeArrayOpt<UInt8> pattern, UOSInt nPattern)
{
	this->index = index;
	this->color = color;
	this->thick = thick;
	UnsafeArray<UInt8> nnpattern;
	if (nPattern > 0 && pattern.SetTo(nnpattern))
	{
		this->nPattern = nPattern;
		this->pattern = MemAlloc(UInt8, this->nPattern);
		MemCopyNO(this->pattern.Ptr(), nnpattern.Ptr(), this->nPattern);
	}
	else
	{
		this->pattern = 0;
		this->nPattern = 0;
	}
}
Media::VectorGraph::VectorPenStyle::~VectorPenStyle()
{
	UnsafeArray<UInt8> nnpattern;
	if (this->pattern.SetTo(nnpattern))
		MemFreeArr(nnpattern);
}

Double Media::VectorGraph::VectorPenStyle::GetThick()
{
	return this->thick;
}

Bool Media::VectorGraph::VectorPenStyle::IsSame(UInt32 color, Double thick, UnsafeArrayOpt<UInt8> pattern, UOSInt nPattern)
{
	UOSInt i;
	if (this->color != color)
		return false;
	if (this->thick != thick)
		return false;
	UnsafeArray<UInt8> thisPattern;
	UnsafeArray<UInt8> nnpattern;
	if (this->nPattern != nPattern)
		return false;
	if (this->nPattern != 0)
	{
		if (this->pattern.SetTo(thisPattern) && pattern.SetTo(nnpattern))
		{
			i = this->nPattern;
			while (i-- > 0)
			{
				if (nnpattern[i] != thisPattern[i])
					return false;
			}
		}
		else
		{
			return false;
		}
	}
	return true;
}

UOSInt Media::VectorGraph::VectorPenStyle::GetIndex()
{
	return this->index;
}

NN<Media::DrawPen> Media::VectorGraph::VectorPenStyle::CreateDrawPen(Double oriDPI, NN<Media::DrawImage> dimg)
{
	Double thick = this->thick * dimg->GetHDPI() / oriDPI;
	return dimg->NewPenARGB(this->color, thick, this->pattern, this->nPattern);
}

Media::VectorGraph::VectorFontStyle::VectorFontStyle(UOSInt index, Text::CStringNN name, Double heightPt, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage)
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

Bool Media::VectorGraph::VectorFontStyle::IsSame(Text::CStringNN name, Double heightPt, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage)
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

NN<Text::String> Media::VectorGraph::VectorFontStyle::GetName() const
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

NN<Media::DrawFont> Media::VectorGraph::VectorFontStyle::CreateDrawFont(Double oriDPI, NN<Media::DrawImage> dimg)
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

NN<Media::DrawBrush> Media::VectorGraph::VectorBrushStyle::CreateDrawBrush(Double oriDPI, NN<Media::DrawImage> dimg)
{
	return dimg->NewBrushARGB(this->color);
}

Media::VectorGraph::VectorGraph(UInt32 srid, Double width, Double height, Math::Unit::Distance::DistanceUnit unit, NN<Media::DrawEngine> refEng, NN<const Media::ColorProfile> colorProfile) : colorProfile(colorProfile)
{
	this->size = Math::Size2DDbl(width, height);
	this->unit = unit;
	this->align = Media::DrawEngine::DRAW_POS_TOPLEFT;
	this->refEng = refEng;
	this->colorSess = colorSess;
}

Media::VectorGraph::~VectorGraph()
{
	this->penStyles.DeleteAll();
	this->fontStyles.DeleteAll();
	this->brushStyles.DeleteAll();
	this->itemStyle.MemFreeAll();
	this->items.DeleteAll();
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

NN<const Media::ColorProfile> Media::VectorGraph::GetColorProfile() const
{
	return this->colorProfile;
}

void Media::VectorGraph::SetColorProfile(NN<const Media::ColorProfile> color)
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

UnsafeArrayOpt<UInt8> Media::VectorGraph::GetImgBits(OutParam<Bool> revOrder)
{
	return 0;
}

void Media::VectorGraph::GetImgBitsEnd(Bool modified)
{
}

Optional<Media::EXIFData> Media::VectorGraph::GetEXIF() const
{
	return 0;
}

Media::PixelFormat Media::VectorGraph::GetPixelFormat() const
{
	return Media::PF_UNKNOWN;
}

void Media::VectorGraph::SetColorSess(Optional<Media::ColorSess> colorSess)
{
	this->colorSess = colorSess;
}

UOSInt Media::VectorGraph::GetImgBpl() const
{
	return 0;
}

Bool Media::VectorGraph::DrawLine(Double x1, Double y1, Double x2, Double y2, NN<DrawPen> p)
{
	Math::Coord2DDbl pt[2];
	pt[0] = Math::Coord2DDbl(x1, y1);
	pt[1] = Math::Coord2DDbl(x2, y2);
	NN<Math::Geometry::LineString> pl;
	NN<VectorStyles> style;
	NEW_CLASSNN(pl, Math::Geometry::LineString(this->srid, pt, 2, 0, 0));
	style = MemAllocNN(VectorStyles);
	style->pen = (Media::VectorGraph::VectorPenStyle*)p.Ptr();
	style->brush = 0;
	style->font = 0;
	this->items.Add(pl);
	this->itemStyle.Add(style);
	return true;
}

Bool Media::VectorGraph::DrawPolylineI(UnsafeArray<const Int32> points, UOSInt nPoints, NN<DrawPen> p)
{
	Double *dPoints = MemAlloc(Double, nPoints * 2);
	Math_Int32Arr2DblArr(dPoints, points.Ptr(), nPoints * 2);
	NN<Math::Geometry::LineString> pl;
	NN<VectorStyles> style;
	NEW_CLASSNN(pl, Math::Geometry::LineString(this->srid, (Math::Coord2DDbl*)dPoints, nPoints, 0, 0));
	style = MemAllocNN(VectorStyles);
	style->pen = (VectorPenStyle*)p.Ptr();
	style->brush = 0;
	style->font = 0;
	this->items.Add(pl);
	this->itemStyle.Add(style);
	MemFree(dPoints);
	return true;
}

Bool Media::VectorGraph::DrawPolygonI(UnsafeArray<const Int32> points, UOSInt nPoints, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	/////////////////////////////////
	return false;
}

Bool Media::VectorGraph::DrawPolyPolygonI(UnsafeArray<const Int32> points, UnsafeArray<const UInt32> pointCnt, UOSInt nPointCnt, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	/////////////////////////////////
	return false;
}

Bool Media::VectorGraph::DrawPolyline(UnsafeArray<const Math::Coord2DDbl> points, UOSInt nPoints, NN<DrawPen> p)
{
	NN<Math::Geometry::LineString> pl;
	NN<VectorStyles> style;
	NEW_CLASSNN(pl, Math::Geometry::LineString(this->srid, points, nPoints, 0, 0));
	style = MemAllocNN(VectorStyles);
	style->pen = (VectorPenStyle*)p.Ptr();
	style->brush = 0;
	style->font = 0;
	this->items.Add(pl);
	this->itemStyle.Add(style);
	return true;
}

Bool Media::VectorGraph::DrawPolygon(UnsafeArray<const Math::Coord2DDbl> points, UOSInt nPoints, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	/////////////////////////////////
	return false;
}

Bool Media::VectorGraph::DrawPolyPolygon(UnsafeArray<const Math::Coord2DDbl> points, UnsafeArray<const UInt32> pointCnt, UOSInt nPointCnt, Optional<DrawPen> p, Optional<DrawBrush> b)
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
	NN<VectorStyles> style;
	NN<Math::Geometry::Ellipse> vstr;
	NEW_CLASSNN(vstr, Math::Geometry::Ellipse(this->srid, tl, size));
	style = MemAllocNN(VectorStyles);
	style->pen = Optional<Media::VectorGraph::VectorPenStyle>::ConvertFrom(p);
	style->brush = Optional<VectorBrushStyle>::ConvertFrom(b);
	style->font = 0;
	this->items.Add(vstr);
	this->itemStyle.Add(style);
	return true;
}

Bool Media::VectorGraph::DrawString(Math::Coord2DDbl tl, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b)
{
	NN<VectorStyles> style;
	NN<Math::Geometry::VectorString> vstr;
	NEW_CLASSNN(vstr, Math::Geometry::VectorString(this->srid, str, tl, 0, 0, this->align));
	style = MemAllocNN(VectorStyles);
	style->pen = 0;
	style->brush = NN<VectorBrushStyle>::ConvertFrom(b);
	style->font = NN<VectorFontStyle>::ConvertFrom(f);
	this->items.Add(vstr);
	this->itemStyle.Add(style);
	return true;
}

Bool Media::VectorGraph::DrawString(Math::Coord2DDbl tl, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b)
{
	NN<VectorStyles> style;
	NN<Math::Geometry::VectorString> vstr;
	NEW_CLASSNN(vstr, Math::Geometry::VectorString(this->srid, str, tl, 0, 0, this->align));
	style = MemAllocNN(VectorStyles);
	style->pen = 0;
	style->brush = NN<VectorBrushStyle>::ConvertFrom(b);
	style->font = NN<VectorFontStyle>::ConvertFrom(f);
	this->items.Add(vstr);
	this->itemStyle.Add(style);
	return true;
}

Bool Media::VectorGraph::DrawStringRot(Math::Coord2DDbl center, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegree)
{
	NN<VectorStyles> style;
	NN<Math::Geometry::VectorString> vstr;
	NEW_CLASSNN(vstr, Math::Geometry::VectorString(this->srid, str, center, angleDegree, 0, this->align));
	style = MemAllocNN(VectorStyles);
	style->pen = 0;
	style->brush = NN<VectorBrushStyle>::ConvertFrom(b);
	style->font = NN<VectorFontStyle>::ConvertFrom(f);
	this->items.Add(vstr);
	this->itemStyle.Add(style);
	return true;
}

Bool Media::VectorGraph::DrawStringRot(Math::Coord2DDbl center, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegree)
{
	NN<VectorStyles> style;
	NN<Math::Geometry::VectorString> vstr;
	NEW_CLASSNN(vstr, Math::Geometry::VectorString(this->srid, str, center, angleDegree, 0, this->align));
	style = MemAllocNN(VectorStyles);
	style->pen = 0;
	style->brush = NN<VectorBrushStyle>::ConvertFrom(b);
	style->font = NN<VectorFontStyle>::ConvertFrom(f);
	this->items.Add(vstr);
	this->itemStyle.Add(style);
	return true;
}

Bool Media::VectorGraph::DrawStringB(Math::Coord2DDbl tl, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b, UOSInt buffSize)
{
	NN<VectorStyles> style;
	NN<Math::Geometry::VectorString> vstr;
	NEW_CLASSNN(vstr, Math::Geometry::VectorString(this->srid, str, tl, 0, UOSInt2Double(buffSize), this->align));
	style = MemAllocNN(VectorStyles);
	style->pen = 0;
	style->brush = NN<VectorBrushStyle>::ConvertFrom(b);
	style->font = NN<VectorFontStyle>::ConvertFrom(f);
	this->items.Add(vstr);
	this->itemStyle.Add(style);
	return true;
}

Bool Media::VectorGraph::DrawStringB(Math::Coord2DDbl tl, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b, UOSInt buffSize)
{
	NN<VectorStyles> style;
	NN<Math::Geometry::VectorString> vstr;
	NEW_CLASSNN(vstr, Math::Geometry::VectorString(this->srid, str, tl, 0, UOSInt2Double(buffSize), this->align));
	style = MemAllocNN(VectorStyles);
	style->pen = 0;
	style->brush = NN<VectorBrushStyle>::ConvertFrom(b);
	style->font = NN<VectorFontStyle>::ConvertFrom(f);
	this->items.Add(vstr);
	this->itemStyle.Add(style);
	return true;
}

Bool Media::VectorGraph::DrawStringRotB(Math::Coord2DDbl center, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegree, UOSInt buffSize)
{
	NN<VectorStyles> style;
	NN<Math::Geometry::VectorString> vstr;
	NEW_CLASSNN(vstr, Math::Geometry::VectorString(this->srid, str, center, angleDegree, UOSInt2Double(buffSize), this->align));
	style = MemAllocNN(VectorStyles);
	style->pen = 0;
	style->brush = NN<VectorBrushStyle>::ConvertFrom(b);
	style->font = NN<VectorFontStyle>::ConvertFrom(f);
	this->items.Add(vstr);
	this->itemStyle.Add(style);
	return true;
}

Bool Media::VectorGraph::DrawStringRotB(Math::Coord2DDbl center, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegree, UOSInt buffSize)
{
	NN<VectorStyles> style;
	NN<Math::Geometry::VectorString> vstr;
	NEW_CLASSNN(vstr, Math::Geometry::VectorString(this->srid, str, center, angleDegree, UOSInt2Double(buffSize), this->align));
	style = MemAllocNN(VectorStyles);
	style->pen = 0;
	style->brush = NN<VectorBrushStyle>::ConvertFrom(b);
	style->font = NN<VectorFontStyle>::ConvertFrom(f);
	this->items.Add(vstr);
	this->itemStyle.Add(style);
	return true;
}

Bool Media::VectorGraph::DrawImagePt(NN<DrawImage> img, Math::Coord2DDbl tl)
{
	NN<VectorStyles> style;
	NN<Media::StaticImage> stImg;
	NN<Math::Geometry::VectorImage> vimg;
	NN<Media::ImageList> imgList;
	if (!img->ToStaticImage().SetTo(stImg))
		return false;
	NEW_CLASSNN(imgList, Media::ImageList(CSTR("VectorGraphImage")));
	imgList->AddImage(stImg, 0);
	Media::SharedImage simg(imgList, 0);
	NEW_CLASSNN(vimg, Math::Geometry::VectorImage(this->srid, simg, tl, Math::Coord2DDbl(tl.x + UOSInt2Double(stImg->info.dispSize.x) * this->GetHDPI() / stImg->info.hdpi, tl.y + UOSInt2Double(stImg->info.dispSize.y) * stImg->info.par2 * this->GetVDPI() / stImg->info.vdpi), true, CSTR_NULL, 0, 0));
	style = MemAllocNN(VectorStyles);
	style->pen = 0;
	style->brush = 0;
	style->font = 0;
	this->items.Add(vimg);
	this->itemStyle.Add(style);
	return true;
}

Bool Media::VectorGraph::DrawImagePt2(NN<Media::StaticImage> img, Math::Coord2DDbl tl)
{
	NN<VectorStyles> style;
	NN<Math::Geometry::VectorImage> vimg;
	NN<Media::ImageList> imgList;
	NEW_CLASSNN(imgList, Media::ImageList(CSTR("VectorGraphImage")));
	imgList->AddImage(img->CreateStaticImage(), 0);
	Media::SharedImage simg(imgList, 0);
	NEW_CLASSNN(vimg, Math::Geometry::VectorImage(this->srid, simg, tl, Math::Coord2DDbl(tl.x + UOSInt2Double(img->info.dispSize.x) * this->GetHDPI() / img->info.hdpi, tl.y + UOSInt2Double(img->info.dispSize.y) * img->info.par2 * this->GetVDPI() / img->info.vdpi), true, CSTR_NULL, 0, 0));
	style = MemAllocNN(VectorStyles);
	style->pen = 0;
	style->brush = 0;
	style->font = 0;
	this->items.Add(vimg);
	this->itemStyle.Add(style);
	return true;
}

Bool Media::VectorGraph::DrawImagePt3(NN<DrawImage> img, Math::Coord2DDbl destTL, Math::Coord2DDbl srcTL, Math::Size2DDbl srcSize)
{
	NN<VectorStyles> style;
	NN<Media::StaticImage> stImg;
	NN<Math::Geometry::VectorImage> vimg;
	NN<Media::ImageList> imgList;
	if (!img->ToStaticImage().SetTo(stImg))
		return false;
	NEW_CLASSNN(imgList, Media::ImageList(CSTR("VectorGraphImage")));
	imgList->AddImage(stImg, 0);
	Media::SharedImage simg(imgList, 0);
	NEW_CLASSNN(vimg, Math::Geometry::VectorImage(this->srid, simg, destTL, Math::Coord2DDbl(destTL.x + UOSInt2Double(stImg->info.dispSize.x) * this->GetHDPI() / stImg->info.hdpi, destTL.y + UOSInt2Double(stImg->info.dispSize.y) * stImg->info.par2 * this->GetVDPI() / stImg->info.vdpi), true, CSTR_NULL, 0, 0));
	style = MemAllocNN(VectorStyles);
	style->pen = 0;
	style->brush = 0;
	style->font = 0;
	this->items.Add(vimg);
	this->itemStyle.Add(style);
	return true;
}

Bool Media::VectorGraph::DrawImageQuad(NN<Media::StaticImage> img, Math::Quadrilateral quad)
{
	return false;
}

NN<Media::DrawPen> Media::VectorGraph::NewPenARGB(UInt32 color, Double thick, UnsafeArrayOpt<UInt8> pattern, UOSInt nPattern)
{
	NN<Media::VectorGraph::VectorPenStyle> pen;
	Data::ArrayIterator<NN<Media::VectorGraph::VectorPenStyle>> it = this->penStyles.Iterator();
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

NN<Media::DrawBrush> Media::VectorGraph::NewBrushARGB(UInt32 color)
{
	NN<Media::VectorGraph::VectorBrushStyle> brush;
	Data::ArrayIterator<NN<Media::VectorGraph::VectorBrushStyle>> it = this->brushStyles.Iterator();
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

NN<Media::DrawFont> Media::VectorGraph::NewFontPt(Text::CStringNN name, Double ptSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage)
{
	NN<Media::VectorGraph::VectorFontStyle> font;
	Data::ArrayIterator<NN<VectorFontStyle>> it = this->fontStyles.Iterator();
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

NN<Media::DrawFont> Media::VectorGraph::NewFontPx(Text::CStringNN name, Double pxSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage)
{
	NN<Media::VectorGraph::VectorFontStyle> font;
	Double ptSize = pxSize * 96.0 / 72.0;
	Data::ArrayIterator<NN<VectorFontStyle>> it = this->fontStyles.Iterator();
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

NN<Media::DrawFont> Media::VectorGraph::CloneFont(NN<Media::DrawFont> f)
{
	NN<Media::VectorGraph::VectorFontStyle> font = NN<Media::VectorGraph::VectorFontStyle>::ConvertFrom(f);
	NN<Text::String> fontName = font->GetName();
	NEW_CLASSNN(font, Media::VectorGraph::VectorFontStyle(this->fontStyles.GetCount(), fontName->ToCString(), font->GetHeightPt(), font->GetStyle(), font->GetCodePage()));
	this->fontStyles.Add(font);
	return font;
}

void Media::VectorGraph::DelPen(NN<DrawPen> p)
{
}

void Media::VectorGraph::DelBrush(NN<DrawBrush> b)
{
}

void Media::VectorGraph::DelFont(NN<DrawFont> f)
{
}

Math::Size2DDbl Media::VectorGraph::GetTextSize(NN<DrawFont> fnt, Text::CStringNN txt)
{
	NN<Media::DrawImage> tmpImg;
	if (this->refEng->CreateImage32(Math::Size2D<UOSInt>(16, 16), Media::AT_ALPHA_ALL_FF).SetTo(tmpImg))
	{
		tmpImg->SetHDPI(this->GetHDPI());
		tmpImg->SetVDPI(this->GetVDPI());
		Math::Size2DDbl sz;
		NN<Media::DrawFont> f;
		NN<Media::VectorGraph::VectorFontStyle> fntStyle = NN<Media::VectorGraph::VectorFontStyle>::ConvertFrom(fnt);
		Double fntSizePt = fntStyle->GetHeightPt();
		if (fntSizePt < 100)
		{
			NN<Text::String> fontName = fntStyle->GetName();
			f = tmpImg->NewFontPt(fontName->ToCString(), 100, fntStyle->GetStyle(), fntStyle->GetCodePage());
			sz = tmpImg->GetTextSize(f, txt);
			tmpImg->DelFont(f);
			sz.x *= fntSizePt / 100.0;
			sz.y *= fntSizePt / 100.0;
		}
		else
		{
			NN<Text::String> fontName = fntStyle->GetName();
			f = tmpImg->NewFontPt(fontName->ToCString(), fntSizePt, fntStyle->GetStyle(), fntStyle->GetCodePage());
			sz = tmpImg->GetTextSize(f, txt);
			tmpImg->DelFont(f);
		}
		this->refEng->DeleteImage(tmpImg);
		return sz;
	}
	else
	{
		NN<Media::VectorGraph::VectorFontStyle> fntStyle = NN<Media::VectorGraph::VectorFontStyle>::ConvertFrom(fnt);
		Double fntSizePt = fntStyle->GetHeightPt();
		return Math::Size2DDbl(UOSInt2Double(txt.leng) * fntSizePt * 0.5, fntSizePt);
	}
}

void Media::VectorGraph::SetTextAlign(Media::DrawEngine::DrawPos pos)
{
	this->align = pos;
}

void Media::VectorGraph::GetStringBound(UnsafeArray<Int32> pos, OSInt centX, OSInt centY, UnsafeArray<const UTF8Char> str, NN<DrawFont> f, OutParam<OSInt> drawX, OutParam<OSInt> drawY)
{
	////////////////////////////////////////
}

void Media::VectorGraph::GetStringBoundRot(UnsafeArray<Int32> pos, Double centX, Double centY, UnsafeArray<const UTF8Char> str, NN<DrawFont> f, Double angleDegree, OutParam<OSInt> drawX, OutParam<OSInt> drawY)
{
	////////////////////////////////////////
}

void Media::VectorGraph::CopyBits(OSInt x, OSInt y, UnsafeArray<UInt8> imgPtr, UOSInt bpl, UOSInt width, UOSInt height, Bool upsideDown) const
{
}

Optional<Media::StaticImage> Media::VectorGraph::ToStaticImage() const
{
	return 0;
}

UOSInt Media::VectorGraph::SavePng(NN<IO::SeekableStream> stm)
{
	return 0;
}

UOSInt Media::VectorGraph::SaveGIF(NN<IO::SeekableStream> stm)
{
	return 0;
}

UOSInt Media::VectorGraph::SaveJPG(NN<IO::SeekableStream> stm)
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

void Media::VectorGraph::DrawTo(NN<Media::DrawImage> dimg, OptOut<UInt32> imgDurMS)
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
	NN<VectorPenStyle> pen;
	NN<VectorFontStyle> font;
	NN<VectorBrushStyle> brush;
	NN<VectorStyles> styles;
	NN<Math::Geometry::Vector2D> vec;
	NN<Media::DrawPen> p;
	Optional<Media::DrawPen> dp;
	NN<Media::DrawBrush> b;
	Optional<Media::DrawBrush> ob;
	NN<Media::DrawFont> f;
	Optional<Media::DrawFont> df;
	Data::ArrayIterator<NN<VectorFontStyle>> itFont = this->fontStyles.Iterator();
	while (itFont.HasNext())
	{
		font = itFont.Next();
		f = font->CreateDrawFont(dpi, dimg);
		dfonts.Add(f);
	}
	Data::ArrayIterator<NN<VectorPenStyle>> itPen = this->penStyles.Iterator();
	while (itPen.HasNext())
	{
		pen = itPen.Next();
		p = pen->CreateDrawPen(dpi, dimg);
		dpens.Add(p);
	}
	Data::ArrayIterator<NN<VectorBrushStyle>> itBrush = this->brushStyles.Iterator();
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
		vec = this->items.GetItemNoCheck(i);
		styles = this->itemStyle.GetItemNoCheck(i);
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
		NN<VectorBrushStyle> brush;
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
				NN<Math::Geometry::Polyline> pl = NN<Math::Geometry::Polyline>::ConvertFrom(vec);
				UOSInt nPoints;
				Math::Coord2DDbl *points;
				UnsafeArray<Math::Coord2DDbl> dpoints;
				NN<Math::Geometry::LineString> lineString;
				Data::ArrayIterator<NN<Math::Geometry::LineString>> it = pl->Iterator();
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
				NN<Math::Geometry::LineString> lineString = NN<Math::Geometry::LineString>::ConvertFrom(vec);
				UOSInt nPoints;
				Math::Coord2DDbl *points;
				UnsafeArray<Math::Coord2DDbl> dpoints;
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
			NN<Math::Geometry::VectorString> vstr = NN<Math::Geometry::VectorString>::ConvertFrom(vec);
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
			NN<Math::Geometry::VectorImage> vimg = NN<Math::Geometry::VectorImage>::ConvertFrom(vec);
			Math::RectAreaDbl bounds;
			UInt32 thisTimeMS;
			bounds = vimg->GetBounds();
			NN<Media::StaticImage> simg;
			if (vimg->GetImage(thisTimeMS).SetTo(simg))
			{
				dimg->DrawImagePt2(simg, bounds.min * scale);
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
			NN<Math::Geometry::Ellipse> ellipse = NN<Math::Geometry::Ellipse>::ConvertFrom(vec);
			Math::RectAreaDbl bounds = ellipse->GetBounds();
			bounds = bounds * scale;
			dimg->DrawEllipse(bounds.min, bounds.GetSize(), dp, ob);
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
