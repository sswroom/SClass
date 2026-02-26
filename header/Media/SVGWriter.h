#ifndef _SM_MEDIA_SVGWRITER
#define _SM_MEDIA_SVGWRITER
#include "IO/Stream.h"
#include "Media/DrawEngine.h"
#include "Text/StringBuilderUTF8.h"

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

	class SVGWriter : public DrawImage
	{
	private:
		NN<Media::DrawEngine> refEng;
		NN<IO::Stream> stm;
		Text::StringBuilderUTF8 sb;
		Math::Size2D<UIntOS> size;
		Media::ColorProfile color;

		void WriteBuffer();
		void WritePenStyle(Optional<DrawPen> pen);
		void WriteBrushStyle(Optional<DrawBrush> brush);
		void WriteFontStyle(NN<SVGFont> font);
	public:
		SVGWriter(NN<IO::Stream> stm, UIntOS width, UIntOS height, NN<Media::DrawEngine> refEng);
		virtual ~SVGWriter();

		virtual UIntOS GetWidth() const;
		virtual UIntOS GetHeight() const;
		virtual Math::Size2D<UIntOS> GetSize() const;
		virtual UInt32 GetBitCount() const;
		virtual NN<const ColorProfile> GetColorProfile() const;
		virtual void SetColorProfile(NN<const ColorProfile> color);
		virtual Media::AlphaType GetAlphaType() const;
		virtual void SetAlphaType(Media::AlphaType atype);
		virtual Double GetHDPI() const;
		virtual Double GetVDPI() const;
		virtual void SetHDPI(Double dpi);
		virtual void SetVDPI(Double dpi);
		virtual UnsafeArrayOpt<UInt8> GetImgBits(OutParam<Bool> revOrder);
		virtual void GetImgBitsEnd(Bool modified);
		virtual UIntOS GetImgBpl() const;
		virtual Optional<Media::EXIFData> GetEXIF() const;
		virtual Media::PixelFormat GetPixelFormat() const;
		virtual void SetColorSess(Optional<Media::ColorSess> colorSess);

		virtual Bool DrawLine(Double x1, Double y1, Double x2, Double y2, NN<DrawPen> p);
		virtual Bool DrawPolylineI(UnsafeArray<const Int32> points, UIntOS nPoints, NN<DrawPen> p);
		virtual Bool DrawPolygonI(UnsafeArray<const Int32> points, UIntOS nPoints, Optional<DrawPen> p, Optional<DrawBrush> b);
		virtual Bool DrawPolyPolygonI(UnsafeArray<const Int32> points, UnsafeArray<const UInt32> pointCnt, UIntOS nPointCnt, Optional<DrawPen> p, Optional<DrawBrush> b);
		virtual Bool DrawPolyline(UnsafeArray<const Math::Coord2DDbl> points, UIntOS nPoints, NN<DrawPen> p);
		virtual Bool DrawPolygon(UnsafeArray<const Math::Coord2DDbl> points, UIntOS nPoints, Optional<DrawPen> p, Optional<DrawBrush> b);
		virtual Bool DrawPolyPolygon(UnsafeArray<const Math::Coord2DDbl> points, UnsafeArray<const UInt32> pointCnt, UIntOS nPointCnt, Optional<DrawPen> p, Optional<DrawBrush> b);
		virtual Bool DrawRect(Math::Coord2DDbl tl, Math::Size2DDbl size, Optional<DrawPen> p, Optional<DrawBrush> b);
		virtual Bool DrawEllipse(Math::Coord2DDbl tl, Math::Size2DDbl size, Optional<DrawPen> p, Optional<DrawBrush> b);
		virtual Bool DrawString(Math::Coord2DDbl tl, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b);
		virtual Bool DrawString(Math::Coord2DDbl tl, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b);
		virtual Bool DrawStringRot(Math::Coord2DDbl center, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegreeACW);
		virtual Bool DrawStringRot(Math::Coord2DDbl center, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegreeACW);
		virtual Bool DrawStringB(Math::Coord2DDbl tl, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b, UIntOS buffSize);
		virtual Bool DrawStringB(Math::Coord2DDbl tl, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b, UIntOS buffSize);
		virtual Bool DrawStringRotB(Math::Coord2DDbl center, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegreeACW, UIntOS buffSize);
		virtual Bool DrawStringRotB(Math::Coord2DDbl center, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegreeACW, UIntOS buffSize);
		virtual Bool DrawImagePt(NN<DrawImage> img, Math::Coord2DDbl tl);
		virtual Bool DrawImagePt2(NN<DrawImage> img, Math::Coord2DDbl destTL, Math::Coord2DDbl srcTL, Math::Size2DDbl srcSize);
		virtual Bool DrawSImagePt(NN<Media::StaticImage> img, Math::Coord2DDbl tl);
		virtual Bool DrawSImagePt2(NN<Media::StaticImage> img, Math::Coord2DDbl destTL, Math::Coord2DDbl srcTL, Math::Size2DDbl srcSize);
		virtual Bool DrawImageQuad(NN<Media::StaticImage> img, Math::Quadrilateral quad);

		virtual NN<DrawPen> NewPenARGB(UInt32 color, Double thick, UnsafeArrayOpt<UInt8> pattern, UIntOS nPattern);
		virtual NN<DrawBrush> NewBrushARGB(UInt32 color);
		virtual NN<DrawFont> NewFontPt(Text::CStringNN name, Double ptSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage); // 72 dpi size
		virtual NN<DrawFont> NewFontPx(Text::CStringNN name, Double pxSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage); // Actual size
		virtual NN<DrawFont> CloneFont(NN<DrawFont> f);
		virtual void DelPen(NN<DrawPen> p);
		virtual void DelBrush(NN<DrawBrush> b);
		virtual void DelFont(NN<DrawFont> f);

		virtual Math::Size2DDbl GetTextSize(NN<DrawFont> fnt, Text::CStringNN txt);
		virtual void SetTextAlign(Media::DrawEngine::DrawPos pos);
		virtual void GetStringBound(UnsafeArray<Int32> pos, IntOS centX, IntOS centY, UnsafeArray<const UTF8Char> str, NN<DrawFont> f, OutParam<IntOS> drawX, OutParam<IntOS> drawY);
		virtual void GetStringBoundRot(UnsafeArray<Int32> pos, Double centX, Double centY, UnsafeArray<const UTF8Char> str, NN<DrawFont> f, Double angleDegree, OutParam<IntOS> drawX, OutParam<IntOS> drawY);
		virtual void CopyBits(IntOS x, IntOS y, UnsafeArray<UInt8> imgPtr, UIntOS bpl, UIntOS width, UIntOS height, Bool upsideDown) const;
		
		virtual Optional<Media::StaticImage> ToStaticImage() const;
		virtual Optional<Media::RasterImage> AsRasterImage();
		virtual UIntOS SavePng(NN<IO::SeekableStream> stm);
		virtual UIntOS SaveGIF(NN<IO::SeekableStream> stm);
		virtual UIntOS SaveJPG(NN<IO::SeekableStream> stm);
	};
}
#endif
