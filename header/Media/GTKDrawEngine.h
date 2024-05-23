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

		virtual Optional<DrawImage> CreateImage32(Math::Size2D<UOSInt> size, Media::AlphaType atype);
		NN<DrawImage> CreateImageScn(void *cr, Math::Coord2D<OSInt> tl, Math::Coord2D<OSInt> br);
		virtual Optional<DrawImage> LoadImage(Text::CStringNN fileName);
		virtual Optional<DrawImage> LoadImageStream(NN<IO::SeekableStream> stm);
		virtual Optional<DrawImage> ConvImage(NN<Media::RasterImage> img);
		virtual Optional<DrawImage> CloneImage(NN<DrawImage> img);
		virtual Bool DeleteImage(NN<DrawImage> img);
	};

	class GTKDrawFont : public DrawFont
	{
	private:
		NN<Text::String> fontName;
		Double fontHeight;
		OSInt fontSlant;
		OSInt fontWeight;
	public:
		GTKDrawFont(Text::CString fontName, Double pxHeight, Media::DrawEngine::DrawFontStyle drawFontStyle);
		GTKDrawFont(Text::String *fontName, Double pxHeight, OSInt fontSlant, OSInt fontWeight);
		GTKDrawFont(NN<Text::String> fontName, Double pxHeight, OSInt fontSlant, OSInt fontWeight);
		~GTKDrawFont();

		void Init(void *cr);
		NN<Text::String> GetFontName();
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

	class GTKDrawImage : public DrawImage, public RasterImage
	{
	private:
		NN<GTKDrawEngine> eng;
		void *surface; //cairo_surface_t *
		void *cr; //cairo_t *
		Math::Coord2D<OSInt> tl;

	public:
		GTKDrawImage(NN<GTKDrawEngine> eng, void *surface, void *cr, Math::Coord2D<OSInt> tl, Math::Size2D<UOSInt> size, UInt32 bitCount, Media::AlphaType atype);
		virtual ~GTKDrawImage();

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
		virtual UInt8 *GetImgBits(OutParam<Bool> upsideDown);
		virtual void GetImgBitsEnd(Bool modified);
		virtual UOSInt GetImgBpl() const;
		virtual Optional<Media::EXIFData> GetEXIF() const;
		virtual Media::PixelFormat GetPixelFormat() const;

		virtual Bool DrawLine(Double x1, Double y1, Double x2, Double y2, NN<DrawPen> p); ////////////////////////////////////
		virtual Bool DrawPolylineI(const Int32 *points, UOSInt nPoints, NN<DrawPen> p); ////////////////////////////////////
		virtual Bool DrawPolygonI(const Int32 *points, UOSInt nPoints, Optional<DrawPen> p, Optional<DrawBrush> b); ////////////////////////////////////
		virtual Bool DrawPolyPolygonI(const Int32 *points, const UInt32 *pointCnt, UOSInt nPointCnt, Optional<DrawPen> p, Optional<DrawBrush> b); ////////////////////////////////////
		virtual Bool DrawPolyline(const Math::Coord2DDbl *points, UOSInt nPoints, NN<DrawPen> p); ////////////////////////////////////
		virtual Bool DrawPolygon(const Math::Coord2DDbl *points, UOSInt nPoints, Optional<DrawPen> p, Optional<DrawBrush> b); ////////////////////////////////////
		virtual Bool DrawPolyPolygon(const Math::Coord2DDbl *points, const UInt32 *pointCnt, UOSInt nPointCnt, Optional<DrawPen> p, Optional<DrawBrush> b); ////////////////////////////////////
		virtual Bool DrawRect(Math::Coord2DDbl tl, Math::Size2DDbl size, Optional<DrawPen> p, Optional<DrawBrush> b);
		virtual Bool DrawEllipse(Math::Coord2DDbl tl, Math::Size2DDbl size, Optional<DrawPen> p, Optional<DrawBrush> b); ////////////////////////////////////
		virtual Bool DrawString(Math::Coord2DDbl tl, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> p);
		virtual Bool DrawString(Math::Coord2DDbl tl, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> p);
		virtual Bool DrawStringRot(Math::Coord2DDbl center, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> p, Double angleDegree);
		virtual Bool DrawStringRot(Math::Coord2DDbl center, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> p, Double angleDegree);
		virtual Bool DrawStringB(Math::Coord2DDbl tl, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> p, UOSInt buffSize);
		virtual Bool DrawStringB(Math::Coord2DDbl tl, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> p, UOSInt buffSize);
		virtual Bool DrawStringRotB(Math::Coord2DDbl center, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> p, Double angleDegree, UOSInt buffSize);
		virtual Bool DrawStringRotB(Math::Coord2DDbl center, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> p, Double angleDegree, UOSInt buffSize);
		virtual Bool DrawImagePt(NN<DrawImage> img, Math::Coord2DDbl tl); ////////////////////////////////////
		virtual Bool DrawImagePt2(NN<Media::StaticImage> img, Math::Coord2DDbl tl); ////////////////////////////////////
		virtual Bool DrawImagePt3(NN<DrawImage> img, Math::Coord2DDbl destTL, Math::Coord2DDbl srcTL, Math::Size2DDbl srcSize); ////////////////////////////////////

		virtual NN<DrawPen> NewPenARGB(UInt32 color, Double thick, UInt8 *pattern, UOSInt nPattern);
		virtual NN<DrawBrush> NewBrushARGB(UInt32 color);
		virtual NN<DrawFont> NewFontPt(Text::CString name, Double ptSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage);
		virtual NN<DrawFont> NewFontPx(Text::CString name, Double pxSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage);
		virtual NN<DrawFont> CloneFont(NN<DrawFont> f);
		virtual void DelPen(NN<DrawPen> p);
		virtual void DelBrush(NN<DrawBrush> b);
		virtual void DelFont(NN<DrawFont> f);

		virtual Math::Size2DDbl GetTextSize(NN<DrawFont> fnt, Text::CStringNN txt);
		virtual void SetTextAlign(DrawEngine::DrawPos pos); ////////////////////////////////////
		virtual void GetStringBound(Int32 *pos, OSInt centX, OSInt centY, const UTF8Char *str, NN<DrawFont> f, OutParam<OSInt> drawX, OutParam<OSInt> drawY); ////////////////////////////////////
		virtual void GetStringBoundRot(Int32 *pos, Double centX, Double centY, const UTF8Char *str, NN<DrawFont> f, Double angleDegree, OutParam<OSInt> drawX, OutParam<OSInt> drawY); ////////////////////////////////////
		virtual void CopyBits(OSInt x, OSInt y, void *imgPtr, UOSInt bpl, UOSInt width, UOSInt height, Bool upsideDown) const; ////////////////////////////////////

		virtual Media::StaticImage *ToStaticImage() const; ////////////////////////////////////
		virtual UOSInt SavePng(NN<IO::SeekableStream> stm);
		virtual UOSInt SaveGIF(NN<IO::SeekableStream> stm); ////////////////////////////////////
		virtual UOSInt SaveJPG(NN<IO::SeekableStream> stm);

		virtual NN<Media::RasterImage> Clone() const; ////////////////////////////////////
		virtual Media::RasterImage::ImageType GetImageType() const;
		virtual void GetRasterData(UInt8 *destBuff, OSInt left, OSInt top, UOSInt width, UOSInt height, UOSInt destBpl, Bool upsideDown, Media::RotateType destRotate) const;
		virtual Int32 GetPixel32(OSInt x, OSInt y) const; ////////////////////////////////////

		void *GetSurface() const;
		void *GetCairo() const;
	};
}
#endif
