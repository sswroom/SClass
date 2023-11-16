#include "stdafx.h"
#include "MyMemory.h"
#include "Sync/Event.h"
#include "IO/Stream.h"
#include "Media/DrawEngine.h"
#include "Media/FogEngine.h"

#include <Fog/Graphics/Image.h>
#include <Fog/Graphics/Painter.h>


Media::FogPen::FogPen(Int32 color, Int32 thick, UInt8 *pattern, Int32 nPattern)
{
	this->color = color;
	this->thick = thick;
	if (pattern)
	{
		Int32 i = this->nPattern = nPattern;
		this->pattern = (Double*)MemAlloc(nPattern * sizeof(Double));
		while (i-- > 0)
		{
			this->pattern[i] = pattern[i];
		}
	}
	else
	{
		this->pattern = 0;
		this->nPattern = 0;
	}
}

Media::FogPen::~FogPen()
{
	if (this->pattern)
	{
		MemFree(this->pattern);
	}
}

void Media::FogImage::SetPen(DrawPen *p)
{
	Fog::Painter *pnt = (Fog::Painter*)painter;
	FogPen *pen = (FogPen*)p;
	pnt->setLineWidth(pen->thick);
	pnt->setSource((Fog::Rgba)pen->color);
	pnt->setLineDash(pen->pattern, pen->nPattern);
}

Media::FogImage::FogImage(Int32 width, Int32 height)
{
	NEW_CLASS(img, Fog::Image(width, height, Fog::Image::FormatARGB32));
	NEW_CLASS(painter, Fog::Painter());
	((Fog::Painter*)painter)->begin(*(Fog::Image*)img);
}

Media::FogImage::~FogImage()
{
	DEL_CLASS((Fog::Painter*)painter);
	DEL_CLASS((Fog::Image*)img);
}

Int32 Media::FogImage::GetWidth()
{
	return ((Fog::Image*)img)->width();
}

Int32 Media::FogImage::GetHeight()
{
	return ((Fog::Image*)img)->height();
}

Int32 Media::FogImage::GetBitCount()
{
	return ((Fog::Image*)img)->bytesPerPixel() << 3;
}

Bool Media::FogImage::DrawLine(Int32 x1, Int32 y1, Int32 x2, Int32 y2, DrawPen *p)
{
	Fog::Painter *pnt = (Fog::Painter*)painter;
	pnt->drawLine(Fog::Point(x1, y1), Fog::Point(x2, y2));
	return true;
}

Bool Media::FogImage::DrawPolyline(Int32 *points, Int32 nPoints, DrawPen *p)
{
	///////////////////////////////////////
	return false;
}

Bool Media::FogImage::DrawPolygon(Int32 *points, Int32 nPoints, DrawPen *p, DrawBrush *b)
{
	///////////////////////////////////////
	return false;
}

Bool Media::FogImage::DrawPolyPolygon(Int32 *points, Int32 *pointCnt, Int32 nPointCnt, DrawPen *p, DrawBrush *b)
{
	///////////////////////////////////////
	return false;
}

Bool Media::FogImage::DrawRect(Int32 x, Int32 y, Int32 w, Int32 h, DrawPen *p, DrawBrush *b)
{
	///////////////////////////////////////
	return false;
}

Bool Media::FogImage::DrawStringW(Int32 tlx, Int32 tly, WChar *str, DrawFont *f, DrawBrush *p)
{
	///////////////////////////////////////
	return false;
}

Bool Media::FogImage::DrawStringRotW(Int32 centX, Int32 centY, WChar *str, DrawFont *f, DrawBrush *p, Int32 angleDegree)
{
	///////////////////////////////////////
	return false;
}

Bool Media::FogImage::DrawString(Int32 tlx, Int32 tly, Char *utf8Str, DrawFont *f, DrawBrush *p)
{
	///////////////////////////////////////
	return false;
}

Bool Media::FogImage::DrawStringRot(Int32 centX, Int32 centY, Char *utf8Str, DrawFont *f, DrawBrush *p, Int32 angleDegree)
{
	///////////////////////////////////////
	return false;
}

Bool Media::FogImage::DrawImagePt(DrawImage *img, Int32 tlx, Int32 tly)
{
	///////////////////////////////////////
	return false;
}

DrawPen *Media::FogImage::NewPenARGB(Int32 color, Int32 thick, UInt8 *pattern, Int32 nPattern)
{
	FogPen *pen;
	NEW_CLASS(pen, FogPen(color, thick, pattern, nPattern));
	return pen;
}

NotNullPtr<DrawBrush> Media::FogImage::NewBrushARGB(Int32 color)
{
	/////////////////////////////////
	return 0;
}

DrawFont *Media::FogImage::NewFontA(Char *name, Int16 pxSize, Int16 fontStyle)
{
	//////////////////////////////////
	return 0;
}

DrawFont *Media::FogImage::NewFontW(WChar *name, Int16 pxSize, Int16 fontStyle)
{
	/////////////////////////////////
	return 0;
}

void Media::FogImage::DelPen(DrawPen *p)
{
	DEL_CLASS((FogPen*)p);
}

void Media::FogImage::DelBrush(DrawBrush *b)
{
	//////////////////////////////////
}

void Media::FogImage::DelFont(DrawFont *f)
{
	//////////////////////////////////
}

Bool Media::FogImage::GetTextSize(DrawFont *fnt, WChar *txt, Int32 txtLen, Int32 *sz)
{
	///////////////////////////////////
	return false;
}

Bool Media::FogImage::SavePng(IO::SeekableStream *stm)
{
	///////////////////////////////////
	return false;
}


Media::FogEngine::FogEngine()
{
}

Media::FogEngine::~FogEngine()
{
}

DrawImage *Media::FogEngine::CreateImage32(Int32 width, Int32 height)
{
	Media::FogImage *img;
	NEW_CLASS(img, Media::FogImage(width, height));
	return img;
}

DrawImage *Media::FogEngine::LoadImageA(Char *fileName)
{
	////////////////////////////////////
	return 0;
}

DrawImage *Media::FogEngine::LoadImageW(WChar *fileName)
{
	////////////////////////////////////
	return 0;
}

Bool Media::FogEngine::DeleteImage(DrawImage *img)
{
	DEL_CLASS((Media::FogImage*)img);
	return true;
}
