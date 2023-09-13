#include "Stdafx.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRUnknownViewer.h"

SSWR::AVIRead::MIMEViewer::AVIRUnknownViewer::AVIRUnknownViewer(NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<UI::GUICore> ui, UI::GUIClientControl *ctrl, NotNullPtr<Media::ColorManagerSess> sess, Text::IMIMEObj *obj) : SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer(core, ctrl, obj)
{
	this->obj = obj;
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Unsupported MIME type: "));
	Text::CString contType = obj->GetContentType();
	sb.AppendC(contType.v, contType.leng);

	NEW_CLASS(this->lblMessage, UI::GUILabel(ui, ctrl, sb.ToCString()));
	this->lblMessage->SetRect(4, 4, 500, 23, false);
	this->lblMessage->SetDockType(UI::GUIControl::DOCK_FILL);
}

SSWR::AVIRead::MIMEViewer::AVIRUnknownViewer::~AVIRUnknownViewer()
{
}
