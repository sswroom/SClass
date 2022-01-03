#include "Stdafx.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRUnknownViewer.h"

SSWR::AVIRead::MIMEViewer::AVIRUnknownViewer::AVIRUnknownViewer(SSWR::AVIRead::AVIRCore *core, UI::GUICore *ui, UI::GUIClientControl *ctrl, Media::ColorManagerSess *sess, Text::IMIMEObj *obj) : SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer(core, ctrl, obj)
{
	this->obj = obj;
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Unsupported MIME type: "));
	sb.Append(obj->GetContentType());

	NEW_CLASS(this->lblMessage, UI::GUILabel(ui, ctrl, sb.ToString()));
	this->lblMessage->SetRect(4, 4, 500, 23, false);
	this->lblMessage->SetDockType(UI::GUIControl::DOCK_FILL);
}

SSWR::AVIRead::MIMEViewer::AVIRUnknownViewer::~AVIRUnknownViewer()
{
}
