#include "Stdafx.h"
#include "IO/MemoryStream.h"
#include "Parser/FileParser/X509Parser.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMEX509Viewer.h"

SSWR::AVIRead::MIMEViewer::AVIRMIMEX509Viewer::AVIRMIMEX509Viewer(SSWR::AVIRead::AVIRCore *core, UI::GUICore *ui, UI::GUIClientControl *ctrl, Media::ColorManagerSess *sess, Text::MIMEObj::UnknownMIMEObj *obj) : SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer(core, ctrl, obj)
{
	this->obj = obj;

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, ctrl));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	
	this->tpDesc = this->tcMain->AddTabPage(CSTR("Desc"));
	NEW_CLASS(this->txtDesc, UI::GUITextBox(ui, this->tpDesc, CSTR(""), true));
	this->txtDesc->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpASN1 = this->tcMain->AddTabPage(CSTR("ASN1"));
	NEW_CLASS(this->txtASN1, UI::GUITextBox(ui, this->tpASN1, CSTR(""), true));
	this->txtASN1->SetDockType(UI::GUIControl::DOCK_FILL);

	UOSInt dataSize;
	const UInt8 *data = this->obj->GetRAWData(&dataSize);
	Net::ASN1Data *asn1 = Parser::FileParser::X509Parser::ParseBuff(data, dataSize, obj->GetSourceNameObj());
	if (asn1)
	{
		Text::StringBuilderUTF8 sb;
		asn1->ToString(&sb);
		this->txtDesc->SetText(sb.ToCString());
		sb.ClearStr();
		asn1->ToASN1String(&sb);
		this->txtASN1->SetText(sb.ToCString());
		DEL_CLASS(asn1);
	}
}

SSWR::AVIRead::MIMEViewer::AVIRMIMEX509Viewer::~AVIRMIMEX509Viewer()
{
}