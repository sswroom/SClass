#ifndef _SM_MEDIA_SVGDOCUMENT
#define _SM_MEDIA_SVGDOCUMENT
#include "Data/ArrayListA.hpp"
#include "Data/ArrayListNN.hpp"
#include "Media/SVGCore.h"
#include "Text/String.h"

namespace Media
{
	class SVGDocument;

	class SVGElement
	{
	private:
		Optional<Text::String> id;

	protected:
		void AppendEleAttr(NN<Text::StringBuilderUTF8> sb) const
		{
			NN<Text::String> s;
			if (this->id.SetTo(s))
			{
				sb->AppendC(UTF8STRC(" id=\""));
				sb->Append(s);
				sb->AppendUTF8Char('\"');
			}
		}
	public:
		SVGElement() { this->id = nullptr; }
		virtual ~SVGElement() {}

		void SetID(Text::CStringNN id)
		{
			OPTSTR_DEL(this->id);
			this->id = Text::String::New(id);
		}

		Optional<Text::String> GetID() const
		{
			return this->id;
		}

		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const = 0;
	};

	class SVGLine : public SVGElement
	{
	private:
		Math::Coord2DDbl pt1;
		Math::Coord2DDbl pt2;
		NN<DrawPen> pen;
	public:
		SVGLine(Math::Coord2DDbl pt1, Math::Coord2DDbl pt2, NN<DrawPen> pen);
		virtual ~SVGLine();

		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
	};

	class SVGPolyline : public SVGElement
	{
	private:
		Data::ArrayListA<Math::Coord2DDbl> points;
		NN<DrawPen> pen;
	public:
		SVGPolyline(NN<DrawPen> pen);
		virtual ~SVGPolyline();

		void AddPoint(Math::Coord2DDbl pt);
		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
	};

	class SVGPolygon : public SVGElement
	{
	private:
		Data::ArrayListA<Math::Coord2DDbl> points;
		Optional<DrawPen> pen;
		Optional<DrawBrush> brush;
	public:
		SVGPolygon(Optional<DrawPen> pen, Optional<DrawBrush> brush);
		virtual ~SVGPolygon();

		void AddPoint(Math::Coord2DDbl pt);
		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
	};

	class SVGRect : public SVGElement
	{
	private:
		Math::Coord2DDbl tl;
		Math::Size2DDbl size;
		Optional<DrawPen> pen;
		Optional<DrawBrush> brush;
	public:
		SVGRect(Math::Coord2DDbl tl, Math::Size2DDbl size, Optional<DrawPen> pen, Optional<DrawBrush> brush);
		virtual ~SVGRect();

		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
	};

	class SVGEllipse : public SVGElement
	{
	private:
		Math::Coord2DDbl center;
		Math::Size2DDbl radius;
		Optional<DrawPen> pen;
		Optional<DrawBrush> brush;
	public:
		SVGEllipse(Math::Coord2DDbl center, Math::Size2DDbl radius, Optional<DrawPen> pen, Optional<DrawBrush> brush);
		virtual ~SVGEllipse();

		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
	};

	class SVGText : public SVGElement
	{
	private:
		Math::Coord2DDbl tl;
		NN<Text::String> txt;
		NN<DrawFont> font;
		NN<DrawBrush> brush;
		Double angleDegreeACW;
		Math::Coord2DDbl rotateCenter;
	public:
		SVGText(Math::Coord2DDbl tl, Text::CStringNN txt, NN<DrawFont> font, NN<DrawBrush> brush);
		virtual ~SVGText();

		void SetRotate(Double angleDegreeACW, Math::Coord2DDbl rotateCenter);

		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
	};

	class SVGImage : public SVGElement
	{
	private:
		Math::Coord2DDbl tl;
		Math::Size2DDbl size;
		NN<Text::String> href;
	public:
		SVGImage(Math::Coord2DDbl tl, Math::Size2DDbl size, Text::CStringNN href);
		virtual ~SVGImage();

		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
	};

	class SVGContainer : public SVGElement, public DrawImage
	{
	private:
		Data::ArrayListNN<SVGElement> elements;
		NN<SVGDocument> doc;
	
	protected:
		NN<Media::DrawEngine> refEng;

		void ToInnerString(NN<Text::StringBuilderUTF8> sb) const;
	public:
		SVGContainer(NN<Media::DrawEngine> refEng, NN<SVGDocument> doc);
		virtual ~SVGContainer();

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
		virtual Bool DrawStringRot(Math::Coord2DDbl center, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegreeACW);
		virtual Bool DrawStringRot(Math::Coord2DDbl center, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegreeACW);
		virtual Bool DrawStringB(Math::Coord2DDbl tl, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b, UIntOS buffSize);
		virtual Bool DrawStringB(Math::Coord2DDbl tl, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b, UIntOS buffSize);
		virtual Bool DrawStringRotB(Math::Coord2DDbl center, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegreeACW, UIntOS buffSize);
		virtual Bool DrawStringRotB(Math::Coord2DDbl center, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegreeACW, UIntOS buffSize);
		virtual Bool DrawImagePt(NN<DrawImage> img, Math::Coord2DDbl tl);
		virtual Bool DrawImagePt2(NN<DrawImage> img, Math::Coord2DDbl destTL, Math::Coord2DDbl srcTL, Math::Size2DDbl srcSize);
		virtual Bool DrawSImagePt(NN<Media::StaticImage> img, Math::Coord2DDbl tl);
		virtual Bool DrawSImagePt2(NN<Media::StaticImage> img, Math::Coord2DDbl destTL, Math::Coord2DDbl srcTL, Math::Size2DDbl srcSize);
		virtual Bool DrawImageQuad(NN<Media::StaticImage> img, Math::Quadrilateral quad);

		virtual NN<DrawPen> NewPenARGB(UInt32 color, Double thick, UnsafeArrayOpt<UInt8> pattern, UIntOS nPattern);
		virtual NN<DrawBrush> NewBrushARGB(UInt32 color);
		virtual NN<DrawFont> NewFontPt(Text::CStringNN name, Double ptSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage); // 72 dpi size
		virtual NN<DrawFont> NewFontPx(Text::CStringNN name, Double pxSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage); // Actual size
		virtual NN<DrawFont> CloneFont(NN<DrawFont> f);
		virtual void DelPen(NN<DrawPen> p);
		virtual void DelBrush(NN<DrawBrush> b);
		virtual void DelFont(NN<DrawFont> f);

		virtual Math::Size2DDbl GetTextSize(NN<DrawFont> fnt, Text::CStringNN txt);
		virtual void SetTextAlign(Media::DrawEngine::DrawPos pos);
		virtual void GetStringBound(UnsafeArray<Int32> pos, IntOS centX, IntOS centY, UnsafeArray<const UTF8Char> str, NN<DrawFont> f, OutParam<IntOS> drawX, OutParam<IntOS> drawY);
		virtual void GetStringBoundRot(UnsafeArray<Int32> pos, Double centX, Double centY, UnsafeArray<const UTF8Char> str, NN<DrawFont> f, Double angleDegree, OutParam<IntOS> drawX, OutParam<IntOS> drawY);
		virtual void CopyBits(IntOS x, IntOS y, UnsafeArray<UInt8> imgPtr, UIntOS bpl, UIntOS width, UIntOS height, Bool upsideDown) const;
		
		virtual Optional<Media::StaticImage> ToStaticImage() const;
		virtual Optional<Media::RasterImage> AsRasterImage();
		virtual UIntOS SavePng(NN<IO::SeekableStream> stm);
		virtual UIntOS SaveGIF(NN<IO::SeekableStream> stm);
		virtual UIntOS SaveJPG(NN<IO::SeekableStream> stm);
	};

	class SVGDocument : public SVGContainer
	{
	private:
		UIntOS width;
		UIntOS height;
		Math::RectArea<IntOS> viewBox;
		Data::ArrayListNN<SVGPen> pens;
		Data::ArrayListNN<SVGBrush> brushes;
		Data::ArrayListNN<SVGFont> fonts;

	public:
		SVGDocument(NN<Media::DrawEngine> refEng);
		virtual ~SVGDocument();

		virtual UIntOS GetWidth() const;
		virtual UIntOS GetHeight() const;

		virtual NN<DrawPen> NewPenARGB(UInt32 color, Double thick, UnsafeArrayOpt<UInt8> pattern, UIntOS nPattern);
		virtual NN<DrawBrush> NewBrushARGB(UInt32 color);
		virtual NN<DrawFont> NewFontPt(Text::CStringNN name, Double ptSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage); // 72 dpi size
		virtual NN<DrawFont> NewFontPx(Text::CStringNN name, Double pxSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage); // Actual size
		virtual NN<DrawFont> CloneFont(NN<DrawFont> f);

		void SetSize(UIntOS width, UIntOS height);
		void SetViewBox(Math::RectArea<IntOS> viewBox);

		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
	};
}
#endif
