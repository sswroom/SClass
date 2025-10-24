#include "Stdafx.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRGISServerForm.h"
#include "SSWR/AVIRead/AVIRSSLCertKeyForm.h"
#include "Sync/ThreadUtil.h"

void __stdcall SSWR::AVIRead::AVIRGISServerForm::OnSSLCertClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISServerForm>();
	SSWR::AVIRead::AVIRSSLCertKeyForm frm(0, me->ui, me->core, me->ssl, me->sslCert, me->sslKey, me->caCerts);
	if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
	{
		NN<Crypto::Cert::X509Cert> nnCert;
		NN<Crypto::Cert::X509File> nnKey;
		me->sslCert.Delete();
		me->sslKey.Delete();
		me->ClearCACerts();
		me->sslCert = frm.GetCert();
		me->sslKey = frm.GetKey();
		frm.GetCACerts(me->caCerts);
		Text::StringBuilderUTF8 sb;
		if (me->sslCert.SetTo(nnCert)) nnCert->ToShortString(sb);
		sb.AppendC(UTF8STRC(", "));
		if (me->sslKey.SetTo(nnKey)) nnKey->ToShortString(sb);
		me->lblSSLCert->SetText(sb.ToCString());
	}
}

void SSWR::AVIRead::AVIRGISServerForm::ClearCACerts()
{
	this->caCerts.DeleteAll();
}

SSWR::AVIRead::AVIRGISServerForm::AVIRGISServerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("GIS Server"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetTCPClientFactory(), true);
	this->sslCert = 0;
	this->sslKey = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;

	this->pnlConn = ui->NewPanel(*this);
	this->pnlConn->SetRect(0, 0, 100, 79, false);
	this->pnlConn->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblPort = ui->NewLabel(this->pnlConn, CSTR("Port"));
	this->lblPort->SetRect(4, 4, 100, 23, false);
	this->txtPort = ui->NewTextBox(this->pnlConn, CSTR("12345"));
	this->txtPort->SetRect(104, 4, 50, 23, false);
	this->lblSSL = ui->NewLabel(this->pnlConn, CSTR("SSL"));
	this->lblSSL->SetRect(4, 28, 100, 23, false);
	this->chkSSL = ui->NewCheckBox(this->pnlConn, CSTR("Enable"), false);
	this->chkSSL->SetRect(104, 28, 100, 23, false);
	this->btnSSLCert = ui->NewButton(this->pnlConn, CSTR("Cert/Key"));
	this->btnSSLCert->SetRect(204, 28, 75, 23, false);
	this->btnSSLCert->HandleButtonClick(OnSSLCertClicked, this);
	this->lblSSLCert = ui->NewLabel(this->pnlConn, CSTR(""));
	this->lblSSLCert->SetRect(284, 28, 200, 23, false);
	this->lblWorkerCnt = ui->NewLabel(this->pnlConn, CSTR("Worker Count"));
	this->lblWorkerCnt->SetRect(4, 52, 100, 23, false);
	sptr = Text::StrUOSInt(sbuff, Sync::ThreadUtil::GetThreadCnt());
	this->txtWorkerCnt = ui->NewTextBox(this->pnlConn, CSTRP(sbuff, sptr));
	this->txtWorkerCnt->SetRect(104, 52, 100, 23, false);
}

SSWR::AVIRead::AVIRGISServerForm::~AVIRGISServerForm()
{
	this->ssl.Delete();
	this->sslCert.Delete();
	this->sslKey.Delete();
	this->ClearCACerts();
}

void SSWR::AVIRead::AVIRGISServerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

