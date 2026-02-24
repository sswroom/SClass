#ifndef _SM_MEDIA_VECTORGRAPH
#define _SM_MEDIA_VECTORGRAPH
#include "Data/ArrayListNN.hpp"
#include "Math/Size2D.h"
#include "Math/Geometry/Vector2D.h"
#include "Math/Unit/Distance.h"
#include "Media/DrawEngine.h"
#include "Media/Image.h"

namespace Media
{
	class VectorGraph : public Media::DrawImage, public Media::Image
	{
	private:
		class VectorPenStyle : public Media::DrawPen
		{
		private:
			UIntOS index;
			UInt32 color;
			Double thick;
			UnsafeArrayOpt<UInt8> pattern;
			UIntOS nPattern;

		public:
			VectorPenStyle(UIntOS index, UInt32 color, Double thick, UnsafeArrayOpt<UInt8> pattern, UIntOS nPattern);
			virtual ~VectorPenStyle();
			
			virtual Double GetThick();
			Bool IsSame(UInt32 color, Double thick, UnsafeArrayOpt<UInt8> pattern, UIntOS nPattern);
			UIntOS GetIndex();
			NN<Media::DrawPen> CreateDrawPen(Double oriDPI, NN<Media::DrawImage> dimg);
		};

		class VectorFontStyle : public Media::DrawFont
		{
		private:
			UIntOS index;
			NN<Text::String> name;
			Double heightPt;
			Media::DrawEngine::DrawFontStyle fontStyle;
			UInt32 codePage;

		public:
			VectorFontStyle(UIntOS index, Text::CStringNN name, Double heightPt, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage);
			virtual ~VectorFontStyle();
			
			Bool IsSame(Text::CStringNN name, Double heightPt, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage);
			UIntOS GetIndex();
			Double GetHeightPt();
			NN<Text::String> GetName() const;
			Media::DrawEngine::DrawFontStyle GetStyle();
			UInt32 GetCodePage();
			NN<Media::DrawFont> CreateDrawFont(Double oriDPI, NN<Media::DrawImage> dimg);
		};

		class VectorBrushStyle : public Media::DrawBrush
		{
		private:
			UIntOS index;
			UInt32 color;

		public:
			VectorBrushStyle(UIntOS index, UInt32 color);
			virtual ~VectorBrushStyle();

			Bool IsSame(UInt32 color);
			UIntOS GetIndex();
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
		Optional<Media::ColorSess> colorSess;
		Media::ColorProfile colorProfile;
		UInt32 srid;
		Math::Size2DDbl size;
		Double hdpi;
		Double vdpi;
		Data::ArrayListNN<VectorPenStyle> penStyles;
		Data::ArrayListNN<VectorFontStyle> fontStyles;
		Data::ArrayListNN<VectorBrushStyle> brushStyles;

		Data::ArrayListNN<VectorStyles> itemStyle;
		Data::ArrayListNN<Math::Geometry::Vector2D> items;
		Media::DrawEngine::DrawPos align;
		
	public:
		VectorGraph(UInt32 srid, Double visibleWidthPx, Double visibleHeightPx, NN<Media::DrawEngine> refEng, NN<const Media::ColorProfile> colorProfile);
		virtual ~VectorGraph();

		Math::Size2DDbl GetSizeDbl() const;
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
		virtual UnsafeArrayOpt<UInt8> GetImgBits(OutParam<Bool> revOrder);
		virtual void GetImgBitsEnd(Bool modified);
		virtual UIntOS GetImgBpl() const;
		virtual Optional<Media::EXIFData> GetEXIF() const;
		virtual Media::PixelFormat GetPixelFormat() const;
		virtual void SetColorSess(Optional<Media::ColorSess> colorSess);

		virtual Bool DrawLine(Double x1, Double y1, Double x2, Double y2, NN<DrawPen> p);
		virtual Bool DrawPolylineI(UnsafeArray<const Int32> points, UIntOS nPoints, NN<DrawPen> p);
		virtual Bool DrawPolygonI(UnsafeArray<const Int32> points, UIntOS nPoints, Optional<DrawPen> p, Optional<DrawBrush> b);
		virtual Bool DrawPolyPolygonI(UnsafeArray<const Int32> points, UnsafeArray<const UInt32> pointCnt, UIntOS nPointCnt, Optional<DrawPen> p, Optional<DrawBrush> b);
		virtual Bool DrawPolyline(UnsafeArray<const Math::Coord2DDbl> points, UIntOS nPoints, NN<DrawPen> p);
		virtual Bool DrawPolygon(UnsafeArray<const Math::Coord2DDbl> points, UIntOS nPoints, Optional<DrawPen> p, Optional<DrawBrush> b);
		virtual Bool DrawPolyPolygon(UnsafeArray<const Math::Coord2DDbl> points, UnsafeArray<const UInt32> pointCnt, UIntOS nPointCnt, Optional<DrawPen> p, Optional<DrawBrush> b);
		virtual Bool DrawRect(Math::Coord2DDbl tl, Math::Size2DDbl size, Optional<DrawPen> p, Optional<DrawBrush> b);
		virtual Bool DrawEllipse(Math::Coord2DDbl tl, Math::Size2DDbl size, Optional<DrawPen> p, Optional<DrawBrush> b);
		virtual Bool DrawString(Math::Coord2DDbl tl, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b);
		virtual Bool DrawString(Math::Coord2DDbl tl, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b);
		virtual Bool DrawStringRot(Math::Coord2DDbl center, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> p, Double angleDegree);
		virtual Bool DrawStringRot(Math::Coord2DDbl center, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> p, Double angleDegree);
		virtual Bool DrawStringB(Math::Coord2DDbl tl, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> p, UIntOS buffSize);
		virtual Bool DrawStringB(Math::Coord2DDbl tl, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> p, UIntOS buffSize);
		virtual Bool DrawStringRotB(Math::Coord2DDbl center, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> p, Double angleDegree, UIntOS buffSize);
		virtual Bool DrawStringRotB(Math::Coord2DDbl center, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> p, Double angleDegree, UIntOS buffSize);
		virtual Bool DrawImagePt(NN<DrawImage> img, Math::Coord2DDbl tl);
		virtual Bool DrawImagePt2(NN<Media::StaticImage> img, Math::Coord2DDbl tl);
		virtual Bool DrawImagePt3(NN<DrawImage> img, Math::Coord2DDbl destTL, Math::Coord2DDbl srcTL, Math::Size2DDbl srcSize); /////////////////////////////
		virtual Bool DrawImageQuad(NN<Media::StaticImage> img, Math::Quadrilateral quad);

		virtual NN<DrawPen> NewPenARGB(UInt32 color, Double thick, UnsafeArrayOpt<UInt8> pattern, UIntOS nPattern);
		virtual NN<DrawBrush> NewBrushARGB(UInt32 color);
		virtual NN<DrawFont> NewFontPt(Text::CStringNN name, Double ptSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage);
		virtual NN<DrawFont> NewFontPx(Text::CStringNN name, Double pxSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage);
		virtual NN<DrawFont> CloneFont(NN<Media::DrawFont> f);
		virtual void DelPen(NN<DrawPen> p);
		virtual void DelBrush(NN<DrawBrush> b);
		virtual void DelFont(NN<DrawFont> f);

		virtual Math::Size2DDbl GetTextSize(NN<DrawFont> fnt, Text::CStringNN txt);
		virtual void SetTextAlign(Media::DrawEngine::DrawPos pos);
		virtual void GetStringBound(UnsafeArray<Int32> pos, IntOS centX, IntOS centY, UnsafeArray<const UTF8Char> str, NN<DrawFont> f, OutParam<IntOS> drawX, OutParam<IntOS> drawY);
		virtual void GetStringBoundRot(UnsafeArray<Int32> pos, Double centX, Double centY, UnsafeArray<const UTF8Char> str, NN<DrawFont> f, Double angleDegree, OutParam<IntOS> drawX, OutParam<IntOS> drawY);
		virtual void CopyBits(IntOS x, IntOS y, UnsafeArray<UInt8> imgPtr, UIntOS bpl, UIntOS width, UIntOS height, Bool upsideDown) const;

		virtual Optional<Media::StaticImage> ToStaticImage() const;
		virtual UIntOS SavePng(NN<IO::SeekableStream> stm);
		virtual UIntOS SaveGIF(NN<IO::SeekableStream> stm);
		virtual UIntOS SaveJPG(NN<IO::SeekableStream> stm);

		virtual Bool IsRaster() const { return false; }
		virtual NN<Media::StaticImage> CreateStaticImage() const;
		virtual NN<Media::StaticImage> CreateSubImage(Math::RectArea<IntOS> area) const;

		Double GetVisibleWidthMM() const;
		Double GetVisibleHeightMM() const;
		UIntOS GetCount() const;
		Optional<Math::Geometry::Vector2D> GetItem(UIntOS index) const;
		Optional<VectorStyles> GetStyle(UIntOS index) const;
		void DrawTo(NN<Media::DrawImage> dimg, OptOut<UInt32> imgDurMS) const;
		void DrawTo(Math::Coord2DDbl ofst, Double scale, NN<Media::DrawImage> dimg, OptOut<UInt32> imgDurMS) const;
	};
}
#endif
