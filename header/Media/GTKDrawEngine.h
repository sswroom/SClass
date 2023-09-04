#ifndef _SM_MEDIA_GDIENGINE
#define _SM_MEDIA_GDIENGINE
#include "Media/DrawEngine.h"
#include "Media/ABlend/AlphaBlend8_C8.h"
#include "Text/String.h"

namespace Media
{
	class GTKImage;

	class GTKDrawEngine : public Media::DrawEngine
	{
	public:
		Media::ABlend::AlphaBlend8_C8 iab;
	public:
		GTKDrawEngine();
		virtual ~GTKDrawEngine();

		virtual DrawImage *CreateImage32(Math::Size2D<UOSInt> size, Media::AlphaType atype);
		NotNullPtr<DrawImage> CreateImageScn(void *cr, Math::Coord2D<OSInt> tl, Math::Coord2D<OSInt> br);
		virtual DrawImage *LoadImage(Text::CStringNN fileName);
		virtual DrawImage *LoadImageStream(NotNullPtr<IO::SeekableStream> stm);
		virtual DrawImage *ConvImage(Media::Image *img);
		virtual DrawImage *CloneImage(NotNullPtr<DrawImage> img);
		virtual Bool DeleteImage(NotNullPtr<DrawImage> img);
	};

	class GTKDrawFont : public DrawFont
	{
	private:
		NotNullPtr<Text::String> fontName;
		Double fontHeight;
		OSInt fontSlant;
		OSInt fontWeight;
	public:
		GTKDrawFont(Text::CString fontName, Double pxHeight, Media::DrawEngine::DrawFontStyle drawFontStyle);
		GTKDrawFont(Text::String *fontName, Double pxHeight, OSInt fontSlant, OSInt fontWeight);
		GTKDrawFont(NotNullPtr<Text::String> fontName, Double pxHeight, OSInt fontSlant, OSInt fontWeight);
		~GTKDrawFont();

		void Init(void *cr);
		NotNullPtr<Text::String> GetFontName();
		Double GetHeight();
		OSInt GetFontWeight();
		OSInt GetFontSlant();
	};

	class GTKDrawPen : public DrawPen
	{
	private:
		UInt32 oriColor;
		Double rV;
		Double gV;
		Double bV;
		Double aV;
		Double thick;

	public:
		GTKDrawPen(UInt32 oriColor, Double thick);
		virtual ~GTKDrawPen();

		virtual Double GetThick();

		void Init(void *cr);
	};

	class GTKDrawBrush : public DrawBrush
	{
	private:
		UInt32 oriColor;
		Double rV;
		Double gV;
		Double bV;
		Double aV;

	public:
		GTKDrawBrush(UInt32 oriColor);
		~GTKDrawBrush();

		void Init(void *cr);
		UInt32 GetOriColor();
	};

	class GTKDrawImage : public DrawImage, public Image
	{
	private:
		GTKDrawEngine *eng;
		void *surface; //cairo_surface_t *
		void *cr; //cairo_t *
		Math::Coord2D<OSInt> tl;

	public:
		GTKDrawImage(GTKDrawEngine *eng, void *surface, void *cr, Math::Coord2D<OSInt> tl, Math::Size2D<UOSInt> size, UInt32 bitCount, Media::AlphaType atype);
		virtual ~GTKDrawImage();

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
		virtual UInt8 *GetImgBits(Bool *upsideDown);
		virtual void GetImgBitsEnd(Bool modified);
		virtual UOSInt GetImgBpl() const;
		virtual Media::EXIFData *GetEXIF() const;
		virtual Media::PixelFormat GetPixelFormat() const;

		virtual Bool DrawLine(Double x1, Double y1, Double x2, Double y2, DrawPen *p); ////////////////////////////////////
		virtual Bool DrawPolylineI(const Int32 *points, UOSInt nPoints, DrawPen *p); ////////////////////////////////////
		virtual Bool DrawPolygonI(const Int32 *points, UOSInt nPoints, DrawPen *p, DrawBrush *b); ////////////////////////////////////
		virtual Bool DrawPolyPolygonI(const Int32 *points, const UInt32 *pointCnt, UOSInt nPointCnt, DrawPen *p, DrawBrush *b); ////////////////////////////////////
		virtual Bool DrawPolyline(const Math::Coord2DDbl *points, UOSInt nPoints, DrawPen *p); ////////////////////////////////////
		virtual Bool DrawPolygon(const Math::Coord2DDbl *points, UOSInt nPoints, DrawPen *p, DrawBrush *b); ////////////////////////////////////
		virtual Bool DrawPolyPolygon(const Math::Coord2DDbl *points, const UInt32 *pointCnt, UOSInt nPointCnt, DrawPen *p, DrawBrush *b); ////////////////////////////////////
		virtual Bool DrawRect(Math::Coord2DDbl tl, Math::Size2DDbl size, DrawPen *p, DrawBrush *b);
		virtual Bool DrawEllipse(Math::Coord2DDbl tl, Math::Size2DDbl size, DrawPen *p, DrawBrush *b); ////////////////////////////////////
		virtual Bool DrawString(Math::Coord2DDbl tl, NotNullPtr<Text::String> str, DrawFont *f, DrawBrush *p);
		virtual Bool DrawString(Math::Coord2DDbl tl, Text::CString str, DrawFont *f, DrawBrush *p);
		virtual Bool DrawStringRot(Math::Coord2DDbl center, NotNullPtr<Text::String> str, DrawFont *f, DrawBrush *p, Double angleDegree);
		virtual Bool DrawStringRot(Math::Coord2DDbl center, Text::CString str, DrawFont *f, DrawBrush *p, Double angleDegree);
		virtual Bool DrawStringB(Math::Coord2DDbl tl, NotNullPtr<Text::String> str, DrawFont *f, DrawBrush *p, UOSInt buffSize);
		virtual Bool DrawStringB(Math::Coord2DDbl tl, Text::CString str, DrawFont *f, DrawBrush *p, UOSInt buffSize);
		virtual Bool DrawStringRotB(Math::Coord2DDbl center, NotNullPtr<Text::String> str, DrawFont *f, DrawBrush *p, Double angleDegree, UOSInt buffSize);
		virtual Bool DrawStringRotB(Math::Coord2DDbl center, Text::CString str, DrawFont *f, DrawBrush *p, Double angleDegree, UOSInt buffSize);
		virtual Bool DrawImagePt(NotNullPtr<DrawImage> img, Math::Coord2DDbl tl); ////////////////////////////////////
		virtual Bool DrawImagePt2(Media::StaticImage *img, Math::Coord2DDbl tl); ////////////////////////////////////
		virtual Bool DrawImagePt3(DrawImage *img, Math::Coord2DDbl destTL, Math::Coord2DDbl srcTL, Math::Size2DDbl srcSize); ////////////////////////////////////

		virtual DrawPen *NewPenARGB(UInt32 color, Double thick, UInt8 *pattern, UOSInt nPattern);
		virtual DrawBrush *NewBrushARGB(UInt32 color);
		virtual DrawFont *NewFontPt(Text::CString name, Double ptSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage);
		virtual DrawFont *NewFontPx(Text::CString name, Double pxSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage);
		virtual DrawFont *CloneFont(DrawFont *f);
		virtual void DelPen(DrawPen *p);
		virtual void DelBrush(DrawBrush *b);
		virtual void DelFont(DrawFont *f);

		virtual Math::Size2DDbl GetTextSize(DrawFont *fnt, Text::CString txt);
		virtual void SetTextAlign(DrawEngine::DrawPos pos); ////////////////////////////////////
		virtual void GetStringBound(Int32 *pos, OSInt centX, OSInt centY, const UTF8Char *str, DrawFont *f, OSInt *drawX, OSInt *drawY); ////////////////////////////////////
		virtual void GetStringBoundRot(Int32 *pos, Double centX, Double centY, const UTF8Char *str, DrawFont *f, Double angleDegree, OSInt *drawX, OSInt *drawY); ////////////////////////////////////
		virtual void CopyBits(OSInt x, OSInt y, void *imgPtr, UOSInt bpl, UOSInt width, UOSInt height, Bool upsideDown) const; ////////////////////////////////////

		virtual Media::StaticImage *ToStaticImage() const; ////////////////////////////////////
		virtual UOSInt SavePng(NotNullPtr<IO::SeekableStream> stm);
		virtual UOSInt SaveGIF(NotNullPtr<IO::SeekableStream> stm); ////////////////////////////////////
		virtual UOSInt SaveJPG(NotNullPtr<IO::SeekableStream> stm);

		virtual Media::Image *Clone() const; ////////////////////////////////////
		virtual Media::Image::ImageType GetImageType() const;
		virtual void GetImageData(UInt8 *destBuff, OSInt left, OSInt top, UOSInt width, UOSInt height, UOSInt destBpl, Bool upsideDown, Media::RotateType destRotate) const;
		virtual Int32 GetPixel32(OSInt x, OSInt y) const; ////////////////////////////////////

		void *GetSurface() const;
		void *GetCairo() const;
	};
}
#endif
