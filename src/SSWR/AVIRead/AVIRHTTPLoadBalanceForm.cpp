#include "Stdafx.h"
#include "Crypto/Cert/CertUtil.h"
#include "IO/Path.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRHTTPSvrForm.h"
#include "SSWR/AVIRead/AVIRHTTPLoadBalanceForm.h"
#include "SSWR/AVIRead/AVIRSSLCertKeyForm.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRHTTPLoadBalanceForm::OnStartClick(void *userObj)
{
	SSWR::AVIRead::AVIRHTTPLoadBalanceForm *me = (SSWR::AVIRead::AVIRHTTPLoadBalanceForm*)userObj;
	if (me->svr)
	{
		return;
	}
	UInt16 port = 0;
	Bool valid = true;
	Text::StringBuilderUTF8 *sb;
	NEW_CLASS(sb, Text::StringBuilderUTF8());
	me->txtPort->GetText(sb);
	Text::StrToUInt16S(sb->ToString(), &port, 0);
	sb->ClearStr();
	me->txtFwdURL->GetText(sb);
	Net::SSLEngine *ssl = 0;
	if (!sb->StartsWith(UTF8STRC("http://")) && !sb->StartsWith(UTF8STRC("https://")))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Invalid Forward URL", (const UTF8Char*)"HTTP Load Balance", me);
		DEL_CLASS(sb);
		return;
	}

	if (me->chkSSL->IsChecked())
	{
		if (me->sslCert == 0 || me->sslKey == 0)
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Please select SSL Cert/Key First", (const UTF8Char*)"HTTP Load Balance", me);
			DEL_CLASS(sb);
			return;
		}
		ssl = me->ssl;
		Crypto::Cert::X509Cert *issuerCert = Crypto::Cert::CertUtil::FindIssuer(me->sslCert);
		ssl->SetServerCertsASN1(me->sslCert, me->sslKey, issuerCert);
		SDEL_CLASS(issuerCert);
	}
	if (port > 0 && port < 65535)
	{
		NEW_CLASS(me->fwdHdlr, Net::WebServer::HTTPForwardHandler(me->core->GetSocketFactory(), me->ssl, sb->ToString(), (Net::WebServer::HTTPForwardHandler::ForwardType)(OSInt)me->cboFwdType->GetSelectedItem()));
		NEW_CLASS(me->svr, Net::WebServer::WebListener(me->core->GetSocketFactory(), ssl, me->fwdHdlr, port, 120, Sync::Thread::GetThreadCnt(), (const UTF8Char*)"sswr", me->chkAllowProxy->IsChecked(), me->chkAllowKA->IsChecked()));
		if (me->svr->IsError())
		{
			valid = false;
			SDEL_CLASS(me->svr);
		}
		else
		{
			sb->ClearStr();
			me->txtLogDir->GetText(sb);
			if (sb->GetEndPtr()[-1] != IO::Path::PATH_SEPERATOR)
			{
				sb->AppendChar(IO::Path::PATH_SEPERATOR, 1);
			}
			sb->AppendC(UTF8STRC("Acccess"));

			if (!me->chkSkipLog->IsChecked())
			{
				NEW_CLASS(me->log, IO::LogTool());
				me->log->AddFileLog(sb->ToString(), IO::ILogHandler::LOG_TYPE_PER_DAY, IO::ILogHandler::LOG_GROUP_TYPE_PER_MONTH, IO::ILogHandler::LOG_LEVEL_RAW, "yyyy-MM-dd HH:mm:ss.fff", false);
				me->svr->SetAccessLog(me->log, IO::ILogHandler::LOG_LEVEL_RAW);
				me->svr->SetRequestLog(me->reqLog);
				NEW_CLASS(me->logger, UI::ListBoxLogger(me, me->lbLog, 500, true));
				me->log->AddLogHandler(me->logger, IO::ILogHandler::LOG_LEVEL_RAW);
			}
		}
	}

	if (valid)
	{
		me->txtPort->SetReadOnly(true);
		me->txtFwdURL->SetReadOnly(true);
		me->txtLogDir->SetReadOnly(true);
		me->chkAllowProxy->SetEnabled(false);
		me->chkSkipLog->SetEnabled(false);
		me->chkAllowKA->SetEnabled(false);
		me->chkSSL->SetEnabled(false);
		me->btnSSLCert->SetEnabled(false);
	}
	else
	{
		SDEL_CLASS(me->svr);
		if (me->fwdHdlr)
		{
			me->fwdHdlr->Release();
			me->fwdHdlr = 0;
		}
		SDEL_CLASS(me->log);
		SDEL_CLASS(me->logger);
	}
	DEL_CLASS(sb);
}

void __stdcall SSWR::AVIRead::AVIRHTTPLoadBalanceForm::OnStopClick(void *userObj)
{
	SSWR::AVIRead::AVIRHTTPLoadBalanceForm *me = (SSWR::AVIRead::AVIRHTTPLoadBalanceForm*)userObj;
	if (me->svr == 0)
	{
		return;
	}
	SDEL_CLASS(me->svr);
	if (me->fwdHdlr)
	{
		me->fwdHdlr->Release();
		me->fwdHdlr = 0;
	}
	SDEL_CLASS(me->log);
	SDEL_CLASS(me->logger);
	me->txtPort->SetReadOnly(false);
	me->txtFwdURL->SetReadOnly(false);
	me->txtLogDir->SetReadOnly(false);
	me->chkAllowProxy->SetEnabled(true);
	me->chkSkipLog->SetEnabled(true);
	me->chkAllowKA->SetEnabled(true);
	me->chkSSL->SetEnabled(true);
	me->btnSSLCert->SetEnabled(true);
}

void __stdcall SSWR::AVIRead::AVIRHTTPLoadBalanceForm::OnLogSel(void *userObj)
{
	SSWR::AVIRead::AVIRHTTPLoadBalanceForm *me = (SSWR::AVIRead::AVIRHTTPLoadBalanceForm*)userObj;
	Text::String *s = me->lbLog->GetSelectedItemTextNew();
	me->txtLog->SetText(s->v);
	s->Release();
}

void __stdcall SSWR::AVIRead::AVIRHTTPLoadBalanceForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRHTTPLoadBalanceForm *me = (SSWR::AVIRead::AVIRHTTPLoadBalanceForm*)userObj;
	UOSInt i;
	UOSInt j;
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	if (me->svr)
	{
		Net::WebServer::WebListener::SERVER_STATUS status;
		me->svr->GetStatus(&status);

		if (me->lastStatus.currConn != status.currConn)
		{
			me->lastStatus.currConn = status.currConn;
			Text::StrUInt32(sbuff, status.currConn);
			me->txtConnCurr->SetText(sbuff);
		}
		if (me->lastStatus.connCnt != status.connCnt)
		{
			me->lastStatus.connCnt = status.connCnt;
			Text::StrUInt32(sbuff, status.connCnt);
			me->txtConnTotal->SetText(sbuff);
		}
		Text::StrUInt64(sbuff, status.totalRead - me->lastStatus.totalRead);
		me->txtDataRateR->SetText(sbuff);
		Text::StrUInt64(sbuff, status.totalWrite - me->lastStatus.totalWrite);
		me->txtDataRateW->SetText(sbuff);
		if (me->lastStatus.totalRead != status.totalRead)
		{
			me->lastStatus.totalRead = status.totalRead;
			Text::StrUInt64(sbuff, status.totalRead);
			me->txtDataTotalR->SetText(sbuff);
		}
		if (me->lastStatus.totalWrite != status.totalWrite)
		{
			me->lastStatus.totalWrite = status.totalWrite;
			Text::StrUInt64(sbuff, status.totalWrite);
			me->txtDataTotalW->SetText(sbuff);
		}
		Text::StrUInt32(sbuff, status.reqCnt - me->lastStatus.reqCnt);
		me->txtReqRate->SetText(sbuff);
		if (me->lastStatus.reqCnt != status.reqCnt)
		{
			me->lastStatus.reqCnt = status.reqCnt;
			Text::StrUInt32(sbuff, status.reqCnt);
			me->txtReqTotal->SetText(sbuff);
		}
	}

	i = me->reqLog->GetNextIndex();
	if (i != me->lastAccessIndex)
	{
		Data::ArrayList<SSWR::AVIRead::AVIRHTTPLog::LogEntry *> logs;
		Data::ArrayList<UOSInt> logIndex;
		SSWR::AVIRead::AVIRHTTPLog::LogEntry *log;
		Text::StringBuilderUTF8 sb;
		Data::DateTime dt;
		Sync::MutexUsage mutUsage;

		me->lastAccessIndex = i;
		me->reqLog->Use(&mutUsage);
		me->reqLog->GetEntries(&logs, &logIndex);
		me->lbAccess->ClearItems();
		me->txtAccess->SetText((const UTF8Char*)"");
		i = 0;
		j = logs.GetCount();
		while (i < j)
		{
			log = logs.GetItem(i);
			sb.ClearStr();
			dt.SetTicks(log->reqTime);
			dt.ToLocalTime();
			sb.AppendDate(&dt);
			sb.AppendC(UTF8STRC(" "));
			sptr = Net::SocketUtil::GetAddrName(sbuff, &log->cliAddr, log->cliPort);
			sb.AppendP(sbuff, sptr);

			me->lbAccess->AddItem(sb.ToCString(), (void*)(OSInt)logIndex.GetItem(i));
			i++;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRHTTPLoadBalanceForm::OnAccessSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRHTTPLoadBalanceForm *me = (SSWR::AVIRead::AVIRHTTPLoadBalanceForm*)userObj;
	Text::StringBuilderUTF8 sb;
	Sync::MutexUsage mutUsage;
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	me->reqLog->Use(&mutUsage);
	UOSInt i = (UOSInt)me->lbAccess->GetSelectedItem();
	UOSInt j;
	SSWR::AVIRead::AVIRHTTPLog::LogEntry *log;
	log = me->reqLog->GetEntry(i);
	Data::DateTime dt;
	dt.SetTicks(log->reqTime);
	dt.ToLocalTime();
	sb.AppendDate(&dt);
	sb.AppendC(UTF8STRC(" "));
	sptr = Net::SocketUtil::GetAddrName(sbuff, &log->cliAddr, log->cliPort);
	sb.AppendP(sbuff, sptr);
	sb.AppendC(UTF8STRC("\r\n"));
	sb.Append(log->reqURI);
	sb.AppendC(UTF8STRC("\r\n\r\nHeaders:"));
	i = 0;
	j = log->headerName->GetCount();
	while (i < j)
	{
		sb.AppendC(UTF8STRC("\r\n"));
		sb.Append(log->headerName->GetItem(i));
		sb.AppendC(UTF8STRC("\t"));
		sb.AppendSlow(log->headerVal->GetItem(i));
		i++;
	}
	me->txtAccess->SetText(sb.ToString());
}

void __stdcall SSWR::AVIRead::AVIRHTTPLoadBalanceForm::OnSSLCertClicked(void *userObj)
{
	SSWR::AVIRead::AVIRHTTPLoadBalanceForm *me = (SSWR::AVIRead::AVIRHTTPLoadBalanceForm*)userObj;
	SSWR::AVIRead::AVIRSSLCertKeyForm *frm;
	NEW_CLASS(frm, SSWR::AVIRead::AVIRSSLCertKeyForm(0, me->ui, me->core, me->ssl, me->sslCert, me->sslKey));
	if (frm->ShowDialog(me) == UI::GUIForm::DR_OK)
	{
		SDEL_CLASS(me->sslCert);
		SDEL_CLASS(me->sslKey);
		me->sslCert = frm->GetCert();
		me->sslKey = frm->GetKey();
		Text::StringBuilderUTF8 sb;
		me->sslCert->ToShortString(&sb);
		sb.AppendC(UTF8STRC(", "));
		me->sslKey->ToShortString(&sb);
		me->lblSSLCert->SetText(sb.ToString());
	}
	DEL_CLASS(frm);
}

SSWR::AVIRead::AVIRHTTPLoadBalanceForm::AVIRHTTPLoadBalanceForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UOSInt i;
	this->core = core;
	this->SetText((const UTF8Char*)"HTTP Load Balance");
	this->SetFont(0, 0, 8.25, false);
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetSocketFactory(), true);
	this->sslCert = 0;
	this->sslKey = 0;
	this->svr = 0;
	this->log = 0;
	this->fwdHdlr = 0;
	this->logger = 0;
	this->lastAccessIndex = 0;
	this->lastStatus.connCnt = 0;
	this->lastStatus.currConn = 0;
	this->lastStatus.reqCnt = 0;
	this->lastStatus.totalRead = 0;
	this->lastStatus.totalWrite = 0;
	NEW_CLASS(this->reqLog, SSWR::AVIRead::AVIRHTTPLog(100));
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpControl = this->tcMain->AddTabPage((const UTF8Char*)"Control");
	this->tpStatus = this->tcMain->AddTabPage((const UTF8Char*)"Status");
	this->tpAccess = this->tcMain->AddTabPage((const UTF8Char*)"Access");
	this->tpLog = this->tcMain->AddTabPage((const UTF8Char*)"Log");

	NEW_CLASS(this->grpParam, UI::GUIGroupBox(ui, this->tpControl, (const UTF8Char*)"Parameters"));
	this->grpParam->SetRect(0, 0, 620, 256, false);
	this->grpParam->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblPort, UI::GUILabel(ui, this->grpParam, (const UTF8Char*)"Port"));
	this->lblPort->SetRect(8, 8, 100, 23, false);
	NEW_CLASS(this->txtPort, UI::GUITextBox(ui, this->grpParam, (const UTF8Char*)"12345"));
	this->txtPort->SetRect(108, 8, 50, 23, false);
	NEW_CLASS(this->lblLogDir, UI::GUILabel(ui, this->grpParam, (const UTF8Char*)"Log Path"));
	this->lblLogDir->SetRect(8, 32, 100, 23, false);
	sptr = IO::Path::GetProcessFileName(sbuff);
	i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
	Text::StrConcatC(&sbuff[i + 1], UTF8STRC("log"));
	NEW_CLASS(this->txtLogDir, UI::GUITextBox(ui, this->grpParam, sbuff));
	this->txtLogDir->SetRect(108, 32, 500, 23, false);
	NEW_CLASS(this->lblSSL, UI::GUILabel(ui, this->grpParam, (const UTF8Char*)"SSL"));
	this->lblSSL->SetRect(8, 56, 100, 23, false);
	NEW_CLASS(this->chkSSL, UI::GUICheckBox(ui, this->grpParam, (const UTF8Char*)"Enable", false));
	this->chkSSL->SetRect(108, 56, 100, 23, false);
	NEW_CLASS(this->btnSSLCert, UI::GUIButton(ui, this->grpParam, (const UTF8Char*)"Cert/Key"));
	this->btnSSLCert->SetRect(208, 56, 75,23, false);
	this->btnSSLCert->HandleButtonClick(OnSSLCertClicked, this);
	NEW_CLASS(this->lblSSLCert, UI::GUILabel(ui, this->grpParam, (const UTF8Char*)""));
	this->lblSSLCert->SetRect(288, 56, 200, 23, false);
	NEW_CLASS(this->lblAllowProxy, UI::GUILabel(ui, this->grpParam, (const UTF8Char*)"Proxy Connection"));
	this->lblAllowProxy->SetRect(8, 80, 100, 23, false);
	NEW_CLASS(this->chkAllowProxy, UI::GUICheckBox(ui, this->grpParam, (const UTF8Char*)"Allow", false));
	this->chkAllowProxy->SetRect(108, 80, 100, 23, false);
	NEW_CLASS(this->lblSkipLog, UI::GUILabel(ui, this->grpParam, (const UTF8Char*)"Skip Logging"));
	this->lblSkipLog->SetRect(8, 104, 100, 23, false);
	NEW_CLASS(this->chkSkipLog, UI::GUICheckBox(ui, this->grpParam, (const UTF8Char*)"Enable", true));
	this->chkSkipLog->SetRect(108, 104, 100, 23, false);
	NEW_CLASS(this->lblAllowKA, UI::GUILabel(ui, this->grpParam, (const UTF8Char*)"Allow KA"));
	this->lblAllowKA->SetRect(8, 128, 100, 23, false);
	NEW_CLASS(this->chkAllowKA, UI::GUICheckBox(ui, this->grpParam, (const UTF8Char*)"Enable", false));
	this->chkAllowKA->SetRect(108, 128, 100, 23, false);
	NEW_CLASS(this->lblFwdURL, UI::GUILabel(ui, this->grpParam, (const UTF8Char*)"Forward URL"));
	this->lblFwdURL->SetRect(8, 152, 100, 23, false);
	NEW_CLASS(this->txtFwdURL, UI::GUITextBox(ui, this->grpParam, (const UTF8Char*)"http://sswroom.no-ip.org:5080/"));
	this->txtFwdURL->SetRect(108, 152, 500, 23, false);
	NEW_CLASS(this->lblFwdType, UI::GUILabel(ui, this->grpParam, (const UTF8Char*)"Forward Type"));
	this->lblFwdType->SetRect(8, 176, 100, 23, false);
	NEW_CLASS(this->cboFwdType, UI::GUIComboBox(ui, this->grpParam,false));
	this->cboFwdType->SetRect(108, 176, 100, 23, false);
	this->cboFwdType->AddItem(CSTR("Normal"), (void*)(OSInt)Net::WebServer::HTTPForwardHandler::ForwardType::Normal);
	this->cboFwdType->AddItem(CSTR("Transparent"), (void*)(OSInt)Net::WebServer::HTTPForwardHandler::ForwardType::Transparent);
	this->cboFwdType->SetSelectedIndex(0);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this->tpControl, (const UTF8Char*)"Start"));
	this->btnStart->SetRect(200, 300, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClick, this);
	NEW_CLASS(this->btnStop, UI::GUIButton(ui, this->tpControl, (const UTF8Char*)"Stop"));
	this->btnStop->SetRect(300, 300, 75, 23, false);
	this->btnStop->HandleButtonClick(OnStopClick, this);

	NEW_CLASS(this->lblConnCurr, UI::GUILabel(ui, this->tpStatus, (const UTF8Char*)"Conn Curr"));
	this->lblConnCurr->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtConnCurr, UI::GUITextBox(ui, this->tpStatus, (const UTF8Char*)"0"));
	this->txtConnCurr->SetRect(104, 4, 100, 23, false);
	this->txtConnCurr->SetReadOnly(true);
	NEW_CLASS(this->lblConnTotal, UI::GUILabel(ui, this->tpStatus, (const UTF8Char*)"Conn Total"));
	this->lblConnTotal->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtConnTotal, UI::GUITextBox(ui, this->tpStatus, (const UTF8Char*)"0"));
	this->txtConnTotal->SetRect(104, 28, 100, 23, false);
	this->txtConnTotal->SetReadOnly(true);
	NEW_CLASS(this->lblDataRateR, UI::GUILabel(ui, this->tpStatus, (const UTF8Char*)"Data Rate(R)"));
	this->lblDataRateR->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtDataRateR, UI::GUITextBox(ui, this->tpStatus, (const UTF8Char*)"0"));
	this->txtDataRateR->SetRect(104, 52, 100, 23, false);
	this->txtDataRateR->SetReadOnly(true);
	NEW_CLASS(this->lblDataRateW, UI::GUILabel(ui, this->tpStatus, (const UTF8Char*)"Data Rate(W)"));
	this->lblDataRateW->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtDataRateW, UI::GUITextBox(ui, this->tpStatus, (const UTF8Char*)"0"));
	this->txtDataRateW->SetRect(104, 76, 100, 23, false);
	this->txtDataRateW->SetReadOnly(true);
	NEW_CLASS(this->lblDataTotalR, UI::GUILabel(ui, this->tpStatus, (const UTF8Char*)"Total Data(R)"));
	this->lblDataTotalR->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtDataTotalR, UI::GUITextBox(ui, this->tpStatus, (const UTF8Char*)"0"));
	this->txtDataTotalR->SetRect(104, 100, 100, 23, false);
	this->txtDataTotalR->SetReadOnly(true);
	NEW_CLASS(this->lblDataTotalW, UI::GUILabel(ui, this->tpStatus, (const UTF8Char*)"Total Data(W)"));
	this->lblDataTotalW->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->txtDataTotalW, UI::GUITextBox(ui, this->tpStatus, (const UTF8Char*)"0"));
	this->txtDataTotalW->SetRect(104, 124, 100, 23, false);
	this->txtDataTotalW->SetReadOnly(true);
	NEW_CLASS(this->lblReqRate, UI::GUILabel(ui, this->tpStatus, (const UTF8Char*)"Request Rate"));
	this->lblReqRate->SetRect(4, 148, 100, 23, false);
	NEW_CLASS(this->txtReqRate, UI::GUITextBox(ui, this->tpStatus, (const UTF8Char*)"0"));
	this->txtReqRate->SetRect(104, 148, 100, 23, false);
	this->txtReqRate->SetReadOnly(true);
	NEW_CLASS(this->lblReqTotal, UI::GUILabel(ui, this->tpStatus, (const UTF8Char*)"Request Total"));
	this->lblReqTotal->SetRect(4, 172, 100, 23, false);
	NEW_CLASS(this->txtReqTotal, UI::GUITextBox(ui, this->tpStatus, (const UTF8Char*)"0"));
	this->txtReqTotal->SetRect(104, 172, 100, 23, false);
	this->txtReqTotal->SetReadOnly(true);

	NEW_CLASS(this->lbAccess, UI::GUIListBox(ui, this->tpAccess, false));
	this->lbAccess->SetRect(0, 0, 200, 100, false);
	this->lbAccess->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbAccess->HandleSelectionChange(OnAccessSelChg, this);
	NEW_CLASS(this->hspAccess, UI::GUIHSplitter(ui, this->tpAccess, 3, false));
	NEW_CLASS(this->txtAccess, UI::GUITextBox(ui, this->tpAccess, (const UTF8Char*)"", true));
	this->txtAccess->SetReadOnly(true);
	this->txtAccess->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->txtLog, UI::GUITextBox(ui, this->tpLog, (const UTF8Char*)""));
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetReadOnly(true);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->lbLog, UI::GUIListBox(ui, this->tpLog, false));
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLog->HandleSelectionChange(OnLogSel, this);

	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRHTTPLoadBalanceForm::~AVIRHTTPLoadBalanceForm()
{
	SDEL_CLASS(this->svr);
	if (this->fwdHdlr)
	{
		this->fwdHdlr->Release();
		this->fwdHdlr = 0;
	}
	SDEL_CLASS(this->log);
	SDEL_CLASS(this->logger);
	SDEL_CLASS(this->reqLog);
	SDEL_CLASS(this->ssl);
	SDEL_CLASS(this->sslCert);
	SDEL_CLASS(this->sslKey);
}

void SSWR::AVIRead::AVIRHTTPLoadBalanceForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
