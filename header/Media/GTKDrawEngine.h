#ifndef _SM_MEDIA_GTKDRAWENGINE
#define _SM_MEDIA_GTKDRAWENGINE
#include "Media/DrawEngine.h"
#include "Media/ABlend/AlphaBlend8_C8.h"
#include "Media/Resizer/LanczosResizerRGB_C8.h"
#include "Sync/Mutex.h"
#include "Text/String.h"

namespace Media
{
	class GTKDrawEngine : public Media::DrawEngine
	{
	public:
		Media::ABlend::AlphaBlend8_C8 iab;
		Sync::Mutex iabMut;
	public:
		GTKDrawEngine();
		virtual ~GTKDrawEngine();

		virtual Optional<DrawImage> CreateImage32(Math::Size2D<UIntOS> size, Media::AlphaType atype);
		NN<DrawImage> CreateImageScn(void *cr, Math::Coord2D<IntOS> tl, Math::Coord2D<IntOS> br, Optional<Media::ColorSess> colorSess);
		virtual Optional<DrawImage> LoadImage(Text::CStringNN fileName);
		virtual Optional<DrawImage> LoadImageStream(NN<IO::SeekableStream> stm);
		virtual Optional<DrawImage> ConvImage(NN<Media::RasterImage> img, Optional<Media::ColorSess> colorSess);
		virtual Optional<DrawImage> CloneImage(NN<DrawImage> img);
		virtual Bool DeleteImage(NN<DrawImage> img);
		virtual void EndColorSess(NN<Media::ColorSess> colorSess);
	};

	class GTKDrawFont : public DrawFont
	{
	private:
		NN<Text::String> fontName;
		Double fontHeight;
		IntOS fontSlant;
		IntOS fontWeight;
	public:
		GTKDrawFont(Text::CStringNN fontName, Double pxHeight, Media::DrawEngine::DrawFontStyle drawFontStyle);
		GTKDrawFont(Optional<Text::String> fontName, Double pxHeight, IntOS fontSlant, IntOS fontWeight);
		~GTKDrawFont();

		void Init(void *cr);
		NN<Text::String> GetFontName();
		Double GetHeight();
		IntOS GetFontWeight();
		IntOS GetFontSlant();
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
		Math::Coord2D<IntOS> tl;
		Optional<Media::ColorSess> colorSess;
		Optional<Media::Resizer::LanczosResizerRGB_C8> resizer;

	public:
		GTKDrawImage(NN<GTKDrawEngine> eng, void *surface, void *cr, Math::Coord2D<IntOS> tl, Math::Size2D<UIntOS> size, UInt32 bitCount, Media::AlphaType atype, Optional<Media::ColorSess> colorSess);
		virtual ~GTKDrawImage();

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
		virtual UnsafeArrayOpt<UInt8> GetImgBits(OutParam<Bool> upsideDown);
		virtual void GetImgBitsEnd(Bool modified);
		virtual UIntOS GetImgBpl() const;
		virtual Optional<Media::EXIFData> GetEXIF() const;
		virtual Media::PixelFormat GetPixelFormat() const;
		virtual void SetColorSess(Optional<Media::ColorSess> colorSess);

		virtual Bool DrawLine(Double x1, Double y1, Double x2, Double y2, NN<DrawPen> p); ////////////////////////////////////
		virtual Bool DrawPolylineI(UnsafeArray<const Int32> points, UIntOS nPoints, NN<DrawPen> p); ////////////////////////////////////
		virtual Bool DrawPolygonI(UnsafeArray<const Int32> points, UIntOS nPoints, Optional<DrawPen> p, Optional<DrawBrush> b); ////////////////////////////////////
		virtual Bool DrawPolyPolygonI(UnsafeArray<const Int32> points, UnsafeArray<const UInt32> pointCnt, UIntOS nPointCnt, Optional<DrawPen> p, Optional<DrawBrush> b); ////////////////////////////////////
		virtual Bool DrawPolyline(UnsafeArray<const Math::Coord2DDbl> points, UIntOS nPoints, NN<DrawPen> p); ////////////////////////////////////
		virtual Bool DrawPolygon(UnsafeArray<const Math::Coord2DDbl> points, UIntOS nPoints, Optional<DrawPen> p, Optional<DrawBrush> b); ////////////////////////////////////
		virtual Bool DrawPolyPolygon(UnsafeArray<const Math::Coord2DDbl> points, UnsafeArray<const UInt32> pointCnt, UIntOS nPointCnt, Optional<DrawPen> p, Optional<DrawBrush> b); ////////////////////////////////////
		virtual Bool DrawRect(Math::Coord2DDbl tl, Math::Size2DDbl size, Optional<DrawPen> p, Optional<DrawBrush> b);
		virtual Bool DrawEllipse(Math::Coord2DDbl tl, Math::Size2DDbl size, Optional<DrawPen> p, Optional<DrawBrush> b); ////////////////////////////////////
		virtual Bool DrawString(Math::Coord2DDbl tl, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> p);
		virtual Bool DrawString(Math::Coord2DDbl tl, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> p);
		virtual Bool DrawStringRot(Math::Coord2DDbl center, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> p, Double angleDegree);
		virtual Bool DrawStringRot(Math::Coord2DDbl center, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> p, Double angleDegree);
		virtual Bool DrawStringB(Math::Coord2DDbl tl, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> p, UIntOS buffSize);
		virtual Bool DrawStringB(Math::Coord2DDbl tl, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> p, UIntOS buffSize);
		virtual Bool DrawStringRotB(Math::Coord2DDbl center, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> p, Double angleDegree, UIntOS buffSize);
		virtual Bool DrawStringRotB(Math::Coord2DDbl center, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> p, Double angleDegree, UIntOS buffSize);
		virtual Bool DrawImagePt(NN<DrawImage> img, Math::Coord2DDbl tl);
		virtual Bool DrawImagePt2(NN<DrawImage> img, Math::Coord2DDbl destTL, Math::Coord2DDbl srcTL, Math::Size2DDbl srcSize);
		virtual Bool DrawSImagePt(NN<Media::StaticImage> img, Math::Coord2DDbl tl);
		virtual Bool DrawSImagePt2(NN<Media::StaticImage> img, Math::Coord2DDbl destTL, Math::Coord2DDbl srcTL, Math::Size2DDbl srcSize);
		virtual Bool DrawImageQuad(NN<Media::StaticImage> img, Math::Quadrilateral quad);
		virtual void SetClip(Math::RectAreaDbl clipRect);
		virtual void ClearClip();

		virtual NN<DrawPen> NewPenARGB(UInt32 color, Double thick, UnsafeArrayOpt<UInt8> pattern, UIntOS nPattern);
		virtual NN<DrawBrush> NewBrushARGB(UInt32 color);
		virtual NN<DrawFont> NewFontPt(Text::CStringNN name, Double ptSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage);
		virtual NN<DrawFont> NewFontPx(Text::CStringNN name, Double pxSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage);
		virtual NN<DrawFont> CloneFont(NN<DrawFont> f);
		virtual void DelPen(NN<DrawPen> p);
		virtual void DelBrush(NN<DrawBrush> b);
		virtual void DelFont(NN<DrawFont> f);

		virtual Math::Size2DDbl GetTextSize(NN<DrawFont> fnt, Text::CStringNN txt);
		virtual void SetTextAlign(DrawEngine::DrawPos pos); ////////////////////////////////////
		virtual void GetStringBound(UnsafeArray<Int32> pos, IntOS centX, IntOS centY, UnsafeArray<const UTF8Char> str, NN<DrawFont> f, OutParam<IntOS> drawX, OutParam<IntOS> drawY); ////////////////////////////////////
		virtual void GetStringBoundRot(UnsafeArray<Int32> pos, Double centX, Double centY, UnsafeArray<const UTF8Char> str, NN<DrawFont> f, Double angleDegree, OutParam<IntOS> drawX, OutParam<IntOS> drawY); ////////////////////////////////////
		virtual void CopyBits(IntOS x, IntOS y, UnsafeArray<UInt8> imgPtr, UIntOS bpl, UIntOS width, UIntOS height, Bool upsideDown) const; ////////////////////////////////////

		virtual Optional<Media::StaticImage> ToStaticImage() const;
		virtual Optional<Media::RasterImage> AsRasterImage();
		virtual UIntOS SavePng(NN<IO::SeekableStream> stm);
		virtual UIntOS SaveGIF(NN<IO::SeekableStream> stm); ////////////////////////////////////
		virtual UIntOS SaveJPG(NN<IO::SeekableStream> stm);

		virtual NN<Media::RasterImage> Clone() const; ////////////////////////////////////
		virtual Media::RasterImage::ImageClass GetImageClass() const;
		virtual void GetRasterData(UnsafeArray<UInt8> destBuff, IntOS left, IntOS top, UIntOS width, UIntOS height, UIntOS destBpl, Bool upsideDown, Media::RotateType destRotate) const;
		virtual Int32 GetPixel32(IntOS x, IntOS y) const; ////////////////////////////////////

		void *GetSurface() const;
		void *GetCairo() const;
	};
}
#endif
