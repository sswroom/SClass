#include "Stdafx.h"
#include "IO/MemoryStream.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMEHTMLViewer.h"
#include "Text/HTMLUtil.h"

SSWR::AVIRead::MIMEViewer::AVIRMIMEHTMLViewer::AVIRMIMEHTMLViewer(SSWR::AVIRead::AVIRCore *core, UI::GUICore *ui, UI::GUIClientControl *ctrl, Media::ColorManagerSess *sess, Text::MIMEObj::UnknownMIMEObj *obj) : SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer(core, ctrl, obj)
{
	this->obj = obj;

	NEW_CLASS(this->txtHTML, UI::GUITextBox(ui, ctrl, CSTR(""), true));
	this->txtHTML->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtHTML->SetReadOnly(true);

	UOSInt size;
	const UInt8 *buff = obj->GetRAWData(&size);
	IO::MemoryStream *mstm;
	NEW_CLASS(mstm, IO::MemoryStream((UInt8*)buff, size, UTF8STRC("SSWR.AVIRead.MIMEViewer.AVIRMIMEHTMLViewer.mstm")));
	Text::StringBuilderUTF8 sb;
	Text::HTMLUtil::HTMLWellFormat(core->GetEncFactory(), mstm, 0, &sb);
	this->txtHTML->SetText(sb.ToCString());
	DEL_CLASS(mstm);
}

SSWR::AVIRead::MIMEViewer::AVIRMIMEHTMLViewer::~AVIRMIMEHTMLViewer()
{
}
