#include "Stdafx.h"
#include "Media/Jasper/JasperXML.h"

#include <stdio.h>

Media::Jasper::JasperReport *Media::Jasper::JasperXML::ParseJasperReport(Text::XMLReader *reader, Text::CString fileName)
{
	if (reader->GetNodeType() != Text::XMLNode::NodeType::Element || !reader->GetNodeText()->Equals(UTF8STRC("jasperReport")))
	{
		return 0;
	}

	UOSInt i;
	UOSInt j;
	UInt32 uval;
	Text::XMLAttrib *attr;
	Text::XMLNode::NodeType nodeType;
	Text::String *nodeText;
	Media::Jasper::JasperReport *report;
	Text::StringBuilderUTF8 sb;
	NEW_CLASS(report, Media::Jasper::JasperReport(fileName));

	i = 0;
	j = reader->GetAttribCount();
	while (i < j)
	{
		attr = reader->GetAttrib(i);
		if (attr->name->Equals(UTF8STRC("xmlns")))
		{

		}
		else if (attr->name->Equals(UTF8STRC("xmlns:xsi")))
		{

		}
		else if (attr->name->Equals(UTF8STRC("xsi:schemaLocation")))
		{

		}
		else if (attr->name->Equals(UTF8STRC("name")))
		{
			report->SetReportName(attr->value);
		}
		else if (attr->name->Equals(UTF8STRC("pageWidth")))
		{
			if (attr->value == 0 || !attr->value->ToUInt32(&uval))
			{
				printf("JasperXML: Unknown attr value in jasperReport (%s)\r\n", attr->name->v);
			}
			else
			{
				report->SetPageWidth(uval);
			}
		}
		else if (attr->name->Equals(UTF8STRC("pageHeight")))
		{
			if (attr->value == 0 || !attr->value->ToUInt32(&uval))
			{
				printf("JasperXML: Unknown attr value in jasperReport (%s)\r\n", attr->name->v);
			}
			else
			{
				report->SetPageHeight(uval);
			}
		}
		else if (attr->name->Equals(UTF8STRC("columnWidth")))
		{
			if (attr->value == 0 || !attr->value->ToUInt32(&uval))
			{
				printf("JasperXML: Unknown attr value in jasperReport (%s)\r\n", attr->name->v);
			}
			else
			{
			}
		}
		else if (attr->name->Equals(UTF8STRC("leftMargin")))
		{
			if (attr->value == 0 || !attr->value->ToUInt32(&uval))
			{
				printf("JasperXML: Unknown attr value in jasperReport (%s)\r\n", attr->name->v);
			}
			else
			{
				report->SetMarginLeft(uval);
			}
		}
		else if (attr->name->Equals(UTF8STRC("rightMargin")))
		{
			if (attr->value == 0 || !attr->value->ToUInt32(&uval))
			{
				printf("JasperXML: Unknown attr value in jasperReport (%s)\r\n", attr->name->v);
			}
			else
			{
				report->SetPageWidth(uval);
			}
		}
		else if (attr->name->Equals(UTF8STRC("topMargin")))
		{
			if (attr->value == 0 || !attr->value->ToUInt32(&uval))
			{
				printf("JasperXML: Unknown attr value in jasperReport (%s)\r\n", attr->name->v);
			}
			else
			{
				report->SetMarginTop(uval);
			}
		}
		else if (attr->name->Equals(UTF8STRC("bottomMargin")))
		{
			if (attr->value == 0 || !attr->value->ToUInt32(&uval))
			{
				printf("JasperXML: Unknown attr value in jasperReport (%s)\r\n", attr->name->v);
			}
			else
			{
				report->SetMarginBottom(uval);
			}
		}
		else if (attr->name->Equals(UTF8STRC("uuid")))
		{
			report->SetUUID(attr->value);
		}
		else
		{
			printf("JasperXML: Unknown attr in jasperReport (%s)\r\n", attr->name->v);
		}

		i++;
	}

	while (reader->ReadNext())
	{
		nodeType = reader->GetNodeType();
		if (nodeType == Text::XMLNode::NodeType::ElementEnd)
		{
			break;
		}
		else if (nodeType == Text::XMLNode::NodeType::Element)
		{
			nodeText = reader->GetNodeText();
			if (nodeText->Equals(UTF8STRC("property")))
			{
				Text::String *name = 0;
				Text::String *value = 0;
				i = 0;
				j = reader->GetAttribCount();
				while (i < j)
				{
					attr = reader->GetAttrib(i);
					if (attr->name->Equals(UTF8STRC("name")))
					{
						name = attr->value;
					}
					else if (attr->name->Equals(UTF8STRC("value")))
					{
						value = attr->value;
					}
					else
					{
						printf("JasperXML: Unknown attr in property (%s)\r\n", attr->name->v);
					}
					i++;
				}
				if (!reader->IsElementEmpty())
				{
					printf("JasperXML: property is not empty element (%s)\r\n", name?name->v:(const UTF8Char*)"");
				}
				if (name && value)
				{
					report->SetProperty(name, value);
				}
			}
			else if (nodeText->Equals(UTF8STRC("import")))
			{
				Text::String *value = 0;
				i = 0;
				j = reader->GetAttribCount();
				while (i < j)
				{
					attr = reader->GetAttrib(i);
					if (attr->name->Equals(UTF8STRC("value")))
					{
						value = attr->value;
					}
					else
					{
						printf("JasperXML: Unknown attr in import (%s)\r\n", attr->name->v);
					}
					i++;
				}
				if (!reader->IsElementEmpty())
				{
					printf("JasperXML: import is not empty element (%s)\r\n", value?value->v:(const UTF8Char*)"");
				}
				if (value)
				{
					report->AddImport(value);
				}
				else
				{
					printf("JasperXML: import does not contain value\r\n");
				}
			}
			else if (nodeText->Equals(UTF8STRC("parameter")))
			{
				Text::String *name = 0;
				Text::String *className = 0;
				i = 0;
				j = reader->GetAttribCount();
				while (i < j)
				{
					attr = reader->GetAttrib(i);
					if (attr->name->Equals(UTF8STRC("name")))
					{
						name = attr->value;
					}
					else if (attr->name->Equals(UTF8STRC("class")))
					{
						className = attr->value;
					}
					else
					{
						printf("JasperXML: Unknown attr in parameter (%s)\r\n", attr->name->v);
					}
					i++;
				}
				if (name && className)
				{
					if (reader->IsElementEmpty())
					{
						report->AddParameter(name, className, CSTR_NULL);
					}
					else
					{
						sb.ClearStr();
						reader->ReadNodeText(&sb);
						report->AddParameter(name, className, sb.ToCString());
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
					attr = reader->GetAttrib(i);
					printf("JasperXML: Unknown attr in queryString (%s)\r\n", attr->name->v);
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
							printf("JasperXML: Unknown element in queryString (%s)\r\n", reader->GetNodeText()->v);
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
				while (reader->ReadNext())
				{
					nodeType = reader->GetNodeType();
					if (nodeType == Text::XMLNode::NodeType::ElementEnd)
					{
						break;
					}
					else if (nodeType == Text::XMLNode::NodeType::Element)
					{
						if (reader->GetNodeText()->Equals(UTF8STRC("band")))
						{
							report->SetTitle(ParseBand(reader));
						}
						else
						{
							printf("JasperXML: Unknown element in title (%s)\r\n", reader->GetNodeText()->v);
						}
					}
				}
			}
			else if (nodeText->Equals(UTF8STRC("detail")))
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
						if (reader->GetNodeText()->Equals(UTF8STRC("band")))
						{
							report->AddDetail(ParseBand(reader));
						}
						else
						{
							printf("JasperXML: Unknown element in detail (%s)\r\n", reader->GetNodeText()->v);
						}
					}
				}
			}
			else
			{
				printf("JasperXML: unknown element in jasperReport: %s\r\n", nodeText->v);
				reader->SkipElement();
			}
		}
	}
	return report;
}

Media::Jasper::JasperBand *Media::Jasper::JasperXML::ParseBand(Text::XMLReader *reader)
{
	if (reader->GetNodeType() != Text::XMLNode::NodeType::Element)
	{
		printf("JasperXML: non element in ParseBand\r\n");
		return 0;
	}
	if (!reader->GetNodeText()->Equals(UTF8STRC("band")))
	{
		printf("JasperXML: unknown element in ParseBand: (%s)\r\n", reader->GetNodeText()->v);
		reader->SkipElement();
		return 0;
	}

	Media::Jasper::JasperBand *band;
	NEW_CLASS(band, Media::Jasper::JasperBand());
	UOSInt i = 0;
	UOSInt j = reader->GetAttribCount();
	while (i < j)
	{
		Text::XMLAttrib *attr = reader->GetAttrib(i);
		if (attr->name->Equals(UTF8STRC("splitType")))
		{
			band->SetSplitType(attr->value);
		}
		else if (attr->name->Equals(UTF8STRC("height")) && attr->value)
		{
			band->SetHeight(attr->value->ToUOSInt());
		}
		else
		{
			printf("JasperXML: unknown attr in band: (%s)\r\n", attr->name->v);
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
				Media::Jasper::JasperElement *element = ParseElement(reader);
				if (element)
				{
					band->AddElement(element);
				}
			}
		}
	}
	return band;
}

Media::Jasper::JasperElement *Media::Jasper::JasperXML::ParseElement(Text::XMLReader *reader)
{
	if (reader->GetNodeType() != Text::XMLNode::NodeType::Element)
	{
		printf("JasperXML: non element in ParseElement\r\n");
		return 0;
	}
	Text::String *nodeName = reader->GetNodeText();
	if (nodeName->Equals(UTF8STRC("staticText")))
	{
		reader->SkipElement();
		return 0;
	}
	else if (nodeName->Equals(UTF8STRC("textField")))
	{
		reader->SkipElement();
		return 0;
	}
	else if (nodeName->Equals(UTF8STRC("line")))
	{
		reader->SkipElement();
		return 0;
	}
	else if (nodeName->Equals(UTF8STRC("image")))
	{
		reader->SkipElement();
		return 0;
	}
	else
	{
		printf("JasperXML: unknown element in ParseElement: (%s)\r\n", reader->GetNodeText()->v);
		reader->SkipElement();
		return 0;
	}
}