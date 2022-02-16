#include "Stdafx.h"
#include "Crypto/Cert/X509File.h"
#include "Net/ASN1Util.h"
#include "SSWR/AVIRead/AVIRASN1ParseForm.h"
#include "Text/TextBinEnc/Base64Enc.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRASN1ParseForm::OnParseClicked(void *userObj)
{
	SSWR::AVIRead::AVIRASN1ParseForm *me = (SSWR::AVIRead::AVIRASN1ParseForm*)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtData->GetText(&sb);
	if (sb.GetLength() == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please input data", (const UTF8Char*)"ASN.1 Parse", me);
		return;
	}
	Text::TextBinEnc::Base64Enc b64;
	UOSInt len = b64.CalcBinSize(sb.ToString(), sb.GetLength());
	UInt8 *buff = MemAlloc(UInt8, len);
	if (b64.DecodeBin(sb.ToString(), sb.GetLength(), buff) != len)
	{
		MemFree(buff);
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in decoding Base64 data", (const UTF8Char*)"ASN.1 Parse", me);
		return;
	}
	if (buff[0] != 0x30)
	{
		MemFree(buff);
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Data is not valid ASN.1 format", (const UTF8Char*)"ASN.1 Parse", me);
		return;
	}
	UInt32 baseLen;
	UOSInt ofst = Net::ASN1Util::PDUParseLen(buff, 1, len, &baseLen);
	if (ofst + baseLen == len)
	{
		Crypto::Cert::X509File *x509 = 0;
		switch (me->cboType->GetSelectedIndex())
		{
		case 0:
			NEW_CLASS(x509, Crypto::Cert::X509Cert(CSTR("Cert"), buff, len));
			break;
		}
		MemFree(buff);
		if (x509)
		{
			me->core->OpenObject(x509);
		}
		else
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Unknown Type", (const UTF8Char*)"ASN.1 Parse", me);
		}
	}
	else
	{
		MemFree(buff);
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Data seems not valid ASN.1 format", (const UTF8Char*)"ASN.1 Parse", me);
	}
}

SSWR::AVIRead::AVIRASN1ParseForm::AVIRASN1ParseForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("ASN.1 Parse"));

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlOptions, UI::GUIPanel(ui, this));
	this->pnlOptions->SetRect(0, 0, 100, 51, false);
	this->pnlOptions->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblType, UI::GUILabel(ui, this->pnlOptions, (const UTF8Char*)"Type"));
	this->lblType->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->cboType, UI::GUIComboBox(ui, this->pnlOptions, false));
	this->cboType->SetRect(104, 4, 200, 23, false);
	this->cboType->AddItem(CSTR("X.509 Certificate"), 0);
	this->cboType->SetSelectedIndex(0);
	NEW_CLASS(this->lblData, UI::GUILabel(ui, this->pnlOptions, (const UTF8Char*)"Base64 encoded data"));
	this->lblData->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->pnlControl, UI::GUIPanel(ui, this));
	this->pnlControl->SetRect(0, 0, 100, 31, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->btnParse, UI::GUIButton(ui, this->pnlControl, CSTR("Parse")));
	this->btnParse->SetRect(4, 4, 75, 23, false);
	this->btnParse->HandleButtonClick(OnParseClicked, this);
	NEW_CLASS(this->txtData, UI::GUITextBox(ui, this, CSTR(""), true));
	this->txtData->SetDockType(UI::GUIControl::DOCK_FILL);
}

SSWR::AVIRead::AVIRASN1ParseForm::~AVIRASN1ParseForm()
{
}

void SSWR::AVIRead::AVIRASN1ParseForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
