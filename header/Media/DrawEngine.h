#ifndef _SM_MEDIA_DRAWENGINE
#define _SM_MEDIA_DRAWENGINE
#include "IO/SeekableStream.h"
#include "Math/Coord2DDbl.h"
#include "Math/Size2D.h"
#include "Math/Size2DDbl.h"
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

		virtual DrawImage *CreateImage32(Math::Size2D<UOSInt> size, Media::AlphaType atype) = 0;
		virtual DrawImage *LoadImage(Text::CStringNN fileName) = 0;
		virtual DrawImage *LoadImageStream(NotNullPtr<IO::SeekableStream> stm) = 0;
		virtual DrawImage *ConvImage(Media::Image *img) = 0;
		virtual DrawImage *CloneImage(NotNullPtr<DrawImage> img) = 0;
		virtual Bool DeleteImage(NotNullPtr<DrawImage> img) = 0;
	};

	class DrawImage
	{
	public:
		virtual UOSInt GetWidth() const = 0;
		virtual UOSInt GetHeight() const = 0;
		virtual Math::Size2D<UOSInt> GetSize() const = 0;
		virtual UInt32 GetBitCount() const = 0;
		virtual NotNullPtr<const ColorProfile> GetColorProfile() const = 0;
		virtual void SetColorProfile(NotNullPtr<const ColorProfile> color) = 0;
		virtual Media::AlphaType GetAlphaType() const = 0;
		virtual void SetAlphaType(Media::AlphaType atype) = 0;
		virtual Double GetHDPI() const = 0;
		virtual Double GetVDPI() const = 0;
		virtual void SetHDPI(Double dpi) = 0;
		virtual void SetVDPI(Double dpi) = 0;
		virtual UInt8 *GetImgBits(OutParam<Bool> revOrder) = 0;
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
		virtual Bool DrawRect(Math::Coord2DDbl tl, Math::Size2DDbl size, DrawPen *p, DrawBrush *b) = 0;
		virtual Bool DrawEllipse(Math::Coord2DDbl tl, Math::Size2DDbl size, DrawPen *p, DrawBrush *b) = 0;
		virtual Bool DrawString(Math::Coord2DDbl tl, NotNullPtr<Text::String> str, DrawFont *f, DrawBrush *b) = 0;
		virtual Bool DrawString(Math::Coord2DDbl tl, Text::CStringNN str, DrawFont *f, DrawBrush *b) = 0;
		virtual Bool DrawStringRot(Math::Coord2DDbl center, NotNullPtr<Text::String> str, DrawFont *f, DrawBrush *b, Double angleDegreeACW) = 0;
		virtual Bool DrawStringRot(Math::Coord2DDbl center, Text::CStringNN str, DrawFont *f, DrawBrush *b, Double angleDegreeACW) = 0;
		virtual Bool DrawStringB(Math::Coord2DDbl tl, NotNullPtr<Text::String> str, DrawFont *f, DrawBrush *b, UOSInt buffSize) = 0;
		virtual Bool DrawStringB(Math::Coord2DDbl tl, Text::CStringNN str, DrawFont *f, DrawBrush *b, UOSInt buffSize) = 0;
		virtual Bool DrawStringRotB(Math::Coord2DDbl center, NotNullPtr<Text::String> str, DrawFont *f, DrawBrush *b, Double angleDegreeACW, UOSInt buffSize) = 0;
		virtual Bool DrawStringRotB(Math::Coord2DDbl center, Text::CStringNN str, DrawFont *f, DrawBrush *b, Double angleDegreeACW, UOSInt buffSize) = 0;
		virtual Bool DrawImagePt(NotNullPtr<DrawImage> img, Math::Coord2DDbl tl) = 0;
		virtual Bool DrawImagePt2(NotNullPtr<Media::StaticImage> img, Math::Coord2DDbl tl) = 0;
		virtual Bool DrawImagePt3(NotNullPtr<DrawImage> img, Math::Coord2DDbl destTL, Math::Coord2DDbl srcTL, Math::Size2DDbl srcSize) = 0;

		virtual DrawPen *NewPenARGB(UInt32 color, Double thick, UInt8 *pattern, UOSInt nPattern) = 0;
		virtual DrawBrush *NewBrushARGB(UInt32 color) = 0;
		virtual DrawFont *NewFontPt(Text::CString name, Double ptSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage) = 0; // 72 dpi size
		virtual DrawFont *NewFontPx(Text::CString name, Double pxSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage) = 0; // Actual size
		virtual DrawFont *CloneFont(DrawFont *f) = 0;
		virtual void DelPen(DrawPen *p) = 0;
		virtual void DelBrush(DrawBrush *b) = 0;
		virtual void DelFont(DrawFont *f) = 0;

		virtual Math::Size2DDbl GetTextSize(DrawFont *fnt, Text::CString txt) = 0;
		virtual void SetTextAlign(Media::DrawEngine::DrawPos pos) = 0;
		virtual void GetStringBound(Int32 *pos, OSInt centX, OSInt centY, const UTF8Char *str, DrawFont *f, OSInt *drawX, OSInt *drawY) = 0;
		virtual void GetStringBoundRot(Int32 *pos, Double centX, Double centY, const UTF8Char *str, DrawFont *f, Double angleDegree, OSInt *drawX, OSInt *drawY) = 0;
		virtual void CopyBits(OSInt x, OSInt y, void *imgPtr, UOSInt bpl, UOSInt width, UOSInt height, Bool upsideDown) const = 0;
		
		Bool DrawStringHAlign(Math::Coord2DDbl tl, Double brx, Text::CStringNN str, DrawFont *f, DrawBrush *b, Text::HAlignment hAlign);
		UInt32 GetPixel32(OSInt x, OSInt y);
		void SetImageAlpha(UInt8 alpha);
		void MulImageAlpha(Double val);

		virtual Media::StaticImage *ToStaticImage() const = 0;
		virtual UOSInt SavePng(NotNullPtr<IO::SeekableStream> stm) = 0;
		virtual UOSInt SaveGIF(NotNullPtr<IO::SeekableStream> stm) = 0;
		virtual UOSInt SaveJPG(NotNullPtr<IO::SeekableStream> stm) = 0;
	};
}
#endif
