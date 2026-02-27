#include "Stdafx.h"
#include "Media/SVGCore.h"

void Media::SVGCore::WritePenStyle(NN<Text::StringBuilderUTF8> sb, Optional<DrawPen> p)
{
	NN<SVGPen> pen;
	if (Optional<SVGPen>::ConvertFrom(p).SetTo(pen))
	{
		sb->AppendC(UTF8STRC(" stroke=\"#"));
		UInt32 col = pen->GetColor();
		sb->AppendHex24(col & 0xffffff);
		if ((col & 0xff000000) != 0xff000000)
		{
			sb->AppendC(UTF8STRC("\" stroke-opacity=\""));
			sb->AppendDouble(((col >> 24) & 0xff) / 255.0);
		}
		sb->AppendC(UTF8STRC("\" stroke-width=\""));
		sb->AppendDouble(pen->GetThick());
		sb->AppendUTF8Char('\"');
	}
	else
	{
		sb->AppendC(UTF8STRC(" stroke=\"none\""));
	}
}

void Media::SVGCore::WriteBrushStyle(NN<Text::StringBuilderUTF8> sb, Optional<DrawBrush> b)
{
	NN<SVGBrush> brush;
	if (Optional<SVGBrush>::ConvertFrom(b).SetTo(brush))
	{
		sb->AppendC(UTF8STRC(" fill=\"#"));
		UInt32 col = brush->GetColor();
		sb->AppendHex24(col & 0xffffff);
		if ((col & 0xff000000) != 0xff000000)
		{
			sb->AppendC(UTF8STRC("\" fill-opacity=\""));
			sb->AppendDouble(((col >> 24) & 0xff) / 255.0);
		}
		sb->AppendUTF8Char('\"');
	}
	else
	{
		sb->AppendC(UTF8STRC(" fill=\"none\""));
	}
}

void Media::SVGCore::WriteFontStyle(NN<Text::StringBuilderUTF8> sb, NN<SVGFont> font)
{
	sb->AppendC(UTF8STRC(" font-family=\""));
	sb->Append(font->GetFontName());
	sb->AppendC(UTF8STRC("\" font-size=\""));
	sb->AppendDouble(font->GetFontSizePx());
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
}

Math::Size2DDbl Media::SVGCore::GetTextSize(NN<Media::DrawEngine> deng, NN<SVGFont> font, Text::CStringNN txt)
{
	NN<Media::DrawImage> refImg;
	Math::Size2DDbl size;
	if (deng->CreateImage32(Math::Size2D<UIntOS>(1, 1), Media::AT_ALPHA).SetTo(refImg))
	{
		NN<Media::DrawFont> dfont = refImg->NewFontPx(font->GetFontName()->ToCString(), font->GetFontSizePx(), font->GetStyle(), 0);
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
		NN<Media::DrawFont> dfont = refImg->NewFontPx(NN<SVGFont>::ConvertFrom(f)->GetFontName()->ToCString(), NN<SVGFont>::ConvertFrom(f)->GetFontSizePx(), NN<SVGFont>::ConvertFrom(f)->GetStyle(), 0);
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
		NN<Media::DrawFont> dfont = refImg->NewFontPx(NN<SVGFont>::ConvertFrom(f)->GetFontName()->ToCString(), NN<SVGFont>::ConvertFrom(f)->GetFontSizePx(), NN<SVGFont>::ConvertFrom(f)->GetStyle(), 0);
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
