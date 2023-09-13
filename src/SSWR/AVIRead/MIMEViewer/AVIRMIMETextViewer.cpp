#include "Stdafx.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMETextViewer.h"

SSWR::AVIRead::MIMEViewer::AVIRMIMETextViewer::AVIRMIMETextViewer(NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<UI::GUICore> ui, UI::GUIClientControl *ctrl, NotNullPtr<Media::ColorManagerSess> sess, Text::MIMEObj::TextMIMEObj *txt) : SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer(core, ctrl, txt)
{
	this->txt = txt;

	Text::StringBuilderUTF8 sb;
	txt->GetText(sb);
	NEW_CLASS(this->txtContent, UI::GUITextBox(ui, ctrl, sb.ToCString(), true));
	this->txtContent->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtContent->SetReadOnly(true);
}

SSWR::AVIRead::MIMEViewer::AVIRMIMETextViewer::~AVIRMIMETextViewer()
{
}
