#include "Stdafx.h"
#include "IO/MemoryStream.h"
#include "Parser/FileParser/X509Parser.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMEX509Viewer.h"

SSWR::AVIRead::MIMEViewer::AVIRMIMEX509Viewer::AVIRMIMEX509Viewer(NN<SSWR::AVIRead::AVIRCore> core, NN<UI::GUICore> ui, NN<UI::GUIClientControl> ctrl, NN<Media::ColorManagerSess> sess, NN<Text::MIMEObj::UnknownMIMEObj> obj) : SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer(core, ctrl, obj)
{
	this->obj = obj;

	this->tcMain = ui->NewTabControl(ctrl);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	
	this->tpDesc = this->tcMain->AddTabPage(CSTR("Desc"));
	this->txtDesc = ui->NewTextBox(this->tpDesc, CSTR(""), true);
	this->txtDesc->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpASN1 = this->tcMain->AddTabPage(CSTR("ASN1"));
	this->txtASN1 = ui->NewTextBox(this->tpASN1, CSTR(""), true);
	this->txtASN1->SetDockType(UI::GUIControl::DOCK_FILL);

	UOSInt dataSize;
	UnsafeArray<const UInt8> data = this->obj->GetRAWData(dataSize);
	NN<Crypto::Cert::X509File> asn1;
	if (Parser::FileParser::X509Parser::ParseBuff(Data::ByteArrayR(data, dataSize), obj->GetSourceNameObj()).SetTo(asn1))
	{
		Text::StringBuilderUTF8 sb;
		asn1->ToString(sb);
		this->txtDesc->SetText(sb.ToCString());
		sb.ClearStr();
		asn1->ToASN1String(sb);
		this->txtASN1->SetText(sb.ToCString());
		asn1.Delete();
	}
}

SSWR::AVIRead::MIMEViewer::AVIRMIMEX509Viewer::~AVIRMIMEX509Viewer()
{
}
