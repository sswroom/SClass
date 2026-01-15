#include "Stdafx.h"
#include "Crypto/Cert/CertUtil.h"
#include "Crypto/Cert/X509FileList.h"
#include "IO/StmData/FileData.h"
#include "SSWR/AVIRead/AVIRSSLCertKeyForm.h"
#include "UI/GUIFileDialog.h"

void __stdcall SSWR::AVIRead::AVIRSSLCertKeyForm::OnFormClosed(AnyType userObj, NN<UI::GUIForm> frm)
{
	NN<SSWR::AVIRead::AVIRSSLCertKeyForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSSLCertKeyForm>();
	if (me->dialogResult != DR_OK)
	{
		me->cert.Delete();
		me->key.Delete();
	}
}

void __stdcall SSWR::AVIRead::AVIRSSLCertKeyForm::OnFileCertClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSSLCertKeyForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSSLCertKeyForm>();
	NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"AVIRSSLCert", false);
	dlg->AddFilter(CSTR("*.crt"), CSTR("Cert file"));
	dlg->SetAllowMultiSel(false);
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->LoadFile(dlg->GetFileName()->ToCString());
	}
	dlg.Delete();
}

void __stdcall SSWR::AVIRead::AVIRSSLCertKeyForm::OnFileKeyClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSSLCertKeyForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSSLCertKeyForm>();
	NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"AVIRSSLKey", false);
	dlg->AddFilter(CSTR("*.kye"), CSTR("Key file"));
	dlg->SetAllowMultiSel(false);
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->LoadFile(dlg->GetFileName()->ToCString());
	}
	dlg.Delete();
}

void __stdcall SSWR::AVIRead::AVIRSSLCertKeyForm::OnFileConfirmClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSSLCertKeyForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSSLCertKeyForm>();
	if (me->cert.NotNull() && me->key.NotNull())
	{
		me->SetDialogResult(DR_OK);
	}
}

void __stdcall SSWR::AVIRead::AVIRSSLCertKeyForm::OnGenerateClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSSLCertKeyForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSSLCertKeyForm>();
	Text::StringBuilderUTF8 sbCountry;
	Text::StringBuilderUTF8 sbCompany;
	Text::StringBuilderUTF8 sbCommonName;
	me->txtGenCountry->GetText(sbCountry);
	me->txtGenCompany->GetText(sbCompany);
	me->txtGenCommonName->GetText(sbCommonName);
	if (sbCountry.GetLength() != 2)
	{
		me->ui->ShowMsgOK(CSTR("Country must be 2 characters"), CSTR("SSL Cert/Key"), me);
		return;
	}
	if (sbCompany.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter company"), CSTR("SSL Cert/Key"), me);
		return;
	}
	if (sbCommonName.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter common name"), CSTR("SSL Cert/Key"), me);
		return;
	}
	NN<Crypto::Cert::X509Cert> certASN1;
	NN<Crypto::Cert::X509File> keyASN1;
	NN<Net::SSLEngine> ssl;
	if (me->ssl.SetTo(ssl) && ssl->GenerateCert(sbCountry.ToCString(), sbCompany.ToCString(), sbCommonName.ToCString(), certASN1, keyASN1, Net::SSLEngine::GetRSAKeyLength()))
	{
		me->cert.Delete();
		me->key.Delete();
		me->cert = certASN1;
		me->key = keyASN1;
		me->SetDialogResult(DR_OK);
	}
	else
	{
		me->ui->ShowMsgOK(CSTR("Error in generating certs"), CSTR("SSL Cert/Key"), me);
		return;
	}	
}

void __stdcall SSWR::AVIRead::AVIRSSLCertKeyForm::OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIRSSLCertKeyForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSSLCertKeyForm>();
	UOSInt i = files.GetCount();
	while (i-- > 0)
	{
		me->LoadFile(files[i]->ToCString());
	}
}

void SSWR::AVIRead::AVIRSSLCertKeyForm::LoadFile(Text::CStringNN fileName)
{
	NN<Net::ASN1Data> asn1;
	{
		IO::StmData::FileData fd(fileName, false);
		if (!Optional<Net::ASN1Data>::ConvertFrom(this->core->GetParserList()->ParseFileType(fd, IO::ParserType::ASN1Data)).SetTo(asn1))
		{
			this->ui->ShowMsgOK(CSTR("Error in parsing file"), CSTR("SSL Cert/Key"), this);
			return;
		}
	}
	if (asn1->GetASN1Type() != Net::ASN1Data::ASN1Type::X509)
	{
		asn1.Delete();
		this->ui->ShowMsgOK(CSTR("Error in parsing file"), CSTR("SSL Cert/Key"), this);
		return;
	}
	NN<Crypto::Cert::X509File> x509 = NN<Crypto::Cert::X509File>::ConvertFrom(asn1);
	if (x509->GetFileType() == Crypto::Cert::X509File::FileType::Cert)
	{
		this->cert.Delete();
		this->cert = NN<Crypto::Cert::X509Cert>::ConvertFrom(x509);

		Text::StringBuilderUTF8 sb;
		NN<Crypto::Cert::X509Cert>::ConvertFrom(x509)->ToShortString(sb);
		this->lblFileCert->SetText(sb.ToCString());
		this->tcMain->SetSelectedPage(this->tpFile);

		this->ClearCACerts();
		NN<Crypto::Cert::X509Cert> issuerCert;
		if (Crypto::Cert::CertUtil::FindIssuer(NN<Crypto::Cert::X509Cert>::ConvertFrom(x509)).SetTo(issuerCert))
		{
			this->caCerts.Add(issuerCert);
		}		
	}
	else if (x509->GetFileType() == Crypto::Cert::X509File::FileType::FileList)
	{
		Bool found = false;
		NN<Crypto::Cert::X509FileList> fileList = NN<Crypto::Cert::X509FileList>::ConvertFrom(x509);
		NN<Crypto::Cert::X509File> file;
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
					this->cert = NN<Crypto::Cert::X509Cert>::ConvertFrom(file->Clone());
					this->ClearCACerts();
				}
				else
				{
					this->caCerts.Add(NN<Crypto::Cert::X509Cert>::ConvertFrom(file->Clone()));
				}
			}
			i++;
		}
		fileList.Delete();
		NN<Crypto::Cert::X509Cert> nncert;
		if (found && this->cert.SetTo(nncert))
		{
			Text::StringBuilderUTF8 sb;
			nncert->ToShortString(sb);
			this->lblFileCert->SetText(sb.ToCString());
			this->tcMain->SetSelectedPage(this->tpFile);
		}
	}
	else if (x509->GetFileType() == Crypto::Cert::X509File::FileType::PrivateKey || x509->GetFileType() == Crypto::Cert::X509File::FileType::Key)
	{
		this->key.Delete();
		this->key = x509;

		Text::StringBuilderUTF8 sb;
		x509->ToShortString(sb);
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

SSWR::AVIRead::AVIRSSLCertKeyForm::AVIRSSLCertKeyForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, Optional<Net::SSLEngine> ssl, Optional<Crypto::Cert::X509Cert> cert, Optional<Crypto::Cert::X509File> key, NN<Data::ArrayListNN<Crypto::Cert::X509Cert>> caCerts) : UI::GUIForm(parent, 456, 200, ui)
{
	this->SetText(CSTR("SSL Cert/Key"));
	this->SetFont(nullptr, 8.25, false);
	this->SetNoResize(true);
	
	this->core = core;
	this->ssl = ssl;
	this->initCert = cert;
	this->initKey = key;
	Data::ArrayIterator<NN<Crypto::Cert::X509Cert>> it = caCerts->Iterator();
	while (it.HasNext())
	{
		this->caCerts.Add(NN<Crypto::Cert::X509Cert>::ConvertFrom(it.Next()->Clone()));
	}
	this->cert = nullptr;
	this->key = nullptr;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->HandleFormClosed(OnFormClosed, this);

	this->pnlCurr = ui->NewPanel(*this);
	this->pnlCurr->SetRect(0, 0, 100, 55, false);
	this->pnlCurr->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblCurrCert = ui->NewLabel(this->pnlCurr, CSTR("Curr Cert"));
	this->lblCurrCert->SetRect(4, 4, 100, 23, false);
	this->txtCurrCert = ui->NewTextBox(this->pnlCurr, CSTR(""));
	this->txtCurrCert->SetRect(104, 4, 200, 23, false);
	this->txtCurrCert->SetReadOnly(true);
	this->lblCurrKey = ui->NewLabel(this->pnlCurr, CSTR("Curr Key"));
	this->lblCurrKey->SetRect(4, 28, 100, 23, false);
	this->txtCurrKey = ui->NewTextBox(this->pnlCurr, CSTR(""));
	this->txtCurrKey->SetRect(104, 28, 200, 23, false);
	this->txtCurrKey->SetReadOnly(true);
	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpFile = this->tcMain->AddTabPage(CSTR("File"));
	this->btnFileCert = ui->NewButton(this->tpFile, CSTR("Cert"));
	this->btnFileCert->SetRect(4, 4, 75, 23, false);
	this->btnFileCert->HandleButtonClick(OnFileCertClicked, this);
	this->lblFileCert = ui->NewLabel(this->tpFile, CSTR(""));
	this->lblFileCert->SetRect(84, 4, 200, 23, false);
	this->btnFileKey = ui->NewButton(this->tpFile, CSTR("Key"));
	this->btnFileKey->SetRect(4, 28, 75, 23, false);
	this->btnFileKey->HandleButtonClick(OnFileKeyClicked, this);
	this->lblFileKey = ui->NewLabel(this->tpFile, CSTR(""));
	this->lblFileKey->SetRect(84, 28, 200, 23, false);
	this->btnFileConfirm = ui->NewButton(this->tpFile, CSTR("OK"));
	this->btnFileConfirm->SetRect(4, 52, 75, 23, false);
	this->btnFileConfirm->HandleButtonClick(OnFileConfirmClicked, this);

	this->tpGenerate = this->tcMain->AddTabPage(CSTR("Generate"));
	this->lblGenCountry = ui->NewLabel(this->tpGenerate, CSTR("Country"));
	this->lblGenCountry->SetRect(4, 4, 100, 23, false);
	this->txtGenCountry = ui->NewTextBox(this->tpGenerate, CSTR("HK"));
	this->txtGenCountry->SetRect(104, 4, 60, 23, false);
	this->lblGenCompany = ui->NewLabel(this->tpGenerate, CSTR("Company"));
	this->lblGenCompany->SetRect(4, 28, 100, 23, false);
	this->txtGenCompany = ui->NewTextBox(this->tpGenerate, CSTR("SSWR"));
	this->txtGenCompany->SetRect(104, 28, 200, 23, false);
	this->lblGenCommonName = ui->NewLabel(this->tpGenerate, CSTR("CommonName"));
	this->lblGenCommonName->SetRect(4, 52, 100, 23, false);
	this->txtGenCommonName = ui->NewTextBox(this->tpGenerate, CSTR("localhost"));
	this->txtGenCommonName->SetRect(104, 52, 200, 23, false);
	this->btnGenerate = ui->NewButton(this->tpGenerate, CSTR("Generate"));
	this->btnGenerate->SetRect(104, 76, 75, 23, false);
	this->btnGenerate->HandleButtonClick(OnGenerateClicked, this);

	Text::StringBuilderUTF8 sb;
	NN<Crypto::Cert::X509Cert> nnCert;
	if (this->initCert.SetTo(nnCert))
	{
		nnCert->ToShortName(sb);
		this->txtCurrCert->SetText(sb.ToCString());
	}
	NN<Crypto::Cert::X509File> nnKey;
	if (this->initKey.SetTo(nnKey))
	{
		sb.ClearStr();
		nnKey->ToShortName(sb);
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

Optional<Crypto::Cert::X509Cert> SSWR::AVIRead::AVIRSSLCertKeyForm::GetCert()
{
	return this->cert;
}

Optional<Crypto::Cert::X509File> SSWR::AVIRead::AVIRSSLCertKeyForm::GetKey()
{
	return this->key;
}

UOSInt SSWR::AVIRead::AVIRSSLCertKeyForm::GetCACerts(NN<Data::ArrayListNN<Crypto::Cert::X509Cert>> caCerts)
{
	Data::ArrayIterator<NN<Crypto::Cert::X509Cert>> it = this->caCerts.Iterator();
	while (it.HasNext())
	{
		caCerts->Add(NN<Crypto::Cert::X509Cert>::ConvertFrom(it.Next()->Clone()));
	}
	return this->caCerts.GetCount();
}
