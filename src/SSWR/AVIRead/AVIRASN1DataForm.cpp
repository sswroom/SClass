#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRASN1DataForm.h"
#include "Text/StringBuilderUTF8.h"

enum MenuItem
{
	MNU_SAVE = 100,
	MNU_CERT_0 = 500
};

SSWR::AVIRead::AVIRASN1DataForm::AVIRASN1DataForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Net::ASN1Data *asn1) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText((const UTF8Char*)"ASN1 Data");
	this->SetFont(0, 8.25, false);

	this->core = core;
	this->asn1 = asn1;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	UI::GUIMenu *mnu;
	NEW_CLASS(this->mnuMain, UI::GUIMainMenu());
	mnu = this->mnuMain->AddSubMenu((const UTF8Char*)"&File");
	mnu->AddItem((const UTF8Char*)"Save", MNU_SAVE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	if (this->asn1->GetASN1Type() == Net::ASN1Data::ASN1Type::X509)
	{
		Crypto::Cert::X509File *x509 = (Crypto::Cert::X509Cert*)this->asn1;
		Text::StringBuilderUTF8 sb;
		UI::GUIMenu *mnu2;
		UOSInt i;
		UOSInt j;
		mnu2 = mnu->AddSubMenu((const UTF8Char*)"Certs");
		i = 0;
		j = x509->GetCertCount();
		if (j == 0)
		{
			mnu2->AddItem((const UTF8Char*)"None", MNU_CERT_0, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
		}
		else
		{
			while (i < j)
			{
				sb.ClearStr();
				if (!x509->GetCertName(i, &sb))
				{
					sb.ClearStr();
					sb.Append((const UTF8Char*)"Cert ");
					sb.AppendUOSInt(i);
				}
				mnu2->AddItem(sb.ToString(), (UInt16)(MNU_CERT_0 + i), UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
				i++;
			}
		}
	}
	this->SetMenu(this->mnuMain);

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	
	this->tpDesc = this->tcMain->AddTabPage((const UTF8Char*)"Desc");
	NEW_CLASS(this->txtDesc, UI::GUITextBox(ui, this->tpDesc, (const UTF8Char*)"", true));
	this->txtDesc->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpASN1 = this->tcMain->AddTabPage((const UTF8Char*)"ASN1");
	NEW_CLASS(this->txtASN1, UI::GUITextBox(ui, this->tpASN1, (const UTF8Char*)"", true));
	this->txtASN1->SetDockType(UI::GUIControl::DOCK_FILL);

	Text::StringBuilderUTF8 sb;
	this->asn1->ToString(&sb);
	this->txtDesc->SetText(sb.ToString());
	sb.ClearStr();
	this->asn1->ToASN1String(&sb);
	this->txtASN1->SetText(sb.ToString());
}

SSWR::AVIRead::AVIRASN1DataForm::~AVIRASN1DataForm()
{
	DEL_CLASS(this->asn1);
}

void SSWR::AVIRead::AVIRASN1DataForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRASN1DataForm::EventMenuClicked(UInt16 cmdId)
{
	switch (cmdId)
	{
	case MNU_SAVE:
		this->core->SaveData(this, this->asn1, L"ASN1Data");
		break;
	default:
		if (cmdId >= MNU_CERT_0)
		{
			Crypto::Cert::X509Cert *cert = ((Crypto::Cert::X509File*)this->asn1)->NewCert(cmdId - MNU_CERT_0);
			if (cert)
			{
				this->core->OpenObject(cert);
			}
		}
		break;
	}
}
