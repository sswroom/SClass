#include "Stdafx.h"
#include "IO/MemoryStream.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMEJSONViewer.h"
#include "Text/JSText.h"

SSWR::AVIRead::MIMEViewer::AVIRMIMEJSONViewer::AVIRMIMEJSONViewer(NN<SSWR::AVIRead::AVIRCore> core, NN<UI::GUICore> ui, NN<UI::GUIClientControl> ctrl, NN<Media::ColorManagerSess> sess, NN<Text::MIMEObj::UnknownMIMEObj> obj) : SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer(core, ctrl, obj)
{
	this->obj = obj;

	this->txtJSON = ui->NewTextBox(ctrl, CSTR(""), true);
	this->txtJSON->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtJSON->SetReadOnly(true);

	UIntOS size;
	UnsafeArray<const UInt8> buff = obj->GetRAWData(size);
	Text::StringBuilderUTF8 sb;
	Text::JSText::JSONWellFormat(buff, size, 0, sb);
	this->txtJSON->SetText(sb.ToCString());
}

SSWR::AVIRead::MIMEViewer::AVIRMIMEJSONViewer::~AVIRMIMEJSONViewer()
{
}
