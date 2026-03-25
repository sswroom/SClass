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
		Optional<Text::String> inkscapeLabel;
		Optional<SVGContainer> parent;
		SVGLineCap lineCap;
		SVGLineJoin lineJoin;
		Bool spacePreserve;

		void AppendEleAttr(NN<Text::StringBuilderUTF8> sb) const;
	public:
		SVGElement(Optional<SVGContainer> parent) { this->id = nullptr; this->lineCap = SVGLineCap::Default; this->lineJoin = SVGLineJoin::Default; this->parent = parent; this->spacePreserve = false; this->inkscapeLabel = nullptr; }
		virtual ~SVGElement() { OPTSTR_DEL(this->id); OPTSTR_DEL(this->inkscapeLabel); }

		virtual Text::CStringNN GetElementName() const = 0;
		Optional<SVGContainer> GetParent() const { return this->parent; }
		Bool IsSpacePreserve() const;

		void SetID(Text::CStringNN id)
		{
			OPTSTR_DEL(this->id);
			this->id = Text::String::New(id);
		}

		Optional<Text::String> GetID() const
		{
			return this->id;
		}

		void SetInkscapeLabel(NN<Text::String> label)
		{
			OPTSTR_DEL(this->inkscapeLabel);
			this->inkscapeLabel = label->Clone();
		}

		Optional<Text::String> GetInkscapeLabel() const
		{
			return this->inkscapeLabel;
		}

		void SetLineCap(SVGLineCap lineCap)
		{
			this->lineCap = lineCap;
		}

		void SetLineJoin(SVGLineJoin lineJoin)
		{
			this->lineJoin = lineJoin;
		}

		void SetSpacePreserve(Bool spacePreserve)
		{
			this->spacePreserve = spacePreserve;
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
		Bool insensitive;
	public:
		SVGRect(NN<SVGContainer> parent, Math::Coord2DDbl tl, Math::Size2DDbl size, Optional<DrawPen> pen, Optional<DrawBrush> brush);
		virtual ~SVGRect();

		virtual Text::CStringNN GetElementName() const { return CSTR("rect"); }

		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
		void SetStyle(Bool stylePen, Bool styleBrush);
		void SetInsensitive(Bool insensitive) { this->insensitive = insensitive; }
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

	class SVGTextComponent
	{
	protected:
		NN<Text::String> text;
	public:
		virtual ~SVGTextComponent() {}

		NN<Text::String> GetText() const { return this->text; }

		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const = 0;
	};

	class SVGStaticText : public SVGTextComponent
	{
	public:
		SVGStaticText(Text::CStringNN text);
		virtual ~SVGStaticText();

		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
	};

	class SVGTSpan : public SVGTextComponent
	{
	private:
		Math::Coord2DDbl offset;
		Bool stylePen;
		Optional<DrawPen> pen;
		Bool styleBrush;
		Optional<DrawBrush> brush;
		Bool styleFont;
		Optional<SVGFont> font;
		Optional<Text::String> id;
		Optional<Text::String> sodipodiRole;
	public:
		SVGTSpan(Text::CStringNN text);
		virtual ~SVGTSpan();

		void SetOffset(Math::Coord2DDbl offset) { this->offset = offset; }
		void SetPen(Bool inStyle, Optional<DrawPen> pen) { this->stylePen = inStyle; this->pen = pen; }
		void SetBrush(Bool inStyle, Optional<DrawBrush> brush) { this->styleBrush = inStyle; this->brush = brush; }
		void SetFont(Bool inStyle, Optional<SVGFont> font) { this->styleFont = inStyle; this->font = font; }
		void SetID(Text::CStringNN id) { OPTSTR_DEL(this->id); this->id = Text::String::New(id); }
		void SetSodipodiRole(Text::CStringNN sodipodiRole) { OPTSTR_DEL(this->sodipodiRole); this->sodipodiRole = Text::String::New(sodipodiRole); }

		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
	};

	class SVGText : public SVGElement
	{
	private:
		Math::Coord2DDbl tl;
		Data::ArrayListNN<SVGTextComponent> components;
		NN<SVGFont> font;
		Bool styleFont;
		NN<SVGBrush> brush;
		Bool styleBrush;
		Optional<SVGPen> pen;
		Bool stylePen;
		Bool insensitive;
		Optional<Text::String> textAlign;
		Optional<Text::String> textAnchor;
		Optional<Text::String> writingMode;
		Optional<Text::String> direction;
		Optional<Text::String> transform;
		Optional<Text::String> display;
		Optional<Text::String> lineHeight;
		Optional<Text::String> shapeInside;
		Optional<Text::String> whiteSpace;
		Optional<Text::String> strokeDasharray;
		Optional<Text::String> shapePadding;
		Optional<Text::String> mixBlendMode;
		Double inkscapeTransformCenterX;
		Double inkscapeTransformCenterY;
	public:
		SVGText(NN<SVGContainer> parent, Math::Coord2DDbl tl, NN<SVGFont> font, NN<SVGBrush> brush, NN<SVGTextComponent> component);
		virtual ~SVGText();

		virtual Text::CStringNN GetElementName() const { return CSTR("text"); }
		void AddTextComponent(NN<SVGTextComponent> component);

		void SetRotate(Double angleDegreeACW, Math::Coord2DDbl rotateCenter);
		void SetTransform(Text::CStringNN transform) { OPTSTR_DEL(this->transform); this->transform = Text::String::New(transform); }
		void SetPen(Bool inStyle, Optional<SVGPen> pen) { this->stylePen = inStyle; this->pen = pen; }
		void SetBrush(Bool inStyle, NN<SVGBrush> brush) { this->styleBrush = inStyle; this->brush = brush; }
		void SetFont(Bool inStyle, NN<SVGFont> font) { this->styleFont = inStyle; this->font = font; }
		void SetTextAlign(Text::CStringNN textAlign) { OPTSTR_DEL(this->textAlign); this->textAlign = Text::String::New(textAlign); }
		void SetTextAnchor(Text::CStringNN textAnchor) { OPTSTR_DEL(this->textAnchor); this->textAnchor = Text::String::New(textAnchor); }
		void SetWritingMode(Text::CStringNN writingMode) { OPTSTR_DEL(this->writingMode); this->writingMode = Text::String::New(writingMode); }
		void SetDirection(Text::CStringNN direction) { OPTSTR_DEL(this->direction); this->direction = Text::String::New(direction); }
		void SetInsensitive(Bool insensitive) { this->insensitive = insensitive; }
		void SetDisplay(Text::CStringNN display) { OPTSTR_DEL(this->display); this->display = Text::String::New(display); }
		void SetLineHeight(Text::CStringNN lineHeight) { OPTSTR_DEL(this->lineHeight); this->lineHeight = Text::String::New(lineHeight); }
		void SetShapeInside(Text::CStringNN shapeInside) { OPTSTR_DEL(this->shapeInside); this->shapeInside = Text::String::New(shapeInside); }
		void SetWhiteSpace(Text::CStringNN whiteSpace) { OPTSTR_DEL(this->whiteSpace); this->whiteSpace = Text::String::New(whiteSpace); }
		void SetStrokeDasharray(Text::CStringNN strokeDasharray) { OPTSTR_DEL(this->strokeDasharray); this->strokeDasharray = Text::String::New(strokeDasharray); }
		void SetShapePadding(Text::CStringNN shapePadding) { OPTSTR_DEL(this->shapePadding); this->shapePadding = Text::String::New(shapePadding); }
		void SetMixBlendMode(Text::CStringNN mixBlendMode) { OPTSTR_DEL(this->mixBlendMode); this->mixBlendMode = Text::String::New(mixBlendMode); }
		void SetInkscapeTransformCenter(Math::Coord2DDbl center) { this->inkscapeTransformCenterX = center.x; this->inkscapeTransformCenterY = center.y; }
		Optional<SVGPen> GetPen() const { return this->pen; }

		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
	};

	class SVGImage : public SVGElement
	{
	private:
		Math::Coord2DDbl tl;
		Math::Size2DDbl size;
		NN<Text::String> href;
		Bool insensitive;
		Optional<Text::String> preserveAspectRatio;
		Optional<Text::String> style;
	public:
		SVGImage(NN<SVGContainer> parent, Math::Coord2DDbl tl, Math::Size2DDbl size, Text::CStringNN href);
		virtual ~SVGImage();

		virtual Text::CStringNN GetElementName() const { return CSTR("image"); }
		void SetInsensitive(Bool insensitive) { this->insensitive = insensitive; }
		void SetPreserveAspectRatio(Text::CStringNN preserveAspectRatio) { OPTSTR_DEL(this->preserveAspectRatio); this->preserveAspectRatio = Text::String::New(preserveAspectRatio); }
		void SetStyle(Text::CStringNN style) { OPTSTR_DEL(this->style); this->style = Text::String::New(style); }

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

	class SVGTitle : public SVGElement
	{
	private:
		NN<Text::String> title;
	public:
		SVGTitle(NN<SVGContainer> parent, Text::CStringNN title);
		virtual ~SVGTitle();

		virtual Text::CStringNN GetElementName() const { return CSTR("title"); }

		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
	};

	class SVGContainer : public SVGElement, public DrawImage
	{
	protected:
		Data::ArrayListNN<SVGElement> elements;
		NN<SVGDocument> doc;
		NN<Media::DrawEngine> refEng;
		Optional<Text::String> inkscapeGroupmode;
		Optional<Text::String> style;
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
		void SetInkscapeGroupMode(NN<Text::String> groupMode);
		void SetDrawRect(Math::RectAreaDbl drawRect);
		void SetStyle(Text::CStringNN style) { OPTSTR_DEL(this->style); this->style = Text::String::New(style); }
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
		Bool xmlnsXlink;
		Bool xmlnsRdf;
		Bool xmlnsCc;
		Bool xmlnsDc;
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
		void SetXMLNSXLink(Bool xmlnsXlink);

		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;

		void RegisterId(NN<Text::String> id, NN<SVGElement> ele);
		Optional<SVGElement> GetElementById(Text::CStringNN id) const;

		static Optional<SVGDocument> ParseFile(Text::CStringNN fileName, NN<Text::EncodingFactory> encFact, NN<Media::DrawEngine> refEng);
		static Optional<SVGDocument> ParseReader(NN<Text::XMLReader> reader, NN<Media::DrawEngine> refEng);
		static Bool ParseContainer(NN<SVGContainer> container, NN<Text::XMLReader> reader);
		static Bool ParseContainerAttr(NN<SVGContainer> container, NN<Text::XMLReader> reader, Bool allowAnyAttr);
		static Bool ParseLine(NN<SVGContainer> container, NN<Text::XMLReader> reader);
		static Bool ParsePolyline(NN<SVGContainer> container, NN<Text::XMLReader> reader);
		static Bool ParsePolygon(NN<SVGContainer> container, NN<Text::XMLReader> reader);
		static Bool ParseRect(NN<SVGContainer> container, NN<Text::XMLReader> reader);
		static Bool ParseEllipse(NN<SVGContainer> container, NN<Text::XMLReader> reader);
		static Bool ParsePath(NN<SVGContainer> container, NN<Text::XMLReader> reader);
		static Bool ParseText(NN<SVGContainer> container, NN<Text::XMLReader> reader);
		static Optional<SVGTSpan> ParseTSpan(NN<SVGContainer> container, NN<Text::XMLReader> reader, Double parentFontSize);
		static Bool ParseImage(NN<SVGContainer> container, NN<Text::XMLReader> reader);
		static Bool ParseTitle(NN<SVGContainer> container, NN<Text::XMLReader> reader);
		static Bool ParseUnknown(NN<SVGContainer> container, NN<Text::XMLReader> reader);
		static Bool ParseUnknownContainer(NN<SVGContainer> container, NN<Text::XMLReader> reader);
	};
}
#endif
