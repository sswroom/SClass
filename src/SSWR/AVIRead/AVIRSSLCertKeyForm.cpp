#include "Stdafx.h"
#include "Crypto/Cert/CertUtil.h"
#include "Crypto/Cert/X509FileList.h"
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
	UI::FileDialog dlg(L"SSWR", L"AVIRead", L"AVIRSSLCert", false);
	dlg.AddFilter(CSTR("*.crt"), CSTR("Cert file"));
	dlg.SetAllowMultiSel(false);
	if (dlg.ShowDialog(me->GetHandle()))
	{
		me->LoadFile(dlg.GetFileName()->ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRSSLCertKeyForm::OnFileKeyClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSSLCertKeyForm *me = (SSWR::AVIRead::AVIRSSLCertKeyForm*)userObj;
	UI::FileDialog dlg(L"SSWR", L"AVIRead", L"AVIRSSLKey", false);
	dlg.AddFilter(CSTR("*.kye"), CSTR("Key file"));
	dlg.SetAllowMultiSel(false);
	if (dlg.ShowDialog(me->GetHandle()))
	{
		me->LoadFile(dlg.GetFileName()->ToCString());
	}
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
	me->txtGenCountry->GetText(sbCountry);
	me->txtGenCompany->GetText(sbCompany);
	me->txtGenCommonName->GetText(sbCommonName);
	if (sbCountry.GetLength() != 2)
	{
		UI::MessageDialog::ShowDialog(CSTR("Country must be 2 characters"), CSTR("SSL Cert/Key"), me);
		return;
	}
	if (sbCompany.GetLength() == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Please enter company"), CSTR("SSL Cert/Key"), me);
		return;
	}
	if (sbCommonName.GetLength() == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Please enter common name"), CSTR("SSL Cert/Key"), me);
		return;
	}
	Crypto::Cert::X509Cert *certASN1;
	Crypto::Cert::X509File *keyASN1;
	NotNullPtr<Net::SSLEngine> ssl;
	if (me->ssl.SetTo(ssl) && ssl->GenerateCert(sbCountry.ToCString(), sbCompany.ToCString(), sbCommonName.ToCString(), certASN1, keyASN1))
	{
		SDEL_CLASS(me->cert);
		SDEL_CLASS(me->key);
		me->cert = certASN1;
		me->key = keyASN1;
		me->SetDialogResult(DR_OK);
	}
	else
	{
		UI::MessageDialog::ShowDialog(CSTR("Error in generating certs"), CSTR("SSL Cert/Key"), me);
		return;
	}	
}

void __stdcall SSWR::AVIRead::AVIRSSLCertKeyForm::OnFileDrop(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles)
{
	SSWR::AVIRead::AVIRSSLCertKeyForm *me = (SSWR::AVIRead::AVIRSSLCertKeyForm*)userObj;
	UOSInt i = nFiles;
	while (i-- > 0)
	{
		me->LoadFile(files[i]->ToCString());
	}
}

void SSWR::AVIRead::AVIRSSLCertKeyForm::LoadFile(Text::CStringNN fileName)
{
	NotNullPtr<Net::ASN1Data> asn1;
	{
		IO::StmData::FileData fd(fileName, false);
		if (!asn1.Set((Net::ASN1Data*)this->core->GetParserList()->ParseFileType(fd, IO::ParserType::ASN1Data)))
		{
			UI::MessageDialog::ShowDialog(CSTR("Error in parsing file"), CSTR("SSL Cert/Key"), this);
			return;
		}
	}
	if (asn1->GetASN1Type() != Net::ASN1Data::ASN1Type::X509)
	{
		asn1.Delete();
		UI::MessageDialog::ShowDialog(CSTR("Error in parsing file"), CSTR("SSL Cert/Key"), this);
		return;
	}
	NotNullPtr<Crypto::Cert::X509File> x509 = NotNullPtr<Crypto::Cert::X509File>::ConvertFrom(asn1);
	if (x509->GetFileType() == Crypto::Cert::X509File::FileType::Cert)
	{
		SDEL_CLASS(this->cert);
		this->cert = (Crypto::Cert::X509Cert*)x509.Ptr();

		Text::StringBuilderUTF8 sb;
		this->cert->ToShortString(sb);
		this->lblFileCert->SetText(sb.ToCString());
		this->tcMain->SetSelectedPage(this->tpFile);

		this->ClearCACerts();
		NotNullPtr<Crypto::Cert::X509Cert> issuerCert;
		if (issuerCert.Set(Crypto::Cert::CertUtil::FindIssuer(NotNullPtr<Crypto::Cert::X509Cert>::ConvertFrom(x509))))
		{
			this->caCerts.Add(issuerCert);
		}		
	}
	else if (x509->GetFileType() == Crypto::Cert::X509File::FileType::FileList)
	{
		Bool found = false;
		Crypto::Cert::X509FileList *fileList = (Crypto::Cert::X509FileList*)x509.Ptr();
		NotNullPtr<Crypto::Cert::X509File> file;
		UOSInt i;
		UOSInt j;
		i = 0;
		j = fileList->GetFileCount();
		while (i < j)
		{
			if (fileList->GetFile(i).SetTo(file) && file->GetFileType() == Crypto::Cert::X509File::FileType::Cert)
			{
				if (!found)
				{
					found = true;
					this->cert = (Crypto::Cert::X509Cert*)file->Clone().Ptr();
					this->ClearCACerts();
				}
				else
				{
					this->caCerts.Add(NotNullPtr<Crypto::Cert::X509Cert>::ConvertFrom(file->Clone()));
				}
			}
			i++;
		}
		DEL_CLASS(fileList);
		if (found)
		{
			Text::StringBuilderUTF8 sb;
			this->cert->ToShortString(sb);
			this->lblFileCert->SetText(sb.ToCString());
			this->tcMain->SetSelectedPage(this->tpFile);
		}
	}
	else if (x509->GetFileType() == Crypto::Cert::X509File::FileType::PrivateKey || x509->GetFileType() == Crypto::Cert::X509File::FileType::Key)
	{
		SDEL_CLASS(this->key);
		this->key = x509.Ptr();

		Text::StringBuilderUTF8 sb;
		this->key->ToShortString(sb);
		this->lblFileKey->SetText(sb.ToCString());
		this->tcMain->SetSelectedPage(this->tpFile);
	}
	else
	{
		x509.Delete();
	}
}

void SSWR::AVIRead::AVIRSSLCertKeyForm::ClearCACerts()
{
	this->caCerts.DeleteAll();
}

SSWR::AVIRead::AVIRSSLCertKeyForm::AVIRSSLCertKeyForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Optional<Net::SSLEngine> ssl, Crypto::Cert::X509Cert *cert, Crypto::Cert::X509File *key, NotNullPtr<Data::ArrayListNN<Crypto::Cert::X509Cert>> caCerts) : UI::GUIForm(parent, 456, 200, ui)
{
	this->SetText(CSTR("SSL Cert/Key"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	
	this->core = core;
	this->ssl = ssl;
	this->initCert = cert;
	this->initKey = key;
	Data::ArrayIterator<NotNullPtr<Crypto::Cert::X509Cert>> it = caCerts->Iterator();
	while (it.HasNext())
	{
		this->caCerts.Add(NotNullPtr<Crypto::Cert::X509Cert>::ConvertFrom(it.Next()->Clone()));
	}
	this->cert = 0;
	this->key = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->HandleFormClosed(OnFormClosed, this);

	NEW_CLASSNN(this->pnlCurr, UI::GUIPanel(ui, *this));
	this->pnlCurr->SetRect(0, 0, 100, 55, false);
	this->pnlCurr->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblCurrCert, UI::GUILabel(ui, this->pnlCurr, CSTR("Curr Cert")));
	this->lblCurrCert->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtCurrCert, UI::GUITextBox(ui, this->pnlCurr, CSTR("")));
	this->txtCurrCert->SetRect(104, 4, 200, 23, false);
	this->txtCurrCert->SetReadOnly(true);
	NEW_CLASS(this->lblCurrKey, UI::GUILabel(ui, this->pnlCurr, CSTR("Curr Key")));
	this->lblCurrKey->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtCurrKey, UI::GUITextBox(ui, this->pnlCurr, CSTR("")));
	this->txtCurrKey->SetRect(104, 28, 200, 23, false);
	this->txtCurrKey->SetReadOnly(true);
	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, *this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpFile = this->tcMain->AddTabPage(CSTR("File"));
	NEW_CLASS(this->btnFileCert, UI::GUIButton(ui, this->tpFile, CSTR("Cert")));
	this->btnFileCert->SetRect(4, 4, 75, 23, false);
	this->btnFileCert->HandleButtonClick(OnFileCertClicked, this);
	NEW_CLASS(this->lblFileCert, UI::GUILabel(ui, this->tpFile, CSTR("")));
	this->lblFileCert->SetRect(84, 4, 200, 23, false);
	NEW_CLASS(this->btnFileKey, UI::GUIButton(ui, this->tpFile, CSTR("Key")));
	this->btnFileKey->SetRect(4, 28, 75, 23, false);
	this->btnFileKey->HandleButtonClick(OnFileKeyClicked, this);
	NEW_CLASS(this->lblFileKey, UI::GUILabel(ui, this->tpFile, CSTR("")));
	this->lblFileKey->SetRect(84, 28, 200, 23, false);
	NEW_CLASS(this->btnFileConfirm, UI::GUIButton(ui, this->tpFile, CSTR("OK")));
	this->btnFileConfirm->SetRect(4, 52, 75, 23, false);
	this->btnFileConfirm->HandleButtonClick(OnFileConfirmClicked, this);

	this->tpGenerate = this->tcMain->AddTabPage(CSTR("Generate"));
	NEW_CLASS(this->lblGenCountry, UI::GUILabel(ui, this->tpGenerate, CSTR("Country")));
	this->lblGenCountry->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtGenCountry, UI::GUITextBox(ui, this->tpGenerate, CSTR("HK")));
	this->txtGenCountry->SetRect(104, 4, 60, 23, false);
	NEW_CLASS(this->lblGenCompany, UI::GUILabel(ui, this->tpGenerate, CSTR("Company")));
	this->lblGenCompany->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtGenCompany, UI::GUITextBox(ui, this->tpGenerate, CSTR("SSWR")));
	this->txtGenCompany->SetRect(104, 28, 200, 23, false);
	NEW_CLASS(this->lblGenCommonName, UI::GUILabel(ui, this->tpGenerate, CSTR("CommonName")));
	this->lblGenCommonName->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtGenCommonName, UI::GUITextBox(ui, this->tpGenerate, CSTR("localhost")));
	this->txtGenCommonName->SetRect(104, 52, 200, 23, false);
	NEW_CLASS(this->btnGenerate, UI::GUIButton(ui, this->tpGenerate, CSTR("Generate")));
	this->btnGenerate->SetRect(104, 76, 75, 23, false);
	this->btnGenerate->HandleButtonClick(OnGenerateClicked, this);

	Text::StringBuilderUTF8 sb;
	if (this->initCert)
	{
		this->initCert->ToShortName(sb);
		this->txtCurrCert->SetText(sb.ToCString());
	}
	if (this->initKey)
	{
		sb.ClearStr();
		this->initKey->ToShortName(sb);
		this->txtCurrKey->SetText(sb.ToCString());
	}
	this->HandleDropFiles(OnFileDrop, this);
}

SSWR::AVIRead::AVIRSSLCertKeyForm::~AVIRSSLCertKeyForm()
{
	this->ClearCACerts();
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

UOSInt SSWR::AVIRead::AVIRSSLCertKeyForm::GetCACerts(NotNullPtr<Data::ArrayListNN<Crypto::Cert::X509Cert>> caCerts)
{
	Data::ArrayIterator<NotNullPtr<Crypto::Cert::X509Cert>> it = this->caCerts.Iterator();
	while (it.HasNext())
	{
		caCerts->Add(NotNullPtr<Crypto::Cert::X509Cert>::ConvertFrom(it.Next()->Clone()));
	}
	return this->caCerts.GetCount();
}
