#include "Stdafx.h"
#include "Exporter/PNGExporter.h"
#include "IO/MemoryStream.h"
#include "Media/StaticImage.h"
#include "Media/SVGDocument.h"
#include "Text/XML.h"
#include "Text/TextBinEnc/Base64Enc.h"

Media::SVGLine::SVGLine(Math::Coord2DDbl pt1, Math::Coord2DDbl pt2, NN<DrawPen> pen)
{
	this->pt1 = pt1;
	this->pt2 = pt2;
	this->pen = pen;
}

Media::SVGLine::~SVGLine()
{
}

void Media::SVGLine::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	sb->AppendC(UTF8STRC("<line x1=\""));
	sb->AppendDouble(this->pt1.x);
	sb->AppendC(UTF8STRC("\" y1=\""));
	sb->AppendDouble(this->pt1.y);
	sb->AppendC(UTF8STRC("\" x2=\""));
	sb->AppendDouble(this->pt2.x);
	sb->AppendC(UTF8STRC("\" y2=\""));
	sb->AppendDouble(this->pt2.y);
	SVGCore::WritePenStyle(sb, this->pen);
	this->AppendEleAttr(sb);
	sb->AppendC(UTF8STRC(" />"));
}

Media::SVGPolyline::SVGPolyline(NN<DrawPen> pen)
{
	this->pen = pen;
}

Media::SVGPolyline::~SVGPolyline()
{
}

void Media::SVGPolyline::AddPoint(Math::Coord2DDbl pt)
{
	this->points.Add(pt);
}

void Media::SVGPolyline::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	sb->AppendC(UTF8STRC("<polyline points=\""));
	if (this->points.GetCount() > 0)
	{
		UIntOS i = 1;
		sb->AppendDouble(this->points.GetItem(0).x);
		sb->AppendUTF8Char(',');
		sb->AppendDouble(this->points.GetItem(0).y);
		while (i < this->points.GetCount())
		{
			sb->AppendUTF8Char(' ');
			sb->AppendDouble(this->points.GetItem(i).x);
			sb->AppendUTF8Char(',');
			sb->AppendDouble(this->points.GetItem(i).y);
			i++;
		}
	}
	sb->AppendUTF8Char('\"');
	SVGCore::WritePenStyle(sb, this->pen);
	this->AppendEleAttr(sb);
	sb->AppendC(UTF8STRC(" />"));
}

Media::SVGPolygon::SVGPolygon(Optional<DrawPen> pen, Optional<DrawBrush> brush)
{
	this->pen = pen;
	this->brush = brush;
}

Media::SVGPolygon::~SVGPolygon()
{
}

void Media::SVGPolygon::AddPoint(Math::Coord2DDbl pt)
{
	this->points.Add(pt);
}

void Media::SVGPolygon::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	sb->AppendC(UTF8STRC("<polygon points=\""));
	if (this->points.GetCount() > 0)
	{
		UIntOS i = 1;
		sb->AppendDouble(this->points.GetItem(0).x);
		sb->AppendUTF8Char(',');
		sb->AppendDouble(this->points.GetItem(0).y);
		while (i < this->points.GetCount())
		{
			sb->AppendUTF8Char(' ');
			sb->AppendDouble(this->points.GetItem(i).x);
			sb->AppendUTF8Char(',');
			sb->AppendDouble(this->points.GetItem(i).y);
			i++;
		}
	}
	sb->AppendUTF8Char('\"');
	SVGCore::WritePenStyle(sb, this->pen);
	SVGCore::WriteBrushStyle(sb, this->brush);
	this->AppendEleAttr(sb);
	sb->AppendC(UTF8STRC(" />"));
}

Media::SVGRect::SVGRect(Math::Coord2DDbl tl, Math::Size2DDbl size, Optional<DrawPen> pen, Optional<DrawBrush> brush)
{
	this->tl = tl;
	this->size = size;
	this->pen = pen;
	this->brush = brush;
}

Media::SVGRect::~SVGRect()
{
}

void Media::SVGRect::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	sb->AppendC(UTF8STRC("<rect x=\""));
	sb->AppendDouble(this->tl.x);
	sb->AppendC(UTF8STRC("\" y=\""));
	sb->AppendDouble(this->tl.y);
	sb->AppendC(UTF8STRC("\" width=\""));
	sb->AppendDouble(this->size.x);
	sb->AppendC(UTF8STRC("\" height=\""));
	sb->AppendDouble(this->size.y);
	sb->AppendC(UTF8STRC("\""));
	SVGCore::WritePenStyle(sb, this->pen);
	SVGCore::WriteBrushStyle(sb, this->brush);
	this->AppendEleAttr(sb);
	sb->AppendC(UTF8STRC(" />"));
}

Media::SVGEllipse::SVGEllipse(Math::Coord2DDbl center, Math::Size2DDbl radius, Optional<DrawPen> pen, Optional<DrawBrush> brush)
{
	this->center = center;
	this->radius = radius;
	this->pen = pen;
	this->brush = brush;
}

Media::SVGEllipse::~SVGEllipse()
{
}

void Media::SVGEllipse::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	sb->AppendC(UTF8STRC("<ellipse cx=\""));
	sb->AppendDouble(this->center.x);
	sb->AppendC(UTF8STRC("\" cy=\""));
	sb->AppendDouble(this->center.y);
	sb->AppendC(UTF8STRC("\" rx=\""));
	sb->AppendDouble(this->radius.x);
	sb->AppendC(UTF8STRC("\" ry=\""));
	sb->AppendDouble(this->radius.y);
	SVGCore::WritePenStyle(sb, this->pen);
	SVGCore::WriteBrushStyle(sb, this->brush);
	this->AppendEleAttr(sb);
	sb->AppendC(UTF8STRC(" />"));
}

Media::SVGText::SVGText(Math::Coord2DDbl tl, Text::CStringNN txt, NN<DrawFont> font, NN<DrawBrush> brush)
{
	this->tl = tl;
	this->txt = Text::String::New(txt);
	this->font = font;
	this->brush = brush;
	this->angleDegreeACW = 0.0;
	this->rotateCenter = Math::Coord2DDbl(0, 0);
}

Media::SVGText::~SVGText()
{
	this->txt->Release();
}

void Media::SVGText::SetRotate(Double angleDegreeACW, Math::Coord2DDbl rotateCenter)
{
	this->angleDegreeACW = angleDegreeACW;
	this->rotateCenter = rotateCenter;
}

void Media::SVGText::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	sb->AppendC(UTF8STRC("<text x=\""));
	sb->AppendDouble(this->tl.x);
	sb->AppendC(UTF8STRC("\" y=\""));
	sb->AppendDouble(this->tl.y);
	sb->AppendC(UTF8STRC("\""));
	if (this->angleDegreeACW != 0.0)
	{
		sb->AppendC(UTF8STRC(" transform=\"rotate("));
		sb->AppendDouble(this->angleDegreeACW);
		sb->AppendC(UTF8STRC(","));
		sb->AppendDouble(this->rotateCenter.x);
		sb->AppendC(UTF8STRC(","));
		sb->AppendDouble(this->rotateCenter.y);
		sb->AppendC(UTF8STRC(")\""));
	}
	SVGCore::WriteFontStyle(sb, this->font);
	SVGCore::WriteBrushStyle(sb, this->brush);
	this->AppendEleAttr(sb);
	sb->AppendC(UTF8STRC(" >"));
	NN<Text::String> s = Text::XML::ToNewXMLText(this->txt->v);
	sb->Append(s);
	s->Release();
	sb->AppendC(UTF8STRC("</text>"));
}

Media::SVGImage::SVGImage(Math::Coord2DDbl tl, Math::Size2DDbl size, Text::CStringNN href)
{
	this->tl = tl;
	this->size = size;
	this->href = Text::String::New(href);
}

Media::SVGImage::~SVGImage()
{
	this->href->Release();
}

void Media::SVGImage::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	sb->AppendC(UTF8STRC("<image x=\""));
	sb->AppendDouble(this->tl.x);
	sb->AppendC(UTF8STRC("\" y=\""));
	sb->AppendDouble(this->tl.y);
	sb->AppendC(UTF8STRC("\" width=\""));
	sb->AppendDouble(this->size.x);
	sb->AppendC(UTF8STRC("\" height=\""));
	sb->AppendDouble(this->size.y);
	sb->AppendC(UTF8STRC("\" href="));
	NN<Text::String> s = Text::XML::ToNewAttrText(this->href->v);
	sb->Append(s);
	s->Release();
	this->AppendEleAttr(sb);
	sb->AppendC(UTF8STRC(" />"));
}

void Media::SVGContainer::ToInnerString(NN<Text::StringBuilderUTF8> sb) const
{
	UIntOS i = 0;
	UIntOS j = this->elements.GetCount();
	while (i < j)
	{
		this->elements.GetItemNoCheck(i)->ToString(sb);
		sb->AppendUTF8Char('\n');
		i++;
	}
}

Media::SVGContainer::SVGContainer(NN<Media::DrawEngine> refEng, NN<SVGDocument> doc)
{
	this->refEng = refEng;
	this->doc = doc;
}

Media::SVGContainer::~SVGContainer()
{
	this->elements.DeleteAll();
}

Math::Size2D<UIntOS> Media::SVGContainer::GetSize() const
{
	return Math::Size2D<UIntOS>(this->GetWidth(), this->GetHeight());
}

UInt32 Media::SVGContainer::GetBitCount() const
{
	return 32;
}

NN<const Media::ColorProfile> Media::SVGContainer::GetColorProfile() const
{
	return this->doc->GetColorProfile();
}

void Media::SVGContainer::SetColorProfile(NN<const Media::ColorProfile> color)
{
	this->doc->SetColorProfile(color);
}
Media::AlphaType Media::SVGContainer::GetAlphaType() const
{
	return Media::AT_ALPHA;
}
void Media::SVGContainer::SetAlphaType(Media::AlphaType atype)
{
}

Double Media::SVGContainer::GetHDPI() const
{
	return 96.0;
}

Double Media::SVGContainer::GetVDPI() const
{
	return 96.0;
}
void Media::SVGContainer::SetHDPI(Double dpi)
{
}

void Media::SVGContainer::SetVDPI(Double dpi)
{
}

UnsafeArrayOpt<UInt8> Media::SVGContainer::GetImgBits(OutParam<Bool> revOrder)
{
	return nullptr;
}
void Media::SVGContainer::GetImgBitsEnd(Bool modified)
{
}

UIntOS Media::SVGContainer::GetImgBpl() const
{
	return this->GetWidth() * 4;
}

Optional<Media::EXIFData> Media::SVGContainer::GetEXIF() const
{
	return nullptr;
}
Media::PixelFormat Media::SVGContainer::GetPixelFormat() const
{
	return Media::PF_B8G8R8A8;
}

void Media::SVGContainer::SetColorSess(Optional<Media::ColorSess> colorSess)
{
}

Bool Media::SVGContainer::DrawLine(Double x1, Double y1, Double x2, Double y2, NN<DrawPen> p)
{
	NN<SVGLine> line;
	NEW_CLASSNN(line, SVGLine(Math::Coord2DDbl(x1, y1), Math::Coord2DDbl(x2, y2), p));
	this->elements.Add(line);
	return true;
}

Bool Media::SVGContainer::DrawPolylineI(UnsafeArray<const Int32> points, UIntOS nPoints, NN<DrawPen> p)
{
	NN<SVGPolyline> pl;
	NEW_CLASSNN(pl, SVGPolyline(p));
	UIntOS i = 0;
	while (i < nPoints * 2)
	{
		pl->AddPoint(Math::Coord2DDbl(points[i], points[i + 1]));
		i += 2;
	}
	this->elements.Add(pl);
	return true;
}

Bool Media::SVGContainer::DrawPolygonI(UnsafeArray<const Int32> points, UIntOS nPoints, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	NN<SVGPolygon> pg;
	NEW_CLASSNN(pg, SVGPolygon(p, b));
	UIntOS i = 0;
	while (i < nPoints * 2)
	{
		pg->AddPoint(Math::Coord2DDbl(points[i], points[i + 1]));
		i += 2;
	}
	this->elements.Add(pg);
	return true;
}

Bool Media::SVGContainer::DrawPolyPolygonI(UnsafeArray<const Int32> points, UnsafeArray<const UInt32> pointCnt, UIntOS nPointCnt, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	if (nPointCnt == 1)
	{
		return this->DrawPolygonI(points, pointCnt[0], p, b);
	}
	/////////////////////////////
	return false;
}

Bool Media::SVGContainer::DrawPolyline(UnsafeArray<const Math::Coord2DDbl> points, UIntOS nPoints, NN<DrawPen> p)
{
	NN<SVGPolyline> pl;
	NEW_CLASSNN(pl, SVGPolyline(p));
	UIntOS i = 0;
	while (i < nPoints)
	{
		pl->AddPoint(points[i]);
		i++;
	}
	this->elements.Add(pl);
	return true;
}

Bool Media::SVGContainer::DrawPolygon(UnsafeArray<const Math::Coord2DDbl> points, UIntOS nPoints, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	NN<SVGPolygon> pg;
	NEW_CLASSNN(pg, SVGPolygon(p, b));
	UIntOS i = 0;
	while (i < nPoints)
	{
		pg->AddPoint(points[i]);
		i++;
	}
	this->elements.Add(pg);
	return true;
}

Bool Media::SVGContainer::DrawPolyPolygon(UnsafeArray<const Math::Coord2DDbl> points, UnsafeArray<const UInt32> pointCnt, UIntOS nPointCnt, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	if (nPointCnt == 1)
	{
		return this->DrawPolygon(points, pointCnt[0], p, b);
	}
	/////////////////////////////
	return false;
}

Bool Media::SVGContainer::DrawRect(Math::Coord2DDbl tl, Math::Size2DDbl size, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	NN<SVGRect> rect;
	NEW_CLASSNN(rect, SVGRect(tl, size, p, b));
	this->elements.Add(rect);
	return true;
}

Bool Media::SVGContainer::DrawEllipse(Math::Coord2DDbl tl, Math::Size2DDbl size, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	NN<SVGEllipse> ellipse;
	NEW_CLASSNN(ellipse, SVGEllipse(tl + size * 0.5, size * 0.5, p, b));
	this->elements.Add(ellipse);
	return true;
}

Bool Media::SVGContainer::DrawString(Math::Coord2DDbl tl, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b)
{
	return this->DrawString(tl, str->ToCString(), f, b);
}

Bool Media::SVGContainer::DrawString(Math::Coord2DDbl tl, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b)
{
	NN<SVGText> text;
	NEW_CLASSNN(text, SVGText(tl, str, f, b));
	this->elements.Add(text);
	return true;
}

Bool Media::SVGContainer::DrawStringRot(Math::Coord2DDbl center, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegreeACW)
{
	return this->DrawStringRot(center, str->ToCString(), f, b, angleDegreeACW);
}

Bool Media::SVGContainer::DrawStringRot(Math::Coord2DDbl center, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegreeACW)
{
	NN<SVGText> text;
	NEW_CLASSNN(text, SVGText(center, str, f, b));
	text->SetRotate(angleDegreeACW, center);
	this->elements.Add(text);
	return true;
}

Bool Media::SVGContainer::DrawStringB(Math::Coord2DDbl tl, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b, UIntOS buffSize)
{
	return this->DrawStringB(tl, str->ToCString(), f, b, buffSize);
}

Bool Media::SVGContainer::DrawStringB(Math::Coord2DDbl tl, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b, UIntOS buffSize)
{
	return this->DrawString(tl, str, f, b);
}

Bool Media::SVGContainer::DrawStringRotB(Math::Coord2DDbl center, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegreeACW, UIntOS buffSize)
{
	return this->DrawStringRotB(center, str->ToCString(), f, b, angleDegreeACW, buffSize);
}

Bool Media::SVGContainer::DrawStringRotB(Math::Coord2DDbl center, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegreeACW, UIntOS buffSize)
{
	return this->DrawStringRot(center, str, f, b, angleDegreeACW);
}

Bool Media::SVGContainer::DrawImagePt(NN<DrawImage> img, Math::Coord2DDbl tl)
{
	NN<Media::StaticImage> simg;
	if (img->ToStaticImage().SetTo(simg))
	{
		Bool succ = this->DrawSImagePt(simg, tl);
		simg.Delete();
		return succ;
	}
	return false;
}

Bool Media::SVGContainer::DrawImagePt2(NN<DrawImage> img, Math::Coord2DDbl destTL, Math::Coord2DDbl srcTL, Math::Size2DDbl srcSize)
{
	NN<Media::StaticImage> simg;
	if (img->ToStaticImage().SetTo(simg))
	{
		Bool succ = this->DrawSImagePt2(simg, destTL, srcTL, srcSize);
		simg.Delete();
		return succ;
	}
	return false;
}

Bool Media::SVGContainer::DrawSImagePt(NN<Media::StaticImage> img, Math::Coord2DDbl tl)
{
	Exporter::PNGExporter exporter;
	IO::MemoryStream memStm;
	if (!exporter.ExportImage(memStm, img))
	{
		return false;
	}
	Text::StringBuilderUTF8 sb;
	NN<SVGImage> svgImg;
	sb.AppendC(UTF8STRC("data:image/png;base64,"));
	Text::TextBinEnc::Base64Enc b64;
	b64.EncodeBin(sb, memStm.GetBuff(), (UIntOS)memStm.GetLength());
	NEW_CLASSNN(svgImg, SVGImage(tl, Math::Size2DDbl(img->info.dispSize.x * 96.0 / img->info.hdpi, img->info.dispSize.y * 96.0 / img->info.vdpi), sb.ToCString()));
	this->elements.Add(svgImg);
	return true;
}

Bool Media::SVGContainer::DrawSImagePt2(NN<Media::StaticImage> img, Math::Coord2DDbl destTL, Math::Coord2DDbl srcTL, Math::Size2DDbl srcSize)
{
	Int32 x = Double2Int32(srcTL.x);
	Int32 y = Double2Int32(srcTL.y);
	Int32 w = Double2Int32(srcSize.x);
	Int32 h = Double2Int32(srcSize.y);
	NN<Media::StaticImage> subImg = img->CreateSubImage(Math::RectArea<IntOS>(x, y, x + w, y + h));
	Bool succ = this->DrawSImagePt(subImg, destTL);
	subImg.Delete();
	return succ;
}

Bool Media::SVGContainer::DrawImageQuad(NN<Media::StaticImage> img, Math::Quadrilateral quad)
{
	/////////////////////////////
	return false;
}

NN<Media::DrawPen> Media::SVGContainer::NewPenARGB(UInt32 color, Double thick, UnsafeArrayOpt<UInt8> pattern, UIntOS nPattern)
{
	return this->doc->NewPenARGB(color, thick, pattern, nPattern);
}

NN<Media::DrawBrush> Media::SVGContainer::NewBrushARGB(UInt32 color)
{
	return this->doc->NewBrushARGB(color);
}

NN<Media::DrawFont> Media::SVGContainer::NewFontPt(Text::CStringNN name, Double ptSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage)
{
	return this->doc->NewFontPt(name, ptSize, fontStyle, codePage);
}

NN<Media::DrawFont> Media::SVGContainer::NewFontPx(Text::CStringNN name, Double pxSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage)
{
	return this->doc->NewFontPx(name, pxSize, fontStyle, codePage);
}

NN<Media::DrawFont> Media::SVGContainer::CloneFont(NN<Media::DrawFont> f)
{
	return this->doc->CloneFont(f);
}

void Media::SVGContainer::DelPen(NN<DrawPen> p)
{
}

void Media::SVGContainer::DelBrush(NN<DrawBrush> b)
{
}

void Media::SVGContainer::DelFont(NN<DrawFont> f)
{
}

Math::Size2DDbl Media::SVGContainer::GetTextSize(NN<DrawFont> fnt, Text::CStringNN txt)
{
	return SVGCore::GetTextSize(this->refEng, NN<Media::SVGFont>::ConvertFrom(fnt), txt);
}

void Media::SVGContainer::SetTextAlign(Media::DrawEngine::DrawPos pos)
{
}

void Media::SVGContainer::GetStringBound(UnsafeArray<Int32> pos, IntOS centX, IntOS centY, UnsafeArray<const UTF8Char> str, NN<DrawFont> f, OutParam<IntOS> drawX, OutParam<IntOS> drawY)
{
	SVGCore::GetStringBound(this->refEng, pos, centX, centY, str, NN<Media::SVGFont>::ConvertFrom(f), drawX, drawY);
}

void Media::SVGContainer::GetStringBoundRot(UnsafeArray<Int32> pos, Double centX, Double centY, UnsafeArray<const UTF8Char> str, NN<DrawFont> f, Double angleDegree, OutParam<IntOS> drawX, OutParam<IntOS> drawY)
{
	SVGCore::GetStringBoundRot(this->refEng, pos, centX, centY, str, NN<Media::SVGFont>::ConvertFrom(f), angleDegree, drawX, drawY);
}

void Media::SVGContainer::CopyBits(IntOS x, IntOS y, UnsafeArray<UInt8> imgPtr, UIntOS bpl, UIntOS width, UIntOS height, Bool upsideDown) const
{
}

Optional<Media::StaticImage> Media::SVGContainer::ToStaticImage() const
{
	return nullptr;
}

Optional<Media::RasterImage> Media::SVGContainer::AsRasterImage()
{
	return nullptr;
}

UIntOS Media::SVGContainer::SavePng(NN<IO::SeekableStream> stm)
{
	return 0;
}

UIntOS Media::SVGContainer::SaveGIF(NN<IO::SeekableStream> stm)
{
	return 0;
}

UIntOS Media::SVGContainer::SaveJPG(NN<IO::SeekableStream> stm)
{
	return 0;
}

Media::SVGDocument::SVGDocument(NN<Media::DrawEngine> refEng) : SVGContainer(refEng, *this)
{
	this->width = 0;
	this->height = 0;
	this->viewBox = Math::RectArea<IntOS>(0, 0, 0, 0);
}

Media::SVGDocument::~SVGDocument()
{
	this->fonts.DeleteAll();
	this->brushes.DeleteAll();
	this->pens.DeleteAll();
}

UIntOS Media::SVGDocument::GetWidth() const
{
	return this->width;
}

UIntOS Media::SVGDocument::GetHeight() const
{
	return this->height;
}

NN<Media::DrawPen> Media::SVGDocument::NewPenARGB(UInt32 color, Double thick, UnsafeArrayOpt<UInt8> pattern, UIntOS nPattern)
{
	NN<SVGPen> pen;
	UIntOS i = 0;
	UIntOS j = this->pens.GetCount();
	while (i < j)
	{
		pen = this->pens.GetItemNoCheck(i);
		if (pen->GetColor() == color && pen->GetThick() == thick)
		{
			return pen;
		}
		i++;
	}
	NEW_CLASSNN(pen, SVGPen(thick, color));
	this->pens.Add(pen);
	return pen;
}

NN<Media::DrawBrush> Media::SVGDocument::NewBrushARGB(UInt32 color)
{
	NN<SVGBrush> brush;
	UIntOS i = 0;
	UIntOS j = this->brushes.GetCount();
	while (i < j)
	{
		brush = this->brushes.GetItemNoCheck(i);
		if (brush->GetColor() == color)
		{
			return brush;
		}
		i++;
	}
	NEW_CLASSNN(brush, SVGBrush(color));
	this->brushes.Add(brush);
	return brush;
}

NN<Media::DrawFont> Media::SVGDocument::NewFontPt(Text::CStringNN name, Double ptSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage)
{
	return this->NewFontPx(name, ptSize * 96.0 / 72.0, fontStyle, codePage);
}

NN<Media::DrawFont> Media::SVGDocument::NewFontPx(Text::CStringNN name, Double pxSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage)
{
	NN<SVGFont> font;
	UIntOS i = 0;
	UIntOS j = this->fonts.GetCount();
	while (i < j)
	{
		font = this->fonts.GetItemNoCheck(i);
		if (font->GetFontName()->Equals(name) && font->GetFontSizePx() == pxSize && font->GetStyle() == fontStyle)
		{
			return font;
		}
		i++;
	}
	NEW_CLASSNN(font, SVGFont(name, pxSize, fontStyle));
	this->fonts.Add(font);
	return font;
}

NN<Media::DrawFont> Media::SVGDocument::CloneFont(NN<Media::DrawFont> f)
{
	return f;
}

void Media::SVGDocument::SetSize(UIntOS width, UIntOS height)
{
	this->width = width;
	this->height = height;
}

void Media::SVGDocument::SetViewBox(Math::RectArea<IntOS> viewBox)
{
	this->viewBox = viewBox;
}

void Media::SVGDocument::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	sb->AppendC(UTF8STRC("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"));
	sb->AppendC(UTF8STRC("<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\""));
	if (this->width > 0)
	{
		sb->AppendC(UTF8STRC(" width=\""));
		sb->AppendUIntOS(this->width);
		sb->AppendC(UTF8STRC("px\""));	
	}
	if (this->height > 0)
	{
		sb->AppendC(UTF8STRC(" height=\""));
		sb->AppendUIntOS(this->height);
		sb->AppendC(UTF8STRC("px\""));	
	}
	if (this->viewBox.GetWidth() > 0 && this->viewBox.GetHeight() > 0)
	{
		sb->AppendC(UTF8STRC(" viewBox=\""));
		sb->AppendIntOS(this->viewBox.min.x);
		sb->AppendC(UTF8STRC(" "));
		sb->AppendIntOS(this->viewBox.min.y);
		sb->AppendC(UTF8STRC(" "));
		sb->AppendIntOS(this->viewBox.GetWidth());
		sb->AppendC(UTF8STRC(" "));
		sb->AppendIntOS(this->viewBox.GetHeight());
		sb->AppendC(UTF8STRC("\""));
	}
	this->AppendEleAttr(sb);
	sb->AppendC(UTF8STRC(">\n"));
	this->ToInnerString(sb);
	sb->AppendC(UTF8STRC("</svg>"));
}
