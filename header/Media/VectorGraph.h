#ifndef _SM_MEDIA_VECTORGRAPH
#define _SM_MEDIA_VECTORGRAPH
#include "Data/ArrayListNN.h"
#include "Math/Size2D.h"
#include "Math/Geometry/Vector2D.h"
#include "Math/Unit/Distance.h"
#include "Media/DrawEngine.h"

namespace Media
{
	class VectorGraph : public Media::DrawImage
	{
	private:
		class VectorPenStyle : public Media::DrawPen
		{
		private:
			UOSInt index;
			UInt32 color;
			Double thick;
			UInt8 *pattern;
			UOSInt nPattern;

		public:
			VectorPenStyle(UOSInt index, UInt32 color, Double thick, UInt8 *pattern, UOSInt nPattern);
			virtual ~VectorPenStyle();
			
			virtual Double GetThick();
			Bool IsSame(UInt32 color, Double thick, UInt8 *pattern, UOSInt nPattern);
			UOSInt GetIndex();
			NN<Media::DrawPen> CreateDrawPen(Double oriDPI, NN<Media::DrawImage> dimg);
		};

		class VectorFontStyle : public Media::DrawFont
		{
		private:
			UOSInt index;
			NN<Text::String> name;
			Double heightPt;
			Media::DrawEngine::DrawFontStyle fontStyle;
			UInt32 codePage;

		public:
			VectorFontStyle(UOSInt index, Text::CStringNN name, Double heightPt, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage);
			virtual ~VectorFontStyle();
			
			Bool IsSame(Text::CStringNN name, Double heightPt, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage);
			UOSInt GetIndex();
			Double GetHeightPt();
			NN<Text::String> GetName() const;
			Media::DrawEngine::DrawFontStyle GetStyle();
			UInt32 GetCodePage();
			NN<Media::DrawFont> CreateDrawFont(Double oriDPI, NN<Media::DrawImage> dimg);
		};

		class VectorBrushStyle : public Media::DrawBrush
		{
		private:
			UOSInt index;
			UInt32 color;

		public:
			VectorBrushStyle(UOSInt index, UInt32 color);
			virtual ~VectorBrushStyle();

			Bool IsSame(UInt32 color);
			UOSInt GetIndex();
			NN<Media::DrawBrush> CreateDrawBrush(Double oriDPI, NN<Media::DrawImage> dimg);
		};

		typedef struct
		{
			Optional<VectorPenStyle> pen;
			Optional<VectorFontStyle> font;
			Optional<VectorBrushStyle> brush;
		} VectorStyles;

	private:
		NN<Media::DrawEngine> refEng;
		Media::ColorProfile colorProfile;
		UInt32 srid;
		Math::Size2DDbl size;
		Math::Unit::Distance::DistanceUnit unit;
		Data::ArrayListNN<VectorPenStyle> penStyles;
		Data::ArrayListNN<VectorFontStyle> fontStyles;
		Data::ArrayListNN<VectorBrushStyle> brushStyles;

		Data::ArrayListNN<VectorStyles> itemStyle;
		Data::ArrayListNN<Math::Geometry::Vector2D> items;
		Media::DrawEngine::DrawPos align;
		
	public:
		VectorGraph(UInt32 srid, Double visibleWidth, Double visibleHeight, Math::Unit::Distance::DistanceUnit unit, NN<Media::DrawEngine> refEng, NN<const Media::ColorProfile> colorProfile);
		virtual ~VectorGraph();

		Math::Size2DDbl GetSizeDbl() const;
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
		virtual UInt8 *GetImgBits(OutParam<Bool> revOrder);
		virtual void GetImgBitsEnd(Bool modified);
		virtual UOSInt GetImgBpl() const;
		virtual Optional<Media::EXIFData> GetEXIF() const;
		virtual Media::PixelFormat GetPixelFormat() const;

		virtual Bool DrawLine(Double x1, Double y1, Double x2, Double y2, NN<DrawPen> p);
		virtual Bool DrawPolylineI(const Int32 *points, UOSInt nPoints, NN<DrawPen> p);
		virtual Bool DrawPolygonI(const Int32 *points, UOSInt nPoints, Optional<DrawPen> p, Optional<DrawBrush> b);
		virtual Bool DrawPolyPolygonI(const Int32 *points, const UInt32 *pointCnt, UOSInt nPointCnt, Optional<DrawPen> p, Optional<DrawBrush> b);
		virtual Bool DrawPolyline(const Math::Coord2DDbl *points, UOSInt nPoints, NN<DrawPen> p);
		virtual Bool DrawPolygon(const Math::Coord2DDbl *points, UOSInt nPoints, Optional<DrawPen> p, Optional<DrawBrush> b);
		virtual Bool DrawPolyPolygon(const Math::Coord2DDbl *points, const UInt32 *pointCnt, UOSInt nPointCnt, Optional<DrawPen> p, Optional<DrawBrush> b);
		virtual Bool DrawRect(Math::Coord2DDbl tl, Math::Size2DDbl size, Optional<DrawPen> p, Optional<DrawBrush> b);
		virtual Bool DrawEllipse(Math::Coord2DDbl tl, Math::Size2DDbl size, Optional<DrawPen> p, Optional<DrawBrush> b);
		virtual Bool DrawString(Math::Coord2DDbl tl, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b);
		virtual Bool DrawString(Math::Coord2DDbl tl, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b);
		virtual Bool DrawStringRot(Math::Coord2DDbl center, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> p, Double angleDegree);
		virtual Bool DrawStringRot(Math::Coord2DDbl center, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> p, Double angleDegree);
		virtual Bool DrawStringB(Math::Coord2DDbl tl, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> p, UOSInt buffSize);
		virtual Bool DrawStringB(Math::Coord2DDbl tl, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> p, UOSInt buffSize);
		virtual Bool DrawStringRotB(Math::Coord2DDbl center, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> p, Double angleDegree, UOSInt buffSize);
		virtual Bool DrawStringRotB(Math::Coord2DDbl center, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> p, Double angleDegree, UOSInt buffSize);
		virtual Bool DrawImagePt(NN<DrawImage> img, Math::Coord2DDbl tl);
		virtual Bool DrawImagePt2(NN<Media::StaticImage> img, Math::Coord2DDbl tl);
		virtual Bool DrawImagePt3(NN<DrawImage> img, Math::Coord2DDbl destTL, Math::Coord2DDbl srcTL, Math::Size2DDbl srcSize); /////////////////////////////

		virtual NN<DrawPen> NewPenARGB(UInt32 color, Double thick, UInt8 *pattern, UOSInt nPattern);
		virtual NN<DrawBrush> NewBrushARGB(UInt32 color);
		virtual NN<DrawFont> NewFontPt(Text::CStringNN name, Double ptSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage);
		virtual NN<DrawFont> NewFontPx(Text::CStringNN name, Double pxSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage);
		virtual NN<DrawFont> CloneFont(NN<Media::DrawFont> f);
		virtual void DelPen(NN<DrawPen> p);
		virtual void DelBrush(NN<DrawBrush> b);
		virtual void DelFont(NN<DrawFont> f);

		virtual Math::Size2DDbl GetTextSize(NN<DrawFont> fnt, Text::CStringNN txt);
		virtual void SetTextAlign(Media::DrawEngine::DrawPos pos);
		virtual void GetStringBound(Int32 *pos, OSInt centX, OSInt centY, const UTF8Char *str, NN<DrawFont> f, OutParam<OSInt> drawX, OutParam<OSInt> drawY);
		virtual void GetStringBoundRot(Int32 *pos, Double centX, Double centY, const UTF8Char *str, NN<DrawFont> f, Double angleDegree, OutParam<OSInt> drawX, OutParam<OSInt> drawY);
		virtual void CopyBits(OSInt x, OSInt y, void *imgPtr, UOSInt bpl, UOSInt width, UOSInt height, Bool upsideDown) const;

		virtual Optional<Media::StaticImage> ToStaticImage() const;
		virtual UOSInt SavePng(NN<IO::SeekableStream> stm);
		virtual UOSInt SaveGIF(NN<IO::SeekableStream> stm);
		virtual UOSInt SaveJPG(NN<IO::SeekableStream> stm);

		Double GetVisibleWidthMM() const;
		Double GetVisibleHeightMM() const;
		void DrawTo(NN<Media::DrawImage> dimg, OptOut<UInt32> imgDurMS);
	};
}
#endif
