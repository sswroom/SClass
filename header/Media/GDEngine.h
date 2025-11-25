#ifndef _SM_MEDIA_GDENGINE
#define _SM_MEDIA_GDENGINE
#include "Media/DrawEngine.h"

namespace Media
{
	class GDEngine : public DrawEngine
	{
	public:
		GDEngine();
		virtual ~GDEngine();

		virtual Optional<DrawImage> CreateImage32(Math::Size2D<UOSInt> size, Media::AlphaType atype);
		virtual Optional<DrawImage> LoadImage(Text::CStringNN fileName);
		virtual Optional<DrawImage> LoadImageStream(NN<IO::SeekableStream> stm);
		virtual Optional<DrawImage> ConvImage(NN<Media::RasterImage> img, Optional<Media::ColorSess> colorSess);
		virtual Optional<DrawImage> CloneImage(NN<DrawImage> img);
		virtual Bool DeleteImage(NN<DrawImage> img);
		virtual void EndColorSess(NN<Media::ColorSess> colorSess);

		void *CreateIOCtx(NN<IO::SeekableStream> stm);
		void DeleteIOCtx(void *obj);
	};

	class GDBrush : public DrawBrush
	{
	private:
		UInt32 color;
	public:
		GDBrush(UInt32 color, NN<DrawImage> img);
		~GDBrush();
		
		Int32 InitImage(NN<DrawImage> img);
	};

	class GDPen : public DrawPen
	{
	private:
		Int32 color;
		Int32 thick;
		int *pattern;
		Int32 nPattern;
	public:
		GDPen(UInt32 color, Double thick, UnsafeArrayOpt<UInt8> pattern, UOSInt nPattern, NN<DrawImage> img);
		~GDPen();
		
		Int32 InitImage(NN<DrawImage> img);
		virtual Double GetThick();
	};

	class GDFont : public DrawFont
	{
	private:
		NN<Text::String> name;
		Optional<Text::String> ttcName;
		Double pxSize;
		Media::DrawEngine::DrawFontStyle style;
	public:
		GDFont(Text::CStringNN name, Double pxSize, Media::DrawEngine::DrawFontStyle style);
		~GDFont();

		NN<Text::String> GetName() const;
		NN<Text::String> GetTTCName();
		Double GetPXSize() const;
		Double GetPointSize() const;
		Media::DrawEngine::DrawFontStyle GetFontStyle() const;
	};

	class GDImage : public DrawImage
	{
	private:
		NN<GDEngine> eng;
		UOSInt width;
		UOSInt height;
		UInt32 bitCount;
	public:
		AnyType imgPtr;

		GDImage(NN<GDEngine> eng, Math::Size2D<UOSInt> size, UInt32 bitCount, AnyType imgPtr);
		virtual ~GDImage();

		virtual UOSInt GetWidth() const;
		virtual UOSInt GetHeight() const;
		virtual Math::Size2D<UOSInt> GetSize() const;
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
		virtual UOSInt GetImgBpl() const;
		virtual Optional<Media::EXIFData> GetEXIF() const;
		virtual Media::PixelFormat GetPixelFormat() const;
		virtual void SetColorSess(Optional<Media::ColorSess> colorSess);

		virtual Bool DrawLine(Double x1, Double y1, Double x2, Double y2, NN<DrawPen> p);
		virtual Bool DrawPolylineI(UnsafeArray<const Int32> points, UOSInt nPoints, NN<DrawPen> p);
		virtual Bool DrawPolygonI(UnsafeArray<const Int32> points, UOSInt nPoints, Optional<DrawPen> p, Optional<DrawBrush> b);
		virtual Bool DrawPolyPolygonI(UnsafeArray<const Int32> points, UnsafeArray<const UInt32> pointCnt, UOSInt nPointCnt, Optional<DrawPen> p, Optional<DrawBrush> b);
		virtual Bool DrawPolyline(UnsafeArray<const Math::Coord2DDbl> points, UOSInt nPoints, NN<DrawPen> p);
		virtual Bool DrawPolygon(UnsafeArray<const Math::Coord2DDbl> points, UOSInt nPoints, Optional<DrawPen> p, Optional<DrawBrush> b);
		virtual Bool DrawPolyPolygon(UnsafeArray<const Math::Coord2DDbl> points, UnsafeArray<const UInt32> pointCnt, UOSInt nPointCnt, Optional<DrawPen> p, Optional<DrawBrush> b);
		virtual Bool DrawRect(Math::Coord2DDbl tl, Math::Size2DDbl size, Optional<DrawPen> p, Optional<DrawBrush> b);
		virtual Bool DrawEllipse(Math::Coord2DDbl tl, Math::Size2DDbl size, Optional<DrawPen> p, Optional<DrawBrush> b);
		virtual Bool DrawString(Math::Coord2DDbl tl, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b);
		virtual Bool DrawString(Math::Coord2DDbl tl, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b);
		virtual Bool DrawStringRot(Math::Coord2DDbl center, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegreeACW);
		virtual Bool DrawStringRot(Math::Coord2DDbl center, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegreeACW);
		virtual Bool DrawStringB(Math::Coord2DDbl tl, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b, UOSInt buffSize);
		virtual Bool DrawStringB(Math::Coord2DDbl tl, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b, UOSInt buffSize);
		virtual Bool DrawStringRotB(Math::Coord2DDbl center, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegreeACW, UOSInt buffSize);
		virtual Bool DrawStringRotB(Math::Coord2DDbl center, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegreeACW, UOSInt buffSize);
		virtual Bool DrawImagePt(NN<DrawImage> img, Math::Coord2DDbl tl);
		virtual Bool DrawImagePt2(NN<Media::StaticImage> img, Math::Coord2DDbl tl);
		virtual Bool DrawImagePt3(NN<DrawImage> img, Math::Coord2DDbl destTL, Math::Coord2DDbl srcTL, Math::Size2DDbl srcSize);
		virtual Bool DrawImageQuad(NN<Media::StaticImage> img, Math::Quadrilateral quad);

		virtual NN<DrawPen> NewPenARGB(UInt32 color, Double thick, UnsafeArrayOpt<UInt8> pattern, UOSInt nPattern);
		virtual NN<DrawBrush> NewBrushARGB(UInt32 color);
		virtual NN<DrawFont> NewFontPt(Text::CStringNN name, Double ptSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage); // 72 dpi size
		virtual NN<DrawFont> NewFontPx(Text::CStringNN name, Double pxSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage); // Actual size
		virtual NN<DrawFont> CloneFont(NN<DrawFont> f);
		virtual void DelPen(NN<DrawPen> p);
		virtual void DelBrush(NN<DrawBrush> b);
		virtual void DelFont(NN<DrawFont> f);

		virtual Math::Size2DDbl GetTextSize(NN<DrawFont> fnt, Text::CStringNN txt);
		virtual void SetTextAlign(Media::DrawEngine::DrawPos pos);
		virtual void GetStringBound(UnsafeArray<Int32> pos, OSInt centX, OSInt centY, UnsafeArray<const UTF8Char> str, NN<DrawFont> f, OutParam<OSInt> drawX, OutParam<OSInt> drawY);
		virtual void GetStringBoundRot(UnsafeArray<Int32> pos, Double centX, Double centY, UnsafeArray<const UTF8Char> str, NN<DrawFont> f, Double angleDegree, OutParam<OSInt> drawX, OutParam<OSInt> drawY);
		virtual void CopyBits(OSInt x, OSInt y, UnsafeArray<UInt8> imgPtr, UOSInt bpl, UOSInt width, UOSInt height, Bool upsideDown) const;

		virtual Optional<Media::StaticImage> ToStaticImage() const;
		virtual UOSInt SavePng(NN<IO::SeekableStream> stm);
		virtual UOSInt SaveGIF(NN<IO::SeekableStream> stm);
		virtual UOSInt SaveJPG(NN<IO::SeekableStream> stm);
	};
}
#endif
