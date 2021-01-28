#include "Stdafx.h"
#include "IO/MemoryStream.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMEXMLViewer.h"
#include "Text/XML.h"
#include "Text/XMLReader.h"

SSWR::AVIRead::MIMEViewer::AVIRMIMEXMLViewer::AVIRMIMEXMLViewer(SSWR::AVIRead::AVIRCore *core, UI::GUICore *ui, UI::GUIClientControl *ctrl, Media::ColorManagerSess *sess, Text::MIMEObj::UnknownMIMEObj *obj) : SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer(core, ctrl, obj)
{
	this->obj = obj;

	NEW_CLASS(this->lvXML, UI::GUIListView(ui, ctrl, UI::GUIListView::LVSTYLE_TABLE, 3));
	this->lvXML->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvXML->SetShowGrid(true);
	this->lvXML->SetFullRowSelect(true);
	this->lvXML->AddColumn((const UTF8Char*)"Path", 300);
	this->lvXML->AddColumn((const UTF8Char*)"Value", 300);

	OSInt size;
	const UInt8 *buff = obj->GetRAWData(&size);
	IO::MemoryStream *mstm;
	Text::XMLReader *reader;
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sbPath;
	Text::StringBuilderUTF8 sbCont;
	Text::XMLAttrib *attr;
	const UTF8Char *csptr;
	OSInt i;
	OSInt j;
	NEW_CLASS(mstm, IO::MemoryStream((UTF8Char*)buff, size, (const UTF8Char*)"SSWR.AVIRead.MIMEViewer.AVIRMIMEXMLViewer.mstm"));
	NEW_CLASS(reader, Text::XMLReader(core->GetEncFactory(), mstm, Text::XMLReader::PM_XML));
	while (reader->ReadNext())
	{
		if (reader->GetNodeType() == Text::XMLNode::NT_DOCUMENT)
		{
			if (sbCont.GetLength() > 0)
			{
				i = this->lvXML->AddItem(sbPath.ToString(), 0);
				this->lvXML->SetSubItem(i, 1, sbCont.ToString());
				sbCont.ClearStr();
			}
			sbPath.ClearStr();
			reader->GetCurrPath(&sbPath);
			if (reader->GetPathLev() > 0)
			{
				sbCont.AppendChar(' ', reader->GetPathLev() << 1);
			}
			sbCont.Append((const UTF8Char*)"<?");
			sbCont.Append(reader->GetNodeText());
			i = 0;
			j = reader->GetAttribCount();
			while (i < j)
			{
				attr = reader->GetAttrib(i);
				sbCont.AppendChar(' ', 1);
				sbCont.Append(attr->name);
				if (attr->value)
				{
					sbCont.AppendChar('=', 1);
					sbCont.AppendChar('"', 1);
					csptr = Text::XML::ToNewAttrText(attr->value);
					sbCont.Append(csptr);
					Text::XML::FreeNewText(csptr);
					sbCont.AppendChar('"', 1);
				}
				i++;
			}
			sbCont.Append((const UTF8Char*)"?>");
			i = this->lvXML->AddItem(sbPath.ToString(), 0);
			this->lvXML->SetSubItem(i, 1, sbCont.ToString());
			sbCont.ClearStr();
		}
		else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
		{
			if (sbCont.GetLength() == 0 && reader->GetPathLev() > 0)
			{
				sbCont.AppendChar(' ', reader->GetPathLev() << 1);
			}
			sbCont.Append((const UTF8Char*)"</");
			sbCont.Append(reader->GetNodeText());
			sbCont.Append((const UTF8Char*)">");
			sb.ClearStr();
			reader->GetCurrPath(&sb);
			i = this->lvXML->AddItem(sb.ToString(), 0);
			this->lvXML->SetSubItem(i, 1, sbCont.ToString());
			sbCont.ClearStr();
		}
		else if (reader->GetNodeType() == Text::XMLNode::NT_TEXT)
		{
			sb.ClearStr();
			sb.Append(reader->GetNodeText());
			sb.TrimWSCRLF();
			if (sb.GetLength() > 0)
			{
				csptr = Text::XML::ToNewXMLText(sb.ToString());
				sbCont.Append(csptr);
				Text::XML::FreeNewText(csptr);
				if (sbPath.GetLength() == 0)
				{
					reader->GetCurrPath(&sbPath);
				}
			}
		}
		else if (reader->GetNodeType() == Text::XMLNode::NT_ELEMENT)
		{
			if (sbCont.GetLength() > 0)
			{
				i = this->lvXML->AddItem(sbPath.ToString(), 0);
				this->lvXML->SetSubItem(i, 1, sbCont.ToString());
				sbCont.ClearStr();
			}
			sbPath.ClearStr();
			reader->GetCurrPath(&sbPath);
			if (reader->GetPathLev() > 0)
			{
				sbCont.AppendChar(' ', reader->GetPathLev() << 1);
			}
			sbCont.Append((const UTF8Char*)"<");
			sbCont.Append(reader->GetNodeText());
			i = 0;
			j = reader->GetAttribCount();
			while (i < j)
			{
				attr = reader->GetAttrib(i);
				sbCont.AppendChar(' ', 1);
				sbCont.Append(attr->name);
				if (attr->value)
				{
					sbCont.AppendChar('=', 1);
					csptr = Text::XML::ToNewAttrText(attr->value);
					sbCont.Append(csptr);
					Text::XML::FreeNewText(csptr);
				}
				i++;
			}
			if (reader->IsElementEmpty())
			{
				sbCont.Append((const UTF8Char*)"/>");
				i = this->lvXML->AddItem(sbPath.ToString(), 0);
				this->lvXML->SetSubItem(i, 1, sbCont.ToString());
				sbCont.ClearStr();
			}
			else
			{
				sbCont.Append((const UTF8Char*)">");
			}
		}
	}
	if (sbCont.GetLength() > 0)
	{
		i = this->lvXML->AddItem(sbPath.ToString(), 0);
		this->lvXML->SetSubItem(i, 1, sbCont.ToString());
		sbCont.ClearStr();
	}
	if (!reader->IsComplete())
	{
		this->lvXML->AddItem((const UTF8Char*)"Incomplete file parsing", 0);
	}
	DEL_CLASS(reader);
	DEL_CLASS(mstm);
}

SSWR::AVIRead::MIMEViewer::AVIRMIMEXMLViewer::~AVIRMIMEXMLViewer()
{
}
