#ifndef _SM_MEDIA_SVGCORE
#define _SM_MEDIA_SVGCORE
#include "Media/DrawEngine.h"

namespace Media
{
	class SVGPen : public DrawPen
	{
	private:
		Double thick;
		UInt32 color;
	public:
		SVGPen(Double thick, UInt32 color) { this->thick = thick; this->color = color; }
		virtual ~SVGPen(){};

		virtual Double GetThick() override { return this->thick; }
		UInt32 GetColor() { return this->color; }
	};

	class SVGBrush : public DrawBrush
	{
	private:
		UInt32 color;
	public:
		SVGBrush(UInt32 color) { this->color = color; }
		virtual ~SVGBrush(){};

		UInt32 GetColor() { return this->color; }
	};

	class SVGFont : public DrawFont
	{
	private:
		NN<Text::String> fontName;
		Double fontSizePx;
		DrawEngine::DrawFontStyle style;
	public:
		SVGFont(Text::CStringNN fontName, Double fontSizePx, DrawEngine::DrawFontStyle style) { this->fontName = Text::String::New(fontName); this->fontSizePx = fontSizePx; this->style = style; }
		virtual ~SVGFont() { this->fontName->Release(); };

		NN<Text::String> GetFontName() { return this->fontName; }
		Double GetFontSizePx() { return this->fontSizePx; }
		DrawEngine::DrawFontStyle GetStyle() { return this->style; }
	};

	class SVGCore
	{
	public:
		static void WritePenStyle(NN<Text::StringBuilderUTF8> sb, Optional<DrawPen> pen);
		static void WriteBrushStyle(NN<Text::StringBuilderUTF8> sb, Optional<DrawBrush> brush);
		static void WriteFontStyle(NN<Text::StringBuilderUTF8> sb, NN<SVGFont> font);
		static Math::Size2DDbl GetTextSize(NN<Media::DrawEngine> deng, NN<SVGFont> font, Text::CStringNN txt);
		static void GetStringBound(NN<Media::DrawEngine> deng, UnsafeArray<Int32> pos, IntOS centX, IntOS centY, UnsafeArray<const UTF8Char> str, NN<DrawFont> f, OutParam<IntOS> drawX, OutParam<IntOS> drawY);
		static void GetStringBoundRot(NN<Media::DrawEngine> deng, UnsafeArray<Int32> pos, Double centX, Double centY, UnsafeArray<const UTF8Char> str, NN<DrawFont> f, Double angleDegree, OutParam<IntOS> drawX, OutParam<IntOS> drawY);
	};
}
#endif
