#include "Stdafx.h"
#include "Exporter/PNGExporter.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "Media/StaticImage.h"
#include "Media/SVGDocument.h"
#include "Text/CSSCore.h"
#include "Text/XML.h"
#include "Text/TextBinEnc/Base64Enc.h"

void Media::SVGElement::AppendAttrs(NN<Text::StringBuilderUTF8> sb, UIntOS level) const
{
	NN<Text::String> s;
	NN<Text::String> attrName;
	UIntOS i = 0;
	UIntOS j = this->attrNames.GetCount();
	while (i < j)
	{
		attrName = this->attrNames.GetItemNoCheck(i);
		if (attrName->Equals(CSTR("style")))
		{
			sb->AppendUTF8Char('\n');
			sb->AppendChar(' ', (level << 1) + 1);
			sb->Append(attrName);
			sb->AppendUTF8Char('=');
			sb->AppendUTF8Char('"');
			UIntOS k = 0;
			UIntOS l = this->style.GetCount();
			while (k < l)
			{
				NN<Text::String> styleName;
				if (this->style.GetKey(k).SetTo(styleName))
				{
					NN<Text::String> styleVal = this->style.GetItemNoCheck(k);
					if (k > 0)
					{
						sb->AppendUTF8Char(';');
					}
					sb->Append(styleName);
					sb->AppendUTF8Char(':');
					sb->Append(styleVal);
				}
				k++;
			}
			sb->AppendUTF8Char('"');
		}
		else
		{
			sb->AppendUTF8Char('\n');
			sb->AppendChar(' ', (level << 1) + 1);
			sb->Append(attrName);
			if (this->attr.Get(attrName).SetTo(s))
			{
				sb->AppendUTF8Char('=');
				s = Text::XML::ToNewAttrText(s->v);
				sb->Append(s);
				s->Release();
			}
		}
		i++;
	}

}

void Media::SVGElement::CloneAttrs(NN<const SVGElement> ele)
{
	NN<Text::String> s;
	UIntOS i = 0;
	UIntOS j = ele->attrNames.GetCount();
	while (i < j)
	{
		this->attrNames.Add(ele->attrNames.GetItemNoCheck(i)->Clone());
		i++;
	}
	i = 0;
	j = ele->attr.GetCount();
	while (i < j)
	{
		if (ele->attr.GetKey(i).SetTo(s))
		{
			this->attr.PutNN(s, ele->attr.GetItemNoCheck(i)->Clone());
		}
		i++;
	}
	i = 0;
	j = ele->style.GetCount();
	while (i < j)
	{
		if (ele->style.GetKey(i).SetTo(s))
		{
			this->style.PutNN(s, ele->style.GetItemNoCheck(i)->Clone());
		}
		i++;
	}
}

Bool Media::SVGElement::IsSpacePreserve() const
{
	NN<const SVGContainer> nnparent;
	NN<Text::String> s;
	if (this->GetAttr(CSTR("xml:space")).SetTo(s))
		return s->Equals(CSTR("preserve"));
	if (this->parent.SetTo(nnparent))
		return nnparent->IsSpacePreserve();
	return false;
}

void Media::SVGElement::SetAttr(Text::CStringNN name, Text::CStringNN value)
{
	NN<Text::String> s;
	if (name.Equals(CSTR("style")))
	{
		if (this->style.GetCount() > 0)
		{
			NNLIST_FREE_STRING(&this->style);
		}
		if (this->attr.GetC(CSTR("style")).IsNull())
		{
			this->attrNames.Add(Text::String::New(CSTR("style")));
			this->attr.PutC(CSTR("style"), Text::String::NewEmpty());
		}
		if (value.leng > 0)
		{
			Text::StringBuilderUTF8 sb;
			sb.Append(value);
			Text::PString sarr[2];
			Text::PString sarr2[2];
			UIntOS i;
			sarr[1] = sb;
			while (true)
			{
				i = Text::StrSplitTrimP(sarr, 2, sarr[1], ';');
				if (Text::StrSplitTrimP(sarr2, 2, sarr[0], ':') == 2)
				{
					this->style.PutC(sarr2[0].ToCString(), Text::String::New(sarr2[1].ToCString()));
				}
				if (i != 2)
					break;
			}
		}
	}
	else
	{
		if (this->attr.PutC(name, Text::String::New(value)).SetTo(s))
		{
			s->Release();
		}
		else
		{
			this->attrNames.Add(Text::String::New(name));
		}
	}
}

void Media::SVGElement::SetAttr(Text::CStringNN name, NN<Text::String> value)
{
	if (name.Equals(CSTR("style")))
	{
		if (this->style.GetCount() > 0)
		{
			NNLIST_FREE_STRING(&this->style);
		}
		if (this->attr.GetC(CSTR("style")).IsNull())
		{
			this->attrNames.Add(Text::String::New(CSTR("style")));
			this->attr.PutC(CSTR("style"), Text::String::NewEmpty());
		}
		if (value->leng > 0)
		{
			Text::StringBuilderUTF8 sb;
			sb.Append(value);
			Text::PString sarr[2];
			Text::PString sarr2[2];
			UIntOS i;
			sarr[1] = sb;
			while (true)
			{
				i = Text::StrSplitTrimP(sarr, 2, sarr[1], ';');
				if (Text::StrSplitTrimP(sarr2, 2, sarr[0], ':') == 2)
				{
					this->style.PutC(sarr2[0].ToCString(), Text::String::New(sarr2[1].ToCString()));
				}
				if (i != 2)
					break;
			}
		}
	}
	else
	{
		if (this->attr.PutC(name, value->Clone()).SetTo(value))
		{
			value->Release();
		}
		else
		{
			this->attrNames.Add(Text::String::New(name));
		}
	}
}

Optional<Text::String> Media::SVGElement::GetAttr(Text::CStringNN name) const
{
	return this->attr.GetC(name);
}

void Media::SVGElement::SetStyle(Text::CStringNN name, Text::CStringNN value)
{
	NN<Text::String> s;
	if (this->attr.GetC(CSTR("style")).IsNull())
	{
		s = Text::String::New(CSTR("style"));
		this->attr.PutNN(s, Text::String::NewEmpty());
		this->attrNames.Add(s);
	}
	if (this->style.PutC(name, Text::String::New(value)).SetTo(s))
	{
		s->Release();
	}
}

void Media::SVGElement::SetPenStyle(NN<Media::DrawPen> pen)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	NN<Media::SVGPen> svgPen = NN<Media::SVGPen>::ConvertFrom(pen);
	NN<Text::String> colorName;
	UInt32 col = svgPen->GetColor();
	if (svgPen->GetColorName().SetTo(colorName))
	{
		this->SetStyle(CSTR("stroke"), colorName->ToCString());
	}
	else
	{
		sptr = sbuff;
		*sptr++ = '#';
		sptr = Text::StrHexVal24(sptr, col);
		this->SetStyle(CSTR("stroke"), CSTRP(sbuff, sptr));
	}
	if ((col & 0xff000000) != 0xff000000)
	{
		Double alpha = (col >> 24) / 255.0;
		sptr = Text::StrDouble(sbuff, alpha);
		this->SetStyle(CSTR("stroke-opacity"), CSTRP(sbuff, sptr));
	}
	Double thick = svgPen->GetThick();
	if (thick > 0)
	{
		sptr = Text::StrDouble(sbuff, thick);
		this->SetStyle(CSTR("stroke-width"), CSTRP(sbuff, sptr));
	}
}

void Media::SVGElement::SetBrushStyle(NN<Media::DrawBrush> brush)
{
	NN<SVGBrush> svgBrush = NN<SVGBrush>::ConvertFrom(brush);
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	NN<Text::String> colorName;
	UInt32 col = svgBrush->GetColor();
	if (svgBrush->GetColorName().SetTo(colorName))
	{
		this->SetStyle(CSTR("fill"), colorName->ToCString());
	}
	else
	{
		sptr = sbuff;
		*sptr++ = '#';
		sptr = Text::StrHexVal24(sptr, col);
		this->SetStyle(CSTR("fill"), CSTRP(sbuff, sptr));
	}
	if ((col & 0xff000000) != 0xff000000)
	{
		Double alpha = (col >> 24) / 255.0;
		sptr = Text::StrDouble(sbuff, alpha);
		this->SetStyle(CSTR("fill-opacity"), CSTRP(sbuff, sptr));
	}
	if (svgBrush->GetFillRule() == Media::SVGFillRule::EvenOdd)
	{
		this->SetStyle(CSTR("fill-rule"), CSTR("evenodd"));
	}
	else if (svgBrush->GetFillRule() == Media::SVGFillRule::NonZero)
	{
		this->SetStyle(CSTR("fill-rule"), CSTR("nonzero"));
	}
}

void Media::SVGElement::SetFontStyle(NN<Media::DrawFont> font)
{
	NN<Media::SVGFont> svgFont = NN<Media::SVGFont>::ConvertFrom(font);
	NN<Text::String> s;
	if (svgFont->GetFontName().SetTo(s))
	{
		this->SetStyle(CSTR("font-family"), s->ToCString());
	}
	else
	{
		this->SetStyle(CSTR("font-family"), CSTR("sans-serif"));
	}
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	if (svgFont->GetFontSizeStr().SetTo(s))
	{
		this->SetStyle(CSTR("font-size"), s->ToCString());
	}
	else
	{
		sptr = Text::StrDouble(sbuff, svgFont->GetFontSizePx());
		this->SetStyle(CSTR("font-size"), CSTRP(sbuff, sptr));
	}
	if (svgFont->GetStyle() & Media::DrawEngine::DFS_BOLD)
	{
		this->SetStyle(CSTR("font-weight"), CSTR("bold"));
	}
	else
	{
		this->SetStyle(CSTR("font-weight"), CSTR("normal"));
	}
	if (svgFont->GetStyle() & Media::DrawEngine::DFS_ITALIC)
	{
		this->SetStyle(CSTR("font-style"), CSTR("italic"));
	}
	else
	{
		this->SetStyle(CSTR("font-style"), CSTR("normal"));
	}
	if (svgFont->GetFontVariant().SetTo(s))
	{
		this->SetStyle(CSTR("font-variant"), s->ToCString());
	}
	if (svgFont->GetFontStretch().SetTo(s))
	{
		this->SetStyle(CSTR("font-stretch"), s->ToCString());
	}
	if (svgFont->GetInkscapeFont().SetTo(s))
	{
		this->SetStyle(CSTR("-inkscape-font-specification"), s->ToCString());
	}
}

Media::SVGLine::SVGLine(NN<const SVGContainer> parent, Math::Coord2DDbl pt1, Math::Coord2DDbl pt2, Bool createAttr) : SVGElement(parent), pt1(pt1), pt2(pt2)
{
	this->pt1 = pt1;
	this->pt2 = pt2;
	if (createAttr)
	{
		UTF8Char sbuff[64];
		UnsafeArray<UTF8Char> sptr;
		sptr = Text::StrDouble(sbuff, pt1.x);
		this->SetAttr(CSTR("x1"), CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, pt1.y);
		this->SetAttr(CSTR("y1"), CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, pt2.x);
		this->SetAttr(CSTR("x2"), CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, pt2.y);
		this->SetAttr(CSTR("y2"), CSTRP(sbuff, sptr));
	}
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
	this->AppendAttrs(sb, level + 1);
	sb->AppendC(UTF8STRC(" />"));
}

NN<Media::SVGElement> Media::SVGLine::CloneElement(NN<const SVGContainer> newParent) const
{
	NN<Media::SVGLine> newLine;
	NEW_CLASSNN(newLine, Media::SVGLine(newParent, this->pt1, this->pt2, false));
	newLine->CloneAttrs(*this);
	return newLine;
}

Media::SVGPolyline::SVGPolyline(NN<const SVGContainer> parent) : SVGElement(parent)
{
}

Media::SVGPolyline::~SVGPolyline()
{
}

void Media::SVGPolyline::AddPoint(Math::Coord2DDbl pt)
{
	if (this->points.GetCount() == 0)
	{
		this->attrNames.Add(Text::String::New(UTF8STRC("points")));
	}
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
	NN<Text::String> s;
	if (this->points.GetCount() > 0 && !this->GetAttr(CSTR("points")).SetTo(s))
	{
		Text::StringBuilderUTF8 sbPoints;
		UIntOS i = 1;
		sbPoints.AppendDouble(this->points.GetItem(0).x);
		sbPoints.AppendUTF8Char(',');
		sbPoints.AppendDouble(this->points.GetItem(0).y);
		while (i < this->points.GetCount())
		{
			sbPoints.AppendUTF8Char(' ');
			sbPoints.AppendDouble(this->points.GetItem(i).x);
			sbPoints.AppendUTF8Char(',');
			sbPoints.AppendDouble(this->points.GetItem(i).y);
			i++;
		}
		((SVGPolyline*)this)->attr.PutC(CSTR("points"), Text::String::New(sbPoints.ToCString()));
	}
	sb->AppendC(UTF8STRC("<polyline"));
	this->AppendAttrs(sb, level + 1);
	sb->AppendC(UTF8STRC(" />"));
}

NN<Media::SVGElement> Media::SVGPolyline::CloneElement(NN<const SVGContainer> newParent) const
{
	NN<Media::SVGPolyline> newPolyline;
	NEW_CLASSNN(newPolyline, Media::SVGPolyline(newParent));
	UIntOS i = 0;
	while (i < this->points.GetCount())
	{
		newPolyline->points.Add(this->points.GetItem(i));
		i++;
	}
	newPolyline->CloneAttrs(*this);
	return newPolyline;
}

Media::SVGPolygon::SVGPolygon(NN<const SVGContainer> parent) : SVGElement(parent)
{
}

Media::SVGPolygon::~SVGPolygon()
{
}

void Media::SVGPolygon::AddPoint(Math::Coord2DDbl pt)
{
	if (this->points.GetCount() == 0)
	{
		this->attrNames.Add(Text::String::New(UTF8STRC("points")));
	}
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
	NN<Text::String> s;
	if (this->points.GetCount() > 0 && !this->GetAttr(CSTR("points")).SetTo(s))
	{
		Text::StringBuilderUTF8 sbPoints;
		UIntOS i = 1;
		sbPoints.AppendDouble(this->points.GetItem(0).x);
		sbPoints.AppendUTF8Char(',');
		sbPoints.AppendDouble(this->points.GetItem(0).y);
		while (i < this->points.GetCount())
		{
			sbPoints.AppendUTF8Char(' ');
			sbPoints.AppendDouble(this->points.GetItem(i).x);
			sbPoints.AppendUTF8Char(',');
			sbPoints.AppendDouble(this->points.GetItem(i).y);
			i++;
		}
		((SVGPolygon*)this)->attr.PutC(CSTR("points"), Text::String::New(sbPoints.ToCString()));
	}
	sb->AppendC(UTF8STRC("<polygon"));
	this->AppendAttrs(sb, level + 1);
	sb->AppendC(UTF8STRC(" />"));
}

NN<Media::SVGElement> Media::SVGPolygon::CloneElement(NN<const SVGContainer> newParent) const
{
	NN<Media::SVGPolygon> newPolygon;
	NEW_CLASSNN(newPolygon, Media::SVGPolygon(newParent));
	UIntOS i = 0;
	while (i < this->points.GetCount())
	{
		newPolygon->AddPoint(this->points.GetItem(i));
		i++;
	}
	newPolygon->CloneAttrs(*this);
	return newPolygon;
}

Media::SVGRect::SVGRect(NN<const SVGContainer> parent, Math::Coord2DDbl tl, Math::Size2DDbl size, Bool createAttr) : SVGElement(parent)
{
	this->tl = tl;
	this->size = size;
	if (createAttr)
	{
		UTF8Char sbuff[64];
		UnsafeArray<UTF8Char> sptr;
		sptr = Text::StrDouble(sbuff, tl.x);
		this->SetAttr(CSTR("x"), CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, tl.y);
		this->SetAttr(CSTR("y"), CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, size.x);
		this->SetAttr(CSTR("width"), CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, size.y);
		this->SetAttr(CSTR("height"), CSTRP(sbuff, sptr));
	}
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
	this->AppendAttrs(sb, level + 1);
	sb->AppendC(UTF8STRC(" />"));
}

NN<Media::SVGElement> Media::SVGRect::CloneElement(NN<const SVGContainer> newParent) const
{
	NN<Media::SVGRect> newRect;
	NEW_CLASSNN(newRect, Media::SVGRect(newParent, this->tl, this->size, false));
	newRect->CloneAttrs(*this);
	return newRect;
}

Media::SVGEllipse::SVGEllipse(NN<const SVGContainer> parent, Math::Coord2DDbl center, Math::Size2DDbl radius, Bool createAttr) : SVGElement(parent)
{
	this->center = center;
	this->radius = radius;
	if (createAttr)
	{
		UTF8Char sbuff[64];
		UnsafeArray<UTF8Char> sptr;
		sptr = Text::StrDouble(sbuff, center.x);
		this->SetAttr(CSTR("cx"), CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, center.y);
		this->SetAttr(CSTR("cy"), CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, radius.x);
		this->SetAttr(CSTR("rx"), CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, radius.y);
		this->SetAttr(CSTR("ry"), CSTRP(sbuff, sptr));
	}
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
	this->AppendAttrs(sb, level + 1);
	sb->AppendC(UTF8STRC(" />"));
}

NN<Media::SVGElement> Media::SVGEllipse::CloneElement(NN<const SVGContainer> newParent) const
{
	NN<Media::SVGEllipse> newEllipse;
	NEW_CLASSNN(newEllipse, Media::SVGEllipse(newParent, this->center, this->radius, false));
	newEllipse->CloneAttrs(*this);
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

Media::SVGText::SVGText(NN<const SVGContainer> parent, NN<SVGTextComponent> component, Math::Coord2DDbl tl, Bool createAttr) : SVGElement(parent)
{
	this->tl = tl;
	this->components.Add(component);
	if (createAttr)
	{
		UTF8Char sbuff[64];
		UnsafeArray<UTF8Char> sptr;
		sptr = Text::StrDouble(sbuff, tl.x);
		this->SetAttr(CSTR("x"), CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, tl.y);
		this->SetAttr(CSTR("y"), CSTRP(sbuff, sptr));
	}
}

Media::SVGText::~SVGText()
{
	this->components.DeleteAll();
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
	this->SetAttr(CSTR("transform"), sb.ToCString());
}

void Media::SVGText::DrawElement(Math::Coord2DDbl ofst, Math::Size2DDbl scale, NN<Media::DrawImage> dimg)
{
	////////////////////////////////////////////////
}

void Media::SVGText::ToString(NN<Text::StringBuilderUTF8> sb, UIntOS level) const
{
	if (!this->IsSpacePreserve())
	{
		sb->AppendChar(' ', level << 1);
	}
	sb->AppendC(UTF8STRC("<text"));
	this->AppendAttrs(sb, level + 1);
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
	NN<SVGText> newText;
	NEW_CLASSNN(newText, Media::SVGText(newParent, this->components.GetItemNoCheck(0)->Clone(newParent), this->tl, false));
	UIntOS i = 1;
	while (i < this->components.GetCount())
	{
		newText->AddTextComponent(this->components.GetItemNoCheck(i)->Clone(newParent));
		i++;
	}
	newText->CloneAttrs(*this);
	return newText;
}

Media::SVGImage::SVGImage(NN<const SVGContainer> parent, Math::Coord2DDbl tl, Math::Size2DDbl size, Text::CStringNN href, Bool createAttr) : SVGElement(parent)
{
	this->tl = tl;
	this->size = size;
	if (createAttr)
	{
		UTF8Char sbuff[64];
		UnsafeArray<UTF8Char> sptr;
		sptr = Text::StrDouble(sbuff, tl.x);
		this->SetAttr(CSTR("x"), CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, tl.y);
		this->SetAttr(CSTR("y"), CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, size.x);
		this->SetAttr(CSTR("width"), CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, size.y);
		this->SetAttr(CSTR("height"), CSTRP(sbuff, sptr));
		this->SetAttr(CSTR("xlink:href"), href);
	}
}

Media::SVGImage::~SVGImage()
{
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
	this->AppendAttrs(sb, level + 1);
	sb->AppendC(UTF8STRC(" />"));
}

NN<Media::SVGElement> Media::SVGImage::CloneElement(NN<const SVGContainer> newParent) const
{
	NN<Media::SVGImage> newImage;
	NEW_CLASSNN(newImage, Media::SVGImage(newParent, this->tl, this->size, Text::String::OrEmpty(this->GetAttr(CSTR("xlink:href")))->ToCString(), false));
	newImage->CloneAttrs(*this);
	return newImage;
}

Media::SVGPath::SVGPath(NN<const SVGContainer> parent) : SVGElement(parent)
{
}

Media::SVGPath::~SVGPath()
{
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
	this->AppendAttrs(sb, level + 1);
	sb->AppendC(UTF8STRC(" />"));
}

NN<Media::SVGElement> Media::SVGPath::CloneElement(NN<const SVGContainer> newParent) const
{
	NN<Media::SVGPath> newPath;
	NEW_CLASSNN(newPath, Media::SVGPath(newParent));
	newPath->CloneAttrs(*this);
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
	newTitle->CloneAttrs(*this);
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
	this->CloneAttrs(fromContainer);
	this->CloneElements(fromContainer);
}

Media::SVGContainer::SVGContainer(Optional<const SVGContainer> parent, NN<Media::DrawEngine> refEng, NN<SVGDocument> doc) : SVGElement(parent)
{
	this->refEng = refEng;
	this->doc = doc;
	this->drawRect = Math::RectAreaDbl(0, 0, 0, 0);
}

Media::SVGContainer::~SVGContainer()
{
	this->elements.DeleteAll();
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

Bool Media::SVGContainer::IsPixelDraw() const
{
	return false;
}

Bool Media::SVGContainer::DrawLine(Double x1, Double y1, Double x2, Double y2, NN<DrawPen> p)
{
	Math::Coord2DDbl scale = this->doc->GetDrawScale();
	NN<SVGLine> line;
	NEW_CLASSNN(line, SVGLine(*this, this->drawRect.min + Math::Coord2DDbl(x1, y1) / scale, this->drawRect.min + Math::Coord2DDbl(x2, y2) / scale, true));
	line->SetPenStyle(p);
	line->SetAttr(CSTR("fill"), CSTR("none"));
	this->elements.Add(line);
	return true;
}

Bool Media::SVGContainer::DrawPolylineI(UnsafeArray<const Int32> points, UIntOS nPoints, NN<DrawPen> p)
{
	Math::Coord2DDbl scale = this->doc->GetDrawScale();
	NN<SVGPolyline> pl;
	NEW_CLASSNN(pl, SVGPolyline(*this));
	UIntOS i = 0;
	while (i < nPoints * 2)
	{
		pl->AddPoint(this->drawRect.min + Math::Coord2DDbl(points[i], points[i + 1]) / scale);
		i += 2;
	}
	pl->SetPenStyle(p);
	pl->SetAttr(CSTR("fill"), CSTR("none"));
	this->elements.Add(pl);
	return true;
}

Bool Media::SVGContainer::DrawPolygonI(UnsafeArray<const Int32> points, UIntOS nPoints, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	Math::Coord2DDbl scale = this->doc->GetDrawScale();
	NN<SVGPolygon> pg;
	NN<DrawPen> nnPen;
	NN<DrawBrush> nnBrush;
	NEW_CLASSNN(pg, SVGPolygon(*this));
	UIntOS i = 0;
	while (i < nPoints * 2)
	{
		pg->AddPoint(this->drawRect.min + Math::Coord2DDbl(points[i], points[i + 1]) / scale);
		i += 2;
	}
	if (p.SetTo(nnPen))
	{
		pg->SetPenStyle(nnPen);
	}
	if (b.SetTo(nnBrush))
	{
		pg->SetBrushStyle(nnBrush);
	}
	else
	{
		pg->SetAttr(CSTR("fill"), CSTR("none"));
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
	NEW_CLASSNN(pl, SVGPolyline(*this));
	UIntOS i = 0;
	while (i < nPoints)
	{
		pl->AddPoint(this->drawRect.min + points[i] / scale);
		i++;
	}
	pl->SetPenStyle(p);
	pl->SetAttr(CSTR("fill"), CSTR("none"));
	this->elements.Add(pl);
	return true;
}

Bool Media::SVGContainer::DrawPolygon(UnsafeArray<const Math::Coord2DDbl> points, UIntOS nPoints, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	Math::Coord2DDbl scale = this->doc->GetDrawScale();
	NN<DrawPen> nnPen;
	NN<DrawBrush> nnBrush;
	NN<SVGPolygon> pg;
	NEW_CLASSNN(pg, SVGPolygon(*this));
	UIntOS i = 0;
	while (i < nPoints)
	{
		pg->AddPoint(this->drawRect.min + points[i] / scale);
		i++;
	}
	if (p.SetTo(nnPen))
	{
		pg->SetPenStyle(nnPen);
	}
	if (b.SetTo(nnBrush))
	{
		pg->SetBrushStyle(nnBrush);
	}
	else
	{
		pg->SetAttr(CSTR("fill"), CSTR("none"));
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
	NEW_CLASSNN(path, SVGPath(*this));
	path->SetAttr(CSTR("d"), sb.ToCString());
	NN<DrawPen> nnPen;
	NN<DrawBrush> nnBrush;
	if (p.SetTo(nnPen))
	{
		path->SetPenStyle(nnPen);
	}
	if (b.SetTo(nnBrush))
	{
		path->SetBrushStyle(nnBrush);
	}
	else
	{
		path->SetAttr(CSTR("fill"), CSTR("none"));
	}
	this->elements.Add(path);
	return true;
}

Bool Media::SVGContainer::DrawRect(Math::Coord2DDbl tl, Math::Size2DDbl size, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	Math::Coord2DDbl scale = this->doc->GetDrawScale();
	NN<SVGRect> rect;
	NEW_CLASSNN(rect, SVGRect(*this, this->drawRect.min + tl / scale, size / scale, true));
	NN<DrawPen> nnPen;
	NN<DrawBrush> nnBrush;
	if (p.SetTo(nnPen))
	{
		rect->SetPenStyle(nnPen);
	}
	if (b.SetTo(nnBrush))
	{
		rect->SetBrushStyle(nnBrush);
	}
	else
	{
		rect->SetAttr(CSTR("fill"), CSTR("none"));
	}
	this->elements.Add(rect);
	return true;
}

Bool Media::SVGContainer::DrawEllipse(Math::Coord2DDbl tl, Math::Size2DDbl size, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	Math::Coord2DDbl scale = this->doc->GetDrawScale();
	NN<SVGEllipse> ellipse;
	NEW_CLASSNN(ellipse, SVGEllipse(*this, this->drawRect.min + tl / scale + size * 0.5 / scale, size * 0.5 / scale, true));
	NN<DrawPen> nnPen;
	NN<DrawBrush> nnBrush;
	if (p.SetTo(nnPen))
	{
		ellipse->SetPenStyle(nnPen);
	}
	if (b.SetTo(nnBrush))
	{
		ellipse->SetBrushStyle(nnBrush);
	}
	else
	{
		ellipse->SetAttr(CSTR("fill"), CSTR("none"));
	}
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
	NEW_CLASSNN(text, SVGText(*this, t, this->drawRect.min + tl / scale, true));
	text->SetFontStyle(f);
	text->SetBrushStyle(b);
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
	NEW_CLASSNN(text, SVGText(*this, t, this->drawRect.min + center / scale, true));
	text->SetFontStyle(f);
	text->SetBrushStyle(b);
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
	NEW_CLASSNN(svgImg, SVGImage(*this, this->drawRect.min + tl / scale, Math::Size2DDbl(UIntOS2Double(img->info.dispSize.x) * 96.0 / img->info.hdpi, UIntOS2Double(img->info.dispSize.y) * 96.0 / img->info.vdpi) / scale, sb.ToCString(), true));
	this->elements.Add(svgImg);
	if (this->doc->GetAttr(CSTR("xmlns:xlink")).IsNull())
	{
		this->doc->SetAttr(CSTR("xmlns:xlink"), CSTR("http://www.w3.org/1999/xlink"));
	}
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
	this->AppendAttrs(sb, level + 1);
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
	newDefs->CloneAttrs(*this);
	newDefs->CloneContainerConts(*this);
	return newDefs;
}

Media::SVGGroup::SVGGroup(NN<const SVGContainer> parent, NN<Media::DrawEngine> refEng, NN<SVGDocument> doc) : SVGContainer(parent, refEng, doc)
{
}

Media::SVGGroup::~SVGGroup()
{
}

void Media::SVGGroup::ToString(NN<Text::StringBuilderUTF8> sb, UIntOS level) const
{
	if (!this->IsSpacePreserve())
	{
		sb->AppendChar(' ', level << 1);
	}
	sb->AppendC(UTF8STRC("<g"));
	this->AppendAttrs(sb, level + 1);
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
	newGroup->CloneAttrs(*this);
	newGroup->CloneContainerConts(*this);
	return newGroup;
}

void Media::SVGGroup::SetClipPath(Text::CStringNN clipPath)
{
	this->SetAttr(CSTR("clip-path"), clipPath);
}

Media::SVGUnknown::SVGUnknown(NN<const SVGContainer> parent, Text::CStringNN name) : SVGElement(parent)
{
	this->name = Text::String::New(name);
}

Media::SVGUnknown::~SVGUnknown()
{
	this->name->Release();
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
	if (!this->IsSpacePreserve())
	{
		sb->AppendChar(' ', level << 1);
	}
	sb->AppendC(UTF8STRC("<"));
	sb->Append(this->name);
	this->AppendAttrs(sb, level + 1);
	sb->AppendC(UTF8STRC(" />"));
}

NN<Media::SVGElement> Media::SVGUnknown::CloneElement(NN<const SVGContainer> newParent) const
{
	NN<Media::SVGUnknown> newUnknown;
	NEW_CLASSNN(newUnknown, Media::SVGUnknown(newParent, this->name->ToCString()));
	newUnknown->CloneAttrs(*this);
	return newUnknown;
}

Media::SVGUnknownContainer::SVGUnknownContainer(NN<const SVGContainer> parent, NN<Media::DrawEngine> refEng, NN<SVGDocument> doc, Text::CStringNN name) : SVGContainer(parent, refEng, doc)
{
	this->name = Text::String::New(name);
}

Media::SVGUnknownContainer::~SVGUnknownContainer()
{
	this->name->Release();
}

Text::CStringNN Media::SVGUnknownContainer::GetElementName() const
{
	return this->name->ToCString();
}

void Media::SVGUnknownContainer::ToString(NN<Text::StringBuilderUTF8> sb, UIntOS level) const
{
	if (!this->IsSpacePreserve())
	{
		sb->AppendChar(' ', level << 1);
	}
	sb->AppendC(UTF8STRC("<"));
	sb->Append(this->name);
	this->AppendAttrs(sb, level + 1);
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
	newUnknown->CloneAttrs(*this);
	newUnknown->CloneContainerConts(*this);
	return newUnknown;
}

Media::SVGDocument::SVGDocument(NN<Media::DrawEngine> refEng, Bool createXmlns) : SVGContainer(nullptr, refEng, *this)
{
	this->width = 0;
	this->height = 0;
	this->unit = Math::Unit::Distance::DU_PIXEL;
	this->viewBox = Math::RectArea<IntOS>(0, 0, 0, 0);
	this->hDrawScale = NAN;
	this->vDrawScale = NAN;
	if (createXmlns)
	{
		this->SetAttr(CSTR("xmlns"), CSTR("http://www.w3.org/2000/svg"));
	}
}

Media::SVGDocument::~SVGDocument()
{
	this->fonts.DeleteAll();
	this->brushes.DeleteAll();
	this->pens.DeleteAll();
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

void Media::SVGDocument::SetSizeCreate(UIntOS width, UIntOS height, Math::Unit::Distance::DistanceUnit unit)
{
	this->width = width;
	this->height = height;
	this->unit = unit;
	this->hDrawScale = NAN;
	this->vDrawScale = NAN;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrUIntOS(sbuff, width);
	sptr = Math::Unit::Distance::GetUnitShortName(this->unit).ConcatTo(sptr);
	this->SetAttr(CSTR("width"), CSTRP(sbuff, sptr));
	sptr = Text::StrUIntOS(sbuff, height);
	sptr = Math::Unit::Distance::GetUnitShortName(this->unit).ConcatTo(sptr);
	this->SetAttr(CSTR("height"), CSTRP(sbuff, sptr));
}

void Media::SVGDocument::SetViewBoxCreate(Math::RectArea<IntOS> viewBox)
{
	this->viewBox = viewBox;
	this->hDrawScale = NAN;
	this->vDrawScale = NAN;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrIntOS(sbuff, viewBox.min.x);
	*sptr++ = ' ';
	sptr = Text::StrIntOS(sptr, viewBox.min.y);
	*sptr++ = ' ';
	sptr = Text::StrIntOS(sptr, viewBox.GetWidth());
	*sptr++ = ' ';
	sptr = Text::StrIntOS(sptr, viewBox.GetHeight());
	this->SetAttr(CSTR("viewBox"), CSTRP(sbuff, sptr));
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

void Media::SVGDocument::ToString(NN<Text::StringBuilderUTF8> sb, UIntOS level) const
{
	sb->AppendC(UTF8STRC("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"));
	sb->AppendC(UTF8STRC("<svg"));
	this->AppendAttrs(sb, level + 1);
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
	NEW_CLASSNN(newDoc, SVGDocument(this->refEng, false));
	newDoc->width = this->width;
	newDoc->height = this->height;
	newDoc->unit = this->unit;
	newDoc->viewBox = this->viewBox;
	newDoc->hDrawScale = this->hDrawScale;
	newDoc->vDrawScale = this->vDrawScale;
	newDoc->CloneAttrs(*this);
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
	NEW_CLASSNN(doc, Media::SVGDocument(refEng, false));
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
					doc->SetSizeCreate(width, height, unit);
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
					doc->SetSizeCreate(width, height, unit);
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
						doc->SetViewBoxCreate(Math::RectArea<IntOS>(vals[0], vals[1], vals[0] + vals[2], vals[1] + vals[3]));
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
			else
			{
				doc->SetAttr(name->ToCString(), value->ToCString());
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
			container->SetAttr(name->ToCString(), value->ToCString());
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
	Double x = NAN;
	Double y = NAN;
	Double width = NAN;
	Double height = NAN;
	UIntOS i = 0;
	UIntOS j = reader->GetAttribCount();
	while (i < j)
	{
		NN<Text::XMLAttrib> attr = reader->GetAttribNoCheck(i);
		if (attr->name.SetTo(name) && attr->value.SetTo(value))
		{
			if (name->Equals(UTF8STRC("x")))
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
		NEW_CLASSNN(rect, Media::SVGRect(container, Math::Coord2DDbl(x, y), Math::Size2DDbl(width, height), false));
		i = 0;
		j = reader->GetAttribCount();
		while (i < j)
		{
			NN<Text::XMLAttrib> attr = reader->GetAttribNoCheck(i);
			if (attr->name.SetTo(name) && attr->value.SetTo(value))
			{
				rect->SetAttr(name->ToCString(), value->ToCString());
			}
			i++;
		}
		container->AddElement(rect);
	}
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
	NN<Media::SVGPath> path;
	Optional<Text::String> d = nullptr;
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
		}
		else
		{
			printf("SVGDocument: Invalid attribute in path\r\n");
		}
		i++;
	}
	if (d.SetTo(s))
	{
		NEW_CLASSNN(path, Media::SVGPath(container));
		i = 0;
		j = reader->GetAttribCount();
		while (i < j)
		{
			attr = reader->GetAttribNoCheck(i);
			if (attr->name.SetTo(name) && attr->value.SetTo(value))
			{
				path->SetAttr(name->ToCString(), value->ToCString());
			}
			i++;
		}
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
	Double x = NAN;
	Double y = NAN;
	Data::ArrayListStringNN attrNames;
	Data::ArrayListStringNN attrValues;

	while (i < j)
	{
		attr = reader->GetAttribNoCheck(i);
		if (attr->name.SetTo(name) && attr->value.SetTo(value))
		{
			if (name->Equals(UTF8STRC("x")))
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
			attrNames.Add(name->Clone());
			attrValues.Add(value->Clone());
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
	else
	{
		Double fontSizePx = 16;
//		if (fontSize.SetTo(value))
//		{
//			fontSizePx = Text::CSSCore::FontSizeToPx(value->ToCString(), fontSizePx);
//		}
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
			NN<SVGText> text;
			NEW_CLASSNN(text, Media::SVGText(container, textComponents.GetItemNoCheck(0), Math::Coord2DDbl(x, y), false));
			i = 1;
			j = textComponents.GetCount();
			while (i < j)
			{
				text->AddTextComponent(textComponents.GetItemNoCheck(i));
				i++;
			}
			i = 0;
			j = attrNames.GetCount();
			while (i < j)
			{
				text->SetAttr(attrNames.GetItemNoCheck(i)->ToCString(), attrValues.GetItemNoCheck(i)->ToCString());
				i++;
			}
			container->AddElement(text);
		}
	}
	attrNames.FreeAll();
	attrValues.FreeAll();
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
	Optional<Text::String> href = nullptr;
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
		OPTSTR_DEL(href);
		printf("SVGDocument: Missing attributes in image\r\n");
		return false;
	}
	NN<SVGImage> image;
	NEW_CLASSNN(image, SVGImage(container, Math::Coord2DDbl(x, y), Math::Size2DDbl(width, height), s->ToCString(), false));
	i = 0;
	j = reader->GetAttribCount();
	while (i < j)
	{
		attr = reader->GetAttribNoCheck(i);
		if (attr->name.SetTo(name) && attr->value.SetTo(value))
		{
			image->SetAttr(name->ToCString(), value->ToCString());
		}
		i++;
	}
	container->AddElement(image);
	OPTSTR_DEL(href);
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
			unknown->SetAttr(name->ToCString(), value->ToCString());
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
			unknown->SetAttr(name->ToCString(), value->ToCString());
		}
		i++;
	}
	Bool succ = ParseContainerAttr(unknown, reader, true);
	container->AddElement(unknown);
	succ = succ && ParseContainer(unknown, reader);
	return succ;

}
