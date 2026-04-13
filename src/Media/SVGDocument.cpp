#include "Stdafx.h"
#include "Exporter/PNGExporter.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "Media/StaticImage.h"
#include "Media/SVGDocument.h"
#include "Text/CSSCore.h"
#include "Text/XML.h"
#include "Text/TextBinEnc/Base64Enc.h"

void Media::SVGElement::AppendEleAttr(NN<Text::StringBuilderUTF8> sb, UIntOS level) const
{
	NN<Text::String> s;
	if (this->id.SetTo(s))
	{
		sb->AppendUTF8Char('\n');
		sb->AppendChar(' ', level << 1);
		sb->AppendC(UTF8STRC(" id=\""));
		sb->Append(s);
		sb->AppendUTF8Char('\"');
	}
	if (this->spacePreserve)
	{
		sb->AppendUTF8Char('\n');
		sb->AppendChar(' ', level << 1);
		sb->AppendC(UTF8STRC(" xml:space=\"preserve\""));
	}
	if (this->lineCap != SVGLineCap::Default)
	{
		sb->AppendUTF8Char('\n');
		sb->AppendChar(' ', level << 1);
		sb->AppendC(UTF8STRC(" stroke-linecap=\""));
		switch (this->lineCap)
		{
		case SVGLineCap::Butt:
			sb->AppendC(UTF8STRC("butt"));
			break;
		case SVGLineCap::Round:
			sb->AppendC(UTF8STRC("round"));
			break;
		case SVGLineCap::Square:
			sb->AppendC(UTF8STRC("square"));
			break;
		case SVGLineCap::Default:
		default:
			break;
		}
		sb->AppendUTF8Char('\"');
	}
	if (this->lineJoin != SVGLineJoin::Default)
	{
		sb->AppendUTF8Char('\n');
		sb->AppendChar(' ', level << 1);
		sb->AppendC(UTF8STRC(" stroke-linejoin=\""));
		switch (this->lineJoin)
		{
		case SVGLineJoin::Miter:
			sb->AppendC(UTF8STRC("miter"));
			break;
		case SVGLineJoin::Round:
			sb->AppendC(UTF8STRC("round"));
			break;
		case SVGLineJoin::Bevel:
			sb->AppendC(UTF8STRC("bevel"));
			break;
		case SVGLineJoin::Default:
		default:
			break;
		}
		sb->AppendUTF8Char('\"');
	}
	if (this->inkscapeLabel.SetTo(s))
	{
		sb->AppendUTF8Char('\n');
		sb->AppendChar(' ', level << 1);
		sb->AppendC(UTF8STRC(" inkscape:label="));
		s = Text::XML::ToNewAttrText(s->v);
		sb->Append(s);
		s->Release();
	}
}

void Media::SVGElement::CloneElementAttrs(NN<const SVGElement> ele)
{
	this->id = Text::String::CopyOrNull(ele->id);
	this->inkscapeLabel = Text::String::CopyOrNull(ele->inkscapeLabel);
	this->lineCap = ele->lineCap;
	this->lineJoin = ele->lineJoin;
	this->spacePreserve = ele->spacePreserve;
}

Bool Media::SVGElement::IsSpacePreserve() const
{
	NN<const SVGContainer> nnparent;
	if (this->spacePreserve) return true;
	if (this->parent.SetTo(nnparent))
		return nnparent->IsSpacePreserve();
	return false;
}

Media::SVGLine::SVGLine(NN<const SVGContainer> parent, Math::Coord2DDbl pt1, Math::Coord2DDbl pt2, NN<DrawPen> pen) : SVGElement(parent)
{
	this->pt1 = pt1;
	this->pt2 = pt2;
	this->pen = pen;
}

Media::SVGLine::~SVGLine()
{
}

void Media::SVGLine::DrawElement(Math::Coord2DDbl ofst, Math::Size2DDbl scale, NN<Media::DrawImage> dimg)
{
	////////////////////////////////////////////////
}

void Media::SVGLine::ToString(NN<Text::StringBuilderUTF8> sb, UIntOS level) const
{
	if (!this->IsSpacePreserve())
	{
		sb->AppendChar(' ', level << 1);
	}
	sb->AppendC(UTF8STRC("<line"));
	sb->AppendUTF8Char('\n');
	sb->AppendChar(' ', (level + 1) << 1);
	sb->AppendC(UTF8STRC(" x1=\""));
	sb->AppendDouble(this->pt1.x);
	sb->AppendUTF8Char('\"');
	sb->AppendUTF8Char('\n');
	sb->AppendChar(' ', (level + 1) << 1);
	sb->AppendC(UTF8STRC(" y1=\""));
	sb->AppendDouble(this->pt1.y);
	sb->AppendUTF8Char('\"');
	sb->AppendUTF8Char('\n');
	sb->AppendChar(' ', (level + 1) << 1);
	sb->AppendC(UTF8STRC(" x2=\""));
	sb->AppendDouble(this->pt2.x);
	sb->AppendUTF8Char('\"');
	sb->AppendUTF8Char('\n');
	sb->AppendChar(' ', (level + 1) << 1);
	sb->AppendC(UTF8STRC(" y2=\""));
	sb->AppendDouble(this->pt2.y);
	sb->AppendUTF8Char('\"');
	SVGCore::WriteAttrPen(sb, this->pen);
	this->AppendEleAttr(sb, level + 1);
	sb->AppendC(UTF8STRC(" />"));
}

NN<Media::SVGElement> Media::SVGLine::CloneElement(NN<const SVGContainer> newParent) const
{
	NN<Media::SVGLine> newLine;
	NEW_CLASSNN(newLine, Media::SVGLine(newParent, this->pt1, this->pt2, newParent->GetDoc()->ClonePen(this->pen)));
	newLine->CloneElementAttrs(*this);
	return newLine;
}

Media::SVGPolyline::SVGPolyline(NN<const SVGContainer> parent, NN<DrawPen> pen) : SVGElement(parent)
{
	this->pen = pen;
}

Media::SVGPolyline::~SVGPolyline()
{
}

void Media::SVGPolyline::AddPoint(Math::Coord2DDbl pt)
{
	this->points.Add(pt);
}

void Media::SVGPolyline::DrawElement(Math::Coord2DDbl ofst, Math::Size2DDbl scale, NN<Media::DrawImage> dimg)
{
	////////////////////////////////////////////////
}

void Media::SVGPolyline::ToString(NN<Text::StringBuilderUTF8> sb, UIntOS level) const
{
	if (!this->IsSpacePreserve())
	{
		sb->AppendChar(' ', level << 1);
	}
	sb->AppendC(UTF8STRC("<polyline"));
	sb->AppendUTF8Char('\n');
	sb->AppendChar(' ', (level + 1) << 1);
	sb->AppendC(UTF8STRC(" points=\""));
	if (this->points.GetCount() > 0)
	{
		UIntOS i = 1;
		sb->AppendDouble(this->points.GetItem(0).x);
		sb->AppendUTF8Char(',');
		sb->AppendDouble(this->points.GetItem(0).y);
		while (i < this->points.GetCount())
		{
			sb->AppendUTF8Char(' ');
			sb->AppendDouble(this->points.GetItem(i).x);
			sb->AppendUTF8Char(',');
			sb->AppendDouble(this->points.GetItem(i).y);
			i++;
		}
	}
	sb->AppendUTF8Char('\"');
	SVGCore::WriteAttrPen(sb, this->pen);
	SVGCore::WriteAttrBrush(sb, nullptr);
	this->AppendEleAttr(sb, level + 1);
	sb->AppendC(UTF8STRC(" />"));
}

NN<Media::SVGElement> Media::SVGPolyline::CloneElement(NN<const SVGContainer> newParent) const
{
	NN<Media::SVGPolyline> newPolyline;
	NEW_CLASSNN(newPolyline, Media::SVGPolyline(newParent, newParent->GetDoc()->ClonePen(this->pen)));
	UIntOS i = 0;
	while (i < this->points.GetCount())
	{
		newPolyline->AddPoint(this->points.GetItem(i));
		i++;
	}
	newPolyline->CloneElementAttrs(*this);
	return newPolyline;
}

Media::SVGPolygon::SVGPolygon(NN<const SVGContainer> parent, Optional<DrawPen> pen, Optional<DrawBrush> brush) : SVGElement(parent)
{
	this->pen = pen;
	this->brush = brush;
}

Media::SVGPolygon::~SVGPolygon()
{
}

void Media::SVGPolygon::AddPoint(Math::Coord2DDbl pt)
{
	this->points.Add(pt);
}

void Media::SVGPolygon::DrawElement(Math::Coord2DDbl ofst, Math::Size2DDbl scale, NN<Media::DrawImage> dimg)
{
	////////////////////////////////////////////////
}

void Media::SVGPolygon::ToString(NN<Text::StringBuilderUTF8> sb, UIntOS level) const
{
	if (!this->IsSpacePreserve())
	{
		sb->AppendChar(' ', level << 1);
	}
	sb->AppendC(UTF8STRC("<polygon"));
	sb->AppendUTF8Char('\n');
	sb->AppendChar(' ', (level + 1) << 1);
	sb->AppendC(UTF8STRC(" points=\""));
	if (this->points.GetCount() > 0)
	{
		UIntOS i = 1;
		sb->AppendDouble(this->points.GetItem(0).x);
		sb->AppendUTF8Char(',');
		sb->AppendDouble(this->points.GetItem(0).y);
		while (i < this->points.GetCount())
		{
			sb->AppendUTF8Char(' ');
			sb->AppendDouble(this->points.GetItem(i).x);
			sb->AppendUTF8Char(',');
			sb->AppendDouble(this->points.GetItem(i).y);
			i++;
		}
	}
	sb->AppendUTF8Char('\"');
	SVGCore::WriteAttrPen(sb, this->pen);
	SVGCore::WriteAttrBrush(sb, this->brush);
	this->AppendEleAttr(sb, level + 1);
	sb->AppendC(UTF8STRC(" />"));
}

NN<Media::SVGElement> Media::SVGPolygon::CloneElement(NN<const SVGContainer> newParent) const
{
	NN<Media::SVGPolygon> newPolygon;
	NN<DrawPen> nnPen;
	Optional<DrawPen> newPen = nullptr;
	NN<DrawBrush> nnBrush;
	Optional<DrawBrush> newBrush = nullptr;
	if (this->brush.SetTo(nnBrush))
		newBrush = newParent->GetDoc()->CloneBrush(nnBrush);
	if (this->pen.SetTo(nnPen))
		newPen = newParent->GetDoc()->ClonePen(nnPen);

	NEW_CLASSNN(newPolygon, Media::SVGPolygon(newParent, newPen, newBrush));
	UIntOS i = 0;
	while (i < this->points.GetCount())
	{
		newPolygon->AddPoint(this->points.GetItem(i));
		i++;
	}
	newPolygon->CloneElementAttrs(*this);
	return newPolygon;
}

Media::SVGRect::SVGRect(NN<const SVGContainer> parent, Math::Coord2DDbl tl, Math::Size2DDbl size, Optional<DrawPen> pen, Optional<DrawBrush> brush) : SVGElement(parent)
{
	this->tl = tl;
	this->size = size;
	this->pen = pen;
	this->brush = brush;
	this->stylePen = false;
	this->styleBrush = false;
	this->insensitive = false;
}

Media::SVGRect::~SVGRect()
{
}

void Media::SVGRect::DrawElement(Math::Coord2DDbl ofst, Math::Size2DDbl scale, NN<Media::DrawImage> dimg)
{
	////////////////////////////////////////////////
}

void Media::SVGRect::ToString(NN<Text::StringBuilderUTF8> sb, UIntOS level) const
{
	if (!this->IsSpacePreserve())
	{
		sb->AppendChar(' ', level << 1);
	}
	sb->AppendC(UTF8STRC("<rect"));
	sb->AppendUTF8Char('\n');
	sb->AppendChar(' ', (level + 1) << 1);
	sb->AppendC(UTF8STRC(" x=\""));
	sb->AppendDouble(this->tl.x);
	sb->AppendUTF8Char('\"');
	sb->AppendUTF8Char('\n');
	sb->AppendChar(' ', (level + 1) << 1);
	sb->AppendC(UTF8STRC(" y=\""));
	sb->AppendDouble(this->tl.y);
	sb->AppendUTF8Char('\"');
	sb->AppendUTF8Char('\n');
	sb->AppendChar(' ', (level + 1) << 1);
	sb->AppendC(UTF8STRC(" width=\""));
	sb->AppendDouble(this->size.x);
	sb->AppendUTF8Char('\"');
	sb->AppendUTF8Char('\n');
	sb->AppendChar(' ', (level + 1) << 1);
	sb->AppendC(UTF8STRC(" height=\""));
	sb->AppendDouble(this->size.y);
	sb->AppendUTF8Char('\"');
	if (this->insensitive)
	{
		sb->AppendUTF8Char('\n');
		sb->AppendChar(' ', (level + 1) << 1);
		sb->AppendC(UTF8STRC(" sodipodi:insensitive=\"true\""));
	}
	if (this->stylePen || this->styleBrush)
	{
		sb->AppendUTF8Char('\n');
		sb->AppendChar(' ', (level + 1) << 1);
		sb->AppendC(UTF8STRC(" style=\""));
		Bool hasOtherStyle = false;
		if (this->stylePen)
		{
			hasOtherStyle = SVGCore::WriteStylePen(sb, this->pen, hasOtherStyle) || hasOtherStyle;
		}
		if (this->styleBrush)
		{
			hasOtherStyle = SVGCore::WriteStyleBrush(sb, this->brush, hasOtherStyle) || hasOtherStyle;
		}
		sb->AppendUTF8Char('\"');
	}
	else
	{
		SVGCore::WriteAttrPen(sb, this->pen);
		SVGCore::WriteAttrBrush(sb, this->brush);
	}
	this->AppendEleAttr(sb, level + 1);
	sb->AppendC(UTF8STRC(" />"));
}

NN<Media::SVGElement> Media::SVGRect::CloneElement(NN<const SVGContainer> newParent) const
{
	NN<Media::SVGRect> newRect;
	NN<DrawPen> nnPen;
	Optional<DrawPen> newPen = nullptr;
	NN<DrawBrush> nnBrush;
	Optional<DrawBrush> newBrush = nullptr;
	if (this->brush.SetTo(nnBrush))
		newBrush = newParent->GetDoc()->CloneBrush(nnBrush);
	if (this->pen.SetTo(nnPen))
		newPen = newParent->GetDoc()->ClonePen(nnPen);

	NEW_CLASSNN(newRect, Media::SVGRect(newParent, this->tl, this->size, newPen, newBrush));
	newRect->styleBrush = this->styleBrush;
	newRect->stylePen = this->stylePen;
	newRect->insensitive = this->insensitive;
	newRect->CloneElementAttrs(*this);
	return newRect;
}

void Media::SVGRect::SetStyle(Bool stylePen, Bool styleBrush)
{
	this->stylePen = stylePen;
	this->styleBrush = styleBrush;
}

Media::SVGEllipse::SVGEllipse(NN<const SVGContainer> parent, Math::Coord2DDbl center, Math::Size2DDbl radius, Optional<DrawPen> pen, Optional<DrawBrush> brush) : SVGElement(parent)
{
	this->center = center;
	this->radius = radius;
	this->pen = pen;
	this->brush = brush;
}

Media::SVGEllipse::~SVGEllipse()
{
}

void Media::SVGEllipse::DrawElement(Math::Coord2DDbl ofst, Math::Size2DDbl scale, NN<Media::DrawImage> dimg)
{
	//////////////////////////
}

void Media::SVGEllipse::ToString(NN<Text::StringBuilderUTF8> sb, UIntOS level) const
{
	if (!this->IsSpacePreserve())
	{
		sb->AppendChar(' ', level << 1);
	}
	sb->AppendC(UTF8STRC("<ellipse"));
	sb->AppendUTF8Char('\n');
	sb->AppendChar(' ', (level + 1) << 1);
	sb->AppendC(UTF8STRC(" cx=\""));
	sb->AppendDouble(this->center.x);
	sb->AppendUTF8Char('\"');
	sb->AppendUTF8Char('\n');
	sb->AppendChar(' ', (level + 1) << 1);
	sb->AppendC(UTF8STRC(" cy=\""));
	sb->AppendDouble(this->center.y);
	sb->AppendUTF8Char('\"');
	sb->AppendUTF8Char('\n');
	sb->AppendChar(' ', (level + 1) << 1);
	sb->AppendC(UTF8STRC(" rx=\""));
	sb->AppendDouble(this->radius.x);
	sb->AppendUTF8Char('\"');
	sb->AppendUTF8Char('\n');
	sb->AppendChar(' ', (level + 1) << 1);
	sb->AppendC(UTF8STRC(" ry=\""));
	sb->AppendDouble(this->radius.y);
	sb->AppendUTF8Char('\"');
	SVGCore::WriteAttrPen(sb, this->pen);
	SVGCore::WriteAttrBrush(sb, this->brush);
	this->AppendEleAttr(sb, level + 1);
	sb->AppendC(UTF8STRC(" />"));
}

NN<Media::SVGElement> Media::SVGEllipse::CloneElement(NN<const SVGContainer> newParent) const
{
	NN<Media::SVGEllipse> newEllipse;
	NN<DrawPen> nnPen;
	Optional<DrawPen> newPen = nullptr;
	NN<DrawBrush> nnBrush;
	Optional<DrawBrush> newBrush = nullptr;
	if (this->brush.SetTo(nnBrush))
		newBrush = newParent->GetDoc()->CloneBrush(nnBrush);
	if (this->pen.SetTo(nnPen))
		newPen = newParent->GetDoc()->ClonePen(nnPen);

	NEW_CLASSNN(newEllipse, Media::SVGEllipse(newParent, this->center, this->radius, newPen, newBrush));
	newEllipse->CloneElementAttrs(*this);
	return newEllipse;
}

Media::SVGStaticText::SVGStaticText(Text::CStringNN text) : SVGTextComponent()
{
	this->text = Text::String::New(text);
}

Media::SVGStaticText::~SVGStaticText()
{
	this->text->Release();
}

void Media::SVGStaticText::ToString(NN<Text::StringBuilderUTF8> sb, UIntOS level) const
{
	NN<Text::String> s = Text::XML::ToNewXMLText(this->text->v);
	sb->Append(s);
	s->Release();
}

NN<Media::SVGTextComponent> Media::SVGStaticText::Clone(NN<const SVGContainer> newParent) const
{
	NN<Media::SVGStaticText> newText;
	NEW_CLASSNN(newText, Media::SVGStaticText(this->text->ToCString()));
	return newText;
}

Media::SVGTSpan::SVGTSpan(Text::CStringNN text) : SVGTextComponent()
{
	this->text = Text::String::New(text);
	this->offset = Math::Coord2DDbl(NAN, NAN);
	this->stylePen = false;
	this->styleBrush = false;
	this->styleFont = false;
	this->pen = nullptr;
	this->brush = nullptr;
	this->font = nullptr;
	this->id = nullptr;
	this->sodipodiRole = nullptr;
}

Media::SVGTSpan::~SVGTSpan()
{
	OPTSTR_DEL(this->id);
	OPTSTR_DEL(this->sodipodiRole);
	this->text->Release();
}

void Media::SVGTSpan::ToString(NN<Text::StringBuilderUTF8> sb, UIntOS level) const
{
	sb->AppendC(UTF8STRC("<tspan"));
	NN<Text::String> s;
	if (!Math::IsNAN(this->offset.x) && !Math::IsNAN(this->offset.y))
	{
		sb->AppendUTF8Char('\n');
		sb->AppendChar(' ', (level + 1) << 1);
		sb->AppendC(UTF8STRC(" x=\""));
		sb->AppendDouble(this->offset.x);
		sb->AppendUTF8Char('\"');
		sb->AppendUTF8Char('\n');
		sb->AppendChar(' ', (level + 1) << 1);
		sb->AppendC(UTF8STRC(" y=\""));
		sb->AppendDouble(this->offset.y);
		sb->AppendUTF8Char('\"');
	}
	if (this->id.SetTo(s))
	{
		sb->AppendUTF8Char('\n');
		sb->AppendChar(' ', (level + 1) << 1);
		sb->AppendC(UTF8STRC(" id="));
		s = Text::XML::ToNewAttrText(s->v);
		sb->Append(s);
		s->Release();
	}
	if (this->sodipodiRole.SetTo(s))
	{
		sb->AppendC(UTF8STRC(" sodipodi:role="));
		sb->AppendUTF8Char('\n');
		sb->AppendChar(' ', (level + 1) << 1);
		s = Text::XML::ToNewAttrText(s->v);
		sb->Append(s);
		s->Release();
	}

	NN<SVGFont> font;
	if ((this->stylePen && this->pen.NotNull()) || (this->styleBrush && this->brush.NotNull()) || (this->styleFont && this->font.NotNull()))
	{
		sb->AppendUTF8Char('\n');
		sb->AppendChar(' ', (level + 1) << 1);
		sb->AppendC(UTF8STRC(" style=\""));
		Bool hasOtherStyle = false;
		if (this->stylePen && this->pen.NotNull())
		{
			hasOtherStyle = SVGCore::WriteStylePen(sb, this->pen, hasOtherStyle) || hasOtherStyle;
		}
		if (this->styleBrush && this->brush.NotNull())
		{
			hasOtherStyle = SVGCore::WriteStyleBrush(sb, this->brush, hasOtherStyle) || hasOtherStyle;
		}
		if (this->styleFont && this->font.SetTo(font))
		{
			hasOtherStyle = SVGCore::WriteStyleFont(sb, font, hasOtherStyle) || hasOtherStyle;
		}
		sb->AppendUTF8Char('\"');
	}
	if (!this->stylePen && this->pen.NotNull())
	{
		SVGCore::WriteAttrPen(sb, this->pen);
	}
	if (!this->styleBrush && this->brush.NotNull())
	{
		SVGCore::WriteAttrBrush(sb, this->brush);
	}
	if (!this->styleFont && this->font.SetTo(font))
	{
		SVGCore::WriteAttrFont(sb, font);
	}
	sb->AppendC(UTF8STRC(" >"));
	s = Text::XML::ToNewXMLText(this->text->v);
	sb->Append(s);
	s->Release();
	sb->AppendC(UTF8STRC("</tspan>"));
}

NN<Media::SVGTextComponent> Media::SVGTSpan::Clone(NN<const SVGContainer> newParent) const
{
	NN<Media::SVGTSpan> newTSpan;
	NEW_CLASSNN(newTSpan, Media::SVGTSpan(this->text->ToCString()));
	newTSpan->offset = this->offset;
	newTSpan->stylePen = this->stylePen;
	newTSpan->styleBrush = this->styleBrush;
	newTSpan->styleFont = this->styleFont;
	NN<DrawPen> nnPen;
	if (this->pen.SetTo(nnPen))
		newTSpan->pen = newParent->GetDoc()->ClonePen(nnPen);
	NN<DrawBrush> nnBrush;
	if (this->brush.SetTo(nnBrush))
		newTSpan->brush = newParent->GetDoc()->CloneBrush(nnBrush);
	NN<SVGFont> nnFont;
	if (this->font.SetTo(nnFont))
		newTSpan->font = NN<SVGFont>::ConvertFrom(newParent->GetDoc()->CloneFont(nnFont));
	newTSpan->id = Text::String::CopyOrNull(this->id);
	newTSpan->sodipodiRole = Text::String::CopyOrNull(this->sodipodiRole);
	return newTSpan;
}

Media::SVGText::SVGText(NN<const SVGContainer> parent, Math::Coord2DDbl tl, NN<SVGFont> font, NN<SVGBrush> brush, NN<SVGTextComponent> component) : SVGElement(parent)
{
	this->tl = tl;
	this->font = font;
	this->brush = brush;
	this->pen = nullptr;
	this->components.Add(component);
	this->styleFont = false;
	this->styleBrush = false;
	this->stylePen = false;
	this->insensitive = false;
	this->textAlign = nullptr;
	this->textAnchor = nullptr;
	this->writingMode = nullptr;
	this->direction = nullptr;
	this->transform = nullptr;
	this->display = nullptr;
	this->lineHeight = nullptr;
	this->shapeInside = nullptr;
	this->whiteSpace = nullptr;
	this->strokeDasharray = nullptr;
	this->shapePadding = nullptr;
	this->mixBlendMode = nullptr;
	this->inkscapeTransformCenterX = NAN;
	this->inkscapeTransformCenterY = NAN;
}

Media::SVGText::~SVGText()
{
	this->components.DeleteAll();
	OPTSTR_DEL(this->textAlign);
	OPTSTR_DEL(this->textAnchor);
	OPTSTR_DEL(this->writingMode);
	OPTSTR_DEL(this->direction);
	OPTSTR_DEL(this->transform);
	OPTSTR_DEL(this->display);
	OPTSTR_DEL(this->lineHeight);
	OPTSTR_DEL(this->shapeInside);
	OPTSTR_DEL(this->whiteSpace);
	OPTSTR_DEL(this->strokeDasharray);
	OPTSTR_DEL(this->shapePadding);
	OPTSTR_DEL(this->mixBlendMode);
}

void Media::SVGText::AddTextComponent(NN<SVGTextComponent> component)
{
	this->components.Add(component);
}

void Media::SVGText::SetText(Text::CStringNN text)
{
	if (this->components.GetCount() == 1)
	{
		NN<SVGTextComponent> comp = this->components.GetItemNoCheck(0);
		comp->SetText(text);
	}
	else
	{
		this->components.DeleteAll();
		NN<SVGStaticText> newText;
		NEW_CLASSNN(newText, SVGStaticText(text));
		this->components.Add(newText);
	}
}

void Media::SVGText::SetRotate(Double angleDegreeACW, Math::Coord2DDbl rotateCenter)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("rotate("));
	sb.AppendDouble(angleDegreeACW);
	sb.AppendC(UTF8STRC(","));
	sb.AppendDouble(rotateCenter.x);
	sb.AppendC(UTF8STRC(","));
	sb.AppendDouble(rotateCenter.y);
	sb.AppendC(UTF8STRC(")"));
	this->SetTransform(sb.ToCString());
}

void Media::SVGText::DrawElement(Math::Coord2DDbl ofst, Math::Size2DDbl scale, NN<Media::DrawImage> dimg)
{
	////////////////////////////////////////////////
}

void Media::SVGText::ToString(NN<Text::StringBuilderUTF8> sb, UIntOS level) const
{
	NN<Text::String> s;
	if (!this->IsSpacePreserve())
	{
		sb->AppendChar(' ', level << 1);
	}
	sb->AppendC(UTF8STRC("<text"));
	sb->AppendUTF8Char('\n');
	sb->AppendChar(' ', (level + 1) << 1);
	sb->AppendC(UTF8STRC(" x=\""));
	sb->AppendDouble(this->tl.x);
	sb->AppendUTF8Char('\"');
	sb->AppendUTF8Char('\n');
	sb->AppendChar(' ', (level + 1) << 1);
	sb->AppendC(UTF8STRC(" y=\""));
	sb->AppendDouble(this->tl.y);
	sb->AppendUTF8Char('\"');
	if (this->transform.SetTo(s))
	{
		sb->AppendUTF8Char('\n');
		sb->AppendChar(' ', (level + 1) << 1);
		sb->AppendC(UTF8STRC(" transform=\""));
		sb->Append(s);
		sb->AppendUTF8Char('\"');
	}
	if (this->insensitive)
	{
		sb->AppendUTF8Char('\n');
		sb->AppendChar(' ', (level + 1) << 1);
		sb->AppendC(UTF8STRC(" sodipodi:insensitive=\"true\""));
	}
	if (!this->styleFont)
	{
		SVGCore::WriteAttrFont(sb, this->font);
	}
	if (!this->styleBrush)
	{
		SVGCore::WriteAttrBrush(sb, this->brush);
	}
	if (!this->stylePen && this->pen.NotNull())
	{
		SVGCore::WriteAttrPen(sb, this->pen);
	}
	if (!Math::IsNAN(this->inkscapeTransformCenterX))
	{
		sb->AppendUTF8Char('\n');
		sb->AppendChar(' ', (level + 1) << 1);
		sb->AppendC(UTF8STRC(" inkscape:transform-center-x=\""));
		sb->AppendDouble(this->inkscapeTransformCenterX);
		sb->AppendUTF8Char('\"');
	}
	if (!Math::IsNAN(this->inkscapeTransformCenterY))
	{
		sb->AppendUTF8Char('\n');
		sb->AppendChar(' ', (level + 1) << 1);
		sb->AppendC(UTF8STRC(" inkscape:transform-center-y=\""));
		sb->AppendDouble(this->inkscapeTransformCenterY);
		sb->AppendUTF8Char('\"');
	}
	this->AppendEleAttr(sb, level + 1);
	if (this->styleFont || this->styleBrush || (this->stylePen && this->pen.NotNull()))
	{
		NN<Text::String> s;
		sb->AppendUTF8Char('\n');
		sb->AppendChar(' ', (level + 1) << 1);
		sb->AppendC(UTF8STRC(" style=\""));
		Bool hasOtherStyle = false;
		if (this->styleFont)
		{
			hasOtherStyle = SVGCore::WriteStyleFont(sb, this->font, hasOtherStyle) || hasOtherStyle;
		}
		if (this->textAlign.SetTo(s))
		{
			if (hasOtherStyle)
			{
				sb->AppendC(UTF8STRC(";"));
			}
			sb->AppendC(UTF8STRC("text-align:"));
			sb->Append(s);
			hasOtherStyle = true;
		}
		if (this->writingMode.SetTo(s))
		{
			if (hasOtherStyle)
			{
				sb->AppendC(UTF8STRC(";"));
			}
			sb->AppendC(UTF8STRC("writing-mode:"));
			sb->Append(s);
			hasOtherStyle = true;
		}
		if (this->direction.SetTo(s))
		{
			if (hasOtherStyle)
			{
				sb->AppendC(UTF8STRC(";"));
			}
			sb->AppendC(UTF8STRC("direction:"));
			sb->Append(s);
			hasOtherStyle = true;
		}
		if (this->textAnchor.SetTo(s))
		{
			if (hasOtherStyle)
			{
				sb->AppendC(UTF8STRC(";"));
			}
			sb->AppendC(UTF8STRC("text-anchor:"));
			sb->Append(s);
			hasOtherStyle = true;
		}
		if (this->display.SetTo(s))
		{
			if (hasOtherStyle)
			{
				sb->AppendC(UTF8STRC(";"));
			}
			sb->AppendC(UTF8STRC("display:"));
			sb->Append(s);
			hasOtherStyle = true;
		}
		if (this->styleBrush)
		{
			hasOtherStyle = SVGCore::WriteStyleBrush(sb, this->brush, hasOtherStyle) || hasOtherStyle;
		}
		if (this->stylePen && this->pen.NotNull())
		{
			hasOtherStyle = SVGCore::WriteStylePen(sb, this->pen, hasOtherStyle) || hasOtherStyle;
		}
		sb->AppendUTF8Char('\"');
	}
	sb->AppendC(UTF8STRC(" >"));
	UIntOS i = 0;
	UIntOS j = this->components.GetCount();
	while (i < j)
	{
		this->components.GetItemNoCheck(i)->ToString(sb, level + 1);
		i++;
	}
	sb->AppendC(UTF8STRC("</text>"));
}

NN<Media::SVGElement> Media::SVGText::CloneElement(NN<const SVGContainer> newParent) const
{
	NN<SVGDocument> newDoc = newParent->GetDoc();
	NN<SVGText> newText;
	NN<SVGPen> nnPen;
	Optional<SVGPen> newPen = nullptr;
	NN<SVGBrush> newBrush = NN<SVGBrush>::ConvertFrom(newDoc->CloneBrush(this->brush));
	NN<SVGFont> newFont = NN<SVGFont>::ConvertFrom(newDoc->CloneFont(this->font));
	if (this->pen.SetTo(nnPen))
		newPen = NN<SVGPen>::ConvertFrom(newParent->GetDoc()->ClonePen(nnPen));
	NEW_CLASSNN(newText, Media::SVGText(newParent, this->tl, newFont, newBrush, this->components.GetItemNoCheck(0)->Clone(newParent)));
	UIntOS i = 1;
	while (i < this->components.GetCount())
	{
		newText->AddTextComponent(this->components.GetItemNoCheck(i)->Clone(newParent));
		i++;
	}
	newText->styleFont = this->styleFont;
	newText->styleBrush = this->styleBrush;
	newText->stylePen = this->stylePen;
	newText->insensitive = this->insensitive;
	newText->textAlign = Text::String::CopyOrNull(this->textAlign);
	newText->textAnchor = Text::String::CopyOrNull(this->textAnchor);
	newText->writingMode = Text::String::CopyOrNull(this->writingMode);
	newText->direction = Text::String::CopyOrNull(this->direction);
	newText->transform = Text::String::CopyOrNull(this->transform);
	newText->display = Text::String::CopyOrNull(this->display);
	newText->lineHeight = Text::String::CopyOrNull(this->lineHeight);
	newText->shapeInside = Text::String::CopyOrNull(this->shapeInside);
	newText->whiteSpace = Text::String::CopyOrNull(this->whiteSpace);
	newText->strokeDasharray = Text::String::CopyOrNull(this->strokeDasharray);
	newText->shapePadding = Text::String::CopyOrNull(this->shapePadding);
	newText->mixBlendMode = Text::String::CopyOrNull(this->mixBlendMode);
	newText->inkscapeTransformCenterX = this->inkscapeTransformCenterX;
	newText->inkscapeTransformCenterY = this->inkscapeTransformCenterY;
	newText->CloneElementAttrs(*this);
	return newText;
}

Media::SVGImage::SVGImage(NN<const SVGContainer> parent, Math::Coord2DDbl tl, Math::Size2DDbl size, Text::CStringNN href) : SVGElement(parent)
{
	this->tl = tl;
	this->size = size;
	this->href = Text::String::New(href);
	this->insensitive = false;
	this->preserveAspectRatio = nullptr;
	this->style = nullptr;
}

Media::SVGImage::~SVGImage()
{
	this->href->Release();
	OPTSTR_DEL(this->preserveAspectRatio);
	OPTSTR_DEL(this->style);
}

void Media::SVGImage::DrawElement(Math::Coord2DDbl ofst, Math::Size2DDbl scale, NN<Media::DrawImage> dimg)
{
	//////////////////////////
}

void Media::SVGImage::ToString(NN<Text::StringBuilderUTF8> sb, UIntOS level) const
{
	if (!this->IsSpacePreserve())
	{
		sb->AppendChar(' ', level << 1);
	}
	sb->AppendC(UTF8STRC("<image"));
	sb->AppendUTF8Char('\n');
	sb->AppendChar(' ', (level + 1) << 1);
	sb->AppendC(UTF8STRC(" x=\""));
	sb->AppendDouble(this->tl.x);
	sb->AppendUTF8Char('\"');
	sb->AppendUTF8Char('\n');
	sb->AppendChar(' ', (level + 1) << 1);
	sb->AppendC(UTF8STRC(" y=\""));
	sb->AppendDouble(this->tl.y);
	sb->AppendUTF8Char('\"');
	sb->AppendUTF8Char('\n');
	sb->AppendChar(' ', (level + 1) << 1);
	sb->AppendC(UTF8STRC(" width=\""));
	sb->AppendDouble(this->size.x);
	sb->AppendUTF8Char('\"');
	sb->AppendUTF8Char('\n');
	sb->AppendChar(' ', (level + 1) << 1);
	sb->AppendC(UTF8STRC(" height=\""));
	sb->AppendDouble(this->size.y);
	sb->AppendUTF8Char('\"');
	sb->AppendUTF8Char('\n');
	sb->AppendChar(' ', (level + 1) << 1);
	sb->AppendC(UTF8STRC(" xlink:href="));
	NN<Text::String> s = Text::XML::ToNewAttrText(this->href->v);
	sb->Append(s);
	s->Release();
	if (this->insensitive)
	{
		sb->AppendUTF8Char('\n');
		sb->AppendChar(' ', (level + 1) << 1);
		sb->AppendC(UTF8STRC(" sodipodi:insensitive=\"true\""));
	}
	if (this->preserveAspectRatio.SetTo(s))
	{
		sb->AppendUTF8Char('\n');
		sb->AppendChar(' ', (level + 1) << 1);
		sb->AppendC(UTF8STRC(" preserveAspectRatio=\""));
		sb->Append(s);
		sb->AppendUTF8Char('\"');
	}
	if (this->style.SetTo(s))
	{
		sb->AppendUTF8Char('\n');
		sb->AppendChar(' ', (level + 1) << 1);
		sb->AppendC(UTF8STRC(" style=\""));
		sb->Append(s);
		sb->AppendUTF8Char('\"');
	}
	this->AppendEleAttr(sb, level + 1);
	sb->AppendC(UTF8STRC(" />"));
}

NN<Media::SVGElement> Media::SVGImage::CloneElement(NN<const SVGContainer> newParent) const
{
	NN<Media::SVGImage> newImage;
	NEW_CLASSNN(newImage, Media::SVGImage(newParent, this->tl, this->size, this->href->ToCString()));
	newImage->insensitive = this->insensitive;
	newImage->preserveAspectRatio = Text::String::CopyOrNull(this->preserveAspectRatio);
	newImage->style = Text::String::CopyOrNull(this->style);
	newImage->CloneElementAttrs(*this);
	return newImage;
}

Media::SVGPath::SVGPath(NN<const SVGContainer> parent, NN<Text::String> d, Optional<DrawPen> pen, Optional<DrawBrush> brush) : SVGElement(parent)
{
	this->d = d->Clone();
	this->pen = pen;
	this->brush = brush;
}

Media::SVGPath::~SVGPath()
{
	this->d->Release();
}

void Media::SVGPath::DrawElement(Math::Coord2DDbl ofst, Math::Size2DDbl scale, NN<Media::DrawImage> dimg)
{
	////////////////////////////////////////////////
}

void Media::SVGPath::ToString(NN<Text::StringBuilderUTF8> sb, UIntOS level) const
{
	if (!this->IsSpacePreserve())
	{
		sb->AppendChar(' ', level << 1);
	}
	sb->AppendC(UTF8STRC("<path"));
	sb->AppendUTF8Char('\n');
	sb->AppendChar(' ', (level + 1) << 1);
	sb->AppendC(UTF8STRC(" d="));
	NN<Text::String> s = Text::XML::ToNewAttrText(this->d->v);
	sb->Append(s);
	s->Release();
	SVGCore::WriteAttrPen(sb, this->pen);
	SVGCore::WriteAttrBrush(sb, this->brush);
	this->AppendEleAttr(sb, level + 1);
	sb->AppendC(UTF8STRC(" />"));
}

NN<Media::SVGElement> Media::SVGPath::CloneElement(NN<const SVGContainer> newParent) const
{
	NN<Media::SVGPath> newPath;
	NN<DrawBrush> nnBrush;
	Optional<DrawBrush> newBrush = nullptr;
	NN<DrawPen> nnPen;
	Optional<DrawPen> newPen = nullptr;
	if (this->brush.SetTo(nnBrush))
		newBrush = newParent->GetDoc()->CloneBrush(nnBrush);
	if (this->pen.SetTo(nnPen))
		newPen = newParent->GetDoc()->ClonePen(nnPen);
	NEW_CLASSNN(newPath, Media::SVGPath(newParent, this->d->Clone(), newPen, newBrush));
	newPath->CloneElementAttrs(*this);
	return newPath;
}

Media::SVGTitle::SVGTitle(NN<const SVGContainer> parent, Text::CStringNN eleName, Text::CStringNN title) : SVGElement(parent)
{
	this->eleName = Text::String::New(eleName);
	this->title = Text::String::New(title);
}

Media::SVGTitle::~SVGTitle()
{
	this->eleName->Release();
	this->title->Release();
}

void Media::SVGTitle::DrawElement(Math::Coord2DDbl ofst, Math::Size2DDbl scale, NN<Media::DrawImage> dimg)
{
}

void Media::SVGTitle::ToString(NN<Text::StringBuilderUTF8> sb, UIntOS level) const
{
	if (!this->IsSpacePreserve())
	{
		sb->AppendChar(' ', level << 1);
	}
	sb->AppendUTF8Char('<');
	sb->Append(this->eleName);
	sb->AppendUTF8Char('>');
	NN<Text::String> s = Text::XML::ToNewXMLText(this->title->v);
	sb->Append(s);
	s->Release();
	sb->AppendC(UTF8STRC("</"));
	sb->Append(this->eleName);
	sb->AppendUTF8Char('>');
	if (!this->IsSpacePreserve())
	{
		sb->AppendUTF8Char('\n');
	}
}

NN<Media::SVGElement> Media::SVGTitle::CloneElement(NN<const SVGContainer> newParent) const
{
	NN<Media::SVGTitle> newTitle;
	NEW_CLASSNN(newTitle, Media::SVGTitle(newParent, this->eleName->ToCString(), this->title->ToCString()));
	newTitle->CloneElementAttrs(*this);
	return newTitle;
}

void Media::SVGContainer::ToInnerString(NN<Text::StringBuilderUTF8> sb, UIntOS level) const
{
	Bool spacePreserve = this->IsSpacePreserve();
	UIntOS i = 0;
	UIntOS j = this->elements.GetCount();
	while (i < j)
	{
		this->elements.GetItemNoCheck(i)->ToString(sb, level);
		if (!spacePreserve)
		{
			sb->AppendUTF8Char('\n');
		}
		i++;
	}
}

void Media::SVGContainer::CloneElements(NN<const SVGContainer> fromContainer)
{
	UIntOS i = 0;
	UIntOS j = fromContainer->elements.GetCount();
	while (i < j)
	{
		this->elements.Add(fromContainer->elements.GetItemNoCheck(i)->CloneElement(*this));
		i++;
	}
}

void Media::SVGContainer::CloneContainerConts(NN<const SVGContainer> fromContainer)
{
	this->drawRect = fromContainer->drawRect;
	this->inkscapeGroupmode = Text::String::CopyOrNull(fromContainer->inkscapeGroupmode);
	this->style = Text::String::CopyOrNull(fromContainer->style);
	this->CloneElementAttrs(fromContainer);
	this->CloneElements(fromContainer);
}

Media::SVGContainer::SVGContainer(Optional<const SVGContainer> parent, NN<Media::DrawEngine> refEng, NN<SVGDocument> doc) : SVGElement(parent)
{
	this->refEng = refEng;
	this->doc = doc;
	this->inkscapeLabel = nullptr;
	this->inkscapeGroupmode = nullptr;
	this->drawRect = Math::RectAreaDbl(0, 0, 0, 0);
	this->style = nullptr;
}

Media::SVGContainer::~SVGContainer()
{
	this->elements.DeleteAll();
	OPTSTR_DEL(this->inkscapeLabel);
	OPTSTR_DEL(this->inkscapeGroupmode);
	OPTSTR_DEL(this->style);
}

Double Media::SVGContainer::GetWidth() const
{
	Double w = this->drawRect.GetWidth();
	if (w > 0)
	{
		return w * this->doc->GetHDrawScale();
	}
	return this->doc->GetWidth();
}

Double Media::SVGContainer::GetHeight() const
{
	Double h = this->drawRect.GetHeight();
	if (h > 0)
	{
		return h * this->doc->GetVDrawScale();
	}
	return this->doc->GetHeight();
}

Math::Size2DDbl Media::SVGContainer::GetSize() const
{
	return Math::Size2DDbl(this->GetWidth(), this->GetHeight());
}

UInt32 Media::SVGContainer::GetBitCount() const
{
	return 32;
}

NN<const Media::ColorProfile> Media::SVGContainer::GetColorProfile() const
{
	return this->doc->GetColorProfile();
}

void Media::SVGContainer::SetColorProfile(NN<const Media::ColorProfile> color)
{
	//this->doc->SetColorProfile(color);
}
Media::AlphaType Media::SVGContainer::GetAlphaType() const
{
	return Media::AT_ALPHA;
}
void Media::SVGContainer::SetAlphaType(Media::AlphaType atype)
{
}

Double Media::SVGContainer::GetHDPI() const
{
	return 96.0;
}

Double Media::SVGContainer::GetVDPI() const
{
	return 96.0;
}
void Media::SVGContainer::SetHDPI(Double dpi)
{
}

void Media::SVGContainer::SetVDPI(Double dpi)
{
}

UnsafeArrayOpt<UInt8> Media::SVGContainer::GetImgBits(OutParam<Bool> revOrder)
{
	return nullptr;
}
void Media::SVGContainer::GetImgBitsEnd(Bool modified)
{
}

UIntOS Media::SVGContainer::GetImgBpl() const
{
	return 0;
}

Optional<Media::EXIFData> Media::SVGContainer::GetEXIF() const
{
	return nullptr;
}
Media::PixelFormat Media::SVGContainer::GetPixelFormat() const
{
	return Media::PF_B8G8R8A8;
}

void Media::SVGContainer::SetColorSess(Optional<Media::ColorSess> colorSess)
{
}

Bool Media::SVGContainer::DrawLine(Double x1, Double y1, Double x2, Double y2, NN<DrawPen> p)
{
	Math::Coord2DDbl scale = this->doc->GetDrawScale();
	NN<SVGLine> line;
	NEW_CLASSNN(line, SVGLine(*this, this->drawRect.min + Math::Coord2DDbl(x1, y1) / scale, this->drawRect.min + Math::Coord2DDbl(x2, y2) / scale, p));
	this->elements.Add(line);
	return true;
}

Bool Media::SVGContainer::DrawPolylineI(UnsafeArray<const Int32> points, UIntOS nPoints, NN<DrawPen> p)
{
	Math::Coord2DDbl scale = this->doc->GetDrawScale();
	NN<SVGPolyline> pl;
	NEW_CLASSNN(pl, SVGPolyline(*this, p));
	UIntOS i = 0;
	while (i < nPoints * 2)
	{
		pl->AddPoint(this->drawRect.min + Math::Coord2DDbl(points[i], points[i + 1]) / scale);
		i += 2;
	}
	this->elements.Add(pl);
	return true;
}

Bool Media::SVGContainer::DrawPolygonI(UnsafeArray<const Int32> points, UIntOS nPoints, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	Math::Coord2DDbl scale = this->doc->GetDrawScale();
	NN<SVGPolygon> pg;
	NEW_CLASSNN(pg, SVGPolygon(*this, p, b));
	UIntOS i = 0;
	while (i < nPoints * 2)
	{
		pg->AddPoint(this->drawRect.min + Math::Coord2DDbl(points[i], points[i + 1]) / scale);
		i += 2;
	}
	this->elements.Add(pg);
	return true;
}

Bool Media::SVGContainer::DrawPolyPolygonI(UnsafeArray<const Int32> points, UnsafeArray<const UInt32> pointCnt, UIntOS nPointCnt, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	if (nPointCnt == 1)
	{
		return this->DrawPolygonI(points, pointCnt[0], p, b);
	}
	UIntOS totalPoints = 0;
	UIntOS i = 0;
	while (i < nPointCnt)
	{
		totalPoints += pointCnt[i];
		i++;
	}
	UnsafeArray<Math::Coord2DDbl> ptArr = MemAllocAArr(Math::Coord2DDbl, totalPoints);
	i = 0;
	while (i < totalPoints)
	{
		ptArr[i] = Math::Coord2DDbl(points[i * 2], points[i * 2 + 1]);
		i++;
	}
	Bool ret = this->DrawPolyPolygon(ptArr, pointCnt, nPointCnt, p, b);
	MemFreeAArr(ptArr);
	return ret;
}

Bool Media::SVGContainer::DrawPolyline(UnsafeArray<const Math::Coord2DDbl> points, UIntOS nPoints, NN<DrawPen> p)
{
	Math::Coord2DDbl scale = this->doc->GetDrawScale();
	NN<SVGPolyline> pl;
	NEW_CLASSNN(pl, SVGPolyline(*this, p));
	UIntOS i = 0;
	while (i < nPoints)
	{
		pl->AddPoint(this->drawRect.min + points[i] / scale);
		i++;
	}
	this->elements.Add(pl);
	return true;
}

Bool Media::SVGContainer::DrawPolygon(UnsafeArray<const Math::Coord2DDbl> points, UIntOS nPoints, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	Math::Coord2DDbl scale = this->doc->GetDrawScale();
	NN<SVGPolygon> pg;
	NEW_CLASSNN(pg, SVGPolygon(*this, p, b));
	UIntOS i = 0;
	while (i < nPoints)
	{
		pg->AddPoint(this->drawRect.min + points[i] / scale);
		i++;
	}
	this->elements.Add(pg);
	return true;
}

Bool Media::SVGContainer::DrawPolyPolygon(UnsafeArray<const Math::Coord2DDbl> points, UnsafeArray<const UInt32> pointCnt, UIntOS nPointCnt, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	if (nPointCnt == 1)
	{
		return this->DrawPolygon(points, pointCnt[0], p, b);
	}
	Double hscale = this->doc->GetHDrawScale();
	Double vscale = this->doc->GetVDrawScale();
	Text::StringBuilderUTF8 sb;
	UIntOS i = 0;
	UIntOS j = 0;
	while (i < nPointCnt)
	{
		sb.AppendUTF8Char('M');
		sb.AppendDouble(this->drawRect.min.x + points[j].x / hscale);
		sb.AppendUTF8Char(',');
		sb.AppendDouble(this->drawRect.min.y + points[j].y / vscale);
		sb.AppendUTF8Char(' ');
		j += 1;
		UIntOS k = 1;
		while (k < pointCnt[i])
		{
			sb.AppendUTF8Char('L');
			sb.AppendDouble(this->drawRect.min.x + points[j].x / hscale);
			sb.AppendUTF8Char(',');
			sb.AppendDouble(this->drawRect.min.y + points[j].y / vscale);
			sb.AppendUTF8Char(' ');
			j += 1;
			k++;
		}
		sb.AppendUTF8Char('Z');
		i++;
	}
	NN<SVGPath> path;
	NN<Text::String> d = Text::String::New(sb.ToCString());
	NEW_CLASSNN(path, SVGPath(*this, d, p, b));
	d->Release();
	this->elements.Add(path);
	return true;
}

Bool Media::SVGContainer::DrawRect(Math::Coord2DDbl tl, Math::Size2DDbl size, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	Math::Coord2DDbl scale = this->doc->GetDrawScale();
	NN<SVGRect> rect;
	NEW_CLASSNN(rect, SVGRect(*this, this->drawRect.min + tl / scale, size / scale, p, b));
	this->elements.Add(rect);
	return true;
}

Bool Media::SVGContainer::DrawEllipse(Math::Coord2DDbl tl, Math::Size2DDbl size, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	Math::Coord2DDbl scale = this->doc->GetDrawScale();
	NN<SVGEllipse> ellipse;
	NEW_CLASSNN(ellipse, SVGEllipse(*this, this->drawRect.min + tl / scale + size * 0.5 / scale, size * 0.5 / scale, p, b));
	this->elements.Add(ellipse);
	return true;
}

Bool Media::SVGContainer::DrawString(Math::Coord2DDbl tl, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b)
{
	return this->DrawString(tl, str->ToCString(), f, b);
}

Bool Media::SVGContainer::DrawString(Math::Coord2DDbl tl, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b)
{
	Math::Coord2DDbl scale = this->doc->GetDrawScale();
	NN<SVGTextComponent> t;
	NEW_CLASSNN(t, SVGStaticText(str));
	NN<SVGText> text;
	NEW_CLASSNN(text, SVGText(*this, this->drawRect.min + tl / scale, NN<SVGFont>::ConvertFrom(f), NN<SVGBrush>::ConvertFrom(b), t));
	this->elements.Add(text);
	return true;
}

Bool Media::SVGContainer::DrawStringRot(Math::Coord2DDbl center, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegreeACW)
{
	return this->DrawStringRot(center, str->ToCString(), f, b, angleDegreeACW);
}

Bool Media::SVGContainer::DrawStringRot(Math::Coord2DDbl center, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegreeACW)
{
	Math::Coord2DDbl scale = this->doc->GetDrawScale();
	NN<SVGTextComponent> t;
	NEW_CLASSNN(t, SVGStaticText(str));
	NN<SVGText> text;
	NEW_CLASSNN(text, SVGText(*this, this->drawRect.min + center / scale, NN<SVGFont>::ConvertFrom(f), NN<SVGBrush>::ConvertFrom(b), t));
	text->SetRotate(angleDegreeACW, this->drawRect.min + center / scale);
	this->elements.Add(text);
	return true;
}

Bool Media::SVGContainer::DrawStringB(Math::Coord2DDbl tl, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b, UIntOS buffSize)
{
	return this->DrawStringB(tl, str->ToCString(), f, b, buffSize);
}

Bool Media::SVGContainer::DrawStringB(Math::Coord2DDbl tl, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b, UIntOS buffSize)
{
	Math::Coord2DDbl scale = this->doc->GetDrawScale();
	return this->DrawString(tl / scale, str, f, b);
}

Bool Media::SVGContainer::DrawStringRotB(Math::Coord2DDbl center, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegreeACW, UIntOS buffSize)
{
	return this->DrawStringRotB(center, str->ToCString(), f, b, angleDegreeACW, buffSize);
}

Bool Media::SVGContainer::DrawStringRotB(Math::Coord2DDbl center, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegreeACW, UIntOS buffSize)
{
	return this->DrawStringRot(center, str, f, b, angleDegreeACW);
}

Bool Media::SVGContainer::DrawImagePt(NN<DrawImage> img, Math::Coord2DDbl tl)
{
	NN<Media::StaticImage> simg;
	if (img->ToStaticImage().SetTo(simg))
	{
		Bool succ = this->DrawSImagePt(simg, tl);
		simg.Delete();
		return succ;
	}
	return false;
}

Bool Media::SVGContainer::DrawImagePt2(NN<DrawImage> img, Math::Coord2DDbl destTL, Math::Coord2DDbl srcTL, Math::Size2DDbl srcSize)
{
	NN<Media::StaticImage> simg;
	if (img->ToStaticImage().SetTo(simg))
	{
		Bool succ = this->DrawSImagePt2(simg, destTL, srcTL, srcSize);
		simg.Delete();
		return succ;
	}
	return false;
}

Bool Media::SVGContainer::DrawSImagePt(NN<Media::StaticImage> img, Math::Coord2DDbl tl)
{
	Exporter::PNGExporter exporter;
	IO::MemoryStream memStm;
	if (!exporter.ExportImage(memStm, img))
	{
		return false;
	}
	Math::Coord2DDbl scale = this->doc->GetDrawScale();
	Text::StringBuilderUTF8 sb;
	NN<SVGImage> svgImg;
	sb.AppendC(UTF8STRC("data:image/png;base64,"));
	Text::TextBinEnc::Base64Enc b64;
	b64.EncodeBin(sb, memStm.GetBuff(), (UIntOS)memStm.GetLength());
	NEW_CLASSNN(svgImg, SVGImage(*this, this->drawRect.min + tl / scale, Math::Size2DDbl(UIntOS2Double(img->info.dispSize.x) * 96.0 / img->info.hdpi, UIntOS2Double(img->info.dispSize.y) * 96.0 / img->info.vdpi) / scale, sb.ToCString()));
	this->elements.Add(svgImg);
	this->doc->SetXMLNSXLink(true);
	return true;
}

Bool Media::SVGContainer::DrawSImagePt2(NN<Media::StaticImage> img, Math::Coord2DDbl destTL, Math::Coord2DDbl srcTL, Math::Size2DDbl srcSize)
{
	Int32 x = Double2Int32(srcTL.x);
	Int32 y = Double2Int32(srcTL.y);
	Int32 w = Double2Int32(srcSize.x);
	Int32 h = Double2Int32(srcSize.y);
	NN<Media::StaticImage> subImg = img->CreateSubImage(Math::RectArea<IntOS>(x, y, x + w, y + h));
	Bool succ = this->DrawSImagePt(subImg, destTL);
	subImg.Delete();
	return succ;
}

Bool Media::SVGContainer::DrawImageQuad(NN<Media::StaticImage> img, Math::Quadrilateral quad)
{
	/////////////////////////////
	return false;
}

NN<Media::DrawPen> Media::SVGContainer::NewPenARGB(UInt32 color, Double thick, UnsafeArrayOpt<UInt8> pattern, UIntOS nPattern)
{
	return this->doc->NewPenARGB(color, thick / this->doc->GetHDrawScale(), pattern, nPattern);
}

NN<Media::DrawBrush> Media::SVGContainer::NewBrushARGB(UInt32 color)
{
	return this->doc->NewBrushARGB(color);
}

NN<Media::DrawFont> Media::SVGContainer::NewFontPt(Text::CStringNN name, Double ptSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage)
{
	return this->doc->NewFontPt(name, ptSize / this->doc->GetVDrawScale(), fontStyle, codePage);
}

NN<Media::DrawFont> Media::SVGContainer::NewFontPx(Text::CStringNN name, Double pxSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage)
{
	return this->doc->NewFontPx(name, pxSize / this->doc->GetVDrawScale(), fontStyle, codePage);
}

NN<Media::DrawFont> Media::SVGContainer::CloneFont(NN<Media::DrawFont> f)
{
	return this->doc->CloneFont(f);
}

void Media::SVGContainer::DelPen(NN<DrawPen> p)
{
}

void Media::SVGContainer::DelBrush(NN<DrawBrush> b)
{
}

void Media::SVGContainer::DelFont(NN<DrawFont> f)
{
}

Math::Size2DDbl Media::SVGContainer::GetTextSize(NN<DrawFont> fnt, Text::CStringNN txt)
{
	return SVGCore::GetTextSize(this->refEng, NN<Media::SVGFont>::ConvertFrom(fnt), txt);
}

void Media::SVGContainer::SetTextAlign(Media::DrawEngine::DrawPos pos)
{
}

void Media::SVGContainer::GetStringBound(UnsafeArray<Int32> pos, IntOS centX, IntOS centY, UnsafeArray<const UTF8Char> str, NN<DrawFont> f, OutParam<IntOS> drawX, OutParam<IntOS> drawY)
{
	SVGCore::GetStringBound(this->refEng, pos, centX, centY, str, NN<Media::SVGFont>::ConvertFrom(f), drawX, drawY);
}

void Media::SVGContainer::GetStringBoundRot(UnsafeArray<Int32> pos, Double centX, Double centY, UnsafeArray<const UTF8Char> str, NN<DrawFont> f, Double angleDegree, OutParam<IntOS> drawX, OutParam<IntOS> drawY)
{
	SVGCore::GetStringBoundRot(this->refEng, pos, centX, centY, str, NN<Media::SVGFont>::ConvertFrom(f), angleDegree, drawX, drawY);
}

void Media::SVGContainer::CopyBits(IntOS x, IntOS y, UnsafeArray<UInt8> imgPtr, UIntOS bpl, UIntOS width, UIntOS height, Bool upsideDown) const
{
}

Optional<Media::StaticImage> Media::SVGContainer::ToStaticImage() const
{
	return nullptr;
}

Optional<Media::RasterImage> Media::SVGContainer::AsRasterImage()
{
	return nullptr;
}

UIntOS Media::SVGContainer::SavePng(NN<IO::SeekableStream> stm)
{
	return 0;
}

UIntOS Media::SVGContainer::SaveGIF(NN<IO::SeekableStream> stm)
{
	return 0;
}

UIntOS Media::SVGContainer::SaveJPG(NN<IO::SeekableStream> stm)
{
	return 0;
}

void Media::SVGContainer::DrawElement(Math::Coord2DDbl ofst, Math::Size2DDbl scale, NN<Media::DrawImage> dimg)
{
	UIntOS i = 0;
	UIntOS j = this->elements.GetCount();
	NN<SVGElement> ele;
	while (i < j)
	{
		ele = this->elements.GetItemNoCheck(i);
		if (!ele->GetElementName().Equals(UTF8STRC("defs")))
		{
			ele->DrawElement(ofst, scale, dimg);
		}
		i++;
	}
}

void Media::SVGContainer::AddElement(NN<SVGElement> ele)
{
	NN<Text::String> id;
	this->elements.Add(ele);
	if (ele->GetID().SetTo(id))
	{
		this->doc->RegisterId(id, ele);
	}
}

UIntOS Media::SVGContainer::GetElementCount() const
{
	return this->elements.GetCount();
}

Optional<Media::SVGElement> Media::SVGContainer::GetElement(UIntOS index) const
{
	return this->elements.GetItem(index);
}

UIntOS Media::SVGContainer::FindElementName(Text::CStringNN name, NN<Data::ArrayListNN<SVGElement>> results) const
{
	NN<SVGElement> ele;
	UIntOS retCnt = 0;
	UIntOS i = 0;
	UIntOS j = this->elements.GetCount();
	while (i < j)
	{
		ele = this->elements.GetItemNoCheck(i);
		if (ele->GetElementName().Equals(name))
		{
			results->Add(ele);
			retCnt++;
		}
		else if (ele->IsContainer())
		{
			retCnt += NN<SVGContainer>::ConvertFrom(ele)->FindElementName(name, results);
		}
		i++;
	}
	return retCnt;
}

void Media::SVGContainer::ClearElements()
{
	this->elements.DeleteAll();
}

void Media::SVGContainer::SetInkscapeGroupMode(NN<Text::String> groupMode)
{
	OPTSTR_DEL(this->inkscapeGroupmode);
	this->inkscapeGroupmode = groupMode->Clone();
}

void Media::SVGContainer::SetDrawRect(Math::RectAreaDbl drawRect)
{
	this->drawRect = drawRect;
}

Media::SVGDefs::SVGDefs(NN<const SVGContainer> parent, NN<Media::DrawEngine> refEng, NN<SVGDocument> doc) : SVGContainer(parent, refEng, doc)
{
}

Media::SVGDefs::~SVGDefs()
{
}

void Media::SVGDefs::ToString(NN<Text::StringBuilderUTF8> sb, UIntOS level) const
{
	if (!this->IsSpacePreserve())
	{
		sb->AppendChar(' ', level << 1);
	}
	sb->AppendC(UTF8STRC("<defs"));
	this->AppendEleAttr(sb, level + 1);
	if (this->elements.GetCount() == 0)
	{
		sb->AppendC(UTF8STRC(" />"));
		return;
	}
	sb->AppendUTF8Char('>');
	if (!this->IsSpacePreserve())
	{
		sb->AppendUTF8Char('\n');
	}
	this->ToInnerString(sb, level + 1);
	sb->AppendC(UTF8STRC("</defs>"));
}

NN<Media::SVGElement> Media::SVGDefs::CloneElement(NN<const SVGContainer> newParent) const
{
	NN<Media::SVGDefs> newDefs;
	NEW_CLASSNN(newDefs, Media::SVGDefs(newParent, this->refEng, newParent->GetDoc()));
	newDefs->CloneContainerConts(*this);
	return newDefs;
}

Media::SVGGroup::SVGGroup(NN<const SVGContainer> parent, NN<Media::DrawEngine> refEng, NN<SVGDocument> doc) : SVGContainer(parent, refEng, doc)
{
	this->clipPath = nullptr;
}

Media::SVGGroup::~SVGGroup()
{
	OPTSTR_DEL(this->clipPath);
}

void Media::SVGGroup::ToString(NN<Text::StringBuilderUTF8> sb, UIntOS level) const
{
	NN<Text::String> s;
	if (!this->IsSpacePreserve())
	{
		sb->AppendChar(' ', level << 1);
	}
	sb->AppendC(UTF8STRC("<g"));
	this->AppendEleAttr(sb, level + 1);
	if (this->inkscapeGroupmode.SetTo(s))
	{
		sb->AppendUTF8Char('\n');
		sb->AppendChar(' ', (level + 1) << 1);
		sb->AppendC(UTF8STRC(" inkscape:groupmode="));
		s = Text::XML::ToNewAttrText(s->v);
		sb->Append(s);
		s->Release();
	}
	if (this->style.SetTo(s))
	{
		sb->AppendUTF8Char('\n');
		sb->AppendChar(' ', (level + 1) << 1);
		sb->AppendC(UTF8STRC(" style="));
		s = Text::XML::ToNewAttrText(s->v);
		sb->Append(s);
		s->Release();
	}
	if (this->clipPath.SetTo(s))
	{
		sb->AppendUTF8Char('\n');
		sb->AppendChar(' ', (level + 1) << 1);
		sb->AppendC(UTF8STRC(" clip-path="));
		s = Text::XML::ToNewAttrText(s->v);
		sb->Append(s);
		s->Release();
	}
	if (this->elements.GetCount() == 0)
	{
		sb->AppendC(UTF8STRC(" />"));
		return;
	}
	sb->AppendUTF8Char('>');
	if (!this->IsSpacePreserve()) sb->AppendUTF8Char('\n');
	this->ToInnerString(sb, level + 1);
	sb->AppendC(UTF8STRC("</g>"));
}

NN<Media::SVGElement> Media::SVGGroup::CloneElement(NN<const SVGContainer> newParent) const
{
	NN<Media::SVGGroup> newGroup;
	NEW_CLASSNN(newGroup, Media::SVGGroup(newParent, this->refEng, newParent->GetDoc()));
	newGroup->clipPath = Text::String::CopyOrNull(this->clipPath);
	newGroup->CloneContainerConts(*this);
	return newGroup;
}

void Media::SVGGroup::SetClipPath(Text::CStringNN clipPath)
{
	OPTSTR_DEL(this->clipPath);
	this->clipPath = Text::String::New(clipPath);
}

Media::SVGUnknown::SVGUnknown(NN<const SVGContainer> parent, Text::CStringNN name) : SVGElement(parent)
{
	this->name = Text::String::New(name);
}

Media::SVGUnknown::~SVGUnknown()
{
	this->name->Release();
	this->attrNames.FreeAll();
	this->attrValues.FreeAll();
}

Text::CStringNN Media::SVGUnknown::GetElementName() const
{
	return this->name->ToCString();
}

void Media::SVGUnknown::DrawElement(Math::Coord2DDbl ofst, Math::Size2DDbl scale, NN<Media::DrawImage> dimg)
{
}

void Media::SVGUnknown::ToString(NN<Text::StringBuilderUTF8> sb, UIntOS level) const
{
	NN<Text::String> s;
	if (!this->IsSpacePreserve())
	{
		sb->AppendChar(' ', level << 1);
	}
	sb->AppendC(UTF8STRC("<"));
	sb->Append(this->name);
	UIntOS i = 0;
	UIntOS j = this->attrNames.GetCount();
	while (i < j)
	{
		sb->AppendUTF8Char('\n');
		sb->AppendChar(' ', (level + 1) << 1);
		sb->AppendUTF8Char(' ');
		sb->Append(this->attrNames.GetItemNoCheck(i));
		sb->AppendUTF8Char('=');
		s = Text::XML::ToNewAttrText(this->attrValues.GetItemNoCheck(i)->v);
		sb->Append(s);
		s->Release();
		i++;
	}
	sb->AppendC(UTF8STRC(" />"));
}

NN<Media::SVGElement> Media::SVGUnknown::CloneElement(NN<const SVGContainer> newParent) const
{
	NN<Media::SVGUnknown> newUnknown;
	NEW_CLASSNN(newUnknown, Media::SVGUnknown(newParent, this->name->ToCString()));
	UIntOS i = 0;
	UIntOS j = this->attrNames.GetCount();
	while (i < j)
	{
		newUnknown->AddAttr(this->attrNames.GetItemNoCheck(i)->ToCString(), this->attrValues.GetItemNoCheck(i)->ToCString());
		i++;
	}
	newUnknown->CloneElementAttrs(*this);
	return newUnknown;
}

void Media::SVGUnknown::AddAttr(Text::CStringNN name, Text::CStringNN value)
{
	this->attrNames.Add(Text::String::New(name));
	this->attrValues.Add(Text::String::New(value));
}

Media::SVGUnknownContainer::SVGUnknownContainer(NN<const SVGContainer> parent, NN<Media::DrawEngine> refEng, NN<SVGDocument> doc, Text::CStringNN name) : SVGContainer(parent, refEng, doc)
{
	this->name = Text::String::New(name);
}

Media::SVGUnknownContainer::~SVGUnknownContainer()
{
	this->name->Release();
	this->attrNames.FreeAll();
	this->attrValues.FreeAll();
}

Text::CStringNN Media::SVGUnknownContainer::GetElementName() const
{
	return this->name->ToCString();
}

void Media::SVGUnknownContainer::ToString(NN<Text::StringBuilderUTF8> sb, UIntOS level) const
{
	NN<Text::String> s;
	if (!this->IsSpacePreserve())
	{
		sb->AppendChar(' ', level << 1);
	}
	sb->AppendC(UTF8STRC("<"));
	sb->Append(this->name);
	UIntOS i = 0;
	UIntOS j = this->attrNames.GetCount();
	while (i < j)
	{
		sb->AppendUTF8Char('\n');
		sb->AppendChar(' ', (level + 1) << 1);
		sb->AppendUTF8Char(' ');
		sb->Append(this->attrNames.GetItemNoCheck(i));
		sb->AppendUTF8Char('=');
		s = Text::XML::ToNewAttrText(this->attrValues.GetItemNoCheck(i)->v);
		sb->Append(s);
		s->Release();
		i++;
	}
	sb->AppendUTF8Char('>');
	if (!this->IsSpacePreserve()) sb->AppendUTF8Char('\n');
	this->ToInnerString(sb, level + 1);
	sb->AppendC(UTF8STRC("</"));
	sb->Append(this->name);
	sb->AppendUTF8Char('>');
	if (!this->IsSpacePreserve()) sb->AppendUTF8Char('\n');
}

NN<Media::SVGElement> Media::SVGUnknownContainer::CloneElement(NN<const SVGContainer> newParent) const
{
	NN<Media::SVGUnknownContainer> newUnknown;
	NEW_CLASSNN(newUnknown, Media::SVGUnknownContainer(newParent, this->refEng, newParent->GetDoc(), this->name->ToCString()));
	UIntOS i = 0;
	UIntOS j = this->attrNames.GetCount();
	while (i < j)
	{
		newUnknown->attrNames.Add(this->attrNames.GetItemNoCheck(i)->Clone());
		newUnknown->attrValues.Add(this->attrValues.GetItemNoCheck(i)->Clone());
		i++;
	}
	newUnknown->CloneContainerConts(*this);
	return newUnknown;
}

void Media::SVGUnknownContainer::AddAttr(Text::CStringNN name, Text::CStringNN value)
{
	this->attrNames.Add(Text::String::New(name));
	this->attrValues.Add(Text::String::New(value));
}

Media::SVGDocument::SVGDocument(NN<Media::DrawEngine> refEng) : SVGContainer(nullptr, refEng, *this)
{
	this->width = 0;
	this->height = 0;
	this->unit = Math::Unit::Distance::DU_PIXEL;
	this->viewBox = Math::RectArea<IntOS>(0, 0, 0, 0);
	this->xmlnsInkscape = false;
	this->xmlnsSvg = false;
	this->xmlnsSodipodi = false;
	this->xmlnsXlink = false;
	this->xmlnsRdf = false;
	this->xmlnsCc = false;
	this->xmlnsDc = false;
	this->version = nullptr;
	this->inkscapeVersion = nullptr;
	this->sodipodiDocname = nullptr;
	this->hDrawScale = NAN;
	this->vDrawScale = NAN;
}

Media::SVGDocument::~SVGDocument()
{
	this->fonts.DeleteAll();
	this->brushes.DeleteAll();
	this->pens.DeleteAll();
	OPTSTR_DEL(this->version);
	OPTSTR_DEL(this->inkscapeVersion);
	OPTSTR_DEL(this->sodipodiDocname);
}

Double Media::SVGDocument::GetWidth() const
{
	return Math::Unit::Distance::Convert(this->unit, Math::Unit::Distance::DU_PIXEL, UIntOS2Double(this->width));
}

Double Media::SVGDocument::GetHeight() const
{
	return Math::Unit::Distance::Convert(this->unit, Math::Unit::Distance::DU_PIXEL, UIntOS2Double(this->height));
}

NN<Media::DrawPen> Media::SVGDocument::NewPenARGB(UInt32 color, Double thick, UnsafeArrayOpt<UInt8> pattern, UIntOS nPattern)
{
	NN<SVGPen> pen;
	UIntOS i = 0;
	UIntOS j = this->pens.GetCount();
	while (i < j)
	{
		pen = this->pens.GetItemNoCheck(i);
		if (pen->GetColor() == color && pen->GetThick() == thick)
		{
			return pen;
		}
		i++;
	}
	NEW_CLASSNN(pen, SVGPen(thick, color));
	this->pens.Add(pen);
	return pen;
}

NN<Media::DrawBrush> Media::SVGDocument::NewBrushARGB(UInt32 color)
{
	NN<SVGBrush> brush;
	UIntOS i = 0;
	UIntOS j = this->brushes.GetCount();
	while (i < j)
	{
		brush = this->brushes.GetItemNoCheck(i);
		if (brush->GetColor() == color)
		{
			return brush;
		}
		i++;
	}
	NEW_CLASSNN(brush, SVGBrush(color));
	this->brushes.Add(brush);
	return brush;
}

NN<Media::DrawFont> Media::SVGDocument::NewFontPt(Text::CStringNN name, Double ptSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage)
{
	return this->NewFontPx(name, ptSize * 96.0 / 72.0, fontStyle, codePage);
}

NN<Media::DrawFont> Media::SVGDocument::NewFontPx(Text::CStringNN name, Double pxSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage)
{
	NN<SVGFont> font;
	NN<Text::String> s;
	UIntOS i = 0;
	UIntOS j = this->fonts.GetCount();
	while (i < j)
	{
		font = this->fonts.GetItemNoCheck(i);
		if (font->GetFontName().SetTo(s) && s->Equals(name) && font->GetFontSizePx() == pxSize && font->GetStyle() == fontStyle)
		{
			return font;
		}
		i++;
	}
	NEW_CLASSNN(font, SVGFont(name, pxSize, fontStyle));
	this->fonts.Add(font);
	return font;
}

NN<Media::DrawPen> Media::SVGDocument::ClonePen(NN<DrawPen> p)
{
	NN<SVGPen> pen = NN<SVGPen>::ConvertFrom(p);
	return this->NewPenARGB(pen->GetColor(), pen->GetThick() * this->GetHDrawScale(), nullptr, 0);
}
NN<Media::DrawBrush> Media::SVGDocument::CloneBrush(NN<DrawBrush> b)
{
	NN<SVGBrush> brush = NN<SVGBrush>::ConvertFrom(b);
	return this->NewBrushARGB(brush->GetColor());
}

NN<Media::DrawFont> Media::SVGDocument::CloneFont(NN<Media::DrawFont> f)
{
	NN<SVGFont> font = NN<SVGFont>::ConvertFrom(f);
	return this->NewFontPx(Text::String::OrEmpty(font->GetFontName())->ToCString(), font->GetFontSizePx(), font->GetStyle(), 0);
}

void Media::SVGDocument::SetSize(UIntOS width, UIntOS height, Math::Unit::Distance::DistanceUnit unit)
{
	this->width = width;
	this->height = height;
	this->unit = unit;
	this->hDrawScale = NAN;
	this->vDrawScale = NAN;
}

void Media::SVGDocument::SetViewBox(Math::RectArea<IntOS> viewBox)
{
	this->viewBox = viewBox;
	this->hDrawScale = NAN;
	this->vDrawScale = NAN;
}

Double Media::SVGDocument::GetHDrawScale()
{
	if (Math::IsNAN(this->hDrawScale))
	{
		if (this->viewBox.GetWidth() == 0)
		{
			this->hDrawScale = Math::Unit::Distance::Convert(this->unit, Math::Unit::Distance::DU_PIXEL, 1.0);
		}
		else
		{
			this->hDrawScale = this->GetWidth() / IntOS2Double(this->viewBox.GetWidth());
		}
	}
	return this->hDrawScale;
}

Double Media::SVGDocument::GetVDrawScale()
{
	if (Math::IsNAN(this->vDrawScale))
	{
		if (this->viewBox.GetHeight() == 0)
		{
			this->vDrawScale = Math::Unit::Distance::Convert(this->unit, Math::Unit::Distance::DU_PIXEL, 1.0);
		}
		else
		{
			this->vDrawScale = this->GetHeight() / IntOS2Double(this->viewBox.GetHeight());
		}
	}
	return this->vDrawScale;
}

Math::Coord2DDbl Media::SVGDocument::GetDrawScale()
{
	return Math::Coord2DDbl(this->GetHDrawScale(), this->GetVDrawScale());
}

void Media::SVGDocument::SetXMLNSXLink(Bool xmlnsXlink)
{
	this->xmlnsXlink = xmlnsXlink;
}

void Media::SVGDocument::ToString(NN<Text::StringBuilderUTF8> sb, UIntOS level) const
{
	NN<Text::String> s;
	sb->AppendC(UTF8STRC("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"));
	sb->AppendC(UTF8STRC("<svg"));
	sb->AppendUTF8Char('\n');
	sb->AppendChar(' ', (level + 1) << 1);
	sb->AppendC(UTF8STRC(" xmlns=\"http://www.w3.org/2000/svg\""));
	if (this->version.SetTo(s))
	{
		sb->AppendUTF8Char('\n');
		sb->AppendChar(' ', (level + 1) << 1);
		sb->AppendC(UTF8STRC(" version="));
		s = Text::XML::ToNewAttrText(s->v);
		sb->Append(s);
		s->Release();
	}
	if (this->width > 0)
	{
		sb->AppendUTF8Char('\n');
		sb->AppendChar(' ', (level + 1) << 1);
		sb->AppendC(UTF8STRC(" width=\""));
		sb->AppendUIntOS(this->width);
		sb->Append(Math::Unit::Distance::GetUnitShortName(this->unit));
		sb->AppendUTF8Char('\"');	
	}
	if (this->height > 0)
	{
		sb->AppendUTF8Char('\n');
		sb->AppendChar(' ', (level + 1) << 1);
		sb->AppendC(UTF8STRC(" height=\""));
		sb->AppendUIntOS(this->height);
		sb->Append(Math::Unit::Distance::GetUnitShortName(this->unit));
		sb->AppendUTF8Char('\"');	
	}
	if (this->viewBox.GetWidth() > 0 && this->viewBox.GetHeight() > 0)
	{
		sb->AppendUTF8Char('\n');
		sb->AppendChar(' ', (level + 1) << 1);
		sb->AppendC(UTF8STRC(" viewBox=\""));
		sb->AppendIntOS(this->viewBox.min.x);
		sb->AppendC(UTF8STRC(" "));
		sb->AppendIntOS(this->viewBox.min.y);
		sb->AppendC(UTF8STRC(" "));
		sb->AppendIntOS(this->viewBox.GetWidth());
		sb->AppendC(UTF8STRC(" "));
		sb->AppendIntOS(this->viewBox.GetHeight());
		sb->AppendC(UTF8STRC("\""));
	}
	if (this->inkscapeVersion.SetTo(s))
	{
		sb->AppendUTF8Char('\n');
		sb->AppendChar(' ', (level + 1) << 1);
		sb->AppendC(UTF8STRC(" inkscape:version="));
		s = Text::XML::ToNewAttrText(s->v);
		sb->Append(s);
		s->Release();
	}
	if (this->sodipodiDocname.SetTo(s))
	{
		sb->AppendUTF8Char('\n');
		sb->AppendChar(' ', (level + 1) << 1);
		sb->AppendC(UTF8STRC(" sodipodi:docname="));
		s = Text::XML::ToNewAttrText(s->v);
		sb->Append(s);
		s->Release();
	}
	if (this->xmlnsInkscape)
	{
		sb->AppendUTF8Char('\n');
		sb->AppendChar(' ', (level + 1) << 1);
		sb->AppendC(UTF8STRC(" xmlns:inkscape=\"http://www.inkscape.org/namespaces/inkscape\""));
	}
	if (this->xmlnsSodipodi)
	{
		sb->AppendUTF8Char('\n');
		sb->AppendChar(' ', (level + 1) << 1);
		sb->AppendC(UTF8STRC(" xmlns:sodipodi=\"http://sodipodi.sourceforge.net/DTD/sodipodi-0.dtd\""));
	}
	if (this->xmlnsSvg)
	{
		sb->AppendUTF8Char('\n');
		sb->AppendChar(' ', (level + 1) << 1);
		sb->AppendC(UTF8STRC(" xmlns:svg=\"http://www.w3.org/2000/svg\""));
	}
	if (this->xmlnsXlink)
	{
		sb->AppendUTF8Char('\n');
		sb->AppendChar(' ', (level + 1) << 1);
		sb->AppendC(UTF8STRC(" xmlns:xlink=\"http://www.w3.org/1999/xlink\""));
	}
	if (this->xmlnsRdf)
	{
		sb->AppendUTF8Char('\n');
		sb->AppendChar(' ', (level + 1) << 1);
		sb->AppendC(UTF8STRC(" xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\""));
	}
	if (this->xmlnsCc)
	{
		sb->AppendUTF8Char('\n');
		sb->AppendChar(' ', (level + 1) << 1);
		sb->AppendC(UTF8STRC(" xmlns:cc=\"http://creativecommons.org/ns#\""));
	}
	if (this->xmlnsDc)
	{
		sb->AppendUTF8Char('\n');
		sb->AppendChar(' ', (level + 1) << 1);
		sb->AppendC(UTF8STRC(" xmlns:dc=\"http://purl.org/dc/elements/1.1/\""));
	}
	this->AppendEleAttr(sb, level + 1);
	sb->AppendUTF8Char('>');
	if (!this->IsSpacePreserve()) sb->AppendUTF8Char('\n');
	this->ToInnerString(sb, level + 1);
	sb->AppendC(UTF8STRC("</svg>"));
}

void Media::SVGDocument::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	sb->Append(CSTR("Width(px): "));
	sb->AppendDouble(this->GetWidth());
	sb->AppendC(UTF8STRC("\r\nHeight(px): "));
	sb->AppendDouble(this->GetHeight());
	sb->AppendC(UTF8STRC("\r\nWidth(mm): "));
	sb->AppendDouble(Math::Unit::Distance::Convert(this->unit, Math::Unit::Distance::DU_MILLIMETER, UIntOS2Double(this->width)));
	sb->AppendC(UTF8STRC("\r\nHeight(mm): "));
	sb->AppendDouble(Math::Unit::Distance::Convert(this->unit, Math::Unit::Distance::DU_MILLIMETER, UIntOS2Double(this->height)));
}

NN<Media::SVGElement> Media::SVGDocument::CloneElement(NN<const SVGContainer> newParent) const
{
	return NN<Media::SVGElement>::ConvertFrom(this->Clone());
}

void Media::SVGDocument::RegisterId(NN<Text::String> id, NN<SVGElement> ele)
{
	this->idMap.Put(id, ele);
}

Optional<Media::SVGElement> Media::SVGDocument::GetElementById(Text::CStringNN id) const
{
	return this->idMap.GetC(id);
}

NN<Media::Image> Media::SVGDocument::Clone() const
{
	NN<SVGDocument> newDoc;
	NEW_CLASSNN(newDoc, SVGDocument(this->refEng));
	newDoc->width = this->width;
	newDoc->height = this->height;
	newDoc->unit = this->unit;
	newDoc->viewBox = this->viewBox;
	newDoc->xmlnsInkscape = this->xmlnsInkscape;
	newDoc->xmlnsSvg = this->xmlnsSvg;
	newDoc->xmlnsSodipodi = this->xmlnsSodipodi;
	newDoc->xmlnsXlink = this->xmlnsXlink;
	newDoc->xmlnsRdf = this->xmlnsRdf;
	newDoc->xmlnsCc = this->xmlnsCc;
	newDoc->xmlnsDc = this->xmlnsDc;
	newDoc->version = Text::String::CopyOrNull(this->version);
	newDoc->inkscapeVersion = Text::String::CopyOrNull(this->inkscapeVersion);
	newDoc->sodipodiDocname = Text::String::CopyOrNull(this->sodipodiDocname);
	newDoc->hDrawScale = this->hDrawScale;
	newDoc->vDrawScale = this->vDrawScale;
	newDoc->CloneContainerConts(*this);
	return newDoc;
}

Media::ImageType Media::SVGDocument::GetImageType() const
{
	return Media::ImageType::SVG;
}

Double Media::SVGDocument::GetVisibleWidthPx() const
{
	return this->GetWidth();
}

Double Media::SVGDocument::GetVisibleHeightPx() const
{
	return this->GetHeight();
}

NN<Media::StaticImage> Media::SVGDocument::CreateStaticImage() const
{
	Double w = this->GetWidth();
	Double h = this->GetHeight();
	NN<Media::StaticImage> simg;
	Media::ColorProfile srgb(Media::ColorProfile::CPT_SRGB);
	NEW_CLASSNN(simg, Media::StaticImage(Math::Size2D<UIntOS>((UIntOS)w, (UIntOS)h), 0, 32, Media::PixelFormat::PF_B8G8R8A8, 0, srgb, Media::ColorProfile::YUVT_BT709, Media::AlphaType::AT_ALPHA, Media::YCOFST_C_CENTER_LEFT));
	return simg;
}

NN<Media::StaticImage> Media::SVGDocument::CreateSubImage(Math::RectArea<IntOS> area) const
{
	NN<Media::StaticImage> simg;
	Media::ColorProfile srgb(Media::ColorProfile::CPT_SRGB);
	NEW_CLASSNN(simg, Media::StaticImage(Math::Size2D<UIntOS>((UIntOS)area.GetWidth(), (UIntOS)area.GetHeight()), 0, 32, Media::PixelFormat::PF_B8G8R8A8, 0, srgb, Media::ColorProfile::YUVT_BT709, Media::AlphaType::AT_ALPHA, Media::YCOFST_C_CENTER_LEFT));
	return simg;
}

void Media::SVGDocument::DrawTo(Math::Coord2DDbl ofst, Math::Size2DDbl scale, NN<Media::DrawImage> dimg)
{
	Math::Coord2DDbl drawScale = this->GetDrawScale();
	this->DrawElement(ofst, scale * drawScale, dimg);
};

Optional<Media::SVGDocument> Media::SVGDocument::ParseFile(Text::CStringNN fileName, NN<Text::EncodingFactory> encFact, NN<Media::DrawEngine> refEng)
{
	IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	Text::XMLReader reader(encFact, fs, Text::XMLReader::PM_XML);
	NN<Text::String> eleName;
	if (reader.NextElementName().SetTo(eleName) && eleName->Equals(UTF8STRC("svg")))
	{
		return Media::SVGDocument::ParseReader(reader, refEng);
	}
	return nullptr;
}

Optional<Media::SVGDocument> Media::SVGDocument::ParseReader(NN<Text::XMLReader> reader, NN<Media::DrawEngine> refEng)
{
	if (!reader->GetElementName().Equals(UTF8STRC("svg")))
	{
		return nullptr;
	}
	NN<Media::SVGDocument> doc;
	NEW_CLASSNN(doc, Media::SVGDocument(refEng));
	UIntOS width = 0;
	UIntOS height = 0;
	Text::StringBuilderUTF8 sb;
	Math::Unit::Distance::DistanceUnit unit = Math::Unit::Distance::DU_PIXEL;
	UIntOS i = 0;
	UIntOS j = reader->GetAttribCount();
	NN<Text::XMLAttrib> attr;
	NN<Text::String> name;
	NN<Text::String> value;
	while (i < j)
	{
		attr = reader->GetAttribNoCheck(i);
		if (attr->name.SetTo(name) && attr->value.SetTo(value))
		{
			if (name->Equals(UTF8STRC("width")))
			{
				if (value->EndsWith(UTF8STRC("px")))
				{
					unit = Math::Unit::Distance::DU_PIXEL;
					sb.ClearStr();
					sb.AppendC(value->v, value->leng - 2);
					if (!sb.ToUIntOS(width))
					{
						printf("SVGDocument: svg Invalid width (%s)\r\n", value->v.Ptr());
					}
				}
				else if (value->EndsWith(UTF8STRC("pt")))
				{
					unit = Math::Unit::Distance::DU_POINT;
					sb.ClearStr();
					sb.AppendC(value->v, value->leng - 2);
					if (!sb.ToUIntOS(width))
					{
						printf("SVGDocument: svg Invalid width (%s)\r\n", value->v.Ptr());
					}
				}
				else if (value->EndsWith(UTF8STRC("mm")))
				{
					unit = Math::Unit::Distance::DU_MILLIMETER;
					sb.ClearStr();
					sb.AppendC(value->v, value->leng - 2);
					if (!sb.ToUIntOS(width))
					{
						printf("SVGDocument: svg Invalid width (%s)\r\n", value->v.Ptr());
					}
				}
				else
				{
					unit = Math::Unit::Distance::DU_PIXEL;
					if (!value->ToUIntOS(width))
					{
						printf("SVGDocument: svg Invalid width (%s)\r\n", value->v.Ptr());
					}
				}
				if (width != 0 && height != 0)
				{
					doc->SetSize(width, height, unit);
				}
			}
			else if (name->Equals(UTF8STRC("height")))
			{
				if (value->EndsWith(UTF8STRC("px")))
				{
					unit = Math::Unit::Distance::DU_PIXEL;
					sb.ClearStr();
					sb.AppendC(value->v, value->leng - 2);
					if (!sb.ToUIntOS(height))
					{
						printf("SVGDocument: svg Invalid height (%s)\r\n", value->v.Ptr());
					}
				}
				else if (value->EndsWith(UTF8STRC("pt")))
				{
					unit = Math::Unit::Distance::DU_POINT;
					sb.ClearStr();
					sb.AppendC(value->v, value->leng - 2);
					if (!sb.ToUIntOS(height))
					{
						printf("SVGDocument: svg Invalid height (%s)\r\n", value->v.Ptr());
					}
				}
				else if (value->EndsWith(UTF8STRC("mm")))
				{
					unit = Math::Unit::Distance::DU_MILLIMETER;
					sb.ClearStr();
					sb.AppendC(value->v, value->leng - 2);
					if (!sb.ToUIntOS(height))
					{
						printf("SVGDocument: svg Invalid height (%s)\r\n", value->v.Ptr());
					}
				}
				else
				{
					unit = Math::Unit::Distance::DU_PIXEL;
					if (!value->ToUIntOS(height))
					{
						printf("SVGDocument: svg Invalid height (%s)\r\n", value->v.Ptr());
					}
				}
				if (width != 0 && height != 0)
				{
					doc->SetSize(width, height, unit);
				}
			}
			else if (name->Equals(UTF8STRC("viewBox")))
			{
				Text::PString sarr[5];
				IntOS vals[4];
				sb.ClearStr();
				sb.Append(value);
				if (Text::StrSplitP(sarr, 5, sb, ' ') == 4)
				{
					if (sarr[0].ToIntOS(vals[0]) && sarr[1].ToIntOS(vals[1]) && sarr[2].ToIntOS(vals[2]) && sarr[3].ToIntOS(vals[3]))
					{
						doc->SetViewBox(Math::RectArea<IntOS>(vals[0], vals[1], vals[0] + vals[2], vals[1] + vals[3]));
					}
					else
					{
						printf("SVGDocument: svg Invalid viewBox (%s)\r\n", value->v.Ptr());
					}
				}
				else
				{
					printf("SVGDocument: svg Invalid viewBox (%s)\r\n", value->v.Ptr());
				}
			}
			else if (name->Equals(UTF8STRC("xmlns")))
			{
				if (!value->Equals(UTF8STRC("http://www.w3.org/2000/svg")))
				{
					printf("SVGDocument: svg Invalid xmlns (%s)\r\n", value->v.Ptr());
				}
			}
			else if (name->Equals(UTF8STRC("version")))
			{
				OPTSTR_DEL(doc->version);
				doc->version = value->Clone();
			}
			else if (name->Equals(UTF8STRC("id")))
			{
				OPTSTR_DEL(doc->id);
				doc->id = value->Clone();
			}
			else if (name->Equals(UTF8STRC("inkscape:version")))
			{
				OPTSTR_DEL(doc->inkscapeVersion);
				doc->inkscapeVersion = value->Clone();
			}
			else if (name->Equals(UTF8STRC("sodipodi:docname")))
			{
				OPTSTR_DEL(doc->sodipodiDocname);
				doc->sodipodiDocname = value->Clone();
			}
			else if (name->Equals(UTF8STRC("xmlns:inkscape")))
			{
				if (!value->Equals(UTF8STRC("http://www.inkscape.org/namespaces/inkscape")))
				{
					printf("SVGDocument: svg Invalid xmlns:inkscape (%s)\r\n", value->v.Ptr());
				}
				else
				{
					doc->xmlnsInkscape = true;
				}
			}
			else if (name->Equals(UTF8STRC("xmlns:sodipodi")))
			{
				if (!value->Equals(UTF8STRC("http://sodipodi.sourceforge.net/DTD/sodipodi-0.dtd")))
				{
					printf("SVGDocument: svg Invalid xmlns:sodipodi (%s)\r\n", value->v.Ptr());
				}
				else
				{
					doc->xmlnsSodipodi = true;
				}
			}
			else if (name->Equals(UTF8STRC("xmlns:svg")))
			{
				if (!value->Equals(UTF8STRC("http://www.w3.org/2000/svg")))
				{
					printf("SVGDocument: svg Invalid xmlns:svg (%s)\r\n", value->v.Ptr());
				}
				else
				{
					doc->xmlnsSvg = true;
				}
			}
			else if (name->Equals(UTF8STRC("xmlns:xlink")))
			{
				if (!value->Equals(UTF8STRC("http://www.w3.org/1999/xlink")))
				{
					printf("SVGDocument: svg Invalid xmlns:xlink (%s)\r\n", value->v.Ptr());
				}
				else
				{
					doc->xmlnsXlink = true;
				}
			}
			else if (name->Equals(UTF8STRC("xmlns:rdf")))
			{
				if (!value->Equals(UTF8STRC("http://www.w3.org/1999/02/22-rdf-syntax-ns#")))
				{
					printf("SVGDocument: svg Invalid xmlns:rdf (%s)\r\n", value->v.Ptr());
				}
				else
				{
					doc->xmlnsRdf = true;
				}
			}
			else if (name->Equals(UTF8STRC("xmlns:cc")))
			{
				if (!value->Equals(UTF8STRC("http://creativecommons.org/ns#")))
				{
					printf("SVGDocument: svg Invalid xmlns:cc (%s)\r\n", value->v.Ptr());
				}
				else
				{
					doc->xmlnsCc = true;
				}
			}
			else if (name->Equals(UTF8STRC("xmlns:dc")))
			{
				if (!value->Equals(UTF8STRC("http://purl.org/dc/elements/1.1/")))
				{
					printf("SVGDocument: svg Invalid xmlns:dc (%s)\r\n", value->v.Ptr());
				}
				else
				{
					doc->xmlnsDc = true;
				}
			}
			else if (name->Equals(UTF8STRC("xml:space")))
			{
				doc->SetSpacePreserve(value->Equals(UTF8STRC("preserve")));
			}
			else
			{
				printf("SVGDocument: svg Unknown attribute: %s\r\n", name->v.Ptr());
			}
		}
		else
		{
			printf("SVGDocument: svg Invalid attribute\r\n");
		}
		i++;
	}
	if (reader->IsElementEmpty() || ParseContainer(doc, reader))
	{
		return doc;
	}
	doc.Delete();
	return nullptr;
}

Bool Media::SVGDocument::ParseContainer(NN<SVGContainer> container, NN<Text::XMLReader> reader)
{
	NN<Text::String> eleName;
	while (reader->NextElementName().SetTo(eleName))
	{
		if (eleName->Equals(UTF8STRC("line")))
		{
			if (!SVGDocument::ParseLine(container, reader))
			{
				return false;
			}
		}
		else if (eleName->Equals(UTF8STRC("polyline")))
		{
			if (!SVGDocument::ParsePolyline(container, reader))
			{
				return false;
			}
		}
		else if (eleName->Equals(UTF8STRC("polygon")))
		{
			if (!SVGDocument::ParsePolygon(container, reader))
			{
				return false;
			}
		}
		else if (eleName->Equals(UTF8STRC("rect")))
		{
			if (!SVGDocument::ParseRect(container, reader))
			{
				return false;
			}
		}
		else if (eleName->Equals(UTF8STRC("ellipse")))
		{
			if (!SVGDocument::ParseEllipse(container, reader))
			{
				return false;
			}
		}
		else if (eleName->Equals(UTF8STRC("text")))
		{
			if (!SVGDocument::ParseText(container, reader))
			{
				return false;
			}
		}
		else if (eleName->Equals(UTF8STRC("image")))
		{
			if (!SVGDocument::ParseImage(container, reader))
			{
				return false;
			}
		}
		else if (eleName->Equals(UTF8STRC("path")))
		{
			if (!SVGDocument::ParsePath(container, reader))
			{
				return false;
			}
		}
		else if (eleName->Equals(UTF8STRC("g")))
		{
			NN<SVGGroup> group;
			NEW_CLASSNN(group, SVGGroup(container, container->GetDrawEngine(), container->GetDoc()));
			ParseContainerAttr(group, reader, false);
			if (!reader->IsElementEmpty() && !SVGDocument::ParseContainer(group, reader))
			{
				return false;
			}
			container->AddElement(group);
		}
		else if (eleName->Equals(UTF8STRC("defs")))
		{
			NN<SVGDefs> defs;
			NEW_CLASSNN(defs, SVGDefs(container, container->GetDrawEngine(), container->GetDoc()));
			ParseContainerAttr(defs, reader, false);
			if (!reader->IsElementEmpty() && !SVGDocument::ParseContainer(defs, reader))
			{
				return false;
			}
			container->AddElement(defs);
		}
		else if (eleName->Equals(UTF8STRC("sodipodi:namedview")))
		{
			if (!SVGDocument::ParseUnknown(container, reader))
			{
				return false;
			}
		}
		else if (eleName->Equals(UTF8STRC("inkscape:grid")))
		{
			if (!SVGDocument::ParseUnknown(container, reader))
			{
				return false;
			}
		}
		else if (eleName->Equals(UTF8STRC("title")))
		{
			if (!SVGDocument::ParseTitle(container, reader, CSTR("title")))
			{
				return false;
			}
		}
		else if (eleName->Equals(UTF8STRC("dc:title")))
		{
			if (!SVGDocument::ParseTitle(container, reader, CSTR("dc:title")))
			{
				return false;
			}
		}
		else if (eleName->Equals(UTF8STRC("linearGradient")))
		{
			if (!SVGDocument::ParseUnknown(container, reader))
			{
				return false;
			}
		}
		else if (eleName->Equals(UTF8STRC("stop")))
		{
			if (!SVGDocument::ParseUnknown(container, reader))
			{
				return false;
			}
		}
		else if (eleName->Equals(UTF8STRC("metadata")))
		{
			if (!SVGDocument::ParseUnknown(container, reader))
			{
				return false;
			}
		}
		else if (eleName->Equals(UTF8STRC("rdf:RDF")))
		{
			if (!SVGDocument::ParseUnknown(container, reader))
			{
				return false;
			}
		}
		else if (eleName->Equals(UTF8STRC("cc:Work")))
		{
			if (!SVGDocument::ParseUnknown(container, reader))
			{
				return false;
			}
		}
		else
		{
			printf("SVGDocument: Unknown element: %s\r\n", eleName->v.Ptr());
			if (!SVGDocument::ParseUnknown(container, reader))
			{
				return false;
			}
		}
	}
	return true;
}

Bool Media::SVGDocument::ParseContainerAttr(NN<SVGContainer> container, NN<Text::XMLReader> reader, Bool allowAnyAttr)
{
	UIntOS i;
	UIntOS j;
	NN<Text::XMLAttrib> attr;
	NN<Text::String> name;
	NN<Text::String> value;
	i = 0;
	j = reader->GetAttribCount();
	while (i < j)
	{
		attr = reader->GetAttribNoCheck(i);
		if (attr->name.SetTo(name) && attr->value.SetTo(value))
		{
			if (name->Equals(UTF8STRC("id")))
			{
				container->SetID(value->ToCString());
			}
			else if (name->Equals(UTF8STRC("stroke-linecap")))
			{
				if (value->Equals(UTF8STRC("butt")))
				{
					container->SetLineCap(SVGLineCap::Butt);
				}
				else if (value->Equals(UTF8STRC("round")))
				{
					container->SetLineCap(SVGLineCap::Round);
				}
				else if (value->Equals(UTF8STRC("square")))
				{
					container->SetLineCap(SVGLineCap::Square);
				}
				else
				{
					printf("SVGDocument: Invalid stroke-linecap value in (%s): %s\r\n", container->GetElementName().v.Ptr(), value->v.Ptr());
					container->SetLineCap(SVGLineCap::Default);
				}
			}
			else if (name->Equals(UTF8STRC("stroke-linejoin")))
			{
				if (value->Equals(UTF8STRC("miter")))
				{
					container->SetLineJoin(SVGLineJoin::Miter);
				}
				else if (value->Equals(UTF8STRC("round")))
				{
					container->SetLineJoin(SVGLineJoin::Round);
				}
				else if (value->Equals(UTF8STRC("bevel")))
				{
					container->SetLineJoin(SVGLineJoin::Bevel);
				}
				else
				{
					printf("SVGDocument: Invalid stroke-linejoin value in (%s): %s\r\n", container->GetElementName().v.Ptr(), value->v.Ptr());
					container->SetLineJoin(SVGLineJoin::Default);
				}
			}
			else if (name->Equals(UTF8STRC("inkscape:label")))
			{
				container->SetInkscapeLabel(value);
			}
			else if (name->Equals(UTF8STRC("inkscape:groupmode")))
			{
				container->SetInkscapeGroupMode(value);
			}
			else if (name->Equals(UTF8STRC("style")))
			{
				container->SetStyle(value->ToCString());
			}
			else if (!allowAnyAttr)
			{
				printf("SVGDocument: Unknown attribute in container (%s): %s\r\n", container->GetElementName().v.Ptr(),name->v.Ptr());
			}
		}
		else
		{
			printf("SVGDocument: Invalid container attribute in (%s)\r\n", container->GetElementName().v.Ptr());
		}
		i++;
	}
	return true;
}

Bool Media::SVGDocument::ParseLine(NN<SVGContainer> container, NN<Text::XMLReader> reader)
{
	printf("SVGDocument: Parse line is not supported\r\n");
	reader->SkipElement();
	return true;
}

Bool Media::SVGDocument::ParsePolyline(NN<SVGContainer> container, NN<Text::XMLReader> reader)
{
	printf("SVGDocument: Parse polyline is not supported\r\n");
	reader->SkipElement();
	return true;
}

Bool Media::SVGDocument::ParsePolygon(NN<SVGContainer> container, NN<Text::XMLReader> reader)
{
	printf("SVGDocument: Parse polygon is not supported\r\n");
	reader->SkipElement();
	return true;
}

Bool Media::SVGDocument::ParseRect(NN<SVGContainer> container, NN<Text::XMLReader> reader)
{
	NN<Text::String> name;
	NN<Text::String> value;
	Optional<Text::String> id = nullptr;
	Double x = NAN;
	Double y = NAN;
	Double width = NAN;
	Double height = NAN;
	Bool stylePen = false;
	Bool styleBrush = false;
	Optional<Text::String> fill = nullptr;
	Optional<Text::String> stroke = nullptr;
	Optional<Text::String> strokeDashArray = nullptr;
	Bool styleStrokeDashArray = false;
	Double strokeOpacity = 1.0;
	Double fillOpacity = 1.0;
	Double strokeWidth = 0;
	Bool insensitive = false;
	Optional<Text::String> inkscapeLabel = nullptr;
	UIntOS i = 0;
	UIntOS j = reader->GetAttribCount();
	while (i < j)
	{
		NN<Text::XMLAttrib> attr = reader->GetAttribNoCheck(i);
		if (attr->name.SetTo(name) && attr->value.SetTo(value))
		{
			if (name->Equals(UTF8STRC("id")))
			{
				OPTSTR_DEL(id);
				id = value->Clone();
			}
			else if (name->Equals(UTF8STRC("x")))
			{
				if (!value->ToDouble(x))
				{
					printf("SVGDocument: Invalid x value in rect: %s\r\n", value->v.Ptr());
					x = NAN;
				}
			}
			else if (name->Equals(UTF8STRC("y")))
			{
				if (!value->ToDouble(y))
				{
					printf("SVGDocument: Invalid y value in rect: %s\r\n", value->v.Ptr());
					y = NAN;
				}
			}
			else if (name->Equals(UTF8STRC("width")))
			{
				if (!value->ToDouble(width))
				{
					printf("SVGDocument: Invalid width value in rect: %s\r\n", value->v.Ptr());
					width = NAN;
				}
			}
			else if (name->Equals(UTF8STRC("height")))
			{
				if (!value->ToDouble(height))
				{
					printf("SVGDocument: Invalid height value in rect: %s\r\n", value->v.Ptr());
					height = NAN;
				}
			}
			else if (name->Equals(UTF8STRC("fill")))
			{
				OPTSTR_DEL(fill);
				fill = value->Clone();
				styleBrush = false;
			}
			else if (name->Equals(UTF8STRC("stroke")))
			{
				OPTSTR_DEL(stroke);
				stroke = value->Clone();
				stylePen = true;
			}
			else if (name->Equals(UTF8STRC("stroke-width")))
			{
				if (!value->ToDouble(strokeWidth))
				{
					printf("SVGDocument: Invalid stroke-width value in rect: %s\r\n", value->v.Ptr());
					strokeWidth = 0;
				}
			}
			else if (name->Equals(UTF8STRC("stroke-opacity")))
			{
				if (!value->ToDouble(strokeOpacity))
				{
					printf("SVGDocument: Invalid stroke-opacity value in rect: %s\r\n", value->v.Ptr());
					strokeOpacity = 1.0;
				}
			}
			else if (name->Equals(UTF8STRC("fill-opacity")))
			{
				if (!value->ToDouble(fillOpacity))
				{
					printf("SVGDocument: Invalid fill-opacity value in rect: %s\r\n", value->v.Ptr());
					fillOpacity = 1.0;
				}
			}
			else if (name->Equals(UTF8STRC("stroke-dasharray")))
			{
				OPTSTR_DEL(strokeDashArray);
				strokeDashArray = value->Clone();
				styleStrokeDashArray = true;
			}
			else if (name->Equals(UTF8STRC("sodipodi:insensitive")))
			{
				if (!value->Equals(CSTR("true")))
				{
					printf("SVGDocument: Invalid sodipodi:insensitive value in rect: %s\r\n", value->v.Ptr());
				}
				else
				{
					insensitive = true;
				}
			}
			else if (name->Equals(UTF8STRC("inkscape:label")))
			{
				OPTSTR_DEL(inkscapeLabel);
				inkscapeLabel = value->Clone();
			}
			else if (name->Equals(UTF8STRC("style")))
			{
				Text::PString sarr[2];
				UIntOS i;
				Text::StringBuilderUTF8 sb;
				sb.Append(value);
				sarr[1] = sb;
				while (true)
				{
					i = Text::StrSplitTrimP(sarr, 2, sarr[1], ';');
					if (sarr[0].StartsWith(UTF8STRC("fill:")))
					{
						OPTSTR_DEL(fill);
						fill = Text::String::New(sarr[0].Substring(5).ToCString());
						styleBrush = true;
					}
					else if (sarr[0].StartsWith(UTF8STRC("stroke:")))
					{
						OPTSTR_DEL(stroke);
						stroke = Text::String::New(sarr[0].Substring(7).ToCString());
						stylePen = true;
					}
					else if (sarr[0].StartsWith(UTF8STRC("stroke-width:")))
					{
						if (!sarr[0].Substring(13).ToDouble(strokeWidth))
						{
							printf("SVGDocument: Invalid stroke-width value in rect style: %s\r\n", &sarr[0].v[13]);
							strokeWidth = 0;
						}
					}
					else if (sarr[0].StartsWith(UTF8STRC("stroke-dasharray:")))
					{
						OPTSTR_DEL(strokeDashArray);
						strokeDashArray = Text::String::New(sarr[0].Substring(17).ToCString());
						styleStrokeDashArray = true;
					}
					else if (sarr[0].StartsWith(UTF8STRC("stroke-opacity:")))
					{
						if (!sarr[0].Substring(15).ToDouble(strokeOpacity))
						{
							printf("SVGDocument: Invalid stroke-opacity value in rect style: %s\r\n", &sarr[0].v[15]);
							strokeOpacity = 1.0;
						}
					}
					else if (sarr[0].StartsWith(UTF8STRC("fill-opacity:")))
					{
						if (!sarr[0].Substring(13).ToDouble(fillOpacity))
						{
							printf("SVGDocument: Invalid fill-opacity value in rect style: %s\r\n", &sarr[0].v[13]);
							fillOpacity = 1.0;
						}
					}
					else
					{
						printf("SVGDocument: Unknown style in rect: %s\r\n", sarr[0].v.Ptr());
					}
					if (i != 2)
					{
						break;
					}
				}
			}
			else
			{
				printf("SVGDocument: Unknown rect attribute: %s\r\n", name->v.Ptr());
			}
		}
		else
		{
			printf("SVGDocument: Invalid rect attribute\r\n");
		}
		i++;
	}
	if (Math::IsNAN(x))
	{
		printf("SVGDocument: rect is missing x attribute\r\n");
	}
	else if (Math::IsNAN(y))
	{
		printf("SVGDocument: rect is missing y attribute\r\n");
	}
	else if (Math::IsNAN(width))
	{
		printf("SVGDocument: rect is missing width attribute\r\n");
	}
	else if (Math::IsNAN(height))
	{
		printf("SVGDocument: rect is missing height attribute\r\n");
	}
	else
	{
		NN<SVGRect> rect;
		Optional<DrawPen> pen = nullptr;
		Optional<DrawBrush> brush = nullptr;
		if (stroke.SetTo(value))
		{
			if (strokeDashArray.SetTo(name))
			{
				if (name->Equals(UTF8STRC("none")))
				{
					brush = nullptr;
				}
				else
				{
					printf("SVGDocument: stroke-dasharray is not supported in rect\r\n");
				}
			}
			pen = container->NewPenARGB(Text::CSSCore::ParseColor(value->ToCString(), strokeOpacity), strokeWidth * container->GetDoc()->GetHDrawScale(), nullptr, 0);
		}
		if (fill.SetTo(value))
		{
			if (value->Equals(UTF8STRC("none")))
			{
				brush = nullptr;
			}
			else
			{
				brush = container->NewBrushARGB(Text::CSSCore::ParseColor(value->ToCString(), fillOpacity));
			}
		}
		NEW_CLASSNN(rect, Media::SVGRect(container, Math::Coord2DDbl(x, y), Math::Size2DDbl(width, height), pen, brush));
		rect->SetStyle(stylePen, styleBrush);
		if (id.SetTo(value))
		{
			rect->SetID(value->ToCString());
		}
		if (inkscapeLabel.SetTo(value))
		{
			rect->SetInkscapeLabel(value);
		}
		rect->SetInsensitive(insensitive);
		container->AddElement(rect);
	}
	OPTSTR_DEL(id);
	OPTSTR_DEL(fill);
	OPTSTR_DEL(stroke);
	OPTSTR_DEL(strokeDashArray);
	OPTSTR_DEL(inkscapeLabel);
	reader->SkipElement();
	return true;
}

Bool Media::SVGDocument::ParseEllipse(NN<SVGContainer> container, NN<Text::XMLReader> reader)
{
	printf("SVGDocument: Parse ellipse is not supported\r\n");
	reader->SkipElement();
	return true;
}

Bool Media::SVGDocument::ParsePath(NN<SVGContainer> container, NN<Text::XMLReader> reader)
{
	NN<Text::String> s;
	NN<Text::String> s2;
	NN<Media::SVGPath> path;
	Optional<Text::String> d = nullptr;
	Optional<Text::String> fill = nullptr;
	Optional<Text::String> stroke = nullptr;
	SVGFillRule fillRule = SVGFillRule::Default;
	Double strokeWidth = 0;
	Double fillOpacity = 1.0;
	Double strokeOpacity = 1.0;
	UIntOS i = 0;
	UIntOS j = reader->GetAttribCount();
	NN<Text::XMLAttrib> attr;
	NN<Text::String> name;
	NN<Text::String> value;
	while (i < j)
	{
		attr = reader->GetAttribNoCheck(i);
		if (attr->name.SetTo(name) && attr->value.SetTo(value))
		{
			if (name->Equals(UTF8STRC("d")))
			{
				d = value;
			}
			else if (name->Equals(UTF8STRC("fill")))
			{
				if (!value->Equals(UTF8STRC("none")))
				{
					fill = value;
				}
			}
			else if (name->Equals(UTF8STRC("fill-rule")))
			{
				if (value->Equals(UTF8STRC("nonzero")))
				{
					fillRule = SVGFillRule::NonZero;
				}
				else if (value->Equals(UTF8STRC("evenodd")))
				{
					fillRule = SVGFillRule::EvenOdd;
				}
				else
				{
					printf("SVGDocument: Invalid fill-rule value in path: %s\r\n", value->v.Ptr());
					fillRule = SVGFillRule::Default;
				}
			}
			else if (name->Equals(UTF8STRC("stroke")))
			{
				stroke = value;
			}
			else if (name->Equals(UTF8STRC("stroke-opacity")))
			{
				if (!value->ToDouble(strokeOpacity))
				{
					printf("SVGDocument: Invalid stroke-opacity value in path: %s\r\n", value->v.Ptr());
					strokeOpacity = 1.0;
				}
			}
			else if (name->Equals(UTF8STRC("fill-opacity")))
			{
				if (!value->ToDouble(fillOpacity))
				{
					printf("SVGDocument: Invalid fill-opacity value in path: %s\r\n", value->v.Ptr());
					fillOpacity = 1.0;
				}
			}
			else if (name->Equals(UTF8STRC("stroke-width")))
			{
				if (!value->ToDouble(strokeWidth))
				{
					printf("SVGDocument: Invalid stroke-width value in path: %s\r\n", value->v.Ptr());
					strokeWidth = 0;
				}
			}
			else
			{
				printf("SVGDocument: Unknown attribute in path: %s\r\n", name->v.Ptr());
			}
		}
		else
		{
			printf("SVGDocument: Invalid attribute in path\r\n");
		}
		i++;
	}
	if (d.SetTo(s))
	{
		Optional<DrawPen> pen = nullptr;
		Optional<DrawBrush> brush = nullptr;
		NN<SVGBrush> nnbrush;
		if (stroke.SetTo(s2))
		{
			pen = container->NewPenARGB(Text::CSSCore::ParseColor(s2->ToCString(), strokeOpacity), strokeWidth * container->GetDoc()->GetHDrawScale(), nullptr, 0);
		}
		if (fill.SetTo(s2))
		{
			nnbrush = NN<SVGBrush>::ConvertFrom(container->NewBrushARGB(Text::CSSCore::ParseColor(s2->ToCString(), fillOpacity)));
			nnbrush->SetFillRule(fillRule);
			brush = nnbrush;
		}
		NEW_CLASSNN(path, Media::SVGPath(container, s, pen, brush));
		container->AddElement(path);
	}
	else
	{
		printf("SVGDocument: path is missing d attribute\r\n");
	}
	reader->SkipElement();
	return true;
}

Bool Media::SVGDocument::ParseText(NN<SVGContainer> container, NN<Text::XMLReader> reader)
{
	UIntOS i = 0;
	UIntOS j = reader->GetAttribCount();
	NN<Text::XMLAttrib> attr;
	NN<Text::String> name;
	NN<Text::String> value;
	Bool spacePreserve = false;
	Optional<Text::String> id = nullptr;
	Double x = NAN;
	Double y = NAN;
	Bool stylePen = false;
	Bool styleBrush = false;
	Bool styleFont = false;
	Optional<Text::String> fill = nullptr;
	Optional<Text::String> stroke = nullptr;
	Double strokeWidth = NAN;
	Double strokeOpacity = 1.0;
	Double fillOpacity = 1.0;
	Double inkscapeTransformCenterX = NAN;
	Double inkscapeTransformCenterY = NAN;
	Bool insensitive = false;
	DrawEngine::DrawFontStyle fontStyle = DrawEngine::DFS_NORMAL;
	Optional<Text::String> fontFamily = nullptr;
	Optional<Text::String> fontVariant = nullptr;
	Optional<Text::String> fontStretch = nullptr;
	Optional<Text::String> inkscapeFont = nullptr;
	Optional<Text::String> fontSize = nullptr;
	Optional<Text::String> textAnchor = nullptr;
	Optional<Text::String> writingMode = nullptr;
	Optional<Text::String> direction = nullptr;
	Optional<Text::String> textAlign = nullptr;
	Optional<Text::String> transform = nullptr;
	Optional<Text::String> inkscapeLabel = nullptr;
	Optional<Text::String> display = nullptr;
	Optional<Text::String> lineHeight = nullptr;
	Optional<Text::String> shapeInside = nullptr;
	Optional<Text::String> whiteSpace = nullptr;
	Optional<Text::String> strokeDasharray = nullptr;
	Optional<Text::String> shapePadding = nullptr;
	Optional<Text::String> mixBlendMode = nullptr;

	while (i < j)
	{
		attr = reader->GetAttribNoCheck(i);
		if (attr->name.SetTo(name) && attr->value.SetTo(value))
		{
			if (name->Equals(UTF8STRC("xml:space")) && value->Equals(UTF8STRC("preserve")))
			{
				spacePreserve = true;
			}
			else if (name->Equals(UTF8STRC("id")))
			{
				OPTSTR_DEL(id);
				id = value->Clone();
			}
			else if (name->Equals(UTF8STRC("x")))
			{
				if (!value->ToDouble(x))
				{
					printf("SVGDocument: Invalid x value in text: %s\r\n", value->v.Ptr());
					x = NAN;
				}
			}
			else if (name->Equals(UTF8STRC("y")))
			{
				if (!value->ToDouble(y))
				{
					printf("SVGDocument: Invalid y value in text: %s\r\n", value->v.Ptr());
					y = NAN;
				}
			}
			else if (name->Equals(UTF8STRC("transform")))
			{
				OPTSTR_DEL(transform);
				transform = value->Clone();
			}
			else if (name->Equals(UTF8STRC("sodipodi:insensitive")))
			{
				if (!value->Equals(CSTR("true")))
				{
					printf("SVGDocument: Invalid sodipodi:insensitive value in text: %s\r\n", value->v.Ptr());
				}
				else
				{
					insensitive = true;
				}
			}
			else if (name->Equals(UTF8STRC("inkscape:label")))
			{
				OPTSTR_DEL(inkscapeLabel);
				inkscapeLabel = value->Clone();
			}
			else if (name->Equals(UTF8STRC("inkscape:transform-center-x")))
			{
				if (!value->ToDouble(inkscapeTransformCenterX))
				{
					printf("SVGDocument: Invalid inkscape:transform-center-x value in text: %s\r\n", value->v.Ptr());
					inkscapeTransformCenterX = NAN;
				}
			}
			else if (name->Equals(UTF8STRC("inkscape:transform-center-y")))
			{
				if (!value->ToDouble(inkscapeTransformCenterY))
				{
					printf("SVGDocument: Invalid inkscape:transform-center-y value in text: %s\r\n", value->v.Ptr());
					inkscapeTransformCenterY = NAN;
				}
			}
			else if (name->Equals(UTF8STRC("style")))
			{
				Text::PString sarr[2];
				UIntOS i;
				Text::StringBuilderUTF8 sb;
				sb.Append(value);
				sarr[1] = sb;
				while (true)
				{
					i = Text::StrSplitTrimP(sarr, 2, sarr[1], ';');
					if (sarr[0].StartsWith(UTF8STRC("xml:space:")))
					{
						sarr[0].SubstrTrim(10);
						if (sarr[0].Substring(10).Equals(UTF8STRC("preserve")))
						{
							spacePreserve = true;
						}
					}
					else if (sarr[0].StartsWith(UTF8STRC("fill:")))
					{
						OPTSTR_DEL(fill);
						sarr[0].SubstrTrim(5);
						fill = Text::String::New(sarr[0].Substring(5).ToCString());
						styleBrush = true;
					}
					else if (sarr[0].StartsWith(UTF8STRC("stroke:")))
					{
						OPTSTR_DEL(stroke);
						sarr[0].SubstrTrim(7);
						stroke = Text::String::New(sarr[0].Substring(7).ToCString());
						stylePen = true;
					}
					else if (sarr[0].StartsWith(UTF8STRC("stroke-width:")))
					{
						sarr[0].SubstrTrim(13);
						if (!sarr[0].Substring(13).ToDouble(strokeWidth))
						{
							printf("SVGDocument: Invalid stroke-width value in text style: %s\r\n", &sarr[0].v[13]);
							strokeWidth = 0;
						}
					}
					else if (sarr[0].StartsWith(UTF8STRC("stroke-opacity:")))
					{
						sarr[0].SubstrTrim(15);
						if (!sarr[0].Substring(15).ToDouble(strokeOpacity))
						{
							printf("SVGDocument: Invalid stroke-opacity value in text style: %s\r\n", &sarr[0].v[15]);
							strokeOpacity = 1.0;
						}
					}
					else if (sarr[0].StartsWith(UTF8STRC("fill-opacity:")))
					{
						sarr[0].SubstrTrim(13);
						if (!sarr[0].Substring(13).ToDouble(fillOpacity))
						{
							printf("SVGDocument: Invalid fill-opacity value in text style: %s\r\n", &sarr[0].v[13]);
							fillOpacity = 1.0;
						}
					}
					else if (sarr[0].StartsWith(UTF8STRC("font-style:")))
					{
						sarr[0].SubstrTrim(11);
						if (sarr[0].Substring(11).Equals(UTF8STRC("normal")))
						{
						}
						else if (sarr[0].Substring(11).Equals(UTF8STRC("italic")))
						{
							fontStyle = (DrawEngine::DrawFontStyle)(fontStyle | DrawEngine::DFS_ITALIC);
						}
						else
						{
							printf("SVGDocument: Unknown font-style in text: %s\r\n", sarr[0].Substring(11).v.Ptr());
						}
						styleFont = true;
					}
					else if (sarr[0].StartsWith(UTF8STRC("font-weight:")))
					{
						sarr[0].SubstrTrim(12);
						if (sarr[0].Substring(12).Equals(UTF8STRC("normal")))
						{
						}
						else if (sarr[0].Substring(12).Equals(UTF8STRC("bold")))
						{
							fontStyle = (DrawEngine::DrawFontStyle)(fontStyle | DrawEngine::DFS_BOLD);
						}
						else
						{
							printf("SVGDocument: Unknown font-weight in text: %s\r\n", sarr[0].Substring(12).v.Ptr());
						}
						styleFont = true;
					}
					else if (sarr[0].StartsWith(UTF8STRC("font-family:")))
					{
						OPTSTR_DEL(fontFamily);
						sarr[0].SubstrTrim(12);
						fontFamily = Text::String::New(sarr[0].Substring(12).ToCString());
						styleFont = true;
					}
					else if (sarr[0].StartsWith(UTF8STRC("font-variant:")))
					{
						OPTSTR_DEL(fontVariant);
						sarr[0].SubstrTrim(13);
						fontVariant = Text::String::New(sarr[0].Substring(13).ToCString());
						styleFont = true;
					}
					else if (sarr[0].StartsWith(UTF8STRC("font-stretch:")))
					{
						OPTSTR_DEL(fontStretch);
						sarr[0].SubstrTrim(13);
						fontStretch = Text::String::New(sarr[0].Substring(13).ToCString());
						styleFont = true;
					}
					else if (sarr[0].StartsWith(UTF8STRC("-inkscape-font-specification:")))
					{
						OPTSTR_DEL(inkscapeFont);
						sarr[0].SubstrTrim(29);
						inkscapeFont = Text::String::New(sarr[0].Substring(29).ToCString());
						styleFont = true;
					}
					else if (sarr[0].StartsWith(UTF8STRC("font-size:")))
					{
						OPTSTR_DEL(fontSize);
						sarr[0].SubstrTrim(10);
						fontSize = Text::String::New(sarr[0].Substring(10).ToCString());
						styleFont = true;
					}
					else if (sarr[0].StartsWith(UTF8STRC("text-anchor:")))
					{
						OPTSTR_DEL(textAnchor);
						sarr[0].SubstrTrim(12);
						textAnchor = Text::String::New(sarr[0].Substring(12).ToCString());
					}
					else if (sarr[0].StartsWith(UTF8STRC("writing-mode:")))
					{
						OPTSTR_DEL(writingMode);
						sarr[0].SubstrTrim(13);
						writingMode = Text::String::New(sarr[0].Substring(13).ToCString());
					}
					else if (sarr[0].StartsWith(UTF8STRC("direction:")))
					{
						OPTSTR_DEL(direction);
						sarr[0].SubstrTrim(10);
						direction = Text::String::New(sarr[0].Substring(10).ToCString());
					}
					else if (sarr[0].StartsWith(UTF8STRC("text-align:")))
					{
						OPTSTR_DEL(textAlign);
						sarr[0].SubstrTrim(11);
						textAlign = Text::String::New(sarr[0].Substring(11).ToCString());
					}
					else if (sarr[0].StartsWith(UTF8STRC("display:")))
					{
						sarr[0].SubstrTrim(8);
						display = Text::String::New(sarr[0].Substring(8).ToCString());
					}
					else if (sarr[0].StartsWith(UTF8STRC("line-height:")))
					{
						OPTSTR_DEL(lineHeight);
						sarr[0].SubstrTrim(12);
						lineHeight = Text::String::New(sarr[0].Substring(12).ToCString());
					}
					else if (sarr[0].StartsWith(UTF8STRC("shape-inside:")))
					{
						OPTSTR_DEL(shapeInside);
						sarr[0].SubstrTrim(13);
						shapeInside = Text::String::New(sarr[0].Substring(13).ToCString());
					}
					else if (sarr[0].StartsWith(UTF8STRC("white-space:")))
					{
						OPTSTR_DEL(whiteSpace);
						sarr[0].SubstrTrim(11);
						whiteSpace = Text::String::New(sarr[0].Substring(11).ToCString());
					}
					else if (sarr[0].StartsWith(UTF8STRC("stroke-dasharray:")))
					{
						OPTSTR_DEL(strokeDasharray);
						sarr[0].SubstrTrim(17);
						strokeDasharray = Text::String::New(sarr[0].Substring(17).ToCString());
					}
					else if (sarr[0].StartsWith(UTF8STRC("shape-padding:")))
					{
						OPTSTR_DEL(shapePadding);
						sarr[0].SubstrTrim(14);
						shapePadding = Text::String::New(sarr[0].Substring(14).ToCString());
					}
					else if (sarr[0].StartsWith(UTF8STRC("mix-blend-mode:")))
					{
						OPTSTR_DEL(mixBlendMode);
						sarr[0].SubstrTrim(15);
						mixBlendMode = Text::String::New(sarr[0].Substring(15).ToCString());
					}
					else
					{
						printf("SVGDocument: Unknown style in text: %s\r\n", sarr[0].v.Ptr());
					}
					if (i != 2)
					{
						break;
					}
				}
			}
			else
			{
				printf("SVGDocument: Unknown attribute in text: %s\r\n", name->v.Ptr());
			}
		}
		else
		{
			printf("SVGDocument: Invalid attribute in text\r\n");
		}
		i++;
	}
	if (reader->IsElementEmpty())
	{
		printf("SVGDocument: Empty text element is not supported\r\n");
	}
	else if (fill.IsNull() || (fontSize.IsNull() && fontFamily.IsNull()))
	{
		printf("SVGDocument: Text element with missing fill or font-size/font-family is not supported\r\n");
		reader->SkipElement();
	}
	else
	{
		Double fontSizePx = 16;
		if (fontSize.SetTo(value))
		{
			fontSizePx = Text::CSSCore::FontSizeToPx(value->ToCString(), fontSizePx);
		}
		NN<SVGTextComponent> component;
		Data::ArrayListNN<SVGTextComponent> textComponents;
		while (reader->ReadNext())
		{
			if (reader->GetNodeType() == Text::XMLNode::NodeType::Text)
			{
				NEW_CLASSNN(component, SVGStaticText(reader->GetNodeTextNN()->ToCString()));
				textComponents.Add(component);
			}
			else if (reader->GetNodeType() == Text::XMLNode::NodeType::Element)
			{
				if (reader->GetNodeTextNN()->Equals(UTF8STRC("tspan")))
				{
					NN<SVGTSpan> tspan;
					if (ParseTSpan(container, reader, fontSizePx).SetTo(tspan))
					{
						textComponents.Add(tspan);
					}
				}
				else
				{
					printf("SVGDocument: Child element in text element is not supported: %s\r\n", reader->GetNodeTextNN()->v.Ptr());
					reader->SkipElement();
				}
			}
			else if (reader->GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
			{
				break;
			}
			else
			{
				printf("SVGDocument: Unknown node type in text element\r\n");
			}
		}
		if (textComponents.GetCount() == 0)
		{
			printf("SVGDocument: Text element with no text content is not supported\r\n");
		}
		else
		{
			NN<SVGBrush> nnbrush;
			NN<SVGFont> nnfont;
			NN<SVGText> text;
			value = Text::String::OrEmpty(fill);
			nnbrush = NN<SVGBrush>::ConvertFrom(container->NewBrushARGB(Text::CSSCore::ParseColor(value->ToCString(), fillOpacity)));
			nnbrush->SetColorName(value->ToCString());
			nnfont = NN<SVGFont>::ConvertFrom(container->NewFontPx(Text::String::OrEmpty(fontFamily)->ToCString(), fontSizePx, fontStyle, 0));
			if (fontSize.SetTo(value))
			{
				nnfont->SetFontSizeStr(value->ToCString());
			}
			if (inkscapeFont.SetTo(value))
			{
				nnfont->SetInkscapeFont(value->ToCString());
			}
			if (fontVariant.SetTo(value))
			{
				nnfont->SetFontVariant(value->ToCString());
			}
			if (fontStretch.SetTo(value))
			{
				nnfont->SetFontStretch(value->ToCString());
			}
			NEW_CLASSNN(text, Media::SVGText(container, Math::Coord2DDbl(x, y), nnfont, nnbrush, textComponents.GetItemNoCheck(0)));
			text->SetSpacePreserve(spacePreserve);
			if (id.SetTo(value))
			{
				text->SetID(value->ToCString());
			}
			if (stroke.SetTo(value))
			{
				Double strokeWidthPx = strokeWidth * container->GetDoc()->GetHDrawScale();
				text->SetPen(stylePen, NN<SVGPen>::ConvertFrom(container->NewPenARGB(Text::CSSCore::ParseColor(value->ToCString(), strokeOpacity), strokeWidthPx, nullptr, 0)));
			}
			text->SetBrush(styleBrush, nnbrush);
			text->SetFont(styleFont, nnfont);
			text->SetInsensitive(insensitive);
			if (writingMode.SetTo(value))
			{
				text->SetWritingMode(value->ToCString());
			}
			if (direction.SetTo(value))
			{
				text->SetDirection(value->ToCString());
			}
			if (textAnchor.SetTo(value))
			{
				text->SetTextAnchor(value->ToCString());
			}
			if (textAlign.SetTo(value))
			{
				text->SetTextAlign(value->ToCString());
			}
			if (transform.SetTo(value))
			{
				text->SetTransform(value->ToCString());
			}
			if (inkscapeLabel.SetTo(value))
			{
				text->SetInkscapeLabel(value);
			}
			if (display.SetTo(value))
			{
				text->SetDisplay(value->ToCString());
			}
			if (lineHeight.SetTo(value))
			{
				text->SetLineHeight(value->ToCString());
			}
			if (shapeInside.SetTo(value))
			{
				text->SetShapeInside(value->ToCString());
			}
			if (strokeDasharray.SetTo(value))
			{
				text->SetStrokeDasharray(value->ToCString());
			}
			if (whiteSpace.SetTo(value))
			{
				text->SetWhiteSpace(value->ToCString());
			}
			if (shapePadding.SetTo(value))
			{
				text->SetShapePadding(value->ToCString());
			}
			if (mixBlendMode.SetTo(value))
			{
				text->SetMixBlendMode(value->ToCString());
			}
			i = 1;
			j = textComponents.GetCount();
			while (i < j)
			{
				text->AddTextComponent(textComponents.GetItemNoCheck(i));
				i++;
			}
			container->AddElement(text);
		}
	}
	OPTSTR_DEL(id);
	OPTSTR_DEL(fill);
	OPTSTR_DEL(stroke);
	OPTSTR_DEL(fontFamily);
	OPTSTR_DEL(fontVariant);
	OPTSTR_DEL(fontStretch);
	OPTSTR_DEL(inkscapeFont);
	OPTSTR_DEL(fontSize);
	OPTSTR_DEL(textAnchor);
	OPTSTR_DEL(writingMode);
	OPTSTR_DEL(direction);
	OPTSTR_DEL(textAlign);
	OPTSTR_DEL(transform);
	OPTSTR_DEL(inkscapeLabel);
	OPTSTR_DEL(display);
	OPTSTR_DEL(lineHeight);
	OPTSTR_DEL(shapeInside);
	OPTSTR_DEL(whiteSpace);
	OPTSTR_DEL(strokeDasharray);
	OPTSTR_DEL(shapePadding);
	OPTSTR_DEL(mixBlendMode);
	return true;
}

Optional<Media::SVGTSpan> Media::SVGDocument::ParseTSpan(NN<SVGContainer> container, NN<Text::XMLReader> reader, Double parentFontSize)
{
	UIntOS i = 0;
	UIntOS j = reader->GetAttribCount();
	NN<Text::XMLAttrib> attr;
	NN<Text::String> name;
	NN<Text::String> value;
	Optional<Text::String> id = nullptr;
	Double x = NAN;
	Double y = NAN;
	Bool stylePen = false;
	Bool styleBrush = false;
	Bool styleFont = false;
	Optional<Text::String> fill = nullptr;
	Optional<Text::String> stroke = nullptr;
	Double fillOpacity = 1.0;
	Double strokeWidth = NAN;
	Double strokeOpacity = 1.0;
	DrawEngine::DrawFontStyle fontStyle = DrawEngine::DFS_NORMAL;
	Optional<Text::String> fontFamily = nullptr;
	Optional<Text::String> fontVariant = nullptr;
	Optional<Text::String> fontStretch = nullptr;
	Optional<Text::String> inkscapeFont = nullptr;
	Optional<Text::String> fontSize = nullptr;
	Optional<Text::String> sodipodiRole = nullptr;

	while (i < j)
	{
		attr = reader->GetAttribNoCheck(i);
		if (attr->name.SetTo(name) && attr->value.SetTo(value))
		{
			if (name->Equals(UTF8STRC("sodipodi:role")))
			{
				OPTSTR_DEL(sodipodiRole);
				sodipodiRole = value->Clone();
			}
			else if (name->Equals(UTF8STRC("id")))
			{
				OPTSTR_DEL(id);
				id = value->Clone();
			}
			else if (name->Equals(UTF8STRC("x")))
			{
				if (!value->ToDouble(x))
				{
					printf("SVGDocument: Invalid x value in text: %s\r\n", value->v.Ptr());
					x = NAN;
				}
			}
			else if (name->Equals(UTF8STRC("y")))
			{
				if (!value->ToDouble(y))
				{
					printf("SVGDocument: Invalid y value in text: %s\r\n", value->v.Ptr());
					y = NAN;
				}
			}
			else if (name->Equals(UTF8STRC("style")))
			{
				Text::PString sarr[2];
				UIntOS i;
				Text::StringBuilderUTF8 sb;
				sb.Append(value);
				sarr[1] = sb;
				while (true)
				{
					i = Text::StrSplitTrimP(sarr, 2, sarr[1], ';');
					if (sarr[0].StartsWith(UTF8STRC("fill:")))
					{
						OPTSTR_DEL(fill);
						sarr[0].SubstrTrim(5);
						fill = Text::String::New(sarr[0].Substring(5).ToCString());
						styleBrush = true;
					}
					else if (sarr[0].StartsWith(UTF8STRC("fill-opacity:")))
					{
						sarr[0].SubstrTrim(13);
						if (!sarr[0].Substring(13).ToDouble(fillOpacity))
						{
							printf("SVGDocument: Invalid fill-opacity value in text style: %s\r\n", &sarr[0].v[13]);
							fillOpacity = NAN;
						}
					}
					else if (sarr[0].StartsWith(UTF8STRC("stroke:")))
					{
						OPTSTR_DEL(stroke);
						sarr[0].SubstrTrim(7);
						stroke = Text::String::New(sarr[0].Substring(7).ToCString());
						stylePen = true;
					}
					else if (sarr[0].StartsWith(UTF8STRC("stroke-width:")))
					{
						sarr[0].SubstrTrim(13);
						if (!sarr[0].Substring(13).ToDouble(strokeWidth))
						{
							printf("SVGDocument: Invalid stroke-width value in text style: %s\r\n", &sarr[0].v[13]);
							strokeWidth = 0;
						}
					}
					else if (sarr[0].StartsWith(UTF8STRC("font-style:")))
					{
						sarr[0].SubstrTrim(11);
						if (sarr[0].Substring(11).Equals(UTF8STRC("normal")))
						{
						}
						else if (sarr[0].Substring(11).Equals(UTF8STRC("italic")))
						{
							fontStyle = (DrawEngine::DrawFontStyle)(fontStyle | DrawEngine::DFS_ITALIC);
						}
						else
						{
							printf("SVGDocument: Unknown font-style in text: %s\r\n", sarr[0].Substring(11).v.Ptr());
						}
						styleFont = true;
					}
					else if (sarr[0].StartsWith(UTF8STRC("font-weight:")))
					{
						sarr[0].SubstrTrim(12);
						if (sarr[0].Substring(12).Equals(UTF8STRC("normal")))
						{
						}
						else if (sarr[0].Substring(12).Equals(UTF8STRC("bold")))
						{
							fontStyle = (DrawEngine::DrawFontStyle)(fontStyle | DrawEngine::DFS_BOLD);
						}
						else
						{
							printf("SVGDocument: Unknown font-weight in text: %s\r\n", sarr[0].Substring(12).v.Ptr());
						}
						styleFont = true;
					}
					else if (sarr[0].StartsWith(UTF8STRC("font-family:")))
					{
						OPTSTR_DEL(fontFamily);
						sarr[0].SubstrTrim(12);
						fontFamily = Text::String::New(sarr[0].Substring(12).ToCString());
						styleFont = true;
					}
					else if (sarr[0].StartsWith(UTF8STRC("font-variant:")))
					{
						OPTSTR_DEL(fontVariant);
						sarr[0].SubstrTrim(13);
						fontVariant = Text::String::New(sarr[0].Substring(13).ToCString());
						styleFont = true;
					}
					else if (sarr[0].StartsWith(UTF8STRC("font-stretch:")))
					{
						OPTSTR_DEL(fontStretch);
						sarr[0].SubstrTrim(13);
						fontStretch = Text::String::New(sarr[0].Substring(13).ToCString());
						styleFont = true;
					}
					else if (sarr[0].StartsWith(UTF8STRC("-inkscape-font-specification:")))
					{
						OPTSTR_DEL(inkscapeFont);
						sarr[0].SubstrTrim(29);
						inkscapeFont = Text::String::New(sarr[0].Substring(29).ToCString());
						styleFont = true;
					}
					else if (sarr[0].StartsWith(UTF8STRC("font-size:")))
					{
						OPTSTR_DEL(fontSize);
						sarr[0].SubstrTrim(10);
						fontSize = Text::String::New(sarr[0].Substring(10).ToCString());
						styleFont = true;
					}
					else
					{
						printf("SVGDocument: Unknown style in tspan: %s\r\n", sarr[0].v.Ptr());
					}
					if (i != 2)
					{
						break;
					}
				}
			}
			else
			{
				printf("SVGDocument: Unknown attribute in tspan: %s\r\n", name->v.Ptr());
			}
		}
		else
		{
			printf("SVGDocument: Invalid attribute in tspan\r\n");
		}
		i++;
	}
	Text::StringBuilderUTF8 sb;
	reader->ReadNodeText(sb);
	NN<SVGTSpan> tspan;
	NEW_CLASSNN(tspan, SVGTSpan(sb.ToCString()));
	if (!Math::IsNAN(x) && !Math::IsNAN(y))
	{
		tspan->SetOffset(Math::Coord2DDbl(x, y));
	}
	if (id.SetTo(value))
	{
		tspan->SetID(value->ToCString());
	}
	if (sodipodiRole.SetTo(value))
	{
		tspan->SetSodipodiRole(value->ToCString());
	}
	if (fill.SetTo(value))
	{
		NN<SVGBrush> nnbrush;
		UInt32 argb = Text::CSSCore::ParseColor(value->ToCString(), fillOpacity);
		NEW_CLASSNN(nnbrush, SVGBrush(argb));
		nnbrush->SetColorName(value->ToCString());
		tspan->SetBrush(styleBrush, nnbrush);
		styleBrush = true;
	}
	if (stroke.SetTo(value))
	{
		NN<SVGPen> nnpen;
		NEW_CLASSNN(nnpen, SVGPen(strokeWidth * container->GetDoc()->GetHDrawScale(),Text::CSSCore::ParseColor(value->ToCString(), strokeOpacity)));
		nnpen->SetColorName(value->ToCString());
		tspan->SetPen(stylePen, nnpen);
		stylePen = true;
	}
	if (fontFamily.NotNull() || fontSize.NotNull())
	{
		Double fontSizePx = 16;
		if (fontSize.SetTo(value))
		{
			fontSizePx = Text::CSSCore::FontSizeToPx(value->ToCString(), parentFontSize);
		}
		NN<SVGFont> nnfont = NN<SVGFont>::ConvertFrom(container->GetDoc()->NewFontPx(Text::String::OrEmpty(fontFamily)->ToCString(), fontSizePx, fontStyle, 0));
		if (fontSize.SetTo(value))
		{
			nnfont->SetFontSizeStr(value->ToCString());
		}
		if (fontVariant.SetTo(value))
		{
			nnfont->SetFontVariant(value->ToCString());
		}
		if (fontStretch.SetTo(value))
		{
			nnfont->SetFontStretch(value->ToCString());
		}
		if (inkscapeFont.SetTo(value))
		{
			nnfont->SetInkscapeFont(value->ToCString());
		}
		tspan->SetFont(styleFont, nnfont);
	}
	OPTSTR_DEL(id);
	OPTSTR_DEL(fill);
	OPTSTR_DEL(stroke);
	OPTSTR_DEL(fontFamily);
	OPTSTR_DEL(fontVariant);
	OPTSTR_DEL(fontStretch);
	OPTSTR_DEL(inkscapeFont);
	OPTSTR_DEL(fontSize);
	return tspan;
}

Bool Media::SVGDocument::ParseImage(NN<SVGContainer> container, NN<Text::XMLReader> reader)
{
	if (!reader->IsElementEmpty())
	{
		printf("SVGDocument: Non-empty image element is not supported\r\n");
		return false;
	}
	UIntOS i = 0;
	UIntOS j = reader->GetAttribCount();
	NN<Text::XMLAttrib> attr;
	NN<Text::String> name;
	NN<Text::String> value;
	Double x = NAN;
	Double y = NAN;
	Double width = NAN;
	Double height = NAN;
	Bool insensitive = false;
	Optional<Text::String> id = nullptr;
	Optional<Text::String> href = nullptr;
	Optional<Text::String> inkscapeLabel = nullptr;
	Optional<Text::String> preserveAspectRatio = nullptr;
	Optional<Text::String> style = nullptr;
	while (i < j)
	{
		attr = reader->GetAttribNoCheck(i);
		if (attr->name.SetTo(name) && attr->value.SetTo(value))
		{
			if (name->Equals(UTF8STRC("xlink:href")))
			{
				OPTSTR_DEL(href);
				href = value->Clone();
			}
			else if (name->Equals(UTF8STRC("id")))
			{
				OPTSTR_DEL(id);
				id = value->Clone();
			}
			else if (name->Equals(UTF8STRC("x")))
			{
				if (!value->ToDouble(x))
				{
					printf("SVGDocument: Invalid x value in image: %s\r\n", value->v.Ptr());
					x = NAN;
				}
			}
			else if (name->Equals(UTF8STRC("y")))
			{
				if (!value->ToDouble(y))
				{
					printf("SVGDocument: Invalid y value in image: %s\r\n", value->v.Ptr());
					y = NAN;
				}
			}
			else if (name->Equals(UTF8STRC("width")))
			{
				if (!value->ToDouble(width))
				{
					printf("SVGDocument: Invalid width value in image: %s\r\n", value->v.Ptr());
					width = NAN;
				}
			}
			else if (name->Equals(UTF8STRC("height")))
			{
				if (!value->ToDouble(height))
				{
					printf("SVGDocument: Invalid height value in image: %s\r\n", value->v.Ptr());
					height = NAN;
				}
			}
			else if (name->Equals(UTF8STRC("inkscape:label")))
			{
				OPTSTR_DEL(inkscapeLabel);
				inkscapeLabel = value->Clone();
			}
			else if (name->Equals(UTF8STRC("sodipodi:insensitive")))
			{
				if (value->Equals(UTF8STRC("true")))
				{
					insensitive = true;
				}
				else
				{
					printf("SVGDocument: Invalid sodipodi:insensitive value in image: %s\r\n", value->v.Ptr());
				}
			}
			else if (name->Equals(UTF8STRC("preserveAspectRatio")))
			{
				OPTSTR_DEL(preserveAspectRatio);
				preserveAspectRatio = value->Clone();
			}
			else if (name->Equals(UTF8STRC("style")))
			{
				OPTSTR_DEL(style);
				style = value->Clone();
			}
			else
			{
				printf("SVGDocument: Unknown attribute in image: %s\r\n", name->v.Ptr());
			}
		}
		else
		{
			printf("SVGDocument: Invalid attribute in image\r\n");
		}
		i++;
	}
	NN<Text::String> s;
	if (!href.SetTo(s) || Math::IsNAN(width) || Math::IsNAN(height) || Math::IsNAN(x) || Math::IsNAN(y))
	{
		OPTSTR_DEL(id);
		OPTSTR_DEL(href);
		OPTSTR_DEL(inkscapeLabel);
		OPTSTR_DEL(preserveAspectRatio);
		OPTSTR_DEL(style);
		printf("SVGDocument: Missing attributes in image\r\n");
		return false;
	}
	NN<SVGImage> image;
	NEW_CLASSNN(image, SVGImage(container, Math::Coord2DDbl(x, y), Math::Size2DDbl(width, height), s->ToCString()));
	if (id.SetTo(s))
	{
		image->SetID(s->ToCString());
	}
	if (inkscapeLabel.SetTo(s))
	{
		image->SetInkscapeLabel(s);
	}
	if (preserveAspectRatio.SetTo(s))
	{
		image->SetPreserveAspectRatio(s->ToCString());
	}
	if (style.SetTo(s))
	{
		image->SetStyle(s->ToCString());
	}
	image->SetInsensitive(insensitive);
	container->AddElement(image);
	OPTSTR_DEL(id);
	OPTSTR_DEL(href);
	OPTSTR_DEL(inkscapeLabel);
	OPTSTR_DEL(preserveAspectRatio);
	OPTSTR_DEL(style);
	reader->SkipElement();
	return true;
}

Bool Media::SVGDocument::ParseTitle(NN<SVGContainer> container, NN<Text::XMLReader> reader, Text::CStringNN eleName)
{
	Optional<Text::String> id = nullptr;
	UIntOS i = 0;
	UIntOS j = reader->GetAttribCount();
	NN<Text::XMLAttrib> attr;
	NN<Text::String> name;
	NN<Text::String> value;
	while (i < j)
	{
		attr = reader->GetAttribNoCheck(i);
		if (attr->name.SetTo(name) && attr->value.SetTo(value))
		{
			if (name->Equals(UTF8STRC("id")))
			{
				OPTSTR_DEL(id);
				id = value->Clone();
			}
			else
			{
				printf("SVGDocument: Unknown attribute in title: %s\r\n", name->v.Ptr());
			}
		}
		else
		{
			printf("SVGDocument: Invalid attribute in title\r\n");
		}
		i++;
	}
	Text::StringBuilderUTF8 sb;
	if (!reader->ReadNodeText(sb))
	{
		printf("SVGDocument: Failed to read title text\r\n");
		OPTSTR_DEL(id);
		return false;
	}
	else
	{
		NN<SVGTitle> title;
		NEW_CLASSNN(title, SVGTitle(container, eleName, sb.ToCString()));
		if (id.SetTo(value))
		{
			title->SetID(value->ToCString());
		}
		container->AddElement(title);
		OPTSTR_DEL(id);
		return true;
	}
}

Bool Media::SVGDocument::ParseUnknown(NN<SVGContainer> container, NN<Text::XMLReader> reader)
{
	if (!reader->IsElementEmpty())
	{
		return ParseUnknownContainer(container, reader);
	}
	NN<SVGUnknown> unknown;
	NEW_CLASSNN(unknown, SVGUnknown(container, reader->GetElementNameWithNS()));
	UIntOS i = 0;
	UIntOS j = reader->GetAttribCount();
	NN<Text::XMLAttrib> attr;
	NN<Text::String> name;
	NN<Text::String> value;
	while (i < j)
	{
		attr = reader->GetAttribNoCheck(i);
		if (attr->name.SetTo(name) && attr->value.SetTo(value))
		{
			unknown->AddAttr(name->ToCString(), value->ToCString());
			if (name->Equals(UTF8STRC("id")))
			{
				unknown->SetID(value->ToCString());
			}
		}
		i++;
	}
	container->AddElement(unknown);
	return true;
}

Bool Media::SVGDocument::ParseUnknownContainer(NN<SVGContainer> container, NN<Text::XMLReader> reader)
{
	NN<SVGUnknownContainer> unknown;
	NEW_CLASSNN(unknown, SVGUnknownContainer(container, container->GetDrawEngine(), container->GetDoc(), reader->GetElementNameWithNS()));
	UIntOS i = 0;
	UIntOS j = reader->GetAttribCount();
	NN<Text::XMLAttrib> attr;
	NN<Text::String> name;
	NN<Text::String> value;
	while (i < j)
	{
		attr = reader->GetAttribNoCheck(i);
		if (attr->name.SetTo(name) && attr->value.SetTo(value))
		{
			unknown->AddAttr(name->ToCString(), value->ToCString());
		}
		i++;
	}
	Bool succ = ParseContainerAttr(unknown, reader, true);
	container->AddElement(unknown);
	succ = succ && ParseContainer(unknown, reader);
	return succ;

}
