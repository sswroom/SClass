#ifndef _SM_MEDIA_FOGENGINE
#define _SM_MEDIA_FOGENGINE
#include "Media/DrawEngine.h"

namespace Media
{
	class FogPen : public DrawPen
	{
	public:
		Int32 color;
		Int32 thick;
		Double *pattern;
		Int32 nPattern;

		FogPen(Int32 color, Int32 thick, UInt8 *pattern, Int32 nPattern);
		~FogPen();

	};

	class FogEngine : public DrawEngine
	{
	public:
		FogEngine();
		virtual ~FogEngine();

		virtual Optional<DrawImage> CreateImage32(Int32 width, Int32 height);
		virtual Optional<DrawImage> LoadImageA(Char *fileName);
		virtual Optional<DrawImage> LoadImageW(WChar *fileName);
		virtual Bool DeleteImage(NN<DrawImage> img);
	};

	class FogImage : public DrawImage
	{
	private:
		void *img;
		void *painter;

		void SetPen(DrawPen *p);
	public:
		FogImage(Int32 width, Int32 height);
		virtual ~FogImage();
		
		virtual Int32 GetWidth();
		virtual Int32 GetHeight();
		virtual Int32 GetBitCount();
		
		virtual Bool DrawLine(Int32 x1, Int32 y1, Int32 x2, Int32 y2, DrawPen *p);
		virtual Bool DrawPolyline(Int32 *points, Int32 nPoints, DrawPen *p);
		virtual Bool DrawPolygon(Int32 *points, Int32 nPoints, DrawPen *p, DrawBrush *b);
		virtual Bool DrawPolyPolygon(Int32 *points, Int32 *pointCnt, Int32 nPointCnt, DrawPen *p, DrawBrush *b);
		virtual Bool DrawRect(Int32 x, Int32 y, Int32 w, Int32 h, DrawPen *p, DrawBrush *b);
		virtual Bool DrawStringW(Int32 tlx, Int32 tly, WChar *str, DrawFont *f, DrawBrush *p);
		virtual Bool DrawStringRotW(Int32 centX, Int32 centY, WChar *str, DrawFont *f, DrawBrush *p, Int32 angleDegree);
		virtual Bool DrawString(Int32 tlx, Int32 tly, Char *utf8Str, DrawFont *f, DrawBrush *p);
		virtual Bool DrawStringRot(Int32 centX, Int32 centY, Char *utf8Str, DrawFont *f, DrawBrush *p, Int32 angleDegree);
		virtual Bool DrawImagePt(DrawImage *img, Int32 tlx, Int32 tly);
		
		virtual DrawPen *NewPenARGB(Int32 color, Int32 thick, UInt8 *pattern, Int32 nPattern);
		virtual DrawBrush *NewBrushARGB(Int32 color);
		virtual DrawFont *NewFontA(Char *name, Int16 pxSize, Int16 fontStyle);
		virtual DrawFont *NewFontW(WChar *name, Int16 pxSize, Int16 fontStyle);
		virtual void DelPen(DrawPen *p);
		virtual void DelBrush(DrawBrush *b);
		virtual void DelFont(DrawFont *f);
		
		Bool GetTextSize(DrawFont *fnt, WChar *txt, Int32 txtLen, Int32 *sz);
		
		virtual Int32 SavePng(IO::SeekableStream *stm);
	};
}
#endif
