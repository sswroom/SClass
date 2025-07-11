#ifndef _SM_MEDIA_GDIENGINE
#define _SM_MEDIA_GDIENGINE
#include "Media/DrawEngine.h"
#include "Media/ABlend/AlphaBlend8_C8.h"
#include "Sync/Mutex.h"

namespace Media
{
	class GDIImage;

	class GDIEngine : public Media::DrawEngine
	{
	public:
		Media::ABlend::AlphaBlend8_C8 iab;
		Sync::Mutex iabMut;
	private:
		void *hdc;
		void *hdcScreen;
		void *hpenBlack;
		void *hbrushWhite;
		Sync::Mutex gdiMut;
#ifndef _WIN32_WCE
		void *gdiplusStartupInput;
		UInt32 gdiplusToken;
#endif

	public:
		GDIEngine();
		virtual ~GDIEngine();

		virtual Optional<DrawImage> CreateImage32(Math::Size2D<UOSInt> size, Media::AlphaType atype);
		Optional<GDIImage> CreateImage24(Math::Size2D<UOSInt> size);
		NN<DrawImage> CreateImageScn(void *hdc, OSInt left, OSInt top, OSInt right, OSInt bottom, Optional<Media::ColorSess> colorSess);
		virtual Optional<DrawImage> LoadImage(Text::CStringNN fileName);
		virtual Optional<DrawImage> LoadImageStream(NN<IO::SeekableStream> stm);
		virtual Optional<DrawImage> ConvImage(NN<Media::RasterImage> img, Optional<Media::ColorSess> colorSess);
		virtual Optional<DrawImage> CloneImage(NN<DrawImage> img);
		virtual Bool DeleteImage(NN<DrawImage> img);
		virtual void EndColorSess(NN<Media::ColorSess> colorSess);
		void *GetBlackPen();
		void *GetWhiteBrush();
	};

	class GDIBrush : public DrawBrush
	{
	public:
		void *hbrush;
		DrawImage *img;
		UInt32 color;
		UInt32 oriColor;

	public:
		GDIBrush(void *hbrush, UInt32 oriColor, DrawImage *img);
		virtual ~GDIBrush();
	};

	class GDIPen : public DrawPen
	{
	public:
		void *hpen;
		UInt32 *pattern;
		UOSInt nPattern;
		DrawImage *img;
		Double thick;
		UInt32 oriColor;
	public:
		GDIPen(void *hpen, UInt32 *pattern, UOSInt nPattern, DrawImage *img, Double thick, UInt32 oriColor);
		virtual ~GDIPen();

		Double GetThick();
	};

	class GDIFont : public DrawFont
	{
	private:
		NN<DrawImage> img;
		void *hdc;
		UnsafeArray<const WChar> fontName;
		Double ptSize;
		Media::DrawEngine::DrawFontStyle style;
		Int32 codePage;
		
	public:
		Int32 pxSize;
		void *hfont;

		GDIFont(void *hdc, UnsafeArray<const Char> fontName, Double ptSize, Media::DrawEngine::DrawFontStyle style, NN<DrawImage> img, Int32 codePage);
		GDIFont(void *hdc, UnsafeArray<const WChar> fontName, Double ptSize, Media::DrawEngine::DrawFontStyle style, NN<DrawImage> img, Int32 codePage);
		virtual ~GDIFont();

		UnsafeArray<const WChar> GetNameW();
		Double GetPointSize();
		Media::DrawEngine::DrawFontStyle GetFontStyle();
		Int32 GetCodePage();
	};

	class GDIImage : public DrawImage, public RasterImage
	{
	private:
		NN<GDIEngine> eng;
		Math::Size2D<UOSInt> size;
		UInt32 bitCount;
		Media::DrawEngine::DrawPos strAlign;

		Optional<DrawBrush> currBrush;
		Optional<DrawFont> currFont;
		Optional<DrawPen> currPen;
		Optional<Media::ColorSess> colorSess;

		Math::Coord2D<OSInt> tl;
	public:
		void *hBmp;
		void *bmpBits;
		void *hdcBmp;

		GDIImage(NN<GDIEngine> eng, Math::Coord2D<OSInt> tl, Math::Size2D<UOSInt> size, UInt32 bitCount, void *hBmp, void *bmpBits, void *hdcBmp, Media::AlphaType atype);
		virtual ~GDIImage();

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

		virtual Bool DrawLine(Double x1, Double y1, Double x2, Double y2, NN<DrawPen>);
		virtual Bool DrawPolylineI(UnsafeArray<const Int32> points, UOSInt nPoints, NN<DrawPen>);
		virtual Bool DrawPolygonI(UnsafeArray<const Int32> points, UOSInt nPoints, Optional<DrawPen> p, Optional<DrawBrush> b);
		virtual Bool DrawPolyPolygonI(UnsafeArray<const Int32> points, UnsafeArray<const UInt32> pointCnt, UOSInt nPointCnt, Optional<DrawPen> p, Optional<DrawBrush> b);
		virtual Bool DrawPolyline(UnsafeArray<const Math::Coord2DDbl> points, UOSInt nPoints, NN<DrawPen>);
		virtual Bool DrawPolygon(UnsafeArray<const Math::Coord2DDbl> points, UOSInt nPoints, Optional<DrawPen> p, Optional<DrawBrush> b);
		virtual Bool DrawPolyPolygon(UnsafeArray<const Math::Coord2DDbl> points, UnsafeArray<const UInt32> pointCnt, UOSInt nPointCnt, Optional<DrawPen> p, Optional<DrawBrush> b);
		virtual Bool DrawRect(Math::Coord2DDbl tl, Math::Size2DDbl size, Optional<DrawPen> p, Optional<DrawBrush> b);
		virtual Bool DrawEllipse(Math::Coord2DDbl tl, Math::Size2DDbl size, Optional<DrawPen> p, Optional<DrawBrush> b);
		virtual Bool DrawString(Math::Coord2DDbl tl, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b);
		virtual Bool DrawString(Math::Coord2DDbl tl, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b);
		Bool DrawStringW(Math::Coord2DDbl tl, UnsafeArray<const WChar> str, NN<DrawFont> f, NN<DrawBrush> b);
		virtual Bool DrawStringRot(Math::Coord2DDbl center, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegree);
		virtual Bool DrawStringRot(Math::Coord2DDbl center, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegree);
		Bool DrawStringRotW(Math::Coord2DDbl center, UnsafeArray<const WChar> str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegree);
		virtual Bool DrawStringB(Math::Coord2DDbl tl, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b, UOSInt buffSize);
		virtual Bool DrawStringB(Math::Coord2DDbl tl, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b, UOSInt buffSize);
		Bool DrawStringBW(Math::Coord2DDbl tl, UnsafeArray<const WChar> str, NN<DrawFont> f, NN<DrawBrush> b, UOSInt buffSize);
		virtual Bool DrawStringRotB(Math::Coord2DDbl center, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegree, UOSInt buffSize);
		virtual Bool DrawStringRotB(Math::Coord2DDbl center, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegree, UOSInt buffSize);
		Bool DrawStringRotBW(Math::Coord2DDbl center, UnsafeArray<const WChar> str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegree, UOSInt buffSize);
		virtual Bool DrawImagePt(NN<DrawImage> img, Math::Coord2DDbl tl);
		virtual Bool DrawImagePt2(NN<Media::StaticImage> img, Math::Coord2DDbl tl);
		virtual Bool DrawImagePt3(NN<DrawImage> img, Math::Coord2DDbl destTL, Math::Coord2DDbl srcTL, Math::Size2DDbl srcSize);
		Bool DrawImageRect(NN<DrawImage> img, OSInt tlx, OSInt tly, OSInt brx, OSInt bry);
		virtual Bool DrawImageQuad(NN<Media::StaticImage> img, Math::Quadrilateral quad);

		virtual NN<DrawPen> NewPenARGB(UInt32 color, Double thick, UnsafeArrayOpt<UInt8> pattern, UOSInt nPattern);
		virtual NN<DrawBrush> NewBrushARGB(UInt32 color);
		virtual NN<DrawFont> NewFontPt(Text::CStringNN name, Double ptSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage);
		NN<DrawFont> NewFontPtW(UnsafeArray<const WChar> name, Double ptSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage);
		virtual NN<DrawFont> NewFontPx(Text::CStringNN name, Double pxSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage);
		NN<DrawFont> NewFontPxW(UnsafeArray<const WChar> name, Double pxSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage);
		virtual NN<DrawFont> CloneFont(NN<Media::DrawFont> f);
		virtual void DelPen(NN<DrawPen> p);
		virtual void DelBrush(NN<DrawBrush> b);
		virtual void DelFont(NN<DrawFont> f);

		virtual Math::Size2DDbl GetTextSize(NN<DrawFont> fnt, Text::CStringNN txt);
		Math::Size2DDbl GetTextSize(NN<DrawFont> fnt, UnsafeArray<const WChar> txt, OSInt txtLen);
		virtual void SetTextAlign(DrawEngine::DrawPos pos);
		virtual void GetStringBound(UnsafeArray<Int32> pos, OSInt centX, OSInt centY, UnsafeArray<const UTF8Char> str, NN<DrawFont> f, OutParam<OSInt> drawX, OutParam<OSInt> drawY);
		void GetStringBoundW(UnsafeArray<Int32> pos, OSInt centX, OSInt centY, UnsafeArray<const WChar> str, NN<DrawFont> f, OutParam<OSInt> drawX, OutParam<OSInt> drawY);
		virtual void GetStringBoundRot(UnsafeArray<Int32> pos, Double centX, Double centY, UnsafeArray<const UTF8Char> str, NN<DrawFont> f, Double angleDegree, OutParam<OSInt> drawX, OutParam<OSInt>drawY);
		void GetStringBoundRotW(UnsafeArray<Int32> pos, Double centX, Double centY, UnsafeArray<const WChar> str, NN<DrawFont> f, Double angleDegree, OutParam<OSInt> drawX, OutParam<OSInt> drawY);
		virtual void CopyBits(OSInt x, OSInt y, UnsafeArray<UInt8> imgPtr, UOSInt bpl, UOSInt width, UOSInt height, Bool upsideDown) const;

		virtual Optional<Media::StaticImage> ToStaticImage() const;
		virtual UOSInt SavePng(NN<IO::SeekableStream> stm);
		virtual UOSInt SaveGIF(NN<IO::SeekableStream> stm);
		virtual UOSInt SaveJPG(NN<IO::SeekableStream> stm);

		virtual NN<Media::RasterImage> Clone() const;
		virtual Media::RasterImage::ImageType GetImageType() const;
		virtual void GetRasterData(UnsafeArray<UInt8> destBuff, OSInt left, OSInt top, UOSInt width, UOSInt height, UOSInt destBpl, Bool upsideDown, Media::RotateType destRotate) const;

		static void PolylineAccel(void *hdc, const Int32 *points, UOSInt nPoints, OSInt ofstX, OSInt ofstY, OSInt width, OSInt height);
		static void PolygonAccel(void *hdc, const Int32 *points, UOSInt nPoints, OSInt ofstX, OSInt ofstY, OSInt width, OSInt height, Int32 penWidth);
		static void PolyPolygonAccel(void *hdc, const Int32 *points, const UInt32 *pointCnt, UOSInt nPointCnt, OSInt ofstX, OSInt ofstY, OSInt width, OSInt height, Int32 penWidth);

		Bool IsOffScreen();
		Bool DrawRectN(OSInt x, OSInt y, OSInt w, OSInt h, DrawPen *p, Optional<DrawBrush> b);

		void *GetHDC();
		void FillAlphaRect(OSInt left, OSInt top, OSInt width, OSInt height, UInt8 alpha);
	private:
		void *CreateGDIImage();
	};
}
#endif
