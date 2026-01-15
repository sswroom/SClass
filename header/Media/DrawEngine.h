#ifndef _SM_MEDIA_DRAWENGINE
#define _SM_MEDIA_DRAWENGINE
#include "IO/SeekableStream.h"
#include "Math/Coord2DDbl.h"
#include "Math/Size2D.h"
#include "Math/Size2DDbl.h"
#include "Media/ColorSess.h"
#include "Media/RasterImage.h"
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

		virtual Optional<DrawImage> CreateImage32(Math::Size2D<UIntOS> size, Media::AlphaType atype) = 0;
		virtual Optional<DrawImage> LoadImage(Text::CStringNN fileName) = 0;
		virtual Optional<DrawImage> LoadImageStream(NN<IO::SeekableStream> stm) = 0;
		virtual Optional<DrawImage> ConvImage(NN<Media::RasterImage> img, Optional<Media::ColorSess> colorSess) = 0;
		virtual Optional<DrawImage> CloneImage(NN<DrawImage> img) = 0;
		virtual Bool DeleteImage(NN<DrawImage> img) = 0;
		virtual void EndColorSess(NN<Media::ColorSess> colorSess) = 0;

		Optional<DrawImage> ConvImageOrNull(Optional<Media::RasterImage> img, Optional<Media::ColorSess> colorSess)
		{
			NN<Media::RasterImage> nnimg;
			if (img.SetTo(nnimg)) return ConvImage(nnimg, colorSess);
			return nullptr;
		}
	};

	class DrawImage
	{
	public:
		virtual UIntOS GetWidth() const = 0;
		virtual UIntOS GetHeight() const = 0;
		virtual Math::Size2D<UIntOS> GetSize() const = 0;
		virtual UInt32 GetBitCount() const = 0;
		virtual NN<const ColorProfile> GetColorProfile() const = 0;
		virtual void SetColorProfile(NN<const ColorProfile> color) = 0;
		virtual Media::AlphaType GetAlphaType() const = 0;
		virtual void SetAlphaType(Media::AlphaType atype) = 0;
		virtual Double GetHDPI() const = 0;
		virtual Double GetVDPI() const = 0;
		virtual void SetHDPI(Double dpi) = 0;
		virtual void SetVDPI(Double dpi) = 0;
		virtual UnsafeArrayOpt<UInt8> GetImgBits(OutParam<Bool> revOrder) = 0;
		virtual void GetImgBitsEnd(Bool modified) = 0;
		virtual UIntOS GetImgBpl() const = 0;
		virtual Optional<Media::EXIFData> GetEXIF() const = 0;
		virtual Media::PixelFormat GetPixelFormat() const = 0;
		virtual void SetColorSess(Optional<Media::ColorSess> colorSess) = 0;

		virtual Bool DrawLine(Double x1, Double y1, Double x2, Double y2, NN<DrawPen> p) = 0;
		virtual Bool DrawPolylineI(UnsafeArray<const Int32> points, UIntOS nPoints, NN<DrawPen> p) = 0;
		virtual Bool DrawPolygonI(UnsafeArray<const Int32> points, UIntOS nPoints, Optional<DrawPen> p, Optional<DrawBrush> b) = 0;
		virtual Bool DrawPolyPolygonI(UnsafeArray<const Int32> points, UnsafeArray<const UInt32> pointCnt, UIntOS nPointCnt, Optional<DrawPen> p, Optional<DrawBrush> b) = 0;
		virtual Bool DrawPolyline(UnsafeArray<const Math::Coord2DDbl> points, UIntOS nPoints, NN<DrawPen> p) = 0;
		virtual Bool DrawPolygon(UnsafeArray<const Math::Coord2DDbl> points, UIntOS nPoints, Optional<DrawPen> p, Optional<DrawBrush> b) = 0;
		virtual Bool DrawPolyPolygon(UnsafeArray<const Math::Coord2DDbl> points, UnsafeArray<const UInt32> pointCnt, UIntOS nPointCnt, Optional<DrawPen> p, Optional<DrawBrush> b) = 0;
		virtual Bool DrawRect(Math::Coord2DDbl tl, Math::Size2DDbl size, Optional<DrawPen> p, Optional<DrawBrush> b) = 0;
		virtual Bool DrawEllipse(Math::Coord2DDbl tl, Math::Size2DDbl size, Optional<DrawPen> p, Optional<DrawBrush> b) = 0;
		virtual Bool DrawString(Math::Coord2DDbl tl, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b) = 0;
		virtual Bool DrawString(Math::Coord2DDbl tl, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b) = 0;
		virtual Bool DrawStringRot(Math::Coord2DDbl center, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegreeACW) = 0;
		virtual Bool DrawStringRot(Math::Coord2DDbl center, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegreeACW) = 0;
		virtual Bool DrawStringB(Math::Coord2DDbl tl, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b, UIntOS buffSize) = 0;
		virtual Bool DrawStringB(Math::Coord2DDbl tl, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b, UIntOS buffSize) = 0;
		virtual Bool DrawStringRotB(Math::Coord2DDbl center, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegreeACW, UIntOS buffSize) = 0;
		virtual Bool DrawStringRotB(Math::Coord2DDbl center, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegreeACW, UIntOS buffSize) = 0;
		virtual Bool DrawImagePt(NN<DrawImage> img, Math::Coord2DDbl tl) = 0;
		virtual Bool DrawImagePt2(NN<Media::StaticImage> img, Math::Coord2DDbl tl) = 0;
		virtual Bool DrawImagePt3(NN<DrawImage> img, Math::Coord2DDbl destTL, Math::Coord2DDbl srcTL, Math::Size2DDbl srcSize) = 0;
		virtual Bool DrawImageQuad(NN<Media::StaticImage> img, Math::Quadrilateral quad) = 0;

		virtual NN<DrawPen> NewPenARGB(UInt32 color, Double thick, UnsafeArrayOpt<UInt8> pattern, UIntOS nPattern) = 0;
		virtual NN<DrawBrush> NewBrushARGB(UInt32 color) = 0;
		virtual NN<DrawFont> NewFontPt(Text::CStringNN name, Double ptSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage) = 0; // 72 dpi size
		virtual NN<DrawFont> NewFontPx(Text::CStringNN name, Double pxSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage) = 0; // Actual size
		virtual NN<DrawFont> CloneFont(NN<DrawFont> f) = 0;
		virtual void DelPen(NN<DrawPen> p) = 0;
		virtual void DelBrush(NN<DrawBrush> b) = 0;
		virtual void DelFont(NN<DrawFont> f) = 0;

		virtual Math::Size2DDbl GetTextSize(NN<DrawFont> fnt, Text::CStringNN txt) = 0;
		virtual void SetTextAlign(Media::DrawEngine::DrawPos pos) = 0;
		virtual void GetStringBound(UnsafeArray<Int32> pos, IntOS centX, IntOS centY, UnsafeArray<const UTF8Char> str, NN<DrawFont> f, OutParam<IntOS> drawX, OutParam<IntOS> drawY) = 0;
		virtual void GetStringBoundRot(UnsafeArray<Int32> pos, Double centX, Double centY, UnsafeArray<const UTF8Char> str, NN<DrawFont> f, Double angleDegree, OutParam<IntOS> drawX, OutParam<IntOS> drawY) = 0;
		virtual void CopyBits(IntOS x, IntOS y, UnsafeArray<UInt8> imgPtr, UIntOS bpl, UIntOS width, UIntOS height, Bool upsideDown) const = 0;
		
		Bool DrawStringHAlign(Math::Coord2DDbl tl, Double brx, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b, Text::HAlignment hAlign);
		UInt32 GetPixel32(IntOS x, IntOS y);
		void SetImageAlpha(UInt8 alpha);
		void MulImageAlpha(Double val);

		virtual Optional<Media::StaticImage> ToStaticImage() const = 0;
		virtual UIntOS SavePng(NN<IO::SeekableStream> stm) = 0;
		virtual UIntOS SaveGIF(NN<IO::SeekableStream> stm) = 0;
		virtual UIntOS SaveJPG(NN<IO::SeekableStream> stm) = 0;
	};
}
#endif
