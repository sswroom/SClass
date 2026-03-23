#include "Stdafx.h"
#include "Media/SVGCore.h"

void Media::SVGCore::WriteAttrPen(NN<Text::StringBuilderUTF8> sb, Optional<DrawPen> p)
{
	NN<Text::String> colorName;
	NN<SVGPen> pen;
	if (Optional<SVGPen>::ConvertFrom(p).SetTo(pen))
	{
		sb->AppendC(UTF8STRC(" stroke=\""));
		UInt32 col = pen->GetColor();
		if (pen->GetColorName().SetTo(colorName))
		{
			sb->Append(colorName);
		}
		else
		{
			sb->AppendUTF8Char('#');
			sb->AppendHex24(col & 0xffffff);
		}
		sb->AppendUTF8Char('\"');
		if ((col & 0xff000000) != 0xff000000)
		{
			sb->AppendC(UTF8STRC(" stroke-opacity=\""));
			sb->AppendDouble(((col >> 24) & 0xff) / 255.0);
			sb->AppendUTF8Char('\"');
		}
		if (pen->GetThick() > 0)
		{
			sb->AppendC(UTF8STRC(" stroke-width=\""));
			sb->AppendDouble(pen->GetThick());
			sb->AppendUTF8Char('\"');
		}
	}
}

void Media::SVGCore::WriteAttrBrush(NN<Text::StringBuilderUTF8> sb, Optional<DrawBrush> b)
{
	NN<SVGBrush> brush;
	NN<Text::String> colorName;
	if (Optional<SVGBrush>::ConvertFrom(b).SetTo(brush))
	{
		sb->AppendC(UTF8STRC(" fill=\""));
		UInt32 col = brush->GetColor();
		if (brush->GetColorName().SetTo(colorName))
		{
			sb->Append(colorName);
		}
		else
		{
			sb->AppendUTF8Char('#');
			sb->AppendHex24(col & 0xffffff);
		}
		sb->AppendUTF8Char('\"');
		if ((col & 0xff000000) != 0xff000000)
		{
			sb->AppendC(UTF8STRC(" fill-opacity=\""));
			sb->AppendDouble(((col >> 24) & 0xff) / 255.0);
			sb->AppendUTF8Char('\"');
		}
		if (brush->GetFillRule() == SVGFillRule::NonZero)
		{
			sb->AppendC(UTF8STRC(" fill-rule=\"nonzero\""));
		}
		else if (brush->GetFillRule() == SVGFillRule::EvenOdd)
		{
			sb->AppendC(UTF8STRC(" fill-rule=\"evenodd\""));
		}
	}
	else
	{
		sb->AppendC(UTF8STRC(" fill=\"none\""));
	}
}

void Media::SVGCore::WriteAttrFont(NN<Text::StringBuilderUTF8> sb, NN<SVGFont> font)
{
	NN<Text::String> s;
	if (font->GetFontName().SetTo(s))
	{
		sb->AppendC(UTF8STRC(" font-family=\""));
		sb->Append(s);
		sb->AppendUTF8Char('\"');
	}
	else
	{
		sb->AppendC(UTF8STRC(" font-family=\""));
		sb->AppendC(UTF8STRC("sans-serif"));
		sb->AppendUTF8Char('\"');
	}
	sb->AppendC(UTF8STRC(" font-size=\""));
	if (font->GetFontSizeStr().SetTo(s))
	{
		sb->Append(s);
	}
	else
	{
		sb->AppendDouble(font->GetFontSizePx());
	}
	sb->AppendUTF8Char('\"');
	if ((font->GetStyle() & Media::DrawEngine::DFS_BOLD) != 0)
	{
		sb->AppendC(UTF8STRC(" font-weight=\"bold\""));
	}
	else
	{
		sb->AppendC(UTF8STRC(" font-weight=\"normal\""));
	}
	if ((font->GetStyle() & Media::DrawEngine::DFS_ITALIC) != 0)
	{
		sb->AppendC(UTF8STRC(" font-style=\"italic\""));
	}
	else
	{
		sb->AppendC(UTF8STRC(" font-style=\"normal\""));
	}
	if (font->GetFontVariant().SetTo(s))
	{
		sb->AppendC(UTF8STRC(" font-variant=\""));
		sb->Append(s);
		sb->AppendUTF8Char('\"');
	}
	if (font->GetFontStretch().SetTo(s))
	{
		sb->AppendC(UTF8STRC(" font-stretch=\""));
		sb->Append(s);
		sb->AppendUTF8Char('\"');
	}
	if (font->GetInkscapeFont().SetTo(s))
	{
		sb->AppendC(UTF8STRC(" -inkscape-font-specification=\""));
		sb->Append(s);
		sb->AppendUTF8Char('\"');
	}
}

Bool Media::SVGCore::WriteStylePen(NN<Text::StringBuilderUTF8> sb, Optional<DrawPen> p, Bool hasOtherStyle)
{
	NN<Text::String> colorName;
	NN<SVGPen> pen;
	if (Optional<SVGPen>::ConvertFrom(p).SetTo(pen))
	{
		if (hasOtherStyle)
		{
			sb->AppendUTF8Char(';');
		}
		sb->AppendC(UTF8STRC("stroke:"));
		UInt32 col = pen->GetColor();
		if (pen->GetColorName().SetTo(colorName))
		{
			sb->Append(colorName);
		}
		else
		{
			sb->AppendUTF8Char('#');
			sb->AppendHex24(col & 0xffffff);
		}
		hasOtherStyle = true;
		if ((col & 0xff000000) != 0xff000000)
		{
			sb->AppendC(UTF8STRC(";stroke-opacity:"));
			sb->AppendDouble(((col >> 24) & 0xff) / 255.0);
		}
		if (pen->GetThick() > 0)
		{
			sb->AppendC(UTF8STRC(";stroke-width:"));
			sb->AppendDouble(pen->GetThick());
		}
		return true;
	}
	return false;
}

Bool Media::SVGCore::WriteStyleBrush(NN<Text::StringBuilderUTF8> sb, Optional<DrawBrush> b, Bool hasOtherStyle)
{
	NN<SVGBrush> brush;
	NN<Text::String> colorName;
	if (hasOtherStyle)
	{
		sb->AppendUTF8Char(';');
	}
	if (Optional<SVGBrush>::ConvertFrom(b).SetTo(brush))
	{
		sb->AppendC(UTF8STRC("fill:"));
		UInt32 col = brush->GetColor();
		Bool requireOpacity = true;
		if (brush->GetColorName().SetTo(colorName))
		{
			sb->Append(colorName);
			if (colorName->Equals(UTF8STRC("transparent")) || colorName->Equals(UTF8STRC("none")))
			{
				requireOpacity = false;
			}
		}
		else
		{
			sb->AppendUTF8Char('#');
			sb->AppendHex24(col & 0xffffff);
		}
		if (requireOpacity)
		{
			sb->AppendC(UTF8STRC(";fill-opacity:"));
			sb->AppendDouble(((col >> 24) & 0xff) / 255.0);
		}
		if (brush->GetFillRule() == SVGFillRule::NonZero)
		{
			sb->AppendC(UTF8STRC(";fill-rule:nonzero"));
		}
		else if (brush->GetFillRule() == SVGFillRule::EvenOdd)
		{
			sb->AppendC(UTF8STRC(";fill-rule:evenodd"));
		}
	}
	else
	{
		sb->AppendC(UTF8STRC("fill:none"));
	}
	return true;
}

Bool Media::SVGCore::WriteStyleFont(NN<Text::StringBuilderUTF8> sb, NN<SVGFont> font, Bool hasOtherStyle)
{
	NN<Text::String> s;
	if (hasOtherStyle)
	{
		sb->AppendUTF8Char(';');
	}
	sb->AppendC(UTF8STRC("font-size:"));
	if (font->GetFontSizeStr().SetTo(s))
	{
		sb->Append(s);
	}
	else
	{
		sb->AppendDouble(font->GetFontSizePx());
	}
	if (font->GetFontName().SetTo(s))
	{
		sb->AppendC(UTF8STRC(";font-family:"));
		sb->Append(s);
	}
	if ((font->GetStyle() & Media::DrawEngine::DFS_BOLD) != 0)
	{
		sb->AppendC(UTF8STRC(";font-weight:bold"));
	}
	else
	{
		sb->AppendC(UTF8STRC(";font-weight:normal"));
	}
	if ((font->GetStyle() & Media::DrawEngine::DFS_ITALIC) != 0)
	{
		sb->AppendC(UTF8STRC(";font-style:italic"));
	}
	else
	{
		sb->AppendC(UTF8STRC(";font-style:normal"));
	}
	if (font->GetFontVariant().SetTo(s))
	{
		sb->AppendC(UTF8STRC(";font-variant:"));
		sb->Append(s);
	}
	if (font->GetFontStretch().SetTo(s))
	{
		sb->AppendC(UTF8STRC(";font-stretch:"));
		sb->Append(s);
	}
	if (font->GetInkscapeFont().SetTo(s))
	{
		sb->AppendC(UTF8STRC(";-inkscape-font-specification:"));
		sb->Append(s);
	}
	return true;
}

NN<Media::DrawFont> Media::SVGCore::CreateDrawFont(NN<Media::DrawImage> img, NN<SVGFont> font)
{
	Text::CStringNN fontName;
	NN<Text::String> s;
	if (!font->GetFontName().SetTo(s))
	{
		fontName = CSTR("Arial");
	}
	else
	{
		fontName = s->ToCString();
	}
	NN<Media::DrawFont> dfont = img->NewFontPx(fontName, font->GetFontSizePx(), font->GetStyle(), 0);
	return dfont;
}

Math::Size2DDbl Media::SVGCore::GetTextSize(NN<Media::DrawEngine> deng, NN<SVGFont> font, Text::CStringNN txt)
{
	NN<Media::DrawImage> refImg;
	Math::Size2DDbl size;
	if (deng->CreateImage32(Math::Size2D<UIntOS>(1, 1), Media::AT_ALPHA).SetTo(refImg))
	{
		NN<Media::DrawFont> dfont = CreateDrawFont(refImg, font);
		size = refImg->GetTextSize(dfont, txt);
		refImg->DelFont(dfont);
		deng->DeleteImage(refImg);
	}
	else
	{
		Double width = UIntOS2Double(txt.leng) * font->GetFontSizePx() * 0.6; // Approximate width
		Double height = font->GetFontSizePx(); // Approximate height
		size = Math::Size2DDbl(width, height);
	}
	return size;
}

void Media::SVGCore::GetStringBound(NN<Media::DrawEngine> deng, UnsafeArray<Int32> pos, IntOS centX, IntOS centY, UnsafeArray<const UTF8Char> str, NN<DrawFont> f, OutParam<IntOS> drawX, OutParam<IntOS> drawY)
{
	NN<Media::DrawImage> refImg;
	if (deng->CreateImage32(Math::Size2D<UIntOS>(1, 1), Media::AT_ALPHA).SetTo(refImg))
	{
		NN<Media::DrawFont> dfont = CreateDrawFont(refImg, NN<SVGFont>::ConvertFrom(f));
		refImg->GetStringBound(pos, centX, centY, str, dfont, drawX, drawY);
		refImg->DelFont(dfont);
		deng->DeleteImage(refImg);
	}
	else
	{
		Double width = UIntOS2Double(Text::StrCharCnt(str)) * NN<SVGFont>::ConvertFrom(f)->GetFontSizePx() * 0.6; // Approximate width
		Double height = NN<SVGFont>::ConvertFrom(f)->GetFontSizePx(); // Approximate height
		pos[0] = Double2Int32(IntOS2Double(centX) - width * 0.5);
		pos[1] = Double2Int32(IntOS2Double(centY) - height * 0.5);
		pos[2] = Double2Int32(IntOS2Double(centX) + width * 0.5);
		pos[3] = Double2Int32(IntOS2Double(centY) + height * 0.5);
		drawX.Set(centX - Double2Int32(width * 0.5));
		drawY.Set(centY - Double2Int32(height * 0.5));
	}
}

void Media::SVGCore::GetStringBoundRot(NN<Media::DrawEngine> deng, UnsafeArray<Int32> pos, Double centX, Double centY, UnsafeArray<const UTF8Char> str, NN<DrawFont> f, Double angleDegree, OutParam<IntOS> drawX, OutParam<IntOS> drawY)
{
	NN<Media::DrawImage> refImg;
	if (deng->CreateImage32(Math::Size2D<UIntOS>(1, 1), Media::AT_ALPHA).SetTo(refImg))
	{
		NN<Media::DrawFont> dfont = CreateDrawFont(refImg, NN<SVGFont>::ConvertFrom(f));
		refImg->GetStringBound(pos, Double2IntOS(centX), Double2IntOS(centY), str, dfont, drawX, drawY);
		refImg->DelFont(dfont);
		deng->DeleteImage(refImg);
	}
	else
	{
		Double width = UIntOS2Double(Text::StrCharCnt(str)) * NN<SVGFont>::ConvertFrom(f)->GetFontSizePx() * 0.6; // Approximate width
		Double height = NN<SVGFont>::ConvertFrom(f)->GetFontSizePx(); // Approximate height
		pos[0] = Double2Int32(centX - width * 0.5);
		pos[1] = Double2Int32(centY - height * 0.5);
		pos[2] = Double2Int32(centX + width * 0.5);
		pos[3] = Double2Int32(centY + height * 0.5);
		drawX.Set(Double2IntOS(centX) - Double2Int32(width * 0.5));
		drawY.Set(Double2IntOS(centY) - Double2Int32(height * 0.5));
	}
}
