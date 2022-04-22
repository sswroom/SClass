#ifndef _SM_MEDIA_VECTORGRAPH
#define _SM_MEDIA_VECTORGRAPH
#include "Data/ArrayList.h"
#include "Math/Vector2D.h"
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
			Text::String *name;
			Double heightPt;
			Media::DrawEngine::DrawFontStyle fontStyle;
			UInt32 codePage;

		public:
			VectorFontStyle(UOSInt index, Text::CString name, Double heightPt, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage);
			virtual ~VectorFontStyle();
			
			Bool IsSame(Text::CString name, Double heightPt, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage);
			UOSInt GetIndex();
			Double GetHeightPt();
			Text::String *GetName();
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
		Media::DrawEngine *refEng;
		Media::ColorProfile *colorProfile;
		UInt32 srid;
		Double width;
		Double height;
		Math::Unit::Distance::DistanceUnit unit;
		Data::ArrayList<VectorPenStyle*> *penStyles;
		Data::ArrayList<VectorFontStyle*> *fontStyles;
		Data::ArrayList<VectorBrushStyle*> *brushStyles;

		Data::ArrayList<VectorStyles*> *itemStyle;
		Data::ArrayList<Math::Vector2D *> *items;
		Media::DrawEngine::DrawPos align;
		
	public:
		VectorGraph(UInt32 srid, Double visibleWidth, Double visibleHeight, Math::Unit::Distance::DistanceUnit unit, Media::DrawEngine *refEng, Media::ColorProfile *colorProfile);
		virtual ~VectorGraph();

		virtual UOSInt GetWidth();
		virtual UOSInt GetHeight();
		virtual UInt32 GetBitCount();
		virtual ColorProfile *GetColorProfile();
		virtual void SetColorProfile(const ColorProfile *color);
		virtual Media::AlphaType GetAlphaType();
		virtual void SetAlphaType(Media::AlphaType atype);
		virtual Double GetHDPI();
		virtual Double GetVDPI();
		virtual void SetHDPI(Double dpi);
		virtual void SetVDPI(Double dpi);
		virtual UInt8 *GetImgBits(Bool *revOrder);
		virtual void GetImgBitsEnd(Bool modified);
		virtual UOSInt GetImgBpl();
		virtual Media::EXIFData *GetEXIF();
		virtual Media::PixelFormat GetPixelFormat();

		virtual Bool DrawLine(Double x1, Double y1, Double x2, Double y2, DrawPen *p);
		virtual Bool DrawPolylineI(Int32 *points, UOSInt nPoints, DrawPen *p);
		virtual Bool DrawPolygonI(Int32 *points, UOSInt nPoints, DrawPen *p, DrawBrush *b);
		virtual Bool DrawPolyPolygonI(Int32 *points, UInt32 *pointCnt, UOSInt nPointCnt, DrawPen *p, DrawBrush *b);
		virtual Bool DrawPolyline(Math::Coord2D<Double> *points, UOSInt nPoints, DrawPen *p);
		virtual Bool DrawPolygon(Math::Coord2D<Double> *points, UOSInt nPoints, DrawPen *p, DrawBrush *b);
		virtual Bool DrawPolyPolygon(Math::Coord2D<Double> *points, UInt32 *pointCnt, UOSInt nPointCnt, DrawPen *p, DrawBrush *b);
		virtual Bool DrawRect(Double x, Double y, Double w, Double h, DrawPen *p, DrawBrush *b);
		virtual Bool DrawEllipse(Double tlx, Double tly, Double w, Double h, DrawPen *p, DrawBrush *b);
		virtual Bool DrawString(Double tlx, Double tly, Text::String *str, DrawFont *f, DrawBrush *b);
		virtual Bool DrawString(Double tlx, Double tly, Text::CString str, DrawFont *f, DrawBrush *b);
		virtual Bool DrawStringRot(Double centX, Double centY, Text::String *str, DrawFont *f, DrawBrush *p, Double angleDegree);
		virtual Bool DrawStringRot(Double centX, Double centY, Text::CString str, DrawFont *f, DrawBrush *p, Double angleDegree);
		virtual Bool DrawStringB(Double tlx, Double tly, Text::String *str, DrawFont *f, DrawBrush *p, UOSInt buffSize);
		virtual Bool DrawStringB(Double tlx, Double tly, Text::CString str, DrawFont *f, DrawBrush *p, UOSInt buffSize);
		virtual Bool DrawStringRotB(Double centX, Double centY, Text::String *str, DrawFont *f, DrawBrush *p, Double angleDegree, UOSInt buffSize);
		virtual Bool DrawStringRotB(Double centX, Double centY, Text::CString str, DrawFont *f, DrawBrush *p, Double angleDegree, UOSInt buffSize);
		virtual Bool DrawImagePt(DrawImage *img, Double tlx, Double tly);
		virtual Bool DrawImagePt2(Media::StaticImage *img, Double tlx, Double tly);
		virtual Bool DrawImagePt3(DrawImage *img, Double destX, Double destY, Double srcX, Double srcY, Double srcW, Double srcH); /////////////////////////////

		virtual DrawPen *NewPenARGB(UInt32 color, Double thick, UInt8 *pattern, UOSInt nPattern);
		virtual DrawBrush *NewBrushARGB(UInt32 color);
		virtual DrawFont *NewFontPt(Text::CString name, Double ptSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage);
		virtual DrawFont *NewFontPx(Text::CString name, Double pxSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage);
		virtual DrawFont *CloneFont(Media::DrawFont *f);
		virtual void DelPen(DrawPen *p);
		virtual void DelBrush(DrawBrush *b);
		virtual void DelFont(DrawFont *f);

		virtual Bool GetTextSize(DrawFont *fnt, Text::CString txt, Double *sz);
		virtual void SetTextAlign(Media::DrawEngine::DrawPos pos);
		virtual void GetStringBound(Int32 *pos, OSInt centX, OSInt centY, const UTF8Char *str, DrawFont *f, OSInt *drawX, OSInt *drawY);
		virtual void GetStringBoundRot(Int32 *pos, Double centX, Double centY, const UTF8Char *str, DrawFont *f, Double angleDegree, OSInt *drawX, OSInt *drawY);
		virtual void CopyBits(OSInt x, OSInt y, void *imgPtr, UOSInt bpl, UOSInt width, UOSInt height, Bool upsideDown);

		virtual Media::StaticImage *ToStaticImage();
		virtual UOSInt SavePng(IO::SeekableStream *stm);
		virtual UOSInt SaveGIF(IO::SeekableStream *stm);
		virtual UOSInt SaveJPG(IO::SeekableStream *stm);

		Double GetVisibleWidthMM();
		Double GetVisibleHeightMM();
		void DrawTo(Media::DrawImage *dimg, UInt32 *imgDurMS);
	};
}
#endif
