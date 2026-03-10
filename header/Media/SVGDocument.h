#ifndef _SM_MEDIA_SVGDOCUMENT
#define _SM_MEDIA_SVGDOCUMENT
#include "Data/ArrayListA.hpp"
#include "Data/ArrayListNN.hpp"
#include "Math/Unit/Distance.h"
#include "Media/SVGCore.h"
#include "Text/String.h"
#include "Text/XMLReader.h"

namespace Media
{
	class SVGDocument;
	class SVGContainer;

	class SVGElement
	{
	protected:
		Optional<Text::String> id;
		Optional<SVGContainer> parent;
		SVGLineCap lineCap;
		SVGLineJoin lineJoin;

		void AppendEleAttr(NN<Text::StringBuilderUTF8> sb) const;
	public:
		SVGElement(Optional<SVGContainer> parent) { this->id = nullptr; this->lineCap = SVGLineCap::Default; this->lineJoin = SVGLineJoin::Default; this->parent = parent; }
		virtual ~SVGElement() { OPTSTR_DEL(this->id);}

		virtual Text::CStringNN GetElementName() const = 0;
		Optional<SVGContainer> GetParent() const { return this->parent; }

		void SetID(Text::CStringNN id)
		{
			OPTSTR_DEL(this->id);
			this->id = Text::String::New(id);
		}

		Optional<Text::String> GetID() const
		{
			return this->id;
		}

		void SetLineCap(SVGLineCap lineCap)
		{
			this->lineCap = lineCap;
		}

		void SetLineJoin(SVGLineJoin lineJoin)
		{
			this->lineJoin = lineJoin;
		}

		virtual Bool IsContainer() const { return false; }

		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const = 0;
	};

	class SVGLine : public SVGElement
	{
	private:
		Math::Coord2DDbl pt1;
		Math::Coord2DDbl pt2;
		NN<DrawPen> pen;
	public:
		SVGLine(NN<SVGContainer> parent, Math::Coord2DDbl pt1, Math::Coord2DDbl pt2, NN<DrawPen> pen);
		virtual ~SVGLine();

		virtual Text::CStringNN GetElementName() const { return CSTR("line"); }

		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
	};

	class SVGPolyline : public SVGElement
	{
	private:
		Data::ArrayListA<Math::Coord2DDbl> points;
		NN<DrawPen> pen;
	public:
		SVGPolyline(NN<SVGContainer> parent, NN<DrawPen> pen);
		virtual ~SVGPolyline();

		virtual Text::CStringNN GetElementName() const { return CSTR("polyline"); }

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
		SVGPolygon(NN<SVGContainer> parent, Optional<DrawPen> pen, Optional<DrawBrush> brush);
		virtual ~SVGPolygon();

		virtual Text::CStringNN GetElementName() const { return CSTR("polygon"); }

		void AddPoint(Math::Coord2DDbl pt);
		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
	};

	class SVGRect : public SVGElement
	{
	private:
		Math::Coord2DDbl tl;
		Math::Size2DDbl size;
		Optional<DrawPen> pen;
		Bool stylePen;
		Optional<DrawBrush> brush;
		Bool styleBrush;
	public:
		SVGRect(NN<SVGContainer> parent, Math::Coord2DDbl tl, Math::Size2DDbl size, Optional<DrawPen> pen, Optional<DrawBrush> brush);
		virtual ~SVGRect();

		virtual Text::CStringNN GetElementName() const { return CSTR("rect"); }

		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
		void SetStyle(Bool stylePen, Bool styleBrush);
		Double GetWidth() const { return this->size.x; }
		Double GetHeight() const { return this->size.y; }
		Math::Coord2DDbl GetTL() const { return this->tl; }
		Math::Size2DDbl GetSize() const { return this->size; }
		Math::RectAreaDbl GetRect() const { return Math::RectAreaDbl(this->tl, this->tl + this->size); }
	};

	class SVGEllipse : public SVGElement
	{
	private:
		Math::Coord2DDbl center;
		Math::Size2DDbl radius;
		Optional<DrawPen> pen;
		Optional<DrawBrush> brush;
	public:
		SVGEllipse(NN<SVGContainer> parent, Math::Coord2DDbl center, Math::Size2DDbl radius, Optional<DrawPen> pen, Optional<DrawBrush> brush);
		virtual ~SVGEllipse();

		virtual Text::CStringNN GetElementName() const { return CSTR("ellipse"); }

		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
	};

	class SVGText : public SVGElement
	{
	private:
		Math::Coord2DDbl tl;
		NN<Text::String> txt;
		NN<SVGFont> font;
		NN<SVGBrush> brush;
		Double angleDegreeACW;
		Math::Coord2DDbl rotateCenter;
	public:
		SVGText(NN<SVGContainer> parent, Math::Coord2DDbl tl, Text::CStringNN txt, NN<SVGFont> font, NN<SVGBrush> brush);
		virtual ~SVGText();

		virtual Text::CStringNN GetElementName() const { return CSTR("text"); }

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
		SVGImage(NN<SVGContainer> parent, Math::Coord2DDbl tl, Math::Size2DDbl size, Text::CStringNN href);
		virtual ~SVGImage();

		virtual Text::CStringNN GetElementName() const { return CSTR("image"); }

		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
	};

	class SVGPath : public SVGElement
	{
	private:
		NN<Text::String> d;
		Optional<DrawPen> pen;
		Optional<DrawBrush> brush;
	public:
		SVGPath(NN<SVGContainer> parent, NN<Text::String> d, Optional<DrawPen> pen, Optional<DrawBrush> brush);
		virtual ~SVGPath();

		virtual Text::CStringNN GetElementName() const { return CSTR("path"); }

		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
	};

	class SVGContainer : public SVGElement, public DrawImage
	{
	protected:
		Data::ArrayListNN<SVGElement> elements;
		NN<SVGDocument> doc;
		NN<Media::DrawEngine> refEng;
		Optional<Text::String> inkscapeLabel;
		Optional<Text::String> inkscapeGroupmode;
		Math::RectAreaDbl drawRect;

		void ToInnerString(NN<Text::StringBuilderUTF8> sb) const;
	public:
		SVGContainer(Optional<SVGContainer> parent, NN<Media::DrawEngine> refEng, NN<SVGDocument> doc);
		virtual ~SVGContainer();

		virtual Double GetWidth() const;
		virtual Double GetHeight() const;
		virtual Math::Size2DDbl GetSize() const;
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
		
		virtual Bool PixelSupported() const { return false; }
		virtual UIntOS PixelGetWidth() const { return 0; }
		virtual UIntOS PixelGetHeight() const { return 0; }
		virtual Media::AlphaType PixelGetAlphaType() const { return Media::AT_IGNORE_ALPHA; }
		virtual void PixelSetAlphaType(Media::AlphaType atype) {}
		virtual UInt32 PixelGetBitCount() const { return 0; }
		virtual UnsafeArrayOpt<UInt8> PixelGetBits(OutParam<Bool> revOrder) { return nullptr; }
		virtual void PixelGetBitsEnd(Bool modified) {}
		virtual UIntOS PixelGetBpl() const { return 0; }
		virtual Media::PixelFormat PixelGetFormat() const { return Media::PF_UNKNOWN; }

		virtual Optional<Media::StaticImage> ToStaticImage() const;
		virtual Optional<Media::RasterImage> AsRasterImage();
		virtual UIntOS SavePng(NN<IO::SeekableStream> stm);
		virtual UIntOS SaveGIF(NN<IO::SeekableStream> stm);
		virtual UIntOS SaveJPG(NN<IO::SeekableStream> stm);

		virtual Bool IsContainer() const { return true; }

		void AddElement(NN<SVGElement> ele);
		UIntOS GetElementCount() const;
		Optional<SVGElement> GetElement(UIntOS index) const;
		UIntOS FindElementName(Text::CStringNN name, NN<Data::ArrayListNN<SVGElement>> results) const;
		void ClearElements();
		NN<Media::DrawEngine> GetDrawEngine() const { return this->refEng; }
		NN<SVGDocument> GetDoc() const { return this->doc; }
		void SetInkscapeLabel(NN<Text::String> label);
		void SetInkscapeGroupMode(NN<Text::String> groupMode);
		void SetDrawRect(Math::RectAreaDbl drawRect);
	};

	class SVGDefs : public SVGContainer
	{
	public:
		SVGDefs(NN<SVGContainer> parent, NN<Media::DrawEngine> refEng, NN<SVGDocument> doc);
		virtual ~SVGDefs();

		virtual Text::CStringNN GetElementName() const { return CSTR("defs"); }

		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
	};

	class SVGGroup : public SVGContainer
	{
	private:
		Optional<Text::String> clipPath;
	public:
		SVGGroup(NN<SVGContainer> parent, NN<Media::DrawEngine> refEng, NN<SVGDocument> doc);
		virtual ~SVGGroup();

		virtual Text::CStringNN GetElementName() const { return CSTR("g"); }

		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
		void SetClipPath(Text::CStringNN clipPath);
	};

	class SVGUnknown : public SVGElement
	{
	private:
		NN<Text::String> name;
		Data::ArrayListStringNN attrNames;
		Data::ArrayListStringNN attrValues;
	public:
		SVGUnknown(NN<SVGContainer> parent, Text::CStringNN name);
		virtual ~SVGUnknown();

		virtual Text::CStringNN GetElementName() const;

		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;

		void AddAttr(Text::CStringNN name, Text::CStringNN value);
	};

	class SVGUnknownContainer : public SVGContainer
	{
	private:
		NN<Text::String> name;
		Data::ArrayListStringNN attrNames;
		Data::ArrayListStringNN attrValues;

	public:
		SVGUnknownContainer(NN<SVGContainer> parent, NN<Media::DrawEngine> refEng, NN<SVGDocument> doc, Text::CStringNN name);
		virtual ~SVGUnknownContainer();

		virtual Text::CStringNN GetElementName() const;

		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;

		void AddAttr(Text::CStringNN name, Text::CStringNN value);
	};

	class SVGDocument : public SVGContainer
	{
	private:
		UIntOS width;
		UIntOS height;
		Bool xmlnsSvg;
		Bool xmlnsInkscape;
		Bool xmlnsSodipodi;
		Optional<Text::String> version;
		Optional<Text::String> inkscapeVersion;
		Optional<Text::String> sodipodiDocname;
		Math::Unit::Distance::DistanceUnit unit;
		Math::RectArea<IntOS> viewBox;
		Data::ArrayListNN<SVGPen> pens;
		Data::ArrayListNN<SVGBrush> brushes;
		Data::ArrayListNN<SVGFont> fonts;
		Data::FastStringMapNN<SVGElement> idMap;
		Double hDrawScale;
		Double vDrawScale;

	public:
		SVGDocument(NN<Media::DrawEngine> refEng);
		virtual ~SVGDocument();

		virtual Text::CStringNN GetElementName() const { return CSTR("svg"); }
		virtual Double GetWidth() const;
		virtual Double GetHeight() const;

		virtual NN<DrawPen> NewPenARGB(UInt32 color, Double thick, UnsafeArrayOpt<UInt8> pattern, UIntOS nPattern);
		virtual NN<DrawBrush> NewBrushARGB(UInt32 color);
		virtual NN<DrawFont> NewFontPt(Text::CStringNN name, Double ptSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage); // 72 dpi size
		virtual NN<DrawFont> NewFontPx(Text::CStringNN name, Double pxSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage); // Actual size
		virtual NN<DrawFont> CloneFont(NN<DrawFont> f);

		void SetSize(UIntOS width, UIntOS height, Math::Unit::Distance::DistanceUnit unit);
		void SetViewBox(Math::RectArea<IntOS> viewBox);
		Double GetHDrawScale();
		Double GetVDrawScale();
		Math::Coord2DDbl GetDrawScale();

		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;

		void RegisterId(NN<Text::String> id, NN<SVGElement> ele);
		Optional<SVGElement> GetElementById(Text::CStringNN id) const;

		static Optional<SVGDocument> ParseFile(Text::CStringNN fileName, NN<Text::EncodingFactory> encFact, NN<Media::DrawEngine> refEng);
		static Optional<SVGDocument> ParseReader(NN<Text::XMLReader> reader, NN<Media::DrawEngine> refEng);
		static Bool ParseContainer(NN<SVGContainer> container, NN<Text::XMLReader> reader);
		static Bool ParseContainerAttr(NN<SVGContainer> container, NN<Text::XMLReader> reader);
		static Bool ParseLine(NN<SVGContainer> container, NN<Text::XMLReader> reader);
		static Bool ParsePolyline(NN<SVGContainer> container, NN<Text::XMLReader> reader);
		static Bool ParsePolygon(NN<SVGContainer> container, NN<Text::XMLReader> reader);
		static Bool ParseRect(NN<SVGContainer> container, NN<Text::XMLReader> reader);
		static Bool ParseEllipse(NN<SVGContainer> container, NN<Text::XMLReader> reader);
		static Bool ParsePath(NN<SVGContainer> container, NN<Text::XMLReader> reader);
		static Bool ParseText(NN<SVGContainer> container, NN<Text::XMLReader> reader);
		static Bool ParseImage(NN<SVGContainer> container, NN<Text::XMLReader> reader);
		static Bool ParseUnknown(NN<SVGContainer> container, NN<Text::XMLReader> reader);
		static Bool ParseUnknownContainer(NN<SVGContainer> container, NN<Text::XMLReader> reader);
	};
}
#endif
