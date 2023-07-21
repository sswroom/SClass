#ifndef _SM_MEDIA_VECTORGRAPH
#define _SM_MEDIA_VECTORGRAPH
#include "Data/ArrayList.h"
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
			Media::DrawPen *CreateDrawPen(Double oriDPI, Media::DrawImage *dimg);
		};

		class VectorFontStyle : public Media::DrawFont
		{
		private:
			UOSInt index;
			NotNullPtr<Text::String> name;
			Double heightPt;
			Media::DrawEngine::DrawFontStyle fontStyle;
			UInt32 codePage;

		public:
			VectorFontStyle(UOSInt index, Text::CString name, Double heightPt, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage);
			virtual ~VectorFontStyle();
			
			Bool IsSame(Text::CString name, Double heightPt, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage);
			UOSInt GetIndex();
			Double GetHeightPt();
			NotNullPtr<Text::String> GetName() const;
			Media::DrawEngine::DrawFontStyle GetStyle();
			UInt32 GetCodePage();
			Media::DrawFont *CreateDrawFont(Double oriDPI, Media::DrawImage *dimg);
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
			Media::DrawBrush *CreateDrawBrush(Double oriDPI, Media::DrawImage *dimg);
		};

		typedef struct
		{
			VectorPenStyle *pen;
			VectorFontStyle *font;
			VectorBrushStyle *brush;
		} VectorStyles;

	private:
		NotNullPtr<Media::DrawEngine> refEng;
		Media::ColorProfile *colorProfile;
		UInt32 srid;
		Math::Size2DDbl size;
		Math::Unit::Distance::DistanceUnit unit;
		Data::ArrayList<VectorPenStyle*> *penStyles;
		Data::ArrayList<VectorFontStyle*> *fontStyles;
		Data::ArrayList<VectorBrushStyle*> *brushStyles;

		Data::ArrayList<VectorStyles*> *itemStyle;
		Data::ArrayList<Math::Geometry::Vector2D *> *items;
		Media::DrawEngine::DrawPos align;
		
	public:
		VectorGraph(UInt32 srid, Double visibleWidth, Double visibleHeight, Math::Unit::Distance::DistanceUnit unit, NotNullPtr<Media::DrawEngine> refEng, Media::ColorProfile *colorProfile);
		virtual ~VectorGraph();

		Math::Size2DDbl GetSizeDbl() const;
		virtual UOSInt GetWidth() const;
		virtual UOSInt GetHeight() const;
		virtual Math::Size2D<UOSInt> GetSize() const;
		virtual UInt32 GetBitCount() const;
		virtual ColorProfile *GetColorProfile() const;
		virtual void SetColorProfile(const ColorProfile *color);
		virtual Media::AlphaType GetAlphaType() const;
		virtual void SetAlphaType(Media::AlphaType atype);
		virtual Double GetHDPI() const;
		virtual Double GetVDPI() const;
		virtual void SetHDPI(Double dpi);
		virtual void SetVDPI(Double dpi);
		virtual UInt8 *GetImgBits(Bool *revOrder);
		virtual void GetImgBitsEnd(Bool modified);
		virtual UOSInt GetImgBpl() const;
		virtual Media::EXIFData *GetEXIF() const;
		virtual Media::PixelFormat GetPixelFormat() const;

		virtual Bool DrawLine(Double x1, Double y1, Double x2, Double y2, DrawPen *p);
		virtual Bool DrawPolylineI(const Int32 *points, UOSInt nPoints, DrawPen *p);
		virtual Bool DrawPolygonI(const Int32 *points, UOSInt nPoints, DrawPen *p, DrawBrush *b);
		virtual Bool DrawPolyPolygonI(const Int32 *points, const UInt32 *pointCnt, UOSInt nPointCnt, DrawPen *p, DrawBrush *b);
		virtual Bool DrawPolyline(const Math::Coord2DDbl *points, UOSInt nPoints, DrawPen *p);
		virtual Bool DrawPolygon(const Math::Coord2DDbl *points, UOSInt nPoints, DrawPen *p, DrawBrush *b);
		virtual Bool DrawPolyPolygon(const Math::Coord2DDbl *points, const UInt32 *pointCnt, UOSInt nPointCnt, DrawPen *p, DrawBrush *b);
		virtual Bool DrawRect(Math::Coord2DDbl tl, Math::Size2DDbl size, DrawPen *p, DrawBrush *b);
		virtual Bool DrawEllipse(Math::Coord2DDbl tl, Math::Size2DDbl size, DrawPen *p, DrawBrush *b);
		virtual Bool DrawString(Math::Coord2DDbl tl, NotNullPtr<Text::String> str, DrawFont *f, DrawBrush *b);
		virtual Bool DrawString(Math::Coord2DDbl tl, Text::CString str, DrawFont *f, DrawBrush *b);
		virtual Bool DrawStringRot(Math::Coord2DDbl center, NotNullPtr<Text::String> str, DrawFont *f, DrawBrush *p, Double angleDegree);
		virtual Bool DrawStringRot(Math::Coord2DDbl center, Text::CString str, DrawFont *f, DrawBrush *p, Double angleDegree);
		virtual Bool DrawStringB(Math::Coord2DDbl tl, NotNullPtr<Text::String> str, DrawFont *f, DrawBrush *p, UOSInt buffSize);
		virtual Bool DrawStringB(Math::Coord2DDbl tl, Text::CString str, DrawFont *f, DrawBrush *p, UOSInt buffSize);
		virtual Bool DrawStringRotB(Math::Coord2DDbl center, NotNullPtr<Text::String> str, DrawFont *f, DrawBrush *p, Double angleDegree, UOSInt buffSize);
		virtual Bool DrawStringRotB(Math::Coord2DDbl center, Text::CString str, DrawFont *f, DrawBrush *p, Double angleDegree, UOSInt buffSize);
		virtual Bool DrawImagePt(DrawImage *img, Math::Coord2DDbl tl);
		virtual Bool DrawImagePt2(Media::StaticImage *img, Math::Coord2DDbl tl);
		virtual Bool DrawImagePt3(DrawImage *img, Math::Coord2DDbl destTL, Math::Coord2DDbl srcTL, Math::Size2DDbl srcSize); /////////////////////////////

		virtual DrawPen *NewPenARGB(UInt32 color, Double thick, UInt8 *pattern, UOSInt nPattern);
		virtual DrawBrush *NewBrushARGB(UInt32 color);
		virtual DrawFont *NewFontPt(Text::CString name, Double ptSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage);
		virtual DrawFont *NewFontPx(Text::CString name, Double pxSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage);
		virtual DrawFont *CloneFont(Media::DrawFont *f);
		virtual void DelPen(DrawPen *p);
		virtual void DelBrush(DrawBrush *b);
		virtual void DelFont(DrawFont *f);

		virtual Math::Size2DDbl GetTextSize(DrawFont *fnt, Text::CString txt);
		virtual void SetTextAlign(Media::DrawEngine::DrawPos pos);
		virtual void GetStringBound(Int32 *pos, OSInt centX, OSInt centY, const UTF8Char *str, DrawFont *f, OSInt *drawX, OSInt *drawY);
		virtual void GetStringBoundRot(Int32 *pos, Double centX, Double centY, const UTF8Char *str, DrawFont *f, Double angleDegree, OSInt *drawX, OSInt *drawY);
		virtual void CopyBits(OSInt x, OSInt y, void *imgPtr, UOSInt bpl, UOSInt width, UOSInt height, Bool upsideDown) const;

		virtual Media::StaticImage *ToStaticImage() const;
		virtual UOSInt SavePng(NotNullPtr<IO::SeekableStream> stm);
		virtual UOSInt SaveGIF(NotNullPtr<IO::SeekableStream> stm);
		virtual UOSInt SaveJPG(NotNullPtr<IO::SeekableStream> stm);

		Double GetVisibleWidthMM() const;
		Double GetVisibleHeightMM() const;
		void DrawTo(Media::DrawImage *dimg, UInt32 *imgDurMS);
	};
}
#endif
