#include "Stdafx.h"
#include "IO/MemoryReadingStream.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMEHTMLViewer.h"
#include "Text/HTMLUtil.h"

SSWR::AVIRead::MIMEViewer::AVIRMIMEHTMLViewer::AVIRMIMEHTMLViewer(NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> ctrl, NotNullPtr<Media::ColorManagerSess> sess, Text::MIMEObj::UnknownMIMEObj *obj) : SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer(core, ctrl, obj)
{
	this->obj = obj;

	NEW_CLASS(this->txtHTML, UI::GUITextBox(ui, ctrl, CSTR(""), true));
	this->txtHTML->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtHTML->SetReadOnly(true);

	UOSInt size;
	const UInt8 *buff = obj->GetRAWData(&size);
	IO::MemoryReadingStream mstm(buff, size);
	Text::StringBuilderUTF8 sb;
	Text::HTMLUtil::HTMLWellFormat(core->GetEncFactory(), mstm, 0, sb);
	this->txtHTML->SetText(sb.ToCString());
}

SSWR::AVIRead::MIMEViewer::AVIRMIMEHTMLViewer::~AVIRMIMEHTMLViewer()
{
}
