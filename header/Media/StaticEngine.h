#ifndef _SM_MEDIA_STATICENGINE
#define _SM_MEDIA_STATICENGINE
#include "Media/DrawEngine.h"
#include "Media/ImageAlphaBlend.h"
#include "Media/StaticImage.h"
#include "Parser/ParserList.h"

namespace Media
{
	class StaticEngine : public DrawEngine
	{
	private:
		Optional<Parser::ParserList> parsers;
	public:
		NN<Media::ImageAlphaBlend> iab32;
	public:
		StaticEngine(Optional<Parser::ParserList> parsers);
		virtual ~StaticEngine();

		virtual Optional<DrawImage> CreateImage32(Math::Size2D<UOSInt> size, Media::AlphaType atype);
		virtual Optional<DrawImage> LoadImage(Text::CStringNN fileName);
		virtual Optional<DrawImage> LoadImageW(UnsafeArray<const WChar> fileName);
		virtual Optional<DrawImage> LoadImageStream(NN<IO::SeekableStream> stm); /////////////////////////
		virtual Optional<DrawImage> ConvImage(NN<Media::RasterImage> img);
		virtual Optional<DrawImage> CloneImage(NN<DrawImage> img);
		virtual Bool DeleteImage(NN<DrawImage> img);
	};

	class StaticBrush : public DrawBrush
	{
	public:
		UInt32 color;
	public:
		StaticBrush(UInt32 color);
		virtual ~StaticBrush();
	};

	class StaticPen : public DrawPen
	{
	public:
		UInt32 color;
		Double thick;
		UnsafeArrayOpt<UInt8> pattern;
		UOSInt nPattern;

	public:
		StaticPen(UInt32 color, Double thick, UnsafeArrayOpt<const UInt8> pattern, UOSInt nPattern);
		virtual ~StaticPen();

		virtual Double GetThick();
	};

	class StaticDrawImage : public Media::StaticImage, public Media::DrawImage
	{
	private:
		NN<Media::StaticEngine> eng;
	public:
		StaticDrawImage(NN<StaticEngine> eng, Math::Size2D<UOSInt> dispSize, Int32 fourcc, Int32 bpp, Media::PixelFormat pf, OSInt maxSize, NN<const Media::ColorProfile> color, Media::ColorProfile::YUVType yuvType, Media::AlphaType atype, Media::YCOffset ycOfst);
		virtual ~StaticDrawImage();

		virtual UOSInt GetWidth() const;
		virtual UOSInt GetHeight() const;
		virtual UInt32 GetBitCount() const;
		virtual NN<const ColorProfile> GetColorProfile() const;
		virtual Media::AlphaType GetAlphaType() const;
		virtual Double GetHDPI() const;
		virtual Double GetVDPI() const;
		virtual void SetHDPI(Double dpi);
		virtual void SetVDPI(Double dpi);
		virtual UnsafeArrayOpt<UInt8> GetImgBits(OutParam<Bool> revOrder);

/*		virtual Bool DrawLine(Double x1, Double y1, Double x2, Double y2, NN<DrawPen> p) = 0;
		virtual Bool DrawPolylineI(UnsafeArray<const Int32> points, OSInt nPoints, NN<DrawPen> p) = 0;
		virtual Bool DrawPolygonI(UnsafeArray<const Int32> points, OSInt nPoints, Optional<DrawPen> p, Optional<DrawBrush> b) = 0;
		virtual Bool DrawPolyPolygonI(UnsafeArray<const Int32> points, UnsafeArray<const UInt32> pointCnt, OSInt nPointCnt, Optional<DrawPen> p, Optional<DrawBrush> b) = 0;
		virtual Bool DrawPolyline(UnsafeArray<const Math::Coord2DDbl> points, OSInt nPoints, NN<DrawPen> p) = 0;
		virtual Bool DrawPolygon(UnsafeArray<const Math::Coord2DDbl> points, OSInt nPoints, Optional<DrawPen> p, Optional<DrawBrush> b) = 0;
		virtual Bool DrawPolyPolygon(UnsafeArray<const Math::Coord2DDbl> points, UnsafeArray<const UInt32> pointCnt, OSInt nPointCnt, Optional<DrawPen> p, Optional<DrawBrush> b) = 0;
		virtual Bool DrawRect(Double x, Double y, Double w, Double h, Optional<DrawPen> p, Optional<DrawBrush> b) = 0;
		virtual Bool DrawEllipse(Double tlx, Double tly, Double w, Double h, Optional<DrawPen> p, Optional<DrawBrush> b) = 0;
		virtual Bool DrawString(Math::Coord2DDbl tl, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b) = 0;
		virtual Bool DrawString(Math::Coord2DDbl tl, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b) = 0;
		virtual Bool DrawStringRot(Math::Coord2DDbl center, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegreeACW) = 0;
		virtual Bool DrawStringRot(Math::Coord2DDbl center, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegreeACW) = 0;
		virtual Bool DrawStringB(Math::Coord2DDbl tl, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b, UOSInt buffSize) = 0;
		virtual Bool DrawStringB(Math::Coord2DDbl tl, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b, UOSInt buffSize) = 0;
		virtual Bool DrawStringRotB(Math::Coord2DDbl center, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegreeACW, UOSInt buffSize) = 0;
		virtual Bool DrawStringRotB(Math::Coord2DDbl center, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegreeACW, UOSInt buffSize) = 0;*/
		virtual Bool DrawImagePt(NN<DrawImage> img, Math::Coord2DDbl tl);
		virtual Bool DrawImagePt2(NN<Media::StaticImage> img, Math::Coord2DDbl tl);
		virtual Bool DrawImagePt3(NN<DrawImage> img, Math::Coord2DDbl destTL, Math::Coord2DDbl srcTL, Math::Size2DDbl srcSize);

		virtual NN<DrawPen> NewPenARGB(UInt32 color, Double thick, UnsafeArrayOpt<UInt8> pattern, UOSInt nPattern);
		virtual NN<DrawBrush> NewBrushARGB(UInt32 color);
/*		virtual NN<DrawFont> NewFontPt(Text::CStringNN name, Double ptSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage) = 0; // 72 dpi size
		virtual NN<DrawFont> NewFontPx(Text::CStringNN name, Double pxSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage) = 0; // Actual size*/
		virtual void DelPen(NN<DrawPen> p);
		virtual void DelBrush(NN<DrawBrush> b);
//		virtual void DelFont(NN<DrawFont> f) = 0;

/*		virtual Math::Size2DDbl GetTextSize(NN<DrawFont> fnt, Text::CStringNN txt) = 0;
		virtual void SetTextAlign(Media::DrawEngine::DrawPos pos) = 0;
		virtual void GetStringBound(UnsafeArray<Int32> pos, OSInt centX, OSInt centY, UnsafeArray<const UTF8Char> str, NN<DrawFont> f, OutParam<OSInt> drawX, OutParam<OSInt> drawY) = 0;
		virtual void GetStringBoundRot(UnsafeArray<Int32> pos, Double centX, Double centY, UnsafeArray<const UTF8Char> str, NN<DrawFont> f, Double angleDegree, OutParam<OSInt> drawX, OutParam<OSInt> drawY) = 0;
		virtual void CopyBits(OSInt x, OSInt y, UnsafeArray<UInt8> imgPtr, UOSInt bpl, UOSInt width, UOSInt height, Bool upsideDown) const = 0;*/

		virtual Optional<Media::StaticImage> ToStaticImage() const;
//		virtual UOSInt SavePng(NN<IO::SeekableStream> stm) = 0;
		virtual UOSInt SaveGIF(NN<IO::SeekableStream> stm);
//		virtual UOSInt SaveJPG(NN<IO::SeekableStream> stm) = 0;
	};
}
#endif
