#include "Stdafx.h"
#include "Crypto/Cert/X509FileList.h"
#include "Net/SSLEngineFactory.h"
#include "Net/Email/GCISClient.h"
#include "SSWR/AVIRead/AVIRGCISClientForm.h"
#include "SSWR/AVIRead/AVIRSSLCertKeyForm.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRGCISClientForm::OnClientCertClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGCISClientForm *me = (SSWR::AVIRead::AVIRGCISClientForm*)userObj;
	SSWR::AVIRead::AVIRSSLCertKeyForm frm(0, me->ui, me->core, me->ssl, me->cliCert, me->cliKey, me->cliCACerts);
	if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
	{
		SDEL_CLASS(me->cliCert);
		SDEL_CLASS(me->cliKey);
		me->ClearCliCACerts();
		me->cliCert = frm.GetCert();
		me->cliKey = frm.GetKey();
		frm.GetCACerts(me->cliCACerts);
		Text::StringBuilderUTF8 sb;
		me->cliCert->ToShortString(sb);
		sb.AppendC(UTF8STRC(", "));
		me->cliKey->ToShortString(sb);
		me->lblClientCertDisp->SetText(sb.ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRGCISClientForm::OnSendClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGCISClientForm *me = (SSWR::AVIRead::AVIRGCISClientForm*)userObj;
	NotNullPtr<Crypto::Cert::X509Cert> cliCert;
	NotNullPtr<Crypto::Cert::X509File> cliKey;
	if (!cliCert.Set(me->cliCert) || !cliKey.Set(me->cliKey))
	{
		UI::MessageDialog::ShowDialog(CSTR("Please select client cert and key first"), CSTR("GCIS Client"), me);
		return;
	}
	Text::StringBuilderUTF8 sbURL;
	me->txtURL->GetText(sbURL);
	if (!sbURL.StartsWith(CSTR("https://")))
	{
		UI::MessageDialog::ShowDialog(CSTR("URL must starts with https://"), CSTR("GCIS Client"), me);
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
		UI::MessageDialog::ShowDialog(CSTR("URL must starts with https://"), CSTR("GCIS Client"), me);
		return;
	}
	Text::StringBuilderUTF8 sbContType;
	me->txtContentType->GetText(sbContType);
	if (sbContType.GetLength() == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Content Type cannot be empty"), CSTR("GCIS Client"), me);
		return;
	}
	Text::StringBuilderUTF8 sbSubject;
	me->txtSubject->GetText(sbSubject);
	if (sbSubject.GetLength() == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Subject cannot be empty"), CSTR("GCIS Client"), me);
		return;
	}
	Text::StringBuilderUTF8 sbContent;
	me->txtContent->GetText(sbContent);
	if (sbContent.GetLength() == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Content cannot be empty"), CSTR("GCIS Client"), me);
		return;
	}
	Net::Email::GCISClient cli(me->core->GetSocketFactory(), me->ssl, sbURL.ToCString(), cliCert, cliKey);
	Text::StringBuilderUTF8 sbErr;
	cli.SendMessage(false, CSTR("UTF-8"), sbContType.ToCString(), sbSubject.ToCString(), sbContent.ToCString(), sbTo.ToCString(), sbCC.ToCString(), sbBCC.ToCString(), &sbErr);
	me->txtResponseStatus->SetText(sbErr.ToCString());
	Crypto::Cert::X509File *cert = cli.GetServerCertChain();
	SDEL_CLASS(me->svrCert);
	if (cert)
	{
		me->svrCert = (Crypto::Cert::X509File*)cert->Clone().Ptr();
		sbErr.ClearStr();
		if (me->svrCert->GetFileType() == Crypto::Cert::X509File::FileType::Cert && ((Crypto::Cert::X509Cert*)me->svrCert)->GetSubjectCN(sbErr))
		{
			me->txtServerCert->SetText(sbErr.ToCString());
		}
		else if (me->svrCert->GetFileType() == Crypto::Cert::X509File::FileType::FileList && ((Crypto::Cert::X509FileList*)me->svrCert)->GetCertName(0, sbErr))
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

void __stdcall SSWR::AVIRead::AVIRGCISClientForm::OnServerCertClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGCISClientForm *me = (SSWR::AVIRead::AVIRGCISClientForm*)userObj;
	if (me->svrCert)
	{
		me->core->OpenObject(me->svrCert->Clone());
	}
}

void SSWR::AVIRead::AVIRGCISClientForm::ClearCliCACerts()
{
	UOSInt i = this->cliCACerts.GetCount();
	while (i-- > 0)
	{
		Crypto::Cert::X509Cert *cert = this->cliCACerts.GetItem(i);
		DEL_CLASS(cert);
	}
	this->cliCACerts.Clear();
}

SSWR::AVIRead::AVIRGCISClientForm::AVIRGCISClientForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 640, 640, ui)
{
	this->SetText(CSTR("GCIS Client"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	this->core = core;
	this->ssl = Net::SSLEngineFactory::Create(core->GetSocketFactory(), true);
	this->cliCert = 0;
	this->cliKey = 0;
	this->svrCert = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));


	NEW_CLASSNN(this->lblURL, UI::GUILabel(ui, *this, CSTR("URL")));
	this->lblURL->SetRect(4, 4, 100, 23, false);
	NEW_CLASSNN(this->txtURL, UI::GUITextBox(ui, *this, CSTR("https://localhost:9444/messaging/ssl/RESTful/NotiSenderRest")));
	this->txtURL->SetRect(104, 4, 400, 23, false);
	NEW_CLASSNN(this->lblClientCert, UI::GUILabel(ui, *this, CSTR("Client Cert")));
	this->lblClientCert->SetRect(4, 28, 100, 23, false);
	NEW_CLASSNN(this->btnClientCert, UI::GUIButton(ui, *this, CSTR("Select")));
	this->btnClientCert->SetRect(104, 28, 75, 23, false);
	this->btnClientCert->HandleButtonClick(OnClientCertClicked, this);
	NEW_CLASSNN(this->lblClientCertDisp, UI::GUILabel(ui, *this, CSTR("-")));
	this->lblClientCertDisp->SetRect(184, 28, 200, 23, false);
	NEW_CLASSNN(this->lblTo, UI::GUILabel(ui, *this, CSTR("To")));
	this->lblTo->SetRect(4, 52, 100, 23, false);
	NEW_CLASSNN(this->txtTo, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtTo->SetRect(104, 52, 400, 23, false);
	NEW_CLASSNN(this->lblCC, UI::GUILabel(ui, *this, CSTR("CC")));
	this->lblCC->SetRect(4, 76, 100, 23, false);
	NEW_CLASSNN(this->txtCC, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtCC->SetRect(104, 76, 400, 23, false);
	NEW_CLASSNN(this->lblBCC, UI::GUILabel(ui, *this, CSTR("BCC")));
	this->lblBCC->SetRect(4, 100, 100, 23, false);
	NEW_CLASSNN(this->txtBCC, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtBCC->SetRect(104, 100, 400, 23, false);
	NEW_CLASSNN(this->lblSubject, UI::GUILabel(ui, *this, CSTR("Subject")));
	this->lblSubject->SetRect(4, 124, 100, 23, false);
	NEW_CLASSNN(this->txtSubject, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtSubject->SetRect(104, 124, 400, 23, false);
	NEW_CLASSNN(this->lblContent, UI::GUILabel(ui, *this, CSTR("Content")));
	this->lblContent->SetRect(4, 148, 100, 23, false);
	NEW_CLASSNN(this->txtContent, UI::GUITextBox(ui, *this, CSTR(""), true));
	this->txtContent->SetRect(104, 148, 400, 119, false);
	NEW_CLASSNN(this->lblContentType, UI::GUILabel(ui, *this, CSTR("Content Type")));
	this->lblContentType->SetRect(4, 268, 100, 23, false);
	NEW_CLASSNN(this->txtContentType, UI::GUITextBox(ui, *this, CSTR("text/html")));
	this->txtContentType->SetRect(104, 268, 400, 23, false);
	NEW_CLASSNN(this->btnSend, UI::GUIButton(ui, *this, CSTR("Send")));
	this->btnSend->SetRect(104, 292, 75, 23, false);
	this->btnSend->HandleButtonClick(OnSendClicked, this);
	NEW_CLASSNN(this->lblResponseStatus, UI::GUILabel(ui, *this, CSTR("Response")));
	this->lblResponseStatus->SetRect(4, 316, 100, 23, false);
	NEW_CLASSNN(this->txtResponseStatus, UI::GUITextBox(ui, *this, CSTR(""), true));
	this->txtResponseStatus->SetRect(104, 316, 400, 119, false);
	this->txtResponseStatus->SetReadOnly(true);
	NEW_CLASSNN(this->lblServerCert, UI::GUILabel(ui, *this, CSTR("Server Cert")));
	this->lblServerCert->SetRect(4, 436, 100, 23, false);
	NEW_CLASSNN(this->txtServerCert, UI::GUITextBox(ui, *this, CSTR("-")));
	this->txtServerCert->SetRect(104, 436, 200, 23, false);
	this->txtServerCert->SetReadOnly(true);
	NEW_CLASSNN(this->btnServerCert, UI::GUIButton(ui, *this, CSTR("View")));
	this->btnServerCert->SetRect(304, 436, 75, 23, false);
	this->btnServerCert->HandleButtonClick(OnServerCertClicked, this);
}

SSWR::AVIRead::AVIRGCISClientForm::~AVIRGCISClientForm()
{
	SDEL_CLASS(this->ssl);
	this->ClearCliCACerts();
	SDEL_CLASS(this->cliCert);
	SDEL_CLASS(this->cliKey);
	SDEL_CLASS(this->svrCert);
}

void SSWR::AVIRead::AVIRGCISClientForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
