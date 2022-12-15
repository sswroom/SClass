#ifndef _SM_MEDIA_DRAWENGINE
#define _SM_MEDIA_DRAWENGINE
#include "IO/SeekableStream.h"
#include "Math/Coord2D.h"
#include "Math/Size2D.h"
#include "Media/Image.h"
#include "Text/CString.h"
#include "Text/String.h"
#include "Text/TextCommon.h"

namespace Media
{
	class DrawBrush
	{
	protected:
		DrawBrush()
		{
		}
	};

	class DrawPen
	{
	protected:
		DrawPen()
		{
		}

	public:
		virtual Double GetThick() = 0;
	};

	class DrawFont
	{
	protected:
		DrawFont()
		{
		}
	};

	class DrawImage;

	class DrawEngine
	{
	public:
		typedef enum
		{
			DRAW_POS_TOPLEFT,
			DRAW_POS_TOPCENTER,
			DRAW_POS_TOPRIGHT,
			DRAW_POS_CENTERLEFT,
			DRAW_POS_CENTER,
			DRAW_POS_CENTERRIGHT,
			DRAW_POS_BOTTOMLEFT,
			DRAW_POS_BOTTOMCENTER,
			DRAW_POS_BOTTOMRIGHT
		} DrawPos;

		typedef enum
		{
			DFS_NORMAL = 0,
			DFS_BOLD = 1,
			DFS_ITALIC = 2,
			DFS_ANTIALIAS = 8
		} DrawFontStyle;

	public:
		virtual ~DrawEngine(){};

		virtual DrawImage *CreateImage32(UOSInt width, UOSInt height, Media::AlphaType atype) = 0;
		virtual DrawImage *LoadImage(Text::CString fileName) = 0;
		virtual DrawImage *LoadImageStream(IO::SeekableStream *stm) = 0;
		virtual DrawImage *ConvImage(Media::Image *img) = 0;
		virtual DrawImage *CloneImage(DrawImage *img) = 0;
		virtual Bool DeleteImage(DrawImage *img) = 0;
	};

	class DrawImage
	{
	public:
		virtual UOSInt GetWidth() const = 0;
		virtual UOSInt GetHeight() const = 0;
		virtual UInt32 GetBitCount() const = 0;
		virtual ColorProfile *GetColorProfile() const = 0;
		virtual void SetColorProfile(const ColorProfile *color) = 0;
		virtual Media::AlphaType GetAlphaType() const = 0;
		virtual void SetAlphaType(Media::AlphaType atype) = 0;
		virtual Double GetHDPI() const = 0;
		virtual Double GetVDPI() const = 0;
		virtual void SetHDPI(Double dpi) = 0;
		virtual void SetVDPI(Double dpi) = 0;
		virtual UInt8 *GetImgBits(Bool *revOrder) = 0;
		virtual void GetImgBitsEnd(Bool modified) = 0;
		virtual UOSInt GetImgBpl() const = 0;
		virtual Media::EXIFData *GetEXIF() const = 0;
		virtual Media::PixelFormat GetPixelFormat() const = 0;

		virtual Bool DrawLine(Double x1, Double y1, Double x2, Double y2, DrawPen *p) = 0;
		virtual Bool DrawPolylineI(const Int32 *points, UOSInt nPoints, DrawPen *p) = 0;
		virtual Bool DrawPolygonI(const Int32 *points, UOSInt nPoints, DrawPen *p, DrawBrush *b) = 0;
		virtual Bool DrawPolyPolygonI(const Int32 *points, const UInt32 *pointCnt, UOSInt nPointCnt, DrawPen *p, DrawBrush *b) = 0;
		virtual Bool DrawPolyline(const Math::Coord2DDbl *points, UOSInt nPoints, DrawPen *p) = 0;
		virtual Bool DrawPolygon(const Math::Coord2DDbl *points, UOSInt nPoints, DrawPen *p, DrawBrush *b) = 0;
		virtual Bool DrawPolyPolygon(const Math::Coord2DDbl *points, const UInt32 *pointCnt, UOSInt nPointCnt, DrawPen *p, DrawBrush *b) = 0;
		virtual Bool DrawRect(Double x, Double y, Double w, Double h, DrawPen *p, DrawBrush *b) = 0;
		virtual Bool DrawEllipse(Double tlx, Double tly, Double w, Double h, DrawPen *p, DrawBrush *b) = 0;
		virtual Bool DrawString(Double tlx, Double tly, Text::String *str, DrawFont *f, DrawBrush *b) = 0;
		virtual Bool DrawString(Double tlx, Double tly, Text::CString str, DrawFont *f, DrawBrush *b) = 0;
		virtual Bool DrawStringRot(Double centX, Double centY, Text::String *str, DrawFont *f, DrawBrush *b, Double angleDegreeACW) = 0;
		virtual Bool DrawStringRot(Double centX, Double centY, Text::CString str, DrawFont *f, DrawBrush *b, Double angleDegreeACW) = 0;
		virtual Bool DrawStringB(Double tlx, Double tly, Text::String *str, DrawFont *f, DrawBrush *b, UOSInt buffSize) = 0;
		virtual Bool DrawStringB(Double tlx, Double tly, Text::CString str, DrawFont *f, DrawBrush *b, UOSInt buffSize) = 0;
		virtual Bool DrawStringRotB(Double centX, Double centY, Text::String *str, DrawFont *f, DrawBrush *b, Double angleDegreeACW, UOSInt buffSize) = 0;
		virtual Bool DrawStringRotB(Double centX, Double centY, Text::CString str, DrawFont *f, DrawBrush *b, Double angleDegreeACW, UOSInt buffSize) = 0;
		virtual Bool DrawImagePt(DrawImage *img, Double tlx, Double tly) = 0;
		virtual Bool DrawImagePt2(Media::StaticImage *img, Double tlx, Double tly) = 0;
		virtual Bool DrawImagePt3(DrawImage *img, Double destX, Double destY, Double srcX, Double srcY, Double srcW, Double srcH) = 0;

		virtual DrawPen *NewPenARGB(UInt32 color, Double thick, UInt8 *pattern, UOSInt nPattern) = 0;
		virtual DrawBrush *NewBrushARGB(UInt32 color) = 0;
		virtual DrawFont *NewFontPt(Text::CString name, Double ptSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage) = 0; // 72 dpi size
		virtual DrawFont *NewFontPx(Text::CString name, Double pxSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage) = 0; // Actual size
		virtual DrawFont *CloneFont(DrawFont *f) = 0;
		virtual void DelPen(DrawPen *p) = 0;
		virtual void DelBrush(DrawBrush *b) = 0;
		virtual void DelFont(DrawFont *f) = 0;

		virtual Math::Size2D<Double> GetTextSize(DrawFont *fnt, Text::CString txt) = 0;
		virtual void SetTextAlign(Media::DrawEngine::DrawPos pos) = 0;
		virtual void GetStringBound(Int32 *pos, OSInt centX, OSInt centY, const UTF8Char *str, DrawFont *f, OSInt *drawX, OSInt *drawY) = 0;
		virtual void GetStringBoundRot(Int32 *pos, Double centX, Double centY, const UTF8Char *str, DrawFont *f, Double angleDegree, OSInt *drawX, OSInt *drawY) = 0;
		virtual void CopyBits(OSInt x, OSInt y, void *imgPtr, UOSInt bpl, UOSInt width, UOSInt height, Bool upsideDown) const = 0;
		
		Bool DrawStringHAlign(Double tlx, Double tly, Double brx, Text::CString str, DrawFont *f, DrawBrush *b, Text::HAlignment hAlign);
		UInt32 GetPixel32(OSInt x, OSInt y);
		void SetImageAlpha(UInt8 alpha);
		void MulImageAlpha(Double val);

		virtual Media::StaticImage *ToStaticImage() const = 0;
		virtual UOSInt SavePng(IO::SeekableStream *stm) = 0;
		virtual UOSInt SaveGIF(IO::SeekableStream *stm) = 0;
		virtual UOSInt SaveJPG(IO::SeekableStream *stm) = 0;
	};
}
#endif
