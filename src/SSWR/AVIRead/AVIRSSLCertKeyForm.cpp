#include "Stdafx.h"
#include "IO/StmData/FileData.h"
#include "SSWR/AVIRead/AVIRSSLCertKeyForm.h"
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRSSLCertKeyForm::OnFormClosed(void *userObj, UI::GUIForm *frm)
{
	SSWR::AVIRead::AVIRSSLCertKeyForm *me = (SSWR::AVIRead::AVIRSSLCertKeyForm*)userObj;
	if (me->dialogResult != DR_OK)
	{
		SDEL_CLASS(me->cert);
		SDEL_CLASS(me->key);
	}
}

void __stdcall SSWR::AVIRead::AVIRSSLCertKeyForm::OnFileCertClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSSLCertKeyForm *me = (SSWR::AVIRead::AVIRSSLCertKeyForm*)userObj;
	UI::FileDialog *dlg;
	NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"AVIRSSLCert", false));
	dlg->AddFilter((const UTF8Char*)"*.crt", (const UTF8Char*)"Cert file");
	dlg->SetAllowMultiSel(false);
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->LoadFile(dlg->GetFileName()->v);
	}
	DEL_CLASS(dlg);
}

void __stdcall SSWR::AVIRead::AVIRSSLCertKeyForm::OnFileKeyClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSSLCertKeyForm *me = (SSWR::AVIRead::AVIRSSLCertKeyForm*)userObj;
	UI::FileDialog *dlg;
	NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"AVIRSSLKey", false));
	dlg->AddFilter((const UTF8Char*)"*.kye", (const UTF8Char*)"Key file");
	dlg->SetAllowMultiSel(false);
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->LoadFile(dlg->GetFileName()->v);
	}
	DEL_CLASS(dlg);
}

void __stdcall SSWR::AVIRead::AVIRSSLCertKeyForm::OnFileConfirmClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSSLCertKeyForm *me = (SSWR::AVIRead::AVIRSSLCertKeyForm*)userObj;
	if (me->cert && me->key)
	{
		me->SetDialogResult(DR_OK);
	}
}

void __stdcall SSWR::AVIRead::AVIRSSLCertKeyForm::OnGenerateClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSSLCertKeyForm *me = (SSWR::AVIRead::AVIRSSLCertKeyForm*)userObj;
	Text::StringBuilderUTF8 sbCountry;
	Text::StringBuilderUTF8 sbCompany;
	Text::StringBuilderUTF8 sbCommonName;
	me->txtGenCountry->GetText(&sbCountry);
	me->txtGenCompany->GetText(&sbCompany);
	me->txtGenCommonName->GetText(&sbCommonName);
	if (sbCountry.GetLength() != 2)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Country must be 2 characters", (const UTF8Char*)"SSL Cert/Key", me);
		return;
	}
	if (sbCompany.GetLength() == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter company", (const UTF8Char*)"SSL Cert/Key", me);
		return;
	}
	if (sbCommonName.GetLength() == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter common name", (const UTF8Char*)"SSL Cert/Key", me);
		return;
	}
	Crypto::Cert::X509Cert *certASN1;
	Crypto::Cert::X509File *keyASN1;
	if (me->ssl->GenerateCert(sbCountry.ToString(), sbCompany.ToString(), sbCommonName.ToString(), &certASN1, &keyASN1))
	{
		SDEL_CLASS(me->cert);
		SDEL_CLASS(me->key);
		me->cert = certASN1;
		me->key = keyASN1;
		me->SetDialogResult(DR_OK);
	}
	else
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in generating certs", (const UTF8Char*)"SSL Cert/Key", me);
		return;
	}	
}

void __stdcall SSWR::AVIRead::AVIRSSLCertKeyForm::OnFileDrop(void *userObj, const UTF8Char **files, UOSInt nFiles)
{
	SSWR::AVIRead::AVIRSSLCertKeyForm *me = (SSWR::AVIRead::AVIRSSLCertKeyForm*)userObj;
	UOSInt i = nFiles;
	while (i-- > 0)
	{
		me->LoadFile(files[i]);
	}
}

void SSWR::AVIRead::AVIRSSLCertKeyForm::LoadFile(const UTF8Char *fileName)
{
	IO::StmData::FileData *fd;
	NEW_CLASS(fd, IO::StmData::FileData(fileName, false));
	Net::ASN1Data *asn1 = (Net::ASN1Data*)this->core->GetParserList()->ParseFileType(fd, IO::ParserType::ASN1Data);
	DEL_CLASS(fd);
	if (asn1 == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in parsing file", (const UTF8Char*)"SSL Cert/Key", this);
		return;
	}
	if (asn1->GetASN1Type() != Net::ASN1Data::ASN1Type::X509)
	{
		DEL_CLASS(asn1);
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in parsing file", (const UTF8Char*)"SSL Cert/Key", this);
		return;
	}
	Crypto::Cert::X509File *x509 = (Crypto::Cert::X509File*)asn1;
	if (x509->GetFileType() == Crypto::Cert::X509File::FileType::Cert)
	{
		SDEL_CLASS(this->cert);
		this->cert = (Crypto::Cert::X509Cert*)x509;

		Text::StringBuilderUTF8 sb;
		this->cert->ToShortString(&sb);
		this->lblFileCert->SetText(sb.ToString());
		this->tcMain->SetSelectedPage(this->tpFile);
	}
	else if (x509->GetFileType() == Crypto::Cert::X509File::FileType::PrivateKey || x509->GetFileType() == Crypto::Cert::X509File::FileType::Key)
	{
		SDEL_CLASS(this->key);
		this->key = x509;

		Text::StringBuilderUTF8 sb;
		this->key->ToShortString(&sb);
		this->lblFileKey->SetText(sb.ToString());
		this->tcMain->SetSelectedPage(this->tpFile);
	}
	else
	{
		DEL_CLASS(x509);
	}
}

SSWR::AVIRead::AVIRSSLCertKeyForm::AVIRSSLCertKeyForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Net::SSLEngine *ssl, Crypto::Cert::X509Cert *cert, Crypto::Cert::X509File *key) : UI::GUIForm(parent, 456, 200, ui)
{
	this->SetText((const UTF8Char*)"SSL Cert/Key");
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	
	this->core = core;
	this->ssl = ssl;
	this->initCert = cert;
	this->initKey = key;
	this->cert = 0;
	this->key = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->HandleFormClosed(OnFormClosed, this);

	NEW_CLASS(this->pnlCurr, UI::GUIPanel(ui, this));
	this->pnlCurr->SetRect(0, 0, 100, 55, false);
	this->pnlCurr->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblCurrCert, UI::GUILabel(ui, this->pnlCurr, (const UTF8Char*)"Curr Cert"));
	this->lblCurrCert->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtCurrCert, UI::GUITextBox(ui, this->pnlCurr, (const UTF8Char*)""));
	this->txtCurrCert->SetRect(104, 4, 200, 23, false);
	this->txtCurrCert->SetReadOnly(true);
	NEW_CLASS(this->lblCurrKey, UI::GUILabel(ui, this->pnlCurr, (const UTF8Char*)"Curr Key"));
	this->lblCurrKey->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtCurrKey, UI::GUITextBox(ui, this->pnlCurr, (const UTF8Char*)""));
	this->txtCurrKey->SetRect(104, 28, 200, 23, false);
	this->txtCurrKey->SetReadOnly(true);
	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpFile = this->tcMain->AddTabPage((const UTF8Char*)"File");
	NEW_CLASS(this->btnFileCert, UI::GUIButton(ui, this->tpFile, (const UTF8Char*)"Cert"));
	this->btnFileCert->SetRect(4, 4, 75, 23, false);
	this->btnFileCert->HandleButtonClick(OnFileCertClicked, this);
	NEW_CLASS(this->lblFileCert, UI::GUILabel(ui, this->tpFile, (const UTF8Char*)""));
	this->lblFileCert->SetRect(84, 4, 200, 23, false);
	NEW_CLASS(this->btnFileKey, UI::GUIButton(ui, this->tpFile, (const UTF8Char*)"Key"));
	this->btnFileKey->SetRect(4, 28, 75, 23, false);
	this->btnFileKey->HandleButtonClick(OnFileKeyClicked, this);
	NEW_CLASS(this->lblFileKey, UI::GUILabel(ui, this->tpFile, (const UTF8Char*)""));
	this->lblFileKey->SetRect(84, 28, 200, 23, false);
	NEW_CLASS(this->btnFileConfirm, UI::GUIButton(ui, this->tpFile, (const UTF8Char*)"OK"));
	this->btnFileConfirm->SetRect(4, 52, 75, 23, false);
	this->btnFileConfirm->HandleButtonClick(OnFileConfirmClicked, this);

	this->tpGenerate = this->tcMain->AddTabPage((const UTF8Char*)"Generate");
	NEW_CLASS(this->lblGenCountry, UI::GUILabel(ui, this->tpGenerate, (const UTF8Char*)"Country"));
	this->lblGenCountry->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtGenCountry, UI::GUITextBox(ui, this->tpGenerate, (const UTF8Char*)"HK"));
	this->txtGenCountry->SetRect(104, 4, 60, 23, false);
	NEW_CLASS(this->lblGenCompany, UI::GUILabel(ui, this->tpGenerate, (const UTF8Char*)"Company"));
	this->lblGenCompany->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtGenCompany, UI::GUITextBox(ui, this->tpGenerate, (const UTF8Char*)"SSWR"));
	this->txtGenCompany->SetRect(104, 28, 200, 23, false);
	NEW_CLASS(this->lblGenCommonName, UI::GUILabel(ui, this->tpGenerate, (const UTF8Char*)"CommonName"));
	this->lblGenCommonName->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtGenCommonName, UI::GUITextBox(ui, this->tpGenerate, (const UTF8Char*)"localhost"));
	this->txtGenCommonName->SetRect(104, 52, 200, 23, false);
	NEW_CLASS(this->btnGenerate, UI::GUIButton(ui, this->tpGenerate, (const UTF8Char*)"Generate"));
	this->btnGenerate->SetRect(104, 76, 75, 23, false);
	this->btnGenerate->HandleButtonClick(OnGenerateClicked, this);

	Text::StringBuilderUTF8 sb;
	if (this->initCert)
	{
		this->initCert->ToShortName(&sb);
		this->txtCurrCert->SetText(sb.ToString());
	}
	if (this->initKey)
	{
		sb.ClearStr();
		this->initKey->ToShortName(&sb);
		this->txtCurrKey->SetText(sb.ToString());
	}
	this->HandleDropFiles(OnFileDrop, this);
}

SSWR::AVIRead::AVIRSSLCertKeyForm::~AVIRSSLCertKeyForm()
{
}

void SSWR::AVIRead::AVIRSSLCertKeyForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

Crypto::Cert::X509Cert *SSWR::AVIRead::AVIRSSLCertKeyForm::GetCert()
{
	return this->cert;
}

Crypto::Cert::X509File *SSWR::AVIRead::AVIRSSLCertKeyForm::GetKey()
{
	return this->key;
}
