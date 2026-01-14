#include "Stdafx.h"
#include "Crypto/Cert/X509FileList.h"
#include "Net/SSLEngineFactory.h"
#include "Net/Email/GCISClient.h"
#include "SSWR/AVIRead/AVIRGCISClientForm.h"
#include "SSWR/AVIRead/AVIRSSLCertKeyForm.h"

void __stdcall SSWR::AVIRead::AVIRGCISClientForm::OnClientCertClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGCISClientForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGCISClientForm>();
	SSWR::AVIRead::AVIRSSLCertKeyForm frm(nullptr, me->ui, me->core, me->ssl, me->cliCert, me->cliKey, me->cliCACerts);
	if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
	{
		NN<Crypto::Cert::X509Cert> nnCert;
		NN<Crypto::Cert::X509File> nnKey;
		me->cliCert.Delete();
		me->cliKey.Delete();
		me->ClearCliCACerts();
		me->cliCert = frm.GetCert();
		me->cliKey = frm.GetKey();
		frm.GetCACerts(me->cliCACerts);
		Text::StringBuilderUTF8 sb;
		if (me->cliCert.SetTo(nnCert)) nnCert->ToShortString(sb);
		sb.AppendC(UTF8STRC(", "));
		if (me->cliKey.SetTo(nnKey)) nnKey->ToShortString(sb);
		me->lblClientCertDisp->SetText(sb.ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRGCISClientForm::OnSendClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGCISClientForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGCISClientForm>();
	NN<Crypto::Cert::X509Cert> cliCert;
	NN<Crypto::Cert::X509File> cliKey;
	if (!me->cliCert.SetTo(cliCert) || !me->cliKey.SetTo(cliKey))
	{
		me->ui->ShowMsgOK(CSTR("Please select client cert and key first"), CSTR("GCIS Client"), me);
		return;
	}
	Text::StringBuilderUTF8 sbURL;
	me->txtURL->GetText(sbURL);
	if (!sbURL.StartsWith(CSTR("https://")))
	{
		me->ui->ShowMsgOK(CSTR("URL must starts with https://"), CSTR("GCIS Client"), me);
		return;
	}
	Text::StringBuilderUTF8 sbTo;
	Text::StringBuilderUTF8 sbCC;
	Text::StringBuilderUTF8 sbBCC;
	me->txtTo->GetText(sbTo);
	me->txtCC->GetText(sbCC);
	me->txtBCC->GetText(sbBCC);
	if (sbTo.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("URL must starts with https://"), CSTR("GCIS Client"), me);
		return;
	}
	Text::StringBuilderUTF8 sbContType;
	me->txtContentType->GetText(sbContType);
	if (sbContType.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Content Type cannot be empty"), CSTR("GCIS Client"), me);
		return;
	}
	Text::StringBuilderUTF8 sbSubject;
	me->txtSubject->GetText(sbSubject);
	if (sbSubject.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Subject cannot be empty"), CSTR("GCIS Client"), me);
		return;
	}
	Text::StringBuilderUTF8 sbContent;
	me->txtContent->GetText(sbContent);
	if (sbContent.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Content cannot be empty"), CSTR("GCIS Client"), me);
		return;
	}
	Net::Email::GCISClient cli(me->core->GetTCPClientFactory(), me->ssl, sbURL.ToCString(), cliCert, cliKey);
	Text::StringBuilderUTF8 sbErr;
	cli.SendMessage(false, CSTR("UTF-8"), sbContType.ToCString(), sbSubject.ToCString(), sbContent.ToCString(), sbTo.ToCString(), sbCC.ToCString(), sbBCC.ToCString(), &sbErr);
	me->txtResponseStatus->SetText(sbErr.ToCString());
	me->svrCert.Delete();
	NN<Crypto::Cert::X509File> cert;
	if (cli.GetServerCertChain().SetTo(cert))
	{
		me->svrCert = cert = NN<Crypto::Cert::X509File>::ConvertFrom(cert->Clone());
		sbErr.ClearStr();
		if (cert->GetFileType() == Crypto::Cert::X509File::FileType::Cert && NN<Crypto::Cert::X509Cert>::ConvertFrom(cert)->GetSubjectCN(sbErr))
		{
			me->txtServerCert->SetText(sbErr.ToCString());
		}
		else if (cert->GetFileType() == Crypto::Cert::X509File::FileType::FileList && NN<Crypto::Cert::X509FileList>::ConvertFrom(cert)->GetCertName(0, sbErr))
		{
			me->txtServerCert->SetText(sbErr.ToCString());
		}
		else
		{
			me->txtServerCert->SetText(CSTR("-"));
		}
	}
	else
	{
		me->txtServerCert->SetText(CSTR("-"));
	}
}

void __stdcall SSWR::AVIRead::AVIRGCISClientForm::OnServerCertClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGCISClientForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGCISClientForm>();
	NN<Crypto::Cert::X509File> svrCert;
	if (me->svrCert.SetTo(svrCert))
	{
		me->core->OpenObject(svrCert->Clone());
	}
}

void SSWR::AVIRead::AVIRGCISClientForm::ClearCliCACerts()
{
	UOSInt i = this->cliCACerts.GetCount();
	while (i-- > 0)
	{
		this->cliCACerts.GetItem(i).Delete();
	}
	this->cliCACerts.Clear();
}

SSWR::AVIRead::AVIRGCISClientForm::AVIRGCISClientForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 640, 640, ui)
{
	this->SetText(CSTR("GCIS Client"));
	this->SetFont(nullptr, 8.25, false);
	this->SetNoResize(true);
	this->core = core;
	this->ssl = Net::SSLEngineFactory::Create(core->GetTCPClientFactory(), true);
	this->cliCert = nullptr;
	this->cliKey = nullptr;
	this->svrCert = nullptr;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));


	this->lblURL = ui->NewLabel(*this, CSTR("URL"));
	this->lblURL->SetRect(4, 4, 100, 23, false);
	this->txtURL = ui->NewTextBox(*this, CSTR("https://localhost:9444/messaging/ssl/RESTful/NotiSenderRest"));
	this->txtURL->SetRect(104, 4, 400, 23, false);
	this->lblClientCert = ui->NewLabel(*this, CSTR("Client Cert"));
	this->lblClientCert->SetRect(4, 28, 100, 23, false);
	this->btnClientCert = ui->NewButton(*this, CSTR("Select"));
	this->btnClientCert->SetRect(104, 28, 75, 23, false);
	this->btnClientCert->HandleButtonClick(OnClientCertClicked, this);
	this->lblClientCertDisp = ui->NewLabel(*this, CSTR("-"));
	this->lblClientCertDisp->SetRect(184, 28, 200, 23, false);
	this->lblTo = ui->NewLabel(*this, CSTR("To"));
	this->lblTo->SetRect(4, 52, 100, 23, false);
	this->txtTo = ui->NewTextBox(*this, CSTR(""));
	this->txtTo->SetRect(104, 52, 400, 23, false);
	this->lblCC = ui->NewLabel(*this, CSTR("CC"));
	this->lblCC->SetRect(4, 76, 100, 23, false);
	this->txtCC = ui->NewTextBox(*this, CSTR(""));
	this->txtCC->SetRect(104, 76, 400, 23, false);
	this->lblBCC = ui->NewLabel(*this, CSTR("BCC"));
	this->lblBCC->SetRect(4, 100, 100, 23, false);
	this->txtBCC = ui->NewTextBox(*this, CSTR(""));
	this->txtBCC->SetRect(104, 100, 400, 23, false);
	this->lblSubject = ui->NewLabel(*this, CSTR("Subject"));
	this->lblSubject->SetRect(4, 124, 100, 23, false);
	this->txtSubject = ui->NewTextBox(*this, CSTR(""));
	this->txtSubject->SetRect(104, 124, 400, 23, false);
	this->lblContent = ui->NewLabel(*this, CSTR("Content"));
	this->lblContent->SetRect(4, 148, 100, 23, false);
	this->txtContent = ui->NewTextBox(*this, CSTR(""), true);
	this->txtContent->SetRect(104, 148, 400, 119, false);
	this->lblContentType = ui->NewLabel(*this, CSTR("Content Type"));
	this->lblContentType->SetRect(4, 268, 100, 23, false);
	this->txtContentType = ui->NewTextBox(*this, CSTR("text/html"));
	this->txtContentType->SetRect(104, 268, 400, 23, false);
	this->btnSend = ui->NewButton(*this, CSTR("Send"));
	this->btnSend->SetRect(104, 292, 75, 23, false);
	this->btnSend->HandleButtonClick(OnSendClicked, this);
	this->lblResponseStatus = ui->NewLabel(*this, CSTR("Response"));
	this->lblResponseStatus->SetRect(4, 316, 100, 23, false);
	this->txtResponseStatus = ui->NewTextBox(*this, CSTR(""), true);
	this->txtResponseStatus->SetRect(104, 316, 400, 119, false);
	this->txtResponseStatus->SetReadOnly(true);
	this->lblServerCert = ui->NewLabel(*this, CSTR("Server Cert"));
	this->lblServerCert->SetRect(4, 436, 100, 23, false);
	this->txtServerCert = ui->NewTextBox(*this, CSTR("-"));
	this->txtServerCert->SetRect(104, 436, 200, 23, false);
	this->txtServerCert->SetReadOnly(true);
	this->btnServerCert = ui->NewButton(*this, CSTR("View"));
	this->btnServerCert->SetRect(304, 436, 75, 23, false);
	this->btnServerCert->HandleButtonClick(OnServerCertClicked, this);
}

SSWR::AVIRead::AVIRGCISClientForm::~AVIRGCISClientForm()
{
	this->ssl.Delete();
	this->ClearCliCACerts();
	this->cliCert.Delete();
	this->cliKey.Delete();
	this->svrCert.Delete();
}

void SSWR::AVIRead::AVIRGCISClientForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
