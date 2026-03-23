#ifndef _SM_MEDIA_SVGCORE
#define _SM_MEDIA_SVGCORE
#include "Media/DrawEngine.h"

namespace Media
{
	enum class SVGLineJoin
	{
		Default,
		Miter,
		Round,
		Bevel
	};

	enum class SVGLineCap
	{
		Default,
		Butt,
		Round,
		Square
	};

	enum class SVGFillRule
	{
		Default,
		NonZero,
		EvenOdd
	};

	class SVGPen : public DrawPen
	{
	private:
		Double thick;
		UInt32 color;
		Optional<Text::String> colorName;
	public:
		SVGPen(Double thick, UInt32 color) { this->thick = thick; this->color = color; this->colorName = nullptr; }
		virtual ~SVGPen(){ OPTSTR_DEL(this->colorName); };

		virtual Double GetThick() override { return this->thick; }
		UInt32 GetColor() { return this->color; }
		Optional<Text::String> GetColorName() { return this->colorName; }
		void SetColorName(Text::CStringNN colorName) { OPTSTR_DEL(this->colorName); this->colorName = Text::String::New(colorName); }
	};

	class SVGBrush : public DrawBrush
	{
	private:
		UInt32 color;
		Optional<Text::String> colorName;
		SVGFillRule fillRule;

	public:
		SVGBrush(UInt32 color) { this->color = color; this->colorName = nullptr; this->fillRule = SVGFillRule::Default; }
		virtual ~SVGBrush(){ OPTSTR_DEL(this->colorName); };

		UInt32 GetColor() { return this->color; }
		Optional<Text::String> GetColorName() { return this->colorName; }
		SVGFillRule GetFillRule() { return this->fillRule; }
		void SetFillRule(SVGFillRule fillRule) { this->fillRule = fillRule; }
		void SetColorName(Text::CStringNN colorName) { OPTSTR_DEL(this->colorName); this->colorName = Text::String::New(colorName); }
	};

	class SVGFont : public DrawFont
	{
	private:
		Optional<Text::String> fontName;
		Double fontSizePx;
		Optional<Text::String> fontSizeStr;
		DrawEngine::DrawFontStyle style;
		Optional<Text::String> fontVariant;
		Optional<Text::String> fontStretch;
		Optional<Text::String> inkscapeFont;
	public:
		SVGFont(Text::CStringNN fontName, Double fontSizePx, DrawEngine::DrawFontStyle style) { this->fontName = (fontName.leng == 0 ? Optional<Text::String>(nullptr) : Text::String::New(fontName)); this->fontSizePx = fontSizePx; this->style = style; this->fontSizeStr = nullptr; this->fontVariant = nullptr; this->fontStretch = nullptr; this->inkscapeFont = nullptr; }
		virtual ~SVGFont() { OPTSTR_DEL(this->fontName); OPTSTR_DEL(this->fontSizeStr); OPTSTR_DEL(this->fontVariant); OPTSTR_DEL(this->fontStretch); OPTSTR_DEL(this->inkscapeFont); }

		Optional<Text::String> GetFontName() { return this->fontName; }
		Double GetFontSizePx() { return this->fontSizePx; }
		void SetFontSizeStr(Text::CStringNN fontSizeStr) { OPTSTR_DEL(this->fontSizeStr); this->fontSizeStr = Text::String::New(fontSizeStr); }
		Optional<Text::String> GetFontSizeStr() { return this->fontSizeStr; }
		DrawEngine::DrawFontStyle GetStyle() { return this->style; }
		void SetFontVariant(Text::CStringNN fontVariant) { OPTSTR_DEL(this->fontVariant); this->fontVariant = Text::String::New(fontVariant); }
		Optional<Text::String> GetFontVariant() { return this->fontVariant; }
		void SetFontStretch(Text::CStringNN fontStretch) { OPTSTR_DEL(this->fontStretch); this->fontStretch = Text::String::New(fontStretch); }
		Optional<Text::String> GetFontStretch() { return this->fontStretch; }
		void SetInkscapeFont(Text::CStringNN inkscapeFont) { OPTSTR_DEL(this->inkscapeFont); this->inkscapeFont = Text::String::New(inkscapeFont); }
		Optional<Text::String> GetInkscapeFont() { return this->inkscapeFont; }
	};

	class SVGCore
	{
	public:
		static void WriteAttrPen(NN<Text::StringBuilderUTF8> sb, Optional<DrawPen> pen);
		static void WriteAttrBrush(NN<Text::StringBuilderUTF8> sb, Optional<DrawBrush> brush);
		static void WriteAttrFont(NN<Text::StringBuilderUTF8> sb, NN<SVGFont> font);
		static Bool WriteStylePen(NN<Text::StringBuilderUTF8> sb, Optional<DrawPen> pen, Bool hasOtherStyle);
		static Bool WriteStyleBrush(NN<Text::StringBuilderUTF8> sb, Optional<DrawBrush> brush, Bool hasOtherStyle);
		static Bool WriteStyleFont(NN<Text::StringBuilderUTF8> sb, NN<SVGFont> font, Bool hasOtherStyle);
		static NN<Media::DrawFont> CreateDrawFont(NN<Media::DrawImage> img, NN<SVGFont> font);
		static Math::Size2DDbl GetTextSize(NN<Media::DrawEngine> deng, NN<SVGFont> font, Text::CStringNN txt);
		static void GetStringBound(NN<Media::DrawEngine> deng, UnsafeArray<Int32> pos, IntOS centX, IntOS centY, UnsafeArray<const UTF8Char> str, NN<DrawFont> f, OutParam<IntOS> drawX, OutParam<IntOS> drawY);
		static void GetStringBoundRot(NN<Media::DrawEngine> deng, UnsafeArray<Int32> pos, Double centX, Double centY, UnsafeArray<const UTF8Char> str, NN<DrawFont> f, Double angleDegree, OutParam<IntOS> drawX, OutParam<IntOS> drawY);
	};
}
#endif
