#include "Stdafx.h"
#include "Exporter/PNGExporter.h"
#include "IO/MemoryStream.h"
#include "Media/StaticImage.h"
#include "Media/SVGWriter.h"
#include "Text/XML.h"
#include "Text/TextBinEnc/Base64Enc.h"

void Media::SVGWriter::WriteBuffer()
{
	if (this->sb.GetLength() > 16384)
	{
		this->stm->Write(sb.ToByteArray());
		this->sb.ClearStr();
	}
}

void Media::SVGWriter::WritePenStyle(Optional<DrawPen> p)
{
	NN<SVGPen> pen;
	if (Optional<SVGPen>::ConvertFrom(p).SetTo(pen))
	{
		this->sb.AppendC(UTF8STRC(" stroke=\"#"));
		UInt32 col = pen->GetColor();
		this->sb.AppendHex24(col & 0xffffff);
		if ((col & 0xff000000) != 0xff000000)
		{
			this->sb.AppendC(UTF8STRC("\" stroke-opacity=\""));
			this->sb.AppendDouble(((col >> 24) & 0xff) / 255.0);
		}
		this->sb.AppendC(UTF8STRC("\" stroke-width=\""));
		this->sb.AppendDouble(pen->GetThick());
		this->sb.AppendUTF8Char('\"');
	}
	else
	{
		this->sb.AppendC(UTF8STRC(" stroke=\"none\""));
	}
}

void Media::SVGWriter::WriteBrushStyle(Optional<DrawBrush> b)
{
	NN<SVGBrush> brush;
	if (Optional<SVGBrush>::ConvertFrom(b).SetTo(brush))
	{
		this->sb.AppendC(UTF8STRC(" fill=\"#"));
		UInt32 col = brush->GetColor();
		this->sb.AppendHex24(col & 0xffffff);
		if ((col & 0xff000000) != 0xff000000)
		{
			this->sb.AppendC(UTF8STRC("\" fill-opacity=\""));
			this->sb.AppendDouble(((col >> 24) & 0xff) / 255.0);
		}
		this->sb.AppendUTF8Char('\"');
	}
	else
	{
		this->sb.AppendC(UTF8STRC(" fill=\"none\""));
	}
}

void Media::SVGWriter::WriteFontStyle(NN<SVGFont> font)
{
	this->sb.AppendC(UTF8STRC(" font-family=\""));
	this->sb.Append(font->GetFontName());
	this->sb.AppendC(UTF8STRC("\" font-size=\""));
	this->sb.AppendDouble(font->GetFontSizePx());
	this->sb.AppendUTF8Char('\"');
	if ((font->GetStyle() & Media::DrawEngine::DFS_BOLD) != 0)
	{
		this->sb.AppendC(UTF8STRC(" font-weight=\"bold\""));
	}
	else
	{
		this->sb.AppendC(UTF8STRC(" font-weight=\"normal\""));
	}
	if ((font->GetStyle() & Media::DrawEngine::DFS_ITALIC) != 0)
	{
		this->sb.AppendC(UTF8STRC(" font-style=\"italic\""));
	}
	else
	{
		this->sb.AppendC(UTF8STRC(" font-style=\"normal\""));
	}
}

Media::SVGWriter::SVGWriter(NN<IO::Stream> stm, UIntOS width, UIntOS height, NN<Media::DrawEngine> refEng) : color(Media::ColorProfile::CPT_SRGB)
{
	this->size = Math::Size2D<UIntOS>(width, height);
	this->stm = stm;
	this->refEng = refEng;
	this->sb.AppendC(UTF8STRC("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"));
	this->sb.AppendC(UTF8STRC("<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\""));
	this->sb.AppendUIntOS(width);
	this->sb.AppendC(UTF8STRC("px\" height=\""));
	this->sb.AppendUIntOS(height);
	this->sb.AppendC(UTF8STRC("px\">\n"));
}

Media::SVGWriter::~SVGWriter()
{
	this->sb.AppendC(UTF8STRC("</svg>\n"));
	if (this->sb.GetLength() > 0)
	{
		this->stm->Write(sb.ToByteArray());
		this->sb.ClearStr();
	}
}

UIntOS Media::SVGWriter::GetWidth() const
{
	return this->size.x;
}

UIntOS Media::SVGWriter::GetHeight() const
{
	return this->size.y;
}

Math::Size2D<UIntOS> Media::SVGWriter::GetSize() const
{
	return this->size;
}

UInt32 Media::SVGWriter::GetBitCount() const
{
	return 32;
}

NN<const Media::ColorProfile> Media::SVGWriter::GetColorProfile() const
{
	return this->color;
}

void Media::SVGWriter::SetColorProfile(NN<const ColorProfile> color)
{
	this->color.Set(color);
}

Media::AlphaType Media::SVGWriter::GetAlphaType() const
{
	return Media::AT_ALPHA;
}

void Media::SVGWriter::SetAlphaType(Media::AlphaType atype)
{
}

Double Media::SVGWriter::GetHDPI() const
{
	return 96;
}

Double Media::SVGWriter::GetVDPI() const
{
	return 96;
}

void Media::SVGWriter::SetHDPI(Double dpi)
{
}

void Media::SVGWriter::SetVDPI(Double dpi)
{
}

UnsafeArrayOpt<UInt8> Media::SVGWriter::GetImgBits(OutParam<Bool> revOrder)
{
	return nullptr;
}

void Media::SVGWriter::GetImgBitsEnd(Bool modified)
{
}

UIntOS Media::SVGWriter::GetImgBpl() const
{
	return 0;
}

Optional<Media::EXIFData> Media::SVGWriter::GetEXIF() const
{
	return nullptr;
}

Media::PixelFormat Media::SVGWriter::GetPixelFormat() const
{
	return Media::PF_R8G8B8A8;
}

void Media::SVGWriter::SetColorSess(Optional<Media::ColorSess> colorSess)
{
}

Bool Media::SVGWriter::DrawLine(Double x1, Double y1, Double x2, Double y2, NN<DrawPen> p)
{
	this->sb.AppendC(UTF8STRC("<line x1=\""));
	this->sb.AppendDouble(x1);
	this->sb.AppendC(UTF8STRC("\" y1=\""));
	this->sb.AppendDouble(y1);
	this->sb.AppendC(UTF8STRC("\" x2=\""));
	this->sb.AppendDouble(x2);
	this->sb.AppendC(UTF8STRC("\" y2=\""));
	this->sb.AppendDouble(y2);
	this->sb.AppendUTF8Char('\"');
	this->WritePenStyle(p);
	this->sb.AppendC(UTF8STRC(" />\n"));
	this->WriteBuffer();
	return true;
}

Bool Media::SVGWriter::DrawPolylineI(UnsafeArray<const Int32> points, UIntOS nPoints, NN<DrawPen> p)
{
	this->sb.AppendC(UTF8STRC("<polyline points=\""));
	if (nPoints > 0)
	{
		UIntOS i = 1;
		this->sb.AppendI32(points[0]);
		while (i < nPoints * 2)
		{
			this->sb.AppendUTF8Char(' ');
			this->sb.AppendI32(points[i]);
			i++;
		}
	}
	this->sb.AppendUTF8Char('\"');
	this->WritePenStyle(p);
	this->sb.AppendC(UTF8STRC(" />\n"));
	this->WriteBuffer();
	return true;
}

Bool Media::SVGWriter::DrawPolygonI(UnsafeArray<const Int32> points, UIntOS nPoints, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	this->sb.AppendC(UTF8STRC("<polygon points=\""));
	if (nPoints > 0)
	{
		UIntOS i = 1;
		this->sb.AppendI32(points[0]);
		while (i < nPoints * 2)
		{
			this->sb.AppendUTF8Char(' ');
			this->sb.AppendI32(points[i]);
			i++;
		}
	}
	this->sb.AppendUTF8Char('\"');
	this->WritePenStyle(p);
	this->WriteBrushStyle(b);
	this->sb.AppendC(UTF8STRC(" />\n"));
	this->WriteBuffer();
	return true;
}

Bool Media::SVGWriter::DrawPolyPolygonI(UnsafeArray<const Int32> points, UnsafeArray<const UInt32> pointCnt, UIntOS nPointCnt, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	if (nPointCnt == 1)
	{
		return this->DrawPolygonI(points, pointCnt[0], p, b);
	}

	this->sb.AppendC(UTF8STRC("<path d=\""));
	UIntOS i = 0;
	UIntOS j = 0;
	while (i < nPointCnt)
	{
		this->sb.AppendUTF8Char('M');
		this->sb.AppendI32(points[j]);
		this->sb.AppendUTF8Char(' ');
		this->sb.AppendI32(points[j + 1]);
		this->sb.AppendUTF8Char(' ');
		j += 2;
		UIntOS k = 1;
		while (k < pointCnt[i])
		{
			this->sb.AppendUTF8Char('L');
			this->sb.AppendI32(points[j]);
			this->sb.AppendUTF8Char(' ');
			this->sb.AppendI32(points[j + 1]);
			this->sb.AppendUTF8Char(' ');
			j += 2;
			k++;
		}
		this->sb.AppendUTF8Char('Z');
		i++;
	}
	this->sb.AppendUTF8Char('\"');
	this->WritePenStyle(p);
	this->WriteBrushStyle(b);
	this->sb.AppendC(UTF8STRC(" />\n"));
	this->WriteBuffer();
	return true;
}

Bool Media::SVGWriter::DrawPolyline(UnsafeArray<const Math::Coord2DDbl> points, UIntOS nPoints, NN<DrawPen> p)
{
	this->sb.AppendC(UTF8STRC("<polyline points=\""));
	if (nPoints > 0)
	{
		UIntOS i = 1;
		this->sb.AppendDouble(points[0].x);
		this->sb.AppendUTF8Char(',');
		this->sb.AppendDouble(points[0].y);
		while (i < nPoints)
		{
			this->sb.AppendUTF8Char(' ');
			this->sb.AppendDouble(points[i].x);
			this->sb.AppendUTF8Char(',');
			this->sb.AppendDouble(points[i].y);
			i++;
		}
	}
	this->sb.AppendUTF8Char('\"');
	this->WritePenStyle(p);
	this->sb.AppendC(UTF8STRC(" />\n"));
	this->WriteBuffer();
	return true;
}

Bool Media::SVGWriter::DrawPolygon(UnsafeArray<const Math::Coord2DDbl> points, UIntOS nPoints, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	this->sb.AppendC(UTF8STRC("<polygon points=\""));
	if (nPoints > 0)
	{
		UIntOS i = 1;
		this->sb.AppendDouble(points[0].x);
		this->sb.AppendUTF8Char(',');
		this->sb.AppendDouble(points[0].y);
		while (i < nPoints)
		{
			this->sb.AppendUTF8Char(' ');
			this->sb.AppendDouble(points[i].x);
			this->sb.AppendUTF8Char(',');
			this->sb.AppendDouble(points[i].y);
			i++;
		}
	}
	this->sb.AppendUTF8Char('\"');
	this->WritePenStyle(p);
	this->WriteBrushStyle(b);
	this->sb.AppendC(UTF8STRC(" />\n"));
	this->WriteBuffer();
	return true;
}

Bool Media::SVGWriter::DrawPolyPolygon(UnsafeArray<const Math::Coord2DDbl> points, UnsafeArray<const UInt32> pointCnt, UIntOS nPointCnt, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	if (nPointCnt == 1)
	{
		return this->DrawPolygon(points, pointCnt[0], p, b);
	}

	this->sb.AppendC(UTF8STRC("<path d=\""));
	UIntOS i = 0;
	UIntOS j = 0;
	while (i < nPointCnt)
	{
		this->sb.AppendUTF8Char('M');
		this->sb.AppendDouble(points[j].x);
		this->sb.AppendUTF8Char(',');
		this->sb.AppendDouble(points[j].y);
		this->sb.AppendUTF8Char(' ');
		j += 1;
		UIntOS k = 1;
		while (k < pointCnt[i])
		{
			this->sb.AppendUTF8Char('L');
			this->sb.AppendDouble(points[j].x);
			this->sb.AppendUTF8Char(',');
			this->sb.AppendDouble(points[j].y);
			this->sb.AppendUTF8Char(' ');
			j += 1;
			k++;
		}
		this->sb.AppendUTF8Char('Z');
		i++;
	}
	this->sb.AppendUTF8Char('\"');
	this->WritePenStyle(p);
	this->WriteBrushStyle(b);
	this->sb.AppendC(UTF8STRC(" />\n"));
	this->WriteBuffer();
	return true;
}

Bool Media::SVGWriter::DrawRect(Math::Coord2DDbl tl, Math::Size2DDbl size, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	this->sb.AppendC(UTF8STRC("<rect x=\""));
	this->sb.AppendDouble(tl.x);
	this->sb.AppendC(UTF8STRC("\" y=\""));
	this->sb.AppendDouble(tl.y);
	this->sb.AppendC(UTF8STRC("\" width=\""));
	this->sb.AppendDouble(size.x);
	this->sb.AppendC(UTF8STRC("\" height=\""));
	this->sb.AppendDouble(size.y);
	this->sb.AppendUTF8Char('\"');
	this->WritePenStyle(p);
	this->WriteBrushStyle(b);
	this->sb.AppendC(UTF8STRC(" />\n"));
	this->WriteBuffer();
	return true;
}

Bool Media::SVGWriter::DrawEllipse(Math::Coord2DDbl tl, Math::Size2DDbl size, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	this->sb.AppendC(UTF8STRC("<ellipse cx=\""));
	this->sb.AppendDouble(tl.x + size.x / 2);
	this->sb.AppendC(UTF8STRC("\" cy=\""));
	this->sb.AppendDouble(tl.y + size.y / 2);
	this->sb.AppendC(UTF8STRC("\" rx=\""));
	this->sb.AppendDouble(size.x / 2);
	this->sb.AppendC(UTF8STRC("\" ry=\""));
	this->sb.AppendDouble(size.y / 2);
	this->sb.AppendUTF8Char('\"');
	this->WritePenStyle(p);
	this->WriteBrushStyle(b);
	this->sb.AppendC(UTF8STRC(" />\n"));
	this->WriteBuffer();
	return true;
}

Bool Media::SVGWriter::DrawString(Math::Coord2DDbl tl, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b)
{
	this->sb.AppendC(UTF8STRC("<text x=\""));
	this->sb.AppendDouble(tl.x);
	this->sb.AppendC(UTF8STRC("\" y=\""));
	this->sb.AppendDouble(tl.y);
	this->sb.AppendUTF8Char('\"');
	this->WriteFontStyle(NN<SVGFont>::ConvertFrom(f));
	this->WriteBrushStyle(b);
	this->sb.AppendC(UTF8STRC(" >"));
	NN<Text::String> s = Text::XML::ToNewXMLText(str->v);
	this->sb.Append(s);
	s->Release();
	this->sb.AppendC(UTF8STRC("</text>\n"));
	this->WriteBuffer();
	return true;
}

Bool Media::SVGWriter::DrawString(Math::Coord2DDbl tl, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b)
{
	this->sb.AppendC(UTF8STRC("<text x=\""));
	this->sb.AppendDouble(tl.x);
	this->sb.AppendC(UTF8STRC("\" y=\""));
	this->sb.AppendDouble(tl.y);
	this->sb.AppendUTF8Char('\"');
	this->WriteFontStyle(NN<SVGFont>::ConvertFrom(f));
	this->WriteBrushStyle(b);
	this->sb.AppendC(UTF8STRC(" >"));
	NN<Text::String> s = Text::XML::ToNewXMLText(str.v);
	this->sb.Append(s);
	s->Release();
	this->sb.AppendC(UTF8STRC("</text>\n"));
	this->WriteBuffer();
	return true;
}

Bool Media::SVGWriter::DrawStringRot(Math::Coord2DDbl center, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegreeACW)
{
	return this->DrawStringRot(center, str->ToCString(), f, b, angleDegreeACW);
}

Bool Media::SVGWriter::DrawStringRot(Math::Coord2DDbl center, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegreeACW)
{
	NN<SVGFont> font = NN<SVGFont>::ConvertFrom(f);
	this->sb.AppendC(UTF8STRC("<text x=\""));
	this->sb.AppendDouble(center.x);
	this->sb.AppendC(UTF8STRC("\" y=\""));
	this->sb.AppendDouble(center.y);
	this->sb.AppendUTF8Char('\"');
	this->WriteFontStyle(font);
	this->WriteBrushStyle(b);
	this->sb.AppendC(UTF8STRC(" transform=\"rotate("));
	this->sb.AppendDouble(angleDegreeACW);
	this->sb.AppendC(UTF8STRC(" "));
	this->sb.AppendDouble(center.x);
	this->sb.AppendUTF8Char(',');
	this->sb.AppendDouble(center.y);
	this->sb.AppendUTF8Char('\"');
	this->sb.AppendC(UTF8STRC(" >"));
	NN<Text::String> s = Text::XML::ToNewXMLText(str.v);
	this->sb.Append(s);
	s->Release();
	this->sb.AppendC(UTF8STRC("</text>\n"));
	this->WriteBuffer();
	return true;
}

Bool Media::SVGWriter::DrawStringB(Math::Coord2DDbl tl, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b, UIntOS buffSize)
{
	return this->DrawStringB(tl, str->ToCString(), f, b, buffSize);
}

Bool Media::SVGWriter::DrawStringB(Math::Coord2DDbl tl, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b, UIntOS buffSize)
{
	return this->DrawString(tl, str, f, b);
}

Bool Media::SVGWriter::DrawStringRotB(Math::Coord2DDbl center, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegreeACW, UIntOS buffSize)
{
	return this->DrawStringRotB(center, str->ToCString(), f, b, angleDegreeACW, buffSize);
}

Bool Media::SVGWriter::DrawStringRotB(Math::Coord2DDbl center, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegreeACW, UIntOS buffSize)
{
	return this->DrawStringRot(center, str, f, b, angleDegreeACW);
}

Bool Media::SVGWriter::DrawImagePt(NN<DrawImage> img, Math::Coord2DDbl tl)
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

Bool Media::SVGWriter::DrawImagePt2(NN<DrawImage> img, Math::Coord2DDbl destTL, Math::Coord2DDbl srcTL, Math::Size2DDbl srcSize)
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

Bool Media::SVGWriter::DrawSImagePt(NN<Media::StaticImage> img, Math::Coord2DDbl tl)
{
	Exporter::PNGExporter exporter;
	IO::MemoryStream memStm;
	if (!exporter.ExportImage(memStm, img))
	{
		return false;
	}
	this->sb.AppendC(UTF8STRC("<image x=\""));
	this->sb.AppendDouble(tl.x);
	this->sb.AppendC(UTF8STRC("\" y=\""));
	this->sb.AppendDouble(tl.y);
	this->sb.AppendC(UTF8STRC("\" width=\""));
	this->sb.AppendDouble(UIntOS2Double(img->info.dispSize.x) * 96.0 / img->info.hdpi);
	this->sb.AppendC(UTF8STRC("\" height=\""));
	this->sb.AppendDouble(UIntOS2Double(img->info.dispSize.y) * 96.0 / img->info.vdpi);
	this->sb.AppendC(UTF8STRC("\" href=\"data:image/png;base64,"));
	Text::TextBinEnc::Base64Enc b64;
	b64.EncodeBin(sb, memStm.GetBuff(), (UIntOS)memStm.GetLength());
	this->sb.AppendUTF8Char('\"');
	this->sb.AppendC(UTF8STRC(" />\n"));
	this->WriteBuffer();
	return true;
}

Bool Media::SVGWriter::DrawSImagePt2(NN<Media::StaticImage> img, Math::Coord2DDbl destTL, Math::Coord2DDbl srcTL, Math::Size2DDbl srcSize)
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

Bool Media::SVGWriter::DrawImageQuad(NN<Media::StaticImage> img, Math::Quadrilateral quad)
{
	// Implementation for drawing image in a quadrilateral
	return false;
}

NN<Media::DrawPen> Media::SVGWriter::NewPenARGB(UInt32 color, Double thick, UnsafeArrayOpt<UInt8> pattern, UIntOS nPattern)
{
	NN<Media::SVGPen> pen;
	NEW_CLASSNN(pen, Media::SVGPen(thick, color));
	return pen;
}

NN<Media::DrawBrush> Media::SVGWriter::NewBrushARGB(UInt32 color)
{
	NN<Media::SVGBrush> brush;
	NEW_CLASSNN(brush, Media::SVGBrush(color));
	return brush;
}

NN<Media::DrawFont> Media::SVGWriter::NewFontPt(Text::CStringNN name, Double ptSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage)
{
	NN<Media::SVGFont> font;
	NEW_CLASSNN(font, Media::SVGFont(name, ptSize * 96.0 / 72.0, fontStyle));
	return font;
}

NN<Media::DrawFont> Media::SVGWriter::NewFontPx(Text::CStringNN name, Double pxSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage)
{
	NN<Media::SVGFont> font;
	NEW_CLASSNN(font, Media::SVGFont(name, pxSize, fontStyle));
	return font;
}

NN<Media::DrawFont> Media::SVGWriter::CloneFont(NN<DrawFont> f)
{
	NN<Media::SVGFont> font;
	NN<Media::SVGFont> srcFont = NN<Media::SVGFont>::ConvertFrom(f);
	NEW_CLASSNN(font, Media::SVGFont(srcFont->GetFontName()->ToCString(), srcFont->GetFontSizePx(), srcFont->GetStyle()));
	return font;
}

void Media::SVGWriter::DelPen(NN<DrawPen> p)
{
	NN<SVGPen> pen = NN<SVGPen>::ConvertFrom(p);
	pen.Delete();
}

void Media::SVGWriter::DelBrush(NN<DrawBrush> b)
{
	NN<SVGBrush> brush = NN<SVGBrush>::ConvertFrom(b);
	brush.Delete();
}

void Media::SVGWriter::DelFont(NN<DrawFont> f)
{
	NN<SVGFont> font = NN<SVGFont>::ConvertFrom(f);
	font.Delete();
}

Math::Size2DDbl Media::SVGWriter::GetTextSize(NN<DrawFont> fnt, Text::CStringNN txt)
{
	NN<Media::SVGFont> font = NN<Media::SVGFont>::ConvertFrom(fnt);
	NN<Media::DrawImage> refImg;
	Math::Size2DDbl size;
	if (this->refEng->CreateImage32(Math::Size2D<UIntOS>(1, 1), Media::AT_ALPHA).SetTo(refImg))
	{
		NN<Media::DrawFont> dfont = refImg->NewFontPx(font->GetFontName()->ToCString(), font->GetFontSizePx(), font->GetStyle(), 0);
		size = refImg->GetTextSize(dfont, txt);
		refImg->DelFont(dfont);
		this->refEng->DeleteImage(refImg);
	}
	else
	{
		Double width = UIntOS2Double(txt.leng) * font->GetFontSizePx() * 0.6; // Approximate width
		Double height = font->GetFontSizePx(); // Approximate height
		size = Math::Size2DDbl(width, height);
	}
	return size;
}

void Media::SVGWriter::SetTextAlign(Media::DrawEngine::DrawPos pos)
{
}

void Media::SVGWriter::GetStringBound(UnsafeArray<Int32> pos, IntOS centX, IntOS centY, UnsafeArray<const UTF8Char> str, NN<DrawFont> f, OutParam<IntOS> drawX, OutParam<IntOS> drawY)
{
	NN<Media::SVGFont> font = NN<Media::SVGFont>::ConvertFrom(f);
	NN<Media::DrawImage> refImg;
	if (this->refEng->CreateImage32(Math::Size2D<UIntOS>(1, 1), Media::AT_ALPHA).SetTo(refImg))
	{
		NN<Media::DrawFont> dfont = refImg->NewFontPx(font->GetFontName()->ToCString(), font->GetFontSizePx(), font->GetStyle(), 0);
		refImg->GetStringBound(pos, centX, centY, str, dfont, drawX, drawY);
		refImg->DelFont(dfont);
		this->refEng->DeleteImage(refImg);
	}
	else
	{
		Double width = UIntOS2Double(Text::StrCharCnt(str)) * font->GetFontSizePx() * 0.6; // Approximate width
		Double height = font->GetFontSizePx(); // Approximate height
		pos[0] = Double2Int32(IntOS2Double(centX) - width * 0.5);
		pos[1] = Double2Int32(IntOS2Double(centY) - height * 0.5);
		pos[2] = Double2Int32(IntOS2Double(centX) + width * 0.5);
		pos[3] = Double2Int32(IntOS2Double(centY) + height * 0.5);
		drawX.Set(centX - Double2Int32(width * 0.5));
		drawY.Set(centY - Double2Int32(height * 0.5));
	}
}
void Media::SVGWriter::GetStringBoundRot(UnsafeArray<Int32> pos, Double centX, Double centY, UnsafeArray<const UTF8Char> str, NN<DrawFont> f, Double angleDegree, OutParam<IntOS> drawX, OutParam<IntOS> drawY)
{
	NN<Media::SVGFont> font = NN<Media::SVGFont>::ConvertFrom(f);
	NN<Media::DrawImage> refImg;
	if (this->refEng->CreateImage32(Math::Size2D<UIntOS>(1, 1), Media::AT_ALPHA).SetTo(refImg))
	{
		NN<Media::DrawFont> dfont = refImg->NewFontPx(font->GetFontName()->ToCString(), font->GetFontSizePx(), font->GetStyle(), 0);
		refImg->GetStringBound(pos, Double2IntOS(centX), Double2IntOS(centY), str, dfont, drawX, drawY);
		refImg->DelFont(dfont);
		this->refEng->DeleteImage(refImg);
	}
	else
	{
		Double width = UIntOS2Double(Text::StrCharCnt(str)) * font->GetFontSizePx() * 0.6; // Approximate width
		Double height = font->GetFontSizePx(); // Approximate height
		pos[0] = Double2Int32(centX - width * 0.5);
		pos[1] = Double2Int32(centY - height * 0.5);
		pos[2] = Double2Int32(centX + width * 0.5);
		pos[3] = Double2Int32(centY + height * 0.5);
		drawX.Set(Double2IntOS(centX) - Double2Int32(width * 0.5));
		drawY.Set(Double2IntOS(centY) - Double2Int32(height * 0.5));
	}
}
void Media::SVGWriter::CopyBits(IntOS x, IntOS y, UnsafeArray<UInt8> imgPtr, UIntOS bpl, UIntOS width, UIntOS height, Bool upsideDown) const
{
}

Optional<Media::StaticImage> Media::SVGWriter::ToStaticImage() const
{
	return nullptr;
}
Optional<Media::RasterImage> Media::SVGWriter::AsRasterImage()
{
	return nullptr;
}

UIntOS Media::SVGWriter::SavePng(NN<IO::SeekableStream> stm)
{
	return 0;
}

UIntOS Media::SVGWriter::SaveGIF(NN<IO::SeekableStream> stm)
{
	return 0;
}

UIntOS Media::SVGWriter::SaveJPG(NN<IO::SeekableStream> stm)
{
	return 0;
}