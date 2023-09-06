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

		virtual DrawImage *CreateImage32(Math::Size2D<UOSInt> size, Media::AlphaType atype);
		GDIImage *CreateImage24(Math::Size2D<UOSInt> size);
		DrawImage *CreateImageScn(void *hdc, OSInt left, OSInt top, OSInt right, OSInt bottom);
		virtual DrawImage *LoadImage(Text::CStringNN fileName);
		virtual DrawImage *LoadImageStream(NotNullPtr<IO::SeekableStream> stm);
		virtual DrawImage *ConvImage(Media::Image *img);
		virtual DrawImage *CloneImage(NotNullPtr<DrawImage> img);
		virtual Bool DeleteImage(NotNullPtr<DrawImage> img);
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
		NotNullPtr<DrawImage> img;
		void *hdc;
		const WChar *fontName;
		Double ptSize;
		Media::DrawEngine::DrawFontStyle style;
		Int32 codePage;
		
	public:
		Int32 pxSize;
		void *hfont;

		GDIFont(void *hdc, const Char *fontName, Double ptSize, Media::DrawEngine::DrawFontStyle style, NotNullPtr<DrawImage> img, Int32 codePage);
		GDIFont(void *hdc, const WChar *fontName, Double ptSize, Media::DrawEngine::DrawFontStyle style, NotNullPtr<DrawImage> img, Int32 codePage);
		virtual ~GDIFont();

		const WChar *GetNameW();
		Double GetPointSize();
		Media::DrawEngine::DrawFontStyle GetFontStyle();
		Int32 GetCodePage();
	};

	class GDIImage : public DrawImage, public Image
	{
	private:
		GDIEngine *eng;
		Math::Size2D<UOSInt> size;
		UInt32 bitCount;
		Media::DrawEngine::DrawPos strAlign;

		DrawBrush *currBrush;
		DrawFont *currFont;
		DrawPen *currPen;

		Math::Coord2D<OSInt> tl;
	public:
		void *hBmp;
		void *bmpBits;
		void *hdcBmp;

		GDIImage(GDIEngine *eng, Math::Coord2D<OSInt> tl, Math::Size2D<UOSInt> size, UInt32 bitCount, void *hBmp, void *bmpBits, void *hdcBmp, Media::AlphaType atype);
		virtual ~GDIImage();

		virtual UOSInt GetWidth() const;
		virtual UOSInt GetHeight() const;
		virtual Math::Size2D<UOSInt> GetSize() const;
		virtual UInt32 GetBitCount() const;
		virtual NotNullPtr<const ColorProfile> GetColorProfile() const;
		virtual void SetColorProfile(NotNullPtr<const ColorProfile> color);
		virtual Media::AlphaType GetAlphaType() const;
		virtual void SetAlphaType(Media::AlphaType atype);
		virtual Double GetHDPI() const;
		virtual Double GetVDPI() const;
		virtual void SetHDPI(Double dpi);
		virtual void SetVDPI(Double dpi);
		virtual UInt8 *GetImgBits(Bool *revOrder);
		virtual void GetImgBitsEnd(Bool modified);
		virtual UOSInt GetImgBpl() const;
		virtual Media::EXIFData *GetEXIF() const;
		virtual Media::PixelFormat GetPixelFormat() const;

		virtual Bool DrawLine(Double x1, Double y1, Double x2, Double y2, DrawPen *p);
		virtual Bool DrawPolylineI(const Int32 *points, UOSInt nPoints, DrawPen *p);
		virtual Bool DrawPolygonI(const Int32 *points, UOSInt nPoints, DrawPen *p, DrawBrush *b);
		virtual Bool DrawPolyPolygonI(const Int32 *points, const UInt32 *pointCnt, UOSInt nPointCnt, DrawPen *p, DrawBrush *b);
		virtual Bool DrawPolyline(const Math::Coord2DDbl *points, UOSInt nPoints, DrawPen *p);
		virtual Bool DrawPolygon(const Math::Coord2DDbl *points, UOSInt nPoints, DrawPen *p, DrawBrush *b);
		virtual Bool DrawPolyPolygon(const Math::Coord2DDbl *points, const UInt32 *pointCnt, UOSInt nPointCnt, DrawPen *p, DrawBrush *b);
		virtual Bool DrawRect(Math::Coord2DDbl tl, Math::Size2DDbl size, DrawPen *p, DrawBrush *b);
		virtual Bool DrawEllipse(Math::Coord2DDbl tl, Math::Size2DDbl size, DrawPen *p, DrawBrush *b);
		virtual Bool DrawString(Math::Coord2DDbl tl, NotNullPtr<Text::String> str, DrawFont *f, DrawBrush *b);
		virtual Bool DrawString(Math::Coord2DDbl tl, Text::CString str, DrawFont *f, DrawBrush *b);
		Bool DrawStringW(Math::Coord2DDbl tl, const WChar *str, DrawFont *f, DrawBrush *p);
		virtual Bool DrawStringRot(Math::Coord2DDbl center, NotNullPtr<Text::String> str, DrawFont *f, DrawBrush *p, Double angleDegree);
		virtual Bool DrawStringRot(Math::Coord2DDbl center, Text::CString str, DrawFont *f, DrawBrush *p, Double angleDegree);
		Bool DrawStringRotW(Math::Coord2DDbl center, const WChar *str, DrawFont *f, DrawBrush *p, Double angleDegree);
		virtual Bool DrawStringB(Math::Coord2DDbl tl, NotNullPtr<Text::String> str, DrawFont *f, DrawBrush *p, UOSInt buffSize);
		virtual Bool DrawStringB(Math::Coord2DDbl tl, Text::CString str, DrawFont *f, DrawBrush *p, UOSInt buffSize);
		Bool DrawStringBW(Math::Coord2DDbl tl, const WChar *str, DrawFont *f, DrawBrush *p, UOSInt buffSize);
		virtual Bool DrawStringRotB(Math::Coord2DDbl center, NotNullPtr<Text::String> str, DrawFont *f, DrawBrush *p, Double angleDegree, UOSInt buffSize);
		virtual Bool DrawStringRotB(Math::Coord2DDbl center, Text::CString str, DrawFont *f, DrawBrush *p, Double angleDegree, UOSInt buffSize);
		Bool DrawStringRotBW(Math::Coord2DDbl center, const WChar *str, DrawFont *f, DrawBrush *p, Double angleDegree, UOSInt buffSize);
		virtual Bool DrawImagePt(NotNullPtr<DrawImage> img, Math::Coord2DDbl tl);
		virtual Bool DrawImagePt2(NotNullPtr<Media::StaticImage> img, Math::Coord2DDbl tl);
		virtual Bool DrawImagePt3(NotNullPtr<DrawImage> img, Math::Coord2DDbl destTL, Math::Coord2DDbl srcTL, Math::Size2DDbl srcSize);
		Bool DrawImageRect(NotNullPtr<DrawImage> img, OSInt tlx, OSInt tly, OSInt brx, OSInt bry);

		virtual DrawPen *NewPenARGB(UInt32 color, Double thick, UInt8 *pattern, UOSInt nPattern);
		virtual DrawBrush *NewBrushARGB(UInt32 color);
		virtual DrawFont *NewFontPt(Text::CString name, Double ptSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage);
		DrawFont *NewFontPtW(const WChar *name, Double ptSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage);
		virtual DrawFont *NewFontPx(Text::CString name, Double pxSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage);
		DrawFont *NewFontPxW(const WChar *name, Double pxSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage);
		virtual DrawFont *CloneFont(Media::DrawFont *f);
		virtual void DelPen(DrawPen *p);
		virtual void DelBrush(DrawBrush *b);
		virtual void DelFont(DrawFont *f);

		virtual Math::Size2DDbl GetTextSize(DrawFont *fnt, Text::CString txt);
		Math::Size2DDbl GetTextSize(DrawFont *fnt, const WChar *txt, OSInt txtLen);
		virtual void SetTextAlign(DrawEngine::DrawPos pos);
		virtual void GetStringBound(Int32 *pos, OSInt centX, OSInt centY, const UTF8Char *str, DrawFont *f, OSInt *drawX, OSInt *drawY);
		void GetStringBoundW(Int32 *pos, OSInt centX, OSInt centY, const WChar *str, DrawFont *f, OSInt *drawX, OSInt *drawY);
		virtual void GetStringBoundRot(Int32 *pos, Double centX, Double centY, const UTF8Char *str, DrawFont *f, Double angleDegree, OSInt *drawX, OSInt *drawY);
		void GetStringBoundRotW(Int32 *pos, Double centX, Double centY, const WChar *str, DrawFont *f, Double angleDegree, OSInt *drawX, OSInt *drawY);
		virtual void CopyBits(OSInt x, OSInt y, void *imgPtr, UOSInt bpl, UOSInt width, UOSInt height, Bool upsideDown) const;

		virtual Media::StaticImage *ToStaticImage() const;
		virtual UOSInt SavePng(NotNullPtr<IO::SeekableStream> stm);
		virtual UOSInt SaveGIF(NotNullPtr<IO::SeekableStream> stm);
		virtual UOSInt SaveJPG(NotNullPtr<IO::SeekableStream> stm);

		virtual Media::Image *Clone() const;
		virtual Media::Image::ImageType GetImageType() const;
		virtual void GetImageData(UInt8 *destBuff, OSInt left, OSInt top, UOSInt width, UOSInt height, UOSInt destBpl, Bool upsideDown, Media::RotateType destRotate) const;

		static void PolylineAccel(void *hdc, const Int32 *points, UOSInt nPoints, OSInt ofstX, OSInt ofstY, OSInt width, OSInt height);
		static void PolygonAccel(void *hdc, const Int32 *points, UOSInt nPoints, OSInt ofstX, OSInt ofstY, OSInt width, OSInt height, Int32 penWidth);
		static void PolyPolygonAccel(void *hdc, const Int32 *points, const UInt32 *pointCnt, UOSInt nPointCnt, OSInt ofstX, OSInt ofstY, OSInt width, OSInt height, Int32 penWidth);

		Bool IsOffScreen();
		Bool DrawRectN(OSInt x, OSInt y, OSInt w, OSInt h, DrawPen *p, DrawBrush *b);

		void *GetHDC();
		void FillAlphaRect(OSInt left, OSInt top, OSInt width, OSInt height, UInt8 alpha);
	private:
		void *CreateGDIImage();
	};
}
#endif
