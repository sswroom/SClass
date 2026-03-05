#ifndef _SM_MEDIA_SVGWRITER
#define _SM_MEDIA_SVGWRITER
#include "IO/Stream.h"
#include "Media/SVGCore.h"
#include "Text/StringBuilderUTF8.h"

namespace Media
{
	class SVGWriter : public DrawImage
	{
	private:
		NN<Media::DrawEngine> refEng;
		NN<IO::Stream> stm;
		Text::StringBuilderUTF8 sb;
		Math::Size2DDbl size;
		Media::ColorProfile color;

		void WriteBuffer();
	public:
		SVGWriter(NN<IO::Stream> stm, Double width, Double height, NN<Media::DrawEngine> refEng);
		virtual ~SVGWriter();

		virtual Double GetWidth() const;
		virtual Double GetHeight() const;
		virtual Math::Size2DDbl GetSize() const;
		virtual UInt32 GetBitCount() const;
		virtual NN<const ColorProfile> GetColorProfile() const;
		virtual void SetColorProfile(NN<const ColorProfile> color);
		virtual Media::AlphaType GetAlphaType() const;
		virtual void SetAlphaType(Media::AlphaType atype);
		virtual Double GetHDPI() const;
		virtual Double GetVDPI() const;
		virtual void SetHDPI(Double dpi);
		virtual void SetVDPI(Double dpi);
		virtual Optional<Media::EXIFData> GetEXIF() const;
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
		
		virtual Bool PixelSupported() const { return false; }
		virtual UIntOS PixelGetWidth() const { return 0; }
		virtual UIntOS PixelGetHeight() const { return 0; }
		virtual Media::AlphaType PixelGetAlphaType() const { return Media::AT_IGNORE_ALPHA; }
		virtual void PixelSetAlphaType(Media::AlphaType atype) {}
		virtual UInt32 PixelGetBitCount() const { return 0; }
		virtual UnsafeArrayOpt<UInt8> PixelGetBits(OutParam<Bool> revOrder) { return nullptr; }
		virtual void PixelGetBitsEnd(Bool modified) {}
		virtual UIntOS PixelGetBpl() const { return 0; }
		virtual Media::PixelFormat PixelGetFormat() const { return Media::PF_UNKNOWN; }

		virtual Optional<Media::StaticImage> ToStaticImage() const;
		virtual Optional<Media::RasterImage> AsRasterImage();
		virtual UIntOS SavePng(NN<IO::SeekableStream> stm);
		virtual UIntOS SaveGIF(NN<IO::SeekableStream> stm);
		virtual UIntOS SaveJPG(NN<IO::SeekableStream> stm);
	};
}
#endif
