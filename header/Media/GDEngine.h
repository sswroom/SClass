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

		virtual DrawImage *CreateImage32(Int32 width, Int32 height);
		virtual DrawImage *LoadImageA(Char *fileName);
		virtual DrawImage *LoadImageW(WChar *fileName);
		virtual Bool DeleteImage(DrawImage *img);

		void *CreateIOCtx(IO::Stream *stm);
		void DeleteIOCtx(void *obj);
	};

	class GDBrush : public DrawBrush
	{
	private:
		Int32 color;
	public:
		GDBrush(Int32 color, DrawImage *img);
		~GDBrush();
		
		Int32 InitImage(DrawImage *img);
	};

	class GDPen : public DrawPen
	{
	private:
		Int32 color;
		Int32 thick;
		int *pattern;
		Int32 nPattern;
	public:
		GDPen(Int32 color, Int32 thick, UInt8 *pattern, Int32 nPattern, DrawImage *img);
		~GDPen();
		
		Int32 InitImage(DrawImage *img);
	};

	class GDFont : public DrawFont
	{
	private:
		Char *name;
		Int16 pxSize;
		Int16 style;
		Bool isTTCName;
	public:
		GDFont(Char *name, Int16 pxSize, Int16 style);
		GDFont(WChar *name, Int16 pxSize, Int16 style);
		~GDFont();

		Char *GetName();
		Char *GetTTCName();
		Int16 GetPointSize();
		Int16 GetFontStyle();
	};

	class GDImage : public DrawImage
	{
	private:
		GDEngine *eng;
		Int32 width;
		Int32 height;
		Int32 bitCount;
	public:
		void *imgPtr;

		GDImage(GDEngine *eng, Int32 width, Int32 height, Int32 bitCount, void *imgPtr);
		virtual ~GDImage();

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
		virtual Bool DrawStringWB(Int32 tlx, Int32 tly, WChar *str, DrawFont *f, DrawBrush *p, Int32 buffSize);
		virtual Bool DrawStringRotWB(Int32 centX, Int32 centY, WChar *str, DrawFont *f, DrawBrush *p, Int32 angleDegree, Int32 buffSize);
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

		virtual Bool GetTextSize(Media::DrawFont *fnt, WChar *txt, Int32 txtLen, Int32 *sz);
		virtual void SetTextAlign(Media::DrawEngine::DrawPos pos);
		virtual void GetStringBoundW(Int32 *pos, Int32 centX, Int32 centY, WChar *str, DrawFont *f, Int32 *drawX, Int32 *drawY);
		virtual void GetStringBoundRotW(Int32 *pos, Int32 centX, Int32 centY, WChar *str, DrawFont *f, Int32 angleDegree, Int32 *drawX, Int32 *drawY);
		virtual void CopyBits(Int32 x, Int32 y, void *imgPtr, Int32 bpl, Int32 width, Int32 height);

		virtual Int32 SavePng(IO::SeekableStream *stm);
		virtual Int32 SaveGIF(IO::SeekableStream *stm);
		virtual Int32 SaveJPG(IO::SeekableStream *stm);
	};
};
#endif
