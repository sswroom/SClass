#include "Stdafx.h"
#include "IO/MemoryReadingStream.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMEXMLViewer.h"
#include "Text/XML.h"
#include "Text/XMLReader.h"

SSWR::AVIRead::MIMEViewer::AVIRMIMEXMLViewer::AVIRMIMEXMLViewer(NN<SSWR::AVIRead::AVIRCore> core, NN<UI::GUICore> ui, NN<UI::GUIClientControl> ctrl, NN<Media::ColorManagerSess> sess, NN<Text::MIMEObj::UnknownMIMEObj> obj) : SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer(core, ctrl, obj)
{
	this->obj = obj;

	this->lvXML = ui->NewListView(ctrl, UI::ListViewStyle::Table, 3);
	this->lvXML->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvXML->SetShowGrid(true);
	this->lvXML->SetFullRowSelect(true);
	this->lvXML->AddColumn(CSTR("Path"), 300);
	this->lvXML->AddColumn(CSTR("Value"), 300);

	UOSInt size;
	UnsafeArray<const UInt8> buff = obj->GetRAWData(size);
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sbPath;
	Text::StringBuilderUTF8 sbCont;
	NN<Text::XMLAttrib> attr;
	NN<Text::String> s;
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
			sbCont.AppendOpt(reader.GetNodeText());
			i = 0;
			j = reader.GetAttribCount();
			while (i < j)
			{
				attr = reader.GetAttribNoCheck(i);
				sbCont.AppendUTF8Char(' ');
				sbCont.AppendOpt(attr->name);
				if (attr->value.SetTo(s))
				{
					sbCont.AppendUTF8Char('=');
					sbCont.AppendUTF8Char('"');
					s = Text::XML::ToNewAttrText(s->v);
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
			sbCont.AppendOpt(reader.GetNodeText());
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
			sb.AppendOpt(reader.GetNodeText());
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
			sbCont.AppendOpt(reader.GetNodeText());
			i = 0;
			j = reader.GetAttribCount();
			while (i < j)
			{
				attr = reader.GetAttribNoCheck(i);
				sbCont.AppendUTF8Char(' ');
				sbCont.AppendOpt(attr->name);
				if (attr->value.SetTo(s))
				{
					sbCont.AppendUTF8Char('=');
					s = Text::XML::ToNewAttrText(s->v);
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
