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
		Parser::ParserList *parsers;
	public:
		Media::ImageAlphaBlend *iab32;
	public:
		StaticEngine(Parser::ParserList *parsers);
		virtual ~StaticEngine();

		virtual DrawImage *CreateImage32(Math::Size2D<UOSInt> size, Media::AlphaType atype);
		virtual DrawImage *LoadImage(Text::CStringNN fileName);
		virtual DrawImage *LoadImageW(const WChar *fileName);
		virtual DrawImage *LoadImageStream(NotNullPtr<IO::SeekableStream> stm); /////////////////////////
		virtual DrawImage *ConvImage(Media::Image *img);
		virtual DrawImage *CloneImage(DrawImage *img);
		virtual Bool DeleteImage(DrawImage *img);
	};

	class StaticBrush : public DrawBrush
	{
	public:
		Int32 color;
	public:
		StaticBrush(Int32 color);
		virtual ~StaticBrush();
	};

	class StaticPen : public DrawPen
	{
	public:
		Int32 color;
		Double thick;
		UInt8 *pattern;
		OSInt nPattern;

	public:
		StaticPen(Int32 color, Double thick, const UInt8 *pattern, OSInt nPattern);
		virtual ~StaticPen();

		virtual Double GetThick();
	};

	class StaticDrawImage : public Media::StaticImage, public Media::DrawImage
	{
	private:
		Media::StaticEngine *eng;
	public:
		StaticDrawImage(StaticEngine *eng, Math::Size2D<UOSInt> dispSize, Int32 fourcc, Int32 bpp, Media::PixelFormat pf, OSInt maxSize, const Media::ColorProfile *color, Media::ColorProfile::YUVType yuvType, Media::AlphaType atype, Media::YCOffset ycOfst);
		virtual ~StaticDrawImage();

		virtual UOSInt GetWidth();
		virtual UOSInt GetHeight();
		virtual UInt32 GetBitCount();
		virtual ColorProfile *GetColorProfile();
		virtual Media::AlphaType GetAlphaType();
		virtual Double GetHDPI();
		virtual Double GetVDPI();
		virtual void SetHDPI(Double dpi);
		virtual void SetVDPI(Double dpi);
		virtual UInt8 *GetImgBits(Bool *revOrder);

/*		virtual Bool DrawLine(Double x1, Double y1, Double x2, Double y2, DrawPen *p) = 0;
		virtual Bool DrawPolylineI(Int32 *points, OSInt nPoints, DrawPen *p) = 0;
		virtual Bool DrawPolygonI(Int32 *points, OSInt nPoints, DrawPen *p, DrawBrush *b) = 0;
		virtual Bool DrawPolyPolygonI(Int32 *points, Int32 *pointCnt, OSInt nPointCnt, DrawPen *p, DrawBrush *b) = 0;
		virtual Bool DrawPolyline(Double *points, OSInt nPoints, DrawPen *p) = 0;
		virtual Bool DrawPolygon(Double *points, OSInt nPoints, DrawPen *p, DrawBrush *b) = 0;
		virtual Bool DrawPolyPolygon(Double *points, Int32 *pointCnt, OSInt nPointCnt, DrawPen *p, DrawBrush *b) = 0;
		virtual Bool DrawRect(Double x, Double y, Double w, Double h, DrawPen *p, DrawBrush *b) = 0;
		virtual Bool DrawEllipse(Double tlx, Double tly, Double w, Double h, DrawPen *p, DrawBrush *b) = 0;
		virtual Bool DrawString(Double tlx, Double tly, const WChar *str, DrawFont *f, DrawBrush *b) = 0;
		virtual Bool DrawStringRot(Double centX, Double centY, const WChar *str, DrawFont *f, DrawBrush *b, Double angleDegree) = 0;
		virtual Bool DrawStringB(Double tlx, Double tly, const WChar *str, DrawFont *f, DrawBrush *b, Int32 buffSize) = 0;
		virtual Bool DrawStringRotB(Double centX, Double centY, const WChar *str, DrawFont *f, DrawBrush *b, Double angleDegree, Int32 buffSize) = 0;*/
		virtual Bool DrawImagePt(DrawImage *img, Double tlx, Double tly);
		virtual Bool DrawImagePt2(Media::StaticImage *img, Double tlx, Double tly);
		virtual Bool DrawImagePt3(DrawImage *img, Double destX, Double destY, Double srcX, Double srcY, Double srcW, Double srcH);

		virtual DrawPen *NewPenARGB(Int32 color, Double thick, UInt8 *pattern, OSInt nPattern);
		virtual DrawBrush *NewBrushARGB(Int32 color);
/*		virtual DrawFont *NewFontA(const Char *name, Int16 pxSize, Media::DrawEngine::DrawFontStyle fontStyle) = 0;
		virtual DrawFont *NewFontW(const WChar *name, Int16 pxSize, Media::DrawEngine::DrawFontStyle fontStyle) = 0;
		virtual DrawFont *NewFontH(const WChar *name, Double height, Media::DrawEngine::DrawFontStyle fontStyle, Int32 codePage) = 0;*/
		virtual void DelPen(DrawPen *p);
		virtual void DelBrush(DrawBrush *b);
//		virtual void DelFont(DrawFont *f) = 0;

/*		virtual Bool GetTextSize(DrawFont *fnt, const WChar *txt, OSInt txtLen, Double *sz) = 0;
		virtual void SetTextAlign(Media::DrawEngine::DrawPos pos) = 0;
		virtual void GetStringBoundW(Int32 *pos, OSInt centX, OSInt centY, const WChar *str, DrawFont *f, OSInt *drawX, OSInt *drawY) = 0;
		virtual void GetStringBoundRotW(Int32 *pos, Double centX, Double centY, const WChar *str, DrawFont *f, Double angleDegree, OSInt *drawX, OSInt *drawY) = 0;
		virtual void CopyBits(OSInt x, OSInt y, void *imgPtr, OSInt bpl, OSInt width, OSInt height) = 0;*/

		virtual Media::StaticImage *ToStaticImage();
//		virtual UOSInt SavePng(NotNullPtr<IO::SeekableStream> stm) = 0;
		virtual UOSInt SaveGIF(NotNullPtr<IO::SeekableStream> stm);
//		virtual UOSInt SaveJPG(NotNullPtr<IO::SeekableStream> stm) = 0;
	};
}
#endif
