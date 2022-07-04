#include "Stdafx.h"
#include "Crypto/Cert/X509FileList.h"
#include "Crypto/Cert/X509PrivKey.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRASN1DataForm.h"
#include "Text/StringBuilderUTF8.h"

enum MenuItem
{
	MNU_SAVE = 100,
	MNU_CERT_0 = 500,
	MNU_KEY_CREATE = 600,
	MNU_CERT_EXT_KEY = 601
};

SSWR::AVIRead::AVIRASN1DataForm::AVIRASN1DataForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Net::ASN1Data *asn1) : UI::GUIForm(parent, 1024, 768, ui)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("ASN1 Data - "));
	sb.Append(asn1->GetSourceNameObj());
	this->SetText(sb.ToCString());
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->asn1 = asn1;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	UI::GUIMenu *mnu;
	NEW_CLASS(this->mnuMain, UI::GUIMainMenu());
	mnu = this->mnuMain->AddSubMenu(CSTR("&File"));
	mnu->AddItem(CSTR("Save"), MNU_SAVE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	if (this->asn1->GetASN1Type() == Net::ASN1Data::ASN1Type::X509)
	{
		Crypto::Cert::X509File *x509 = (Crypto::Cert::X509Cert*)this->asn1;
		Text::StringBuilderUTF8 sb;
		UI::GUIMenu *mnu2;
		UOSInt i;
		UOSInt j;
		mnu2 = mnu->AddSubMenu(CSTR("Certs"));
		i = 0;
		j = x509->GetCertCount();
		if (j == 0)
		{
			mnu2->AddItem(CSTR("None"), MNU_CERT_0, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
		}
		else
		{
			while (i < j)
			{
				sb.ClearStr();
				if (!x509->GetCertName(i, &sb))
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("Cert "));
					sb.AppendUOSInt(i);
				}
				mnu2->AddItem(sb.ToCString(), (UInt16)(MNU_CERT_0 + i), UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
				i++;
			}
		}
		if (x509->GetFileType() == Crypto::Cert::X509File::FileType::PrivateKey)
		{
			mnu2 = mnu->AddSubMenu(CSTR("Private Key"));
			mnu2->AddItem(CSTR("Create Key"), MNU_KEY_CREATE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
		}
		else if (x509->GetFileType() == Crypto::Cert::X509File::FileType::Cert)
		{
			mnu2 = mnu->AddSubMenu(CSTR("Key"));
			mnu2->AddItem(CSTR("Extract Key"), MNU_CERT_EXT_KEY, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
		}
	}
	this->SetMenu(this->mnuMain);

	NEW_CLASS(this->pnlStatus, UI::GUIPanel(ui, this));
	this->pnlStatus->SetRect(0, 0, 100, 31, false);
	this->pnlStatus->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->lblStatus, UI::GUILabel(ui, this->pnlStatus, CSTR("Valid Status")));
	this->lblStatus->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtStatus, UI::GUITextBox(ui, this->pnlStatus, CSTR("")));
	this->txtStatus->SetRect(104, 4, 200, 23, false);
	this->txtStatus->SetReadOnly(true);
	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	
	this->tpDesc = this->tcMain->AddTabPage(CSTR("Desc"));
	NEW_CLASS(this->txtDesc, UI::GUITextBox(ui, this->tpDesc, CSTR(""), true));
	this->txtDesc->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpASN1 = this->tcMain->AddTabPage(CSTR("ASN1"));
	NEW_CLASS(this->txtASN1, UI::GUITextBox(ui, this->tpASN1, CSTR(""), true));
	this->txtASN1->SetDockType(UI::GUIControl::DOCK_FILL);

	sb.ClearStr();
	this->asn1->ToString(&sb);
	this->txtDesc->SetText(sb.ToCString());
	sb.ClearStr();
	this->asn1->ToASN1String(&sb);
	this->txtASN1->SetText(sb.ToCString());

	if (this->asn1->GetASN1Type() == Net::ASN1Data::ASN1Type::X509)
	{
		Crypto::Cert::X509File *x509 = (Crypto::Cert::X509File*)this->asn1;
		Net::SSLEngine *ssl = Net::SSLEngineFactory::Create(this->core->GetSocketFactory(), false);
		this->txtStatus->SetText(Crypto::Cert::X509File::ValidStatusGetDesc(x509->IsValid(ssl, ssl->GetTrustStore())));
		SDEL_CLASS(ssl);

		if (x509->GetFileType() == Crypto::Cert::X509File::FileType::FileList)
		{
			Crypto::Cert::X509FileList *fileList = (Crypto::Cert::X509FileList*)x509;
			sb.ClearStr();
			if (((Crypto::Cert::X509Cert*)fileList->GetFile(0))->GetSubjectCN(&sb))
			{
				this->tcMain->SetTabPageName(1, sb.ToCString());
			}
			UI::GUITabPage *tp;
			UI::GUITextBox *txt;
			Crypto::Cert::X509File *file;
			UOSInt i = 1;
			UOSInt j = fileList->GetFileCount();
			while (i < j)
			{
				file = fileList->GetFile(i);
				sb.ClearStr();
				file->ToShortName(&sb);
				tp = this->tcMain->AddTabPage(sb.ToCString());
				sb.ClearStr();
				file->ToASN1String(&sb);
				NEW_CLASS(txt, UI::GUITextBox(ui, tp, sb.ToCString(), true));
				txt->SetDockType(UI::GUIControl::DOCK_FILL);

				i++;
			}
		}
	}
	else
	{
		this->txtStatus->SetText(CSTR("-"));
	}
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
	case MNU_KEY_CREATE:
		{
			Crypto::Cert::X509Key *key = ((Crypto::Cert::X509PrivKey*)this->asn1)->CreateKey();
			if (key)
			{
				this->core->OpenObject(key);
			}
		}
	case MNU_CERT_EXT_KEY:
		{
			Crypto::Cert::X509Key *key = ((Crypto::Cert::X509Cert*)this->asn1)->GetNewPublicKey();
			if (key)
			{
				this->core->OpenObject(key);
			}
		}
	default:
		if (cmdId >= MNU_CERT_0)
		{
			Crypto::Cert::X509Cert *cert = ((Crypto::Cert::X509File*)this->asn1)->GetNewCert((UOSInt)(cmdId - MNU_CERT_0));
			if (cert)
			{
				this->core->OpenObject(cert);
			}
		}
		break;
	}
}
