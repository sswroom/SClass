#include "Stdafx.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMETextViewer.h"

SSWR::AVIRead::MIMEViewer::AVIRMIMETextViewer::AVIRMIMETextViewer(NN<SSWR::AVIRead::AVIRCore> core, NN<UI::GUICore> ui, NN<UI::GUIClientControl> ctrl, NN<Media::ColorManagerSess> sess, Text::MIMEObj::TextMIMEObj *txt) : SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer(core, ctrl, txt)
{
	this->txt = txt;

	Text::StringBuilderUTF8 sb;
	txt->GetText(sb);
	this->txtContent = ui->NewTextBox(ctrl, sb.ToCString(), true);
	this->txtContent->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtContent->SetReadOnly(true);
}

SSWR::AVIRead::MIMEViewer::AVIRMIMETextViewer::~AVIRMIMETextViewer()
{
}
