#include "Stdafx.h"
#include "IO/MemoryStream.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMEJSONViewer.h"
#include "Text/JSText.h"

SSWR::AVIRead::MIMEViewer::AVIRMIMEJSONViewer::AVIRMIMEJSONViewer(SSWR::AVIRead::AVIRCore *core, UI::GUICore *ui, UI::GUIClientControl *ctrl, Media::ColorManagerSess *sess, Text::MIMEObj::UnknownMIMEObj *obj) : SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer(core, ctrl, obj)
{
	this->obj = obj;

	NEW_CLASS(this->txtJSON, UI::GUITextBox(ui, ctrl, (const UTF8Char*)"", true));
	this->txtJSON->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtJSON->SetReadOnly(true);

	UOSInt size;
	const UInt8 *buff = obj->GetRAWData(&size);
	Text::StringBuilderUTF8 sb;
	Text::JSText::JSONWellFormat(buff, size, 0, &sb);
	this->txtJSON->SetText(sb.ToString());
}

SSWR::AVIRead::MIMEViewer::AVIRMIMEJSONViewer::~AVIRMIMEJSONViewer()
{
}
