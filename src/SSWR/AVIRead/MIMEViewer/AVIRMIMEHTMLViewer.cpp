#include "Stdafx.h"
#include "IO/MemoryReadingStream.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMEHTMLViewer.h"
#include "Text/HTMLUtil.h"

SSWR::AVIRead::MIMEViewer::AVIRMIMEHTMLViewer::AVIRMIMEHTMLViewer(NN<SSWR::AVIRead::AVIRCore> core, NN<UI::GUICore> ui, NN<UI::GUIClientControl> ctrl, NN<Media::ColorManagerSess> sess, NN<Text::MIMEObj::UnknownMIMEObj> obj) : SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer(core, ctrl, obj)
{
	this->obj = obj;

	this->txtHTML = ui->NewTextBox(ctrl, CSTR(""), true);
	this->txtHTML->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtHTML->SetReadOnly(true);

	UOSInt size;
	UnsafeArray<const UInt8> buff = obj->GetRAWData(size);
	IO::MemoryReadingStream mstm(buff, size);
	Text::StringBuilderUTF8 sb;
	Text::HTMLUtil::HTMLWellFormat(core->GetEncFactory(), mstm, 0, sb);
	this->txtHTML->SetText(sb.ToCString());
}

SSWR::AVIRead::MIMEViewer::AVIRMIMEHTMLViewer::~AVIRMIMEHTMLViewer()
{
}
