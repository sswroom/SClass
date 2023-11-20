#include "Stdafx.h"
#include "IO/MemoryReadingStream.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMEXMLViewer.h"
#include "Text/XML.h"
#include "Text/XMLReader.h"

SSWR::AVIRead::MIMEViewer::AVIRMIMEXMLViewer::AVIRMIMEXMLViewer(NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> ctrl, NotNullPtr<Media::ColorManagerSess> sess, Text::MIMEObj::UnknownMIMEObj *obj) : SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer(core, ctrl, obj)
{
	this->obj = obj;

	NEW_CLASS(this->lvXML, UI::GUIListView(ui, ctrl, UI::GUIListView::LVSTYLE_TABLE, 3));
	this->lvXML->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvXML->SetShowGrid(true);
	this->lvXML->SetFullRowSelect(true);
	this->lvXML->AddColumn(CSTR("Path"), 300);
	this->lvXML->AddColumn(CSTR("Value"), 300);

	UOSInt size;
	const UInt8 *buff = obj->GetRAWData(&size);
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sbPath;
	Text::StringBuilderUTF8 sbCont;
	Text::XMLAttrib *attr;
	NotNullPtr<Text::String> s;
	UOSInt i;
	UOSInt j;
	IO::MemoryReadingStream mstm(buff, size);
	Text::XMLReader reader(core->GetEncFactory(), mstm, Text::XMLReader::PM_XML);
	while (reader.ReadNext())
	{
		if (reader.GetNodeType() == Text::XMLNode::NodeType::Document)
		{
			if (sbCont.GetLength() > 0)
			{
				i = this->lvXML->AddItem(sbPath.ToCString(), 0);
				this->lvXML->SetSubItem(i, 1, sbCont.ToCString());
				sbCont.ClearStr();
			}
			sbPath.ClearStr();
			reader.GetCurrPath(sbPath);
			if (reader.GetPathLev() > 0)
			{
				sbCont.AppendChar(' ', reader.GetPathLev() << 1);
			}
			sbCont.AppendC(UTF8STRC("<?"));
			sbCont.Append(reader.GetNodeText());
			i = 0;
			j = reader.GetAttribCount();
			while (i < j)
			{
				attr = reader.GetAttrib(i);
				sbCont.AppendUTF8Char(' ');
				sbCont.Append(attr->name);
				if (attr->value)
				{
					sbCont.AppendUTF8Char('=');
					sbCont.AppendUTF8Char('"');
					s = Text::XML::ToNewAttrText(attr->value->v);
					sbCont.Append(s);
					s->Release();
					sbCont.AppendUTF8Char('"');
				}
				i++;
			}
			sbCont.AppendC(UTF8STRC("?>"));
			i = this->lvXML->AddItem(sbPath.ToCString(), 0);
			this->lvXML->SetSubItem(i, 1, sbCont.ToCString());
			sbCont.ClearStr();
		}
		else if (reader.GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
		{
			if (sbCont.GetLength() == 0 && reader.GetPathLev() > 0)
			{
				sbCont.AppendChar(' ', reader.GetPathLev() << 1);
			}
			sbCont.AppendC(UTF8STRC("</"));
			sbCont.Append(reader.GetNodeText());
			sbCont.AppendC(UTF8STRC(">"));
			sb.ClearStr();
			reader.GetCurrPath(sb);
			i = this->lvXML->AddItem(sb.ToCString(), 0);
			this->lvXML->SetSubItem(i, 1, sbCont.ToCString());
			sbCont.ClearStr();
		}
		else if (reader.GetNodeType() == Text::XMLNode::NodeType::Text)
		{
			sb.ClearStr();
			sb.Append(reader.GetNodeText());
			sb.TrimWSCRLF();
			if (sb.GetLength() > 0)
			{
				s = Text::XML::ToNewXMLText(sb.ToString());
				sbCont.Append(s);
				s->Release();
				if (sbPath.GetLength() == 0)
				{
					reader.GetCurrPath(sbPath);
				}
			}
		}
		else if (reader.GetNodeType() == Text::XMLNode::NodeType::Element)
		{
			if (sbCont.GetLength() > 0)
			{
				i = this->lvXML->AddItem(sbPath.ToCString(), 0);
				this->lvXML->SetSubItem(i, 1, sbCont.ToCString());
				sbCont.ClearStr();
			}
			sbPath.ClearStr();
			reader.GetCurrPath(sbPath);
			if (reader.GetPathLev() > 0)
			{
				sbCont.AppendChar(' ', reader.GetPathLev() << 1);
			}
			sbCont.AppendC(UTF8STRC("<"));
			sbCont.Append(reader.GetNodeText());
			i = 0;
			j = reader.GetAttribCount();
			while (i < j)
			{
				attr = reader.GetAttrib(i);
				sbCont.AppendUTF8Char(' ');
				sbCont.Append(attr->name);
				if (attr->value)
				{
					sbCont.AppendUTF8Char('=');
					s = Text::XML::ToNewAttrText(attr->value->v);
					sbCont.Append(s);
					s->Release();
				}
				i++;
			}
			if (reader.IsElementEmpty())
			{
				sbCont.AppendC(UTF8STRC("/>"));
				i = this->lvXML->AddItem(sbPath.ToCString(), 0);
				this->lvXML->SetSubItem(i, 1, sbCont.ToCString());
				sbCont.ClearStr();
			}
			else
			{
				sbCont.AppendC(UTF8STRC(">"));
			}
		}
	}
	if (sbCont.GetLength() > 0)
	{
		i = this->lvXML->AddItem(sbPath.ToCString(), 0);
		this->lvXML->SetSubItem(i, 1, sbCont.ToCString());
		sbCont.ClearStr();
	}
	if (!reader.IsComplete())
	{
		this->lvXML->AddItem(CSTR("Incomplete file parsing"), 0);
	}
}

SSWR::AVIRead::MIMEViewer::AVIRMIMEXMLViewer::~AVIRMIMEXMLViewer()
{
}
