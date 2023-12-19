#include "Stdafx.h"
#include "Crypto/Cert/X509File.h"
#include "Data/ByteBuffer.h"
#include "Net/ASN1Util.h"
#include "SSWR/AVIRead/AVIRASN1ParseForm.h"
#include "Text/TextBinEnc/Base64Enc.h"

void __stdcall SSWR::AVIRead::AVIRASN1ParseForm::OnParseClicked(void *userObj)
{
	SSWR::AVIRead::AVIRASN1ParseForm *me = (SSWR::AVIRead::AVIRASN1ParseForm*)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtData->GetText(sb);
	if (sb.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please input data"), CSTR("ASN.1 Parse"), me);
		return;
	}
	Text::TextBinEnc::Base64Enc b64;
	UOSInt len = b64.CalcBinSize(sb.ToString(), sb.GetLength());
	Data::ByteBuffer buff(len);
	if (b64.DecodeBin(sb.ToString(), sb.GetLength(), buff.Ptr()) != len)
	{
		me->ui->ShowMsgOK(CSTR("Error in decoding Base64 data"), CSTR("ASN.1 Parse"), me);
		return;
	}
	if (buff[0] != 0x30)
	{
		me->ui->ShowMsgOK(CSTR("Data is not valid ASN.1 format"), CSTR("ASN.1 Parse"), me);
		return;
	}
	UInt32 baseLen;
	UOSInt ofst = Net::ASN1Util::PDUParseLen(buff.Ptr(), 1, len, &baseLen);
	if (ofst + baseLen == len)
	{
		Crypto::Cert::X509File *x509 = 0;
		NotNullPtr<Crypto::Cert::X509File> pobj;
		switch (me->cboType->GetSelectedIndex())
		{
		case 0:
			NEW_CLASS(x509, Crypto::Cert::X509Cert(CSTR("Cert"), buff));
			break;
		}
		if (pobj.Set(x509))
		{
			me->core->OpenObject(pobj);
		}
		else
		{
			me->ui->ShowMsgOK(CSTR("Unknown Type"), CSTR("ASN.1 Parse"), me);
		}
	}
	else
	{
		me->ui->ShowMsgOK(CSTR("Data seems not valid ASN.1 format"), CSTR("ASN.1 Parse"), me);
	}
}

SSWR::AVIRead::AVIRASN1ParseForm::AVIRASN1ParseForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("ASN.1 Parse"));

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASSNN(this->pnlOptions, UI::GUIPanel(ui, *this));
	this->pnlOptions->SetRect(0, 0, 100, 51, false);
	this->pnlOptions->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblType = ui->NewLabel(this->pnlOptions, CSTR("Type"));
	this->lblType->SetRect(4, 4, 100, 23, false);
	this->cboType = ui->NewComboBox(this->pnlOptions, false);
	this->cboType->SetRect(104, 4, 200, 23, false);
	this->cboType->AddItem(CSTR("X.509 Certificate"), 0);
	this->cboType->SetSelectedIndex(0);
	this->lblData = ui->NewLabel(this->pnlOptions, CSTR("Base64 encoded data"));
	this->lblData->SetRect(4, 28, 100, 23, false);
	NEW_CLASSNN(this->pnlControl, UI::GUIPanel(ui, *this));
	this->pnlControl->SetRect(0, 0, 100, 31, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->btnParse = ui->NewButton(this->pnlControl, CSTR("Parse"));
	this->btnParse->SetRect(4, 4, 75, 23, false);
	this->btnParse->HandleButtonClick(OnParseClicked, this);
	this->txtData = ui->NewTextBox(*this, CSTR(""), true);
	this->txtData->SetDockType(UI::GUIControl::DOCK_FILL);
}

SSWR::AVIRead::AVIRASN1ParseForm::~AVIRASN1ParseForm()
{
}

void SSWR::AVIRead::AVIRASN1ParseForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
