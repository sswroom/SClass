#include "Stdafx.h"
#include "Media/Jasper/JasperXML.h"

#include <stdio.h>

Media::Jasper::JasperReport *Media::Jasper::JasperXML::ParseJasperReport(NN<Text::XMLReader> reader, Text::CStringNN fileName)
{
	if (reader->GetNodeType() != Text::XMLNode::NodeType::Element || !reader->GetNodeTextNN()->Equals(UTF8STRC("jasperReport")))
	{
		return 0;
	}

	UOSInt i;
	UOSInt j;
	UInt32 uval;
	NN<Text::XMLAttrib> attr;
	Text::XMLNode::NodeType nodeType;
	NN<Text::String> nodeText;
	Media::Jasper::JasperReport *report;
	Text::StringBuilderUTF8 sb;
	NN<Text::String> aname;
	NN<Text::String> nns;
	NN<JasperBand> band;
	NEW_CLASS(report, Media::Jasper::JasperReport(fileName));
	i = 0;
	j = reader->GetAttribCount();
	while (i < j)
	{
		attr = reader->GetAttribNoCheck(i);
		aname = Text::String::OrEmpty(attr->name);
		if (aname->Equals(UTF8STRC("xmlns")))
		{

		}
		else if (aname->Equals(UTF8STRC("xmlns:xsi")))
		{

		}
		else if (aname->Equals(UTF8STRC("xsi:schemaLocation")))
		{

		}
		else if (aname->Equals(UTF8STRC("name")))
		{
			report->SetReportName(attr->value);
		}
		else if (aname->Equals(UTF8STRC("pageWidth")))
		{
			if (!attr->value.SetTo(nns) || !nns->ToUInt32(uval))
			{
				printf("JasperXML: Unknown attr value in jasperReport (%s)\r\n", aname->v.Ptr());
			}
			else
			{
				report->SetPageWidth(uval);
			}
		}
		else if (aname->Equals(UTF8STRC("pageHeight")))
		{
			if (!attr->value.SetTo(nns) || !nns->ToUInt32(uval))
			{
				printf("JasperXML: Unknown attr value in jasperReport (%s)\r\n", aname->v.Ptr());
			}
			else
			{
				report->SetPageHeight(uval);
			}
		}
		else if (aname->Equals(UTF8STRC("columnWidth")))
		{
			if (!attr->value.SetTo(nns) || !nns->ToUInt32(uval))
			{
				printf("JasperXML: Unknown attr value in jasperReport (%s)\r\n", aname->v.Ptr());
			}
			else
			{
			}
		}
		else if (aname->Equals(UTF8STRC("leftMargin")))
		{
			if (!attr->value.SetTo(nns) || !nns->ToUInt32(uval))
			{
				printf("JasperXML: Unknown attr value in jasperReport (%s)\r\n", aname->v.Ptr());
			}
			else
			{
				report->SetMarginLeft(uval);
			}
		}
		else if (aname->Equals(UTF8STRC("rightMargin")))
		{
			if (!attr->value.SetTo(nns) || !nns->ToUInt32(uval))
			{
				printf("JasperXML: Unknown attr value in jasperReport (%s)\r\n", aname->v.Ptr());
			}
			else
			{
				report->SetPageWidth(uval);
			}
		}
		else if (aname->Equals(UTF8STRC("topMargin")))
		{
			if (!attr->value.SetTo(nns) || !nns->ToUInt32(uval))
			{
				printf("JasperXML: Unknown attr value in jasperReport (%s)\r\n", aname->v.Ptr());
			}
			else
			{
				report->SetMarginTop(uval);
			}
		}
		else if (aname->Equals(UTF8STRC("bottomMargin")))
		{
			if (!attr->value.SetTo(nns) || !nns->ToUInt32(uval))
			{
				printf("JasperXML: Unknown attr value in jasperReport (%s)\r\n", aname->v.Ptr());
			}
			else
			{
				report->SetMarginBottom(uval);
			}
		}
		else if (aname->Equals(UTF8STRC("uuid")))
		{
			report->SetUUID(attr->value);
		}
		else
		{
			printf("JasperXML: Unknown attr in jasperReport (%s)\r\n", aname->v.Ptr());
		}

		i++;
	}

	while (reader->NextElementName().SetTo(nodeText))
	{
		if (nodeText->Equals(UTF8STRC("property")))
		{
			Optional<Text::String> name = nullptr;
			Optional<Text::String> value = nullptr;
			NN<Text::String> nameStr;
			NN<Text::String> valueStr;
			i = 0;
			j = reader->GetAttribCount();
			while (i < j)
			{
				attr = reader->GetAttribNoCheck(i);
				aname = Text::String::OrEmpty(attr->name);
				if (aname->Equals(UTF8STRC("name")))
				{
					name = attr->value;
				}
				else if (aname->Equals(UTF8STRC("value")))
				{
					value = attr->value;
				}
				else
				{
					printf("JasperXML: Unknown attr in property (%s)\r\n", aname->v.Ptr());
				}
				i++;
			}
			if (!reader->IsElementEmpty())
			{
				printf("JasperXML: property is not empty element (%s)\r\n", name.SetTo(nameStr)?nameStr->v.Ptr():(const UTF8Char*)"");
			}
			if (name.SetTo(nameStr) && value.SetTo(valueStr))
			{
				report->SetProperty(nameStr, valueStr);
			}
		}
		else if (nodeText->Equals(UTF8STRC("import")))
		{
			Optional<Text::String> value = nullptr;
			NN<Text::String> valueStr;
			i = 0;
			j = reader->GetAttribCount();
			while (i < j)
			{
				attr = reader->GetAttribNoCheck(i);
				aname = Text::String::OrEmpty(attr->name);
				if (aname->Equals(UTF8STRC("value")))
				{
					value = attr->value;
				}
				else
				{
					printf("JasperXML: Unknown attr in import (%s)\r\n", aname->v.Ptr());
				}
				i++;
			}
			if (!reader->IsElementEmpty())
			{
				printf("JasperXML: import is not empty element (%s)\r\n", value.SetTo(valueStr)?valueStr->v.Ptr():(const UTF8Char*)"");
			}
			if (value.SetTo(valueStr))
			{
				report->AddImport(valueStr);
			}
			else
			{
				printf("JasperXML: import does not contain value\r\n");
			}
		}
		else if (nodeText->Equals(UTF8STRC("parameter")))
		{
			Optional<Text::String> name = nullptr;
			Optional<Text::String> className = nullptr;
			NN<Text::String> nameStr;
			NN<Text::String> classNameStr;
			i = 0;
			j = reader->GetAttribCount();
			while (i < j)
			{
				attr = reader->GetAttribNoCheck(i);
				aname = Text::String::OrEmpty(attr->name);
				if (aname->Equals(UTF8STRC("name")))
				{
					name = attr->value;
				}
				else if (aname->Equals(UTF8STRC("class")))
				{
					className = attr->value;
				}
				else
				{
					printf("JasperXML: Unknown attr in parameter (%s)\r\n", aname->v.Ptr());
				}
				i++;
			}
			if (name.SetTo(nameStr) && className.SetTo(classNameStr))
			{
				if (reader->IsElementEmpty())
				{
					report->AddParameter(nameStr, classNameStr, nullptr);
				}
				else
				{
					sb.ClearStr();
					reader->ReadNodeText(sb);
					report->AddParameter(nameStr, classNameStr, sb.ToCString());
				}
			}
			else
			{
				printf("JasperXML: parameter missing attribute\r\n");
			}

		}
		else if (nodeText->Equals(UTF8STRC("queryString")))
		{
			i = 0;
			j = reader->GetAttribCount();
			while (i < j)
			{
				attr = reader->GetAttribNoCheck(i);
				printf("JasperXML: Unknown attr in queryString (%s)\r\n", Text::String::OrEmpty(attr->name)->v.Ptr());
				i++;
			}
			if (!reader->IsElementEmpty())
			{
				while (reader->ReadNext())
				{
					nodeType = reader->GetNodeType();
					if (nodeType == Text::XMLNode::NodeType::ElementEnd)
					{
						break;
					}
					else if (nodeType == Text::XMLNode::NodeType::Element)
					{
						printf("JasperXML: Unknown element in queryString (%s)\r\n", reader->GetNodeTextNN()->v.Ptr());
						reader->SkipElement();
					}
					else if (nodeType == Text::XMLNode::NodeType::CData)
					{
						report->SetQueryString(reader->GetNodeText());
					}
				}
			}
		}
		else if (nodeText->Equals(UTF8STRC("title")))
		{
			while (reader->NextElementName().SetTo(nodeText))
			{
				if (nodeText->Equals(UTF8STRC("band")) && ParseBand(reader).SetTo(band))
				{
					report->SetTitle(band);
				}
				else
				{
					printf("JasperXML: Unknown element in title (%s)\r\n", nodeText->v.Ptr());
					reader->SkipElement();
				}
			}
		}
		else if (nodeText->Equals(UTF8STRC("detail")))
		{
			while (reader->NextElementName().SetTo(nodeText))
			{
				if (nodeText->Equals(UTF8STRC("band")) && ParseBand(reader).SetTo(band))
				{
					report->AddDetail(band);
				}
				else
				{
					printf("JasperXML: Unknown element in detail (%s)\r\n", nodeText->v.Ptr());
					reader->SkipElement();
				}
			}
		}
		else
		{
			printf("JasperXML: unknown element in jasperReport: %s\r\n", nodeText->v.Ptr());
			reader->SkipElement();
		}
	}
	return report;
}

Optional<Media::Jasper::JasperBand> Media::Jasper::JasperXML::ParseBand(NN<Text::XMLReader> reader)
{
	if (reader->GetNodeType() != Text::XMLNode::NodeType::Element)
	{
		printf("JasperXML: non element in ParseBand\r\n");
		return nullptr;
	}
	if (!reader->GetNodeTextNN()->Equals(UTF8STRC("band")))
	{
		printf("JasperXML: unknown element in ParseBand: (%s)\r\n", reader->GetNodeTextNN()->v.Ptr());
		reader->SkipElement();
		return nullptr;
	}

	NN<Media::Jasper::JasperBand> band;
	NEW_CLASSNN(band, Media::Jasper::JasperBand());
	NN<Text::String> aname;
	NN<Text::String> nns;
	UOSInt i = 0;
	UOSInt j = reader->GetAttribCount();
	while (i < j)
	{
		NN<Text::XMLAttrib> attr = reader->GetAttribNoCheck(i);
		aname = Text::String::OrEmpty(attr->name);
		if (aname->Equals(UTF8STRC("splitType")))
		{
			band->SetSplitType(attr->value);
		}
		else if (aname->Equals(UTF8STRC("height")) && attr->value.SetTo(nns))
		{
			band->SetHeight(nns->ToUOSInt());
		}
		else
		{
			printf("JasperXML: unknown attr in band: (%s)\r\n", aname->v.Ptr());
		}
		i++;
	}
	if (!reader->IsElementEmpty())
	{
		while (reader->ReadNext())
		{
			Text::XMLNode::NodeType nodeType = reader->GetNodeType();
			if (nodeType == Text::XMLNode::NodeType::ElementEnd)
			{
				break;
			}
			else if (nodeType == Text::XMLNode::NodeType::Element)
			{
				NN<Media::Jasper::JasperElement> element;
				if (ParseElement(reader).SetTo(element))
				{
					band->AddElement(element);
				}
			}
		}
	}
	return band;
}

Optional<Media::Jasper::JasperElement> Media::Jasper::JasperXML::ParseElement(NN<Text::XMLReader> reader)
{
	if (reader->GetNodeType() != Text::XMLNode::NodeType::Element)
	{
		printf("JasperXML: non element in ParseElement\r\n");
		return nullptr;
	}
	NN<Text::String> nodeName = reader->GetNodeTextNN();
	if (nodeName->Equals(UTF8STRC("staticText")))
	{
		reader->SkipElement();
		return nullptr;
	}
	else if (nodeName->Equals(UTF8STRC("textField")))
	{
		reader->SkipElement();
		return nullptr;
	}
	else if (nodeName->Equals(UTF8STRC("line")))
	{
		reader->SkipElement();
		return nullptr;
	}
	else if (nodeName->Equals(UTF8STRC("image")))
	{
		reader->SkipElement();
		return nullptr;
	}
	else
	{
		printf("JasperXML: unknown element in ParseElement: (%s)\r\n", nodeName->v.Ptr());
		reader->SkipElement();
		return nullptr;
	}
}
