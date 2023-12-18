#include "Stdafx.h"
#include "Crypto/Cert/CertUtil.h"
#include "IO/Path.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRHTTPSvrForm.h"
#include "SSWR/AVIRead/AVIRHTTPLoadBalanceForm.h"
#include "SSWR/AVIRead/AVIRSSLCertKeyForm.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRHTTPLoadBalanceForm::OnStartClick(void *userObj)
{
	SSWR::AVIRead::AVIRHTTPLoadBalanceForm *me = (SSWR::AVIRead::AVIRHTTPLoadBalanceForm*)userObj;
	if (me->svr)
	{
		return;
	}
	UInt16 port = 0;
	Bool valid = true;
	Text::StringBuilderUTF8 sb;
	me->txtPort->GetText(sb);
	Text::StrToUInt16S(sb.ToString(), port, 0);
	sb.ClearStr();
	me->txtFwdURL->GetText(sb);
	Optional<Net::SSLEngine> ssl = 0;
	if (!sb.StartsWith(UTF8STRC("http://")) && !sb.StartsWith(UTF8STRC("https://")))
	{
		me->ui->ShowMsgOK(CSTR("Invalid Forward URL"), CSTR("HTTP Load Balance"), me);
		return;
	}

	if (me->chkSSL->IsChecked())
	{
		NotNullPtr<Crypto::Cert::X509Cert> sslCert;
		NotNullPtr<Crypto::Cert::X509File> sslKey;
		if (!sslCert.Set(me->sslCert) || !sslKey.Set(me->sslKey))
		{
			me->ui->ShowMsgOK(CSTR("Please select SSL Cert/Key First"), CSTR("HTTP Load Balance"), me);
			return;
		}
		ssl = me->ssl;
		NotNullPtr<Net::SSLEngine> nnssl;
		if (ssl.SetTo(nnssl))
		{
			nnssl->ServerSetCertsASN1(sslCert, sslKey, me->caCerts);
		}
	}
	if (port > 0 && port < 65535)
	{
		NotNullPtr<Net::WebServer::HTTPForwardHandler> fwdHdlr;
		NEW_CLASSNN(fwdHdlr, Net::WebServer::HTTPForwardHandler(me->core->GetSocketFactory(), me->ssl, sb.ToCString(), (Net::WebServer::HTTPForwardHandler::ForwardType)(OSInt)me->cboFwdType->GetSelectedItem()));
		NEW_CLASS(me->svr, Net::WebServer::WebListener(me->core->GetSocketFactory(), ssl, fwdHdlr, port, 120, 2, Sync::ThreadUtil::GetThreadCnt(), CSTR("sswr"), me->chkAllowProxy->IsChecked(), me->chkAllowKA->IsChecked()?Net::WebServer::KeepAlive::Always:Net::WebServer::KeepAlive::Default, false));
		if (me->svr->IsError())
		{
			valid = false;
			SDEL_CLASS(me->svr);
			fwdHdlr.Delete();
		}
		else
		{
			me->fwdHdlr = fwdHdlr.Ptr();
			sb.ClearStr();
			me->txtLogDir->GetText(sb);
			if (sb.GetEndPtr()[-1] != IO::Path::PATH_SEPERATOR)
			{
				sb.AppendUTF8Char(IO::Path::PATH_SEPERATOR);
			}
			sb.AppendC(UTF8STRC("Acccess"));

			if (!me->chkSkipLog->IsChecked())
			{
				NEW_CLASS(me->log, IO::LogTool());
				me->log->AddFileLog(sb.ToCString(), IO::LogHandler::LogType::PerDay, IO::LogHandler::LogGroup::PerMonth, IO::LogHandler::LogLevel::Raw, "yyyy-MM-dd HH:mm:ss.fff", false);
				me->svr->SetAccessLog(me->log, IO::LogHandler::LogLevel::Raw);
				me->svr->SetRequestLog(me->reqLog);
				NotNullPtr<UI::ListBoxLogger> logger;
				NEW_CLASSNN(logger, UI::ListBoxLogger(*me, me->lbLog, 500, true));
				me->logger = logger.Ptr();
				me->log->AddLogHandler(logger, IO::LogHandler::LogLevel::Raw);
			}
			if (!me->svr->Start())
			{
				valid = false;
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
		SDEL_CLASS(me->fwdHdlr);
		SDEL_CLASS(me->log);
		SDEL_CLASS(me->logger);
	}
}

void __stdcall SSWR::AVIRead::AVIRHTTPLoadBalanceForm::OnStopClick(void *userObj)
{
	SSWR::AVIRead::AVIRHTTPLoadBalanceForm *me = (SSWR::AVIRead::AVIRHTTPLoadBalanceForm*)userObj;
	if (me->svr == 0)
	{
		return;
	}
	SDEL_CLASS(me->svr);
	SDEL_CLASS(me->fwdHdlr);
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
	NotNullPtr<Text::String> s;
	if (me->lbLog->GetSelectedItemTextNew().SetTo(s))
	{
		me->txtLog->SetText(s->ToCString());
		s->Release();
	}
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
			sptr = Text::StrUInt32(sbuff, status.currConn);
			me->txtConnCurr->SetText(CSTRP(sbuff, sptr));
		}
		if (me->lastStatus.connCnt != status.connCnt)
		{
			me->lastStatus.connCnt = status.connCnt;
			sptr = Text::StrUInt32(sbuff, status.connCnt);
			me->txtConnTotal->SetText(CSTRP(sbuff, sptr));
		}
		sptr = Text::StrUInt64(sbuff, status.totalRead - me->lastStatus.totalRead);
		me->txtDataRateR->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrUInt64(sbuff, status.totalWrite - me->lastStatus.totalWrite);
		me->txtDataRateW->SetText(CSTRP(sbuff, sptr));
		if (me->lastStatus.totalRead != status.totalRead)
		{
			me->lastStatus.totalRead = status.totalRead;
			sptr =Text::StrUInt64(sbuff, status.totalRead);
			me->txtDataTotalR->SetText(CSTRP(sbuff, sptr));
		}
		if (me->lastStatus.totalWrite != status.totalWrite)
		{
			me->lastStatus.totalWrite = status.totalWrite;
			sptr =Text::StrUInt64(sbuff, status.totalWrite);
			me->txtDataTotalW->SetText(CSTRP(sbuff, sptr));
		}
		sptr = Text::StrUInt32(sbuff, status.reqCnt - me->lastStatus.reqCnt);
		me->txtReqRate->SetText(CSTRP(sbuff, sptr));
		if (me->lastStatus.reqCnt != status.reqCnt)
		{
			me->lastStatus.reqCnt = status.reqCnt;
			sptr = Text::StrUInt32(sbuff, status.reqCnt);
			me->txtReqTotal->SetText(CSTRP(sbuff, sptr));
		}
	}

	i = me->reqLog->GetNextIndex();
	if (i != me->lastAccessIndex)
	{
		Data::ArrayList<SSWR::AVIRead::AVIRHTTPLog::LogEntry *> logs;
		Data::ArrayList<UOSInt> logIndex;
		SSWR::AVIRead::AVIRHTTPLog::LogEntry *log;
		Text::StringBuilderUTF8 sb;
		Sync::MutexUsage mutUsage;

		me->lastAccessIndex = i;
		me->reqLog->Use(mutUsage);
		me->reqLog->GetEntries(&logs, &logIndex);
		me->lbAccess->ClearItems();
		me->txtAccess->SetText(CSTR(""));
		i = 0;
		j = logs.GetCount();
		while (i < j)
		{
			log = logs.GetItem(i);
			sb.ClearStr();
			sb.AppendTSNoZone(Data::Timestamp(log->reqTime, Data::DateTimeUtil::GetLocalTzQhr()));
			sb.AppendC(UTF8STRC(" "));
			sptr = Net::SocketUtil::GetAddrName(sbuff, log->cliAddr, log->cliPort);
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
	me->reqLog->Use(mutUsage);
	UOSInt i = (UOSInt)me->lbAccess->GetSelectedItem();
	UOSInt j;
	SSWR::AVIRead::AVIRHTTPLog::LogEntry *log;
	log = me->reqLog->GetEntry(i);
	sb.AppendTSNoZone(Data::Timestamp(log->reqTime, Data::DateTimeUtil::GetLocalTzQhr()));
	sb.AppendC(UTF8STRC(" "));
	sptr = Net::SocketUtil::GetAddrName(sbuff, log->cliAddr, log->cliPort);
	sb.AppendP(sbuff, sptr);
	sb.AppendC(UTF8STRC("\r\n"));
	sb.Append(log->reqURI);
	sb.AppendC(UTF8STRC("\r\n\r\nHeaders:"));
	i = 0;
	j = log->headerName->GetCount();
	while (i < j)
	{
		sb.AppendC(UTF8STRC("\r\n"));
		sb.Append(Text::String::OrEmpty(log->headerName->GetItem(i)));
		sb.AppendC(UTF8STRC("\t"));
		sb.Append(Text::String::OrEmpty(log->headerVal->GetItem(i)));
		i++;
	}
	me->txtAccess->SetText(sb.ToCString());
}

void __stdcall SSWR::AVIRead::AVIRHTTPLoadBalanceForm::OnSSLCertClicked(void *userObj)
{
	SSWR::AVIRead::AVIRHTTPLoadBalanceForm *me = (SSWR::AVIRead::AVIRHTTPLoadBalanceForm*)userObj;
	SSWR::AVIRead::AVIRSSLCertKeyForm frm(0, me->ui, me->core, me->ssl, me->sslCert, me->sslKey, me->caCerts);
	if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
	{
		SDEL_CLASS(me->sslCert);
		SDEL_CLASS(me->sslKey);
		me->ClearCACerts();
		me->sslCert = frm.GetCert();
		me->sslKey = frm.GetKey();
		frm.GetCACerts(me->caCerts);
		Text::StringBuilderUTF8 sb;
		me->sslCert->ToShortString(sb);
		sb.AppendC(UTF8STRC(", "));
		me->sslKey->ToShortString(sb);
		me->lblSSLCert->SetText(sb.ToCString());
	}
}

void SSWR::AVIRead::AVIRHTTPLoadBalanceForm::ClearCACerts()
{
	UOSInt i = this->caCerts.GetCount();
	while (i-- > 0)
	{
		this->caCerts.GetItem(i).Delete();
	}
	this->caCerts.Clear();
}

SSWR::AVIRead::AVIRHTTPLoadBalanceForm::AVIRHTTPLoadBalanceForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UOSInt i;
	this->core = core;
	this->SetText(CSTR("HTTP Load Balance"));
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

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, *this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpControl = this->tcMain->AddTabPage(CSTR("Control"));
	this->tpStatus = this->tcMain->AddTabPage(CSTR("Status"));
	this->tpAccess = this->tcMain->AddTabPage(CSTR("Access"));
	this->tpLog = this->tcMain->AddTabPage(CSTR("Log"));

	this->grpParam = ui->NewGroupBox(this->tpControl, CSTR("Parameters"));
	this->grpParam->SetRect(0, 0, 620, 256, false);
	this->grpParam->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblPort, UI::GUILabel(ui, this->grpParam, CSTR("Port")));
	this->lblPort->SetRect(8, 8, 100, 23, false);
	NEW_CLASS(this->txtPort, UI::GUITextBox(ui, this->grpParam, CSTR("12345")));
	this->txtPort->SetRect(108, 8, 50, 23, false);
	NEW_CLASS(this->lblLogDir, UI::GUILabel(ui, this->grpParam, CSTR("Log Path")));
	this->lblLogDir->SetRect(8, 32, 100, 23, false);
	sptr = IO::Path::GetProcessFileName(sbuff);
	i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
	sptr = Text::StrConcatC(&sbuff[i + 1], UTF8STRC("log"));
	NEW_CLASS(this->txtLogDir, UI::GUITextBox(ui, this->grpParam, CSTRP(sbuff, sptr)));
	this->txtLogDir->SetRect(108, 32, 500, 23, false);
	NEW_CLASS(this->lblSSL, UI::GUILabel(ui, this->grpParam, CSTR("SSL")));
	this->lblSSL->SetRect(8, 56, 100, 23, false);
	NEW_CLASS(this->chkSSL, UI::GUICheckBox(ui, this->grpParam, CSTR("Enable"), false));
	this->chkSSL->SetRect(108, 56, 100, 23, false);
	this->btnSSLCert = ui->NewButton(this->grpParam, CSTR("Cert/Key"));
	this->btnSSLCert->SetRect(208, 56, 75,23, false);
	this->btnSSLCert->HandleButtonClick(OnSSLCertClicked, this);
	NEW_CLASS(this->lblSSLCert, UI::GUILabel(ui, this->grpParam, CSTR("")));
	this->lblSSLCert->SetRect(288, 56, 200, 23, false);
	NEW_CLASS(this->lblAllowProxy, UI::GUILabel(ui, this->grpParam, CSTR("Proxy Connection")));
	this->lblAllowProxy->SetRect(8, 80, 100, 23, false);
	NEW_CLASS(this->chkAllowProxy, UI::GUICheckBox(ui, this->grpParam, CSTR("Allow"), false));
	this->chkAllowProxy->SetRect(108, 80, 100, 23, false);
	NEW_CLASS(this->lblSkipLog, UI::GUILabel(ui, this->grpParam, CSTR("Skip Logging")));
	this->lblSkipLog->SetRect(8, 104, 100, 23, false);
	NEW_CLASS(this->chkSkipLog, UI::GUICheckBox(ui, this->grpParam, CSTR("Enable"), true));
	this->chkSkipLog->SetRect(108, 104, 100, 23, false);
	NEW_CLASS(this->lblAllowKA, UI::GUILabel(ui, this->grpParam, CSTR("Allow KA")));
	this->lblAllowKA->SetRect(8, 128, 100, 23, false);
	NEW_CLASS(this->chkAllowKA, UI::GUICheckBox(ui, this->grpParam, CSTR("Enable"), false));
	this->chkAllowKA->SetRect(108, 128, 100, 23, false);
	NEW_CLASS(this->lblFwdURL, UI::GUILabel(ui, this->grpParam, CSTR("Forward URL")));
	this->lblFwdURL->SetRect(8, 152, 100, 23, false);
	NEW_CLASS(this->txtFwdURL, UI::GUITextBox(ui, this->grpParam, CSTR("http://sswroom.no-ip.org:5080/")));
	this->txtFwdURL->SetRect(108, 152, 500, 23, false);
	NEW_CLASS(this->lblFwdType, UI::GUILabel(ui, this->grpParam, CSTR("Forward Type")));
	this->lblFwdType->SetRect(8, 176, 100, 23, false);
	this->cboFwdType = ui->NewComboBox(this->grpParam,false);
	this->cboFwdType->SetRect(108, 176, 100, 23, false);
	this->cboFwdType->AddItem(CSTR("Normal"), (void*)(OSInt)Net::WebServer::HTTPForwardHandler::ForwardType::Normal);
	this->cboFwdType->AddItem(CSTR("Transparent"), (void*)(OSInt)Net::WebServer::HTTPForwardHandler::ForwardType::Transparent);
	this->cboFwdType->SetSelectedIndex(0);
	this->btnStart = ui->NewButton(this->tpControl, CSTR("Start"));
	this->btnStart->SetRect(200, 300, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClick, this);
	this->btnStop = ui->NewButton(this->tpControl, CSTR("Stop"));
	this->btnStop->SetRect(300, 300, 75, 23, false);
	this->btnStop->HandleButtonClick(OnStopClick, this);

	NEW_CLASS(this->lblConnCurr, UI::GUILabel(ui, this->tpStatus, CSTR("Conn Curr")));
	this->lblConnCurr->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtConnCurr, UI::GUITextBox(ui, this->tpStatus, CSTR("0")));
	this->txtConnCurr->SetRect(104, 4, 100, 23, false);
	this->txtConnCurr->SetReadOnly(true);
	NEW_CLASS(this->lblConnTotal, UI::GUILabel(ui, this->tpStatus, CSTR("Conn Total")));
	this->lblConnTotal->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtConnTotal, UI::GUITextBox(ui, this->tpStatus, CSTR("0")));
	this->txtConnTotal->SetRect(104, 28, 100, 23, false);
	this->txtConnTotal->SetReadOnly(true);
	NEW_CLASS(this->lblDataRateR, UI::GUILabel(ui, this->tpStatus, CSTR("Data Rate(R)")));
	this->lblDataRateR->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtDataRateR, UI::GUITextBox(ui, this->tpStatus, CSTR("0")));
	this->txtDataRateR->SetRect(104, 52, 100, 23, false);
	this->txtDataRateR->SetReadOnly(true);
	NEW_CLASS(this->lblDataRateW, UI::GUILabel(ui, this->tpStatus, CSTR("Data Rate(W)")));
	this->lblDataRateW->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtDataRateW, UI::GUITextBox(ui, this->tpStatus, CSTR("0")));
	this->txtDataRateW->SetRect(104, 76, 100, 23, false);
	this->txtDataRateW->SetReadOnly(true);
	NEW_CLASS(this->lblDataTotalR, UI::GUILabel(ui, this->tpStatus, CSTR("Total Data(R)")));
	this->lblDataTotalR->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtDataTotalR, UI::GUITextBox(ui, this->tpStatus, CSTR("0")));
	this->txtDataTotalR->SetRect(104, 100, 100, 23, false);
	this->txtDataTotalR->SetReadOnly(true);
	NEW_CLASS(this->lblDataTotalW, UI::GUILabel(ui, this->tpStatus, CSTR("Total Data(W)")));
	this->lblDataTotalW->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->txtDataTotalW, UI::GUITextBox(ui, this->tpStatus, CSTR("0")));
	this->txtDataTotalW->SetRect(104, 124, 100, 23, false);
	this->txtDataTotalW->SetReadOnly(true);
	NEW_CLASS(this->lblReqRate, UI::GUILabel(ui, this->tpStatus, CSTR("Request Rate")));
	this->lblReqRate->SetRect(4, 148, 100, 23, false);
	NEW_CLASS(this->txtReqRate, UI::GUITextBox(ui, this->tpStatus, CSTR("0")));
	this->txtReqRate->SetRect(104, 148, 100, 23, false);
	this->txtReqRate->SetReadOnly(true);
	NEW_CLASS(this->lblReqTotal, UI::GUILabel(ui, this->tpStatus, CSTR("Request Total")));
	this->lblReqTotal->SetRect(4, 172, 100, 23, false);
	NEW_CLASS(this->txtReqTotal, UI::GUITextBox(ui, this->tpStatus, CSTR("0")));
	this->txtReqTotal->SetRect(104, 172, 100, 23, false);
	this->txtReqTotal->SetReadOnly(true);

	NEW_CLASS(this->lbAccess, UI::GUIListBox(ui, this->tpAccess, false));
	this->lbAccess->SetRect(0, 0, 200, 100, false);
	this->lbAccess->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbAccess->HandleSelectionChange(OnAccessSelChg, this);
	this->hspAccess = ui->NewHSplitter(this->tpAccess, 3, false);
	NEW_CLASS(this->txtAccess, UI::GUITextBox(ui, this->tpAccess, CSTR(""), true));
	this->txtAccess->SetReadOnly(true);
	this->txtAccess->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->txtLog, UI::GUITextBox(ui, this->tpLog, CSTR("")));
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetReadOnly(true);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASSNN(this->lbLog, UI::GUIListBox(ui, this->tpLog, false));
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLog->HandleSelectionChange(OnLogSel, this);

	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRHTTPLoadBalanceForm::~AVIRHTTPLoadBalanceForm()
{
	SDEL_CLASS(this->svr);
	SDEL_CLASS(this->fwdHdlr);
	SDEL_CLASS(this->log);
	SDEL_CLASS(this->logger);
	SDEL_CLASS(this->reqLog);
	this->ssl.Delete();
	SDEL_CLASS(this->sslCert);
	SDEL_CLASS(this->sslKey);
	this->ClearCACerts();
}

void SSWR::AVIRead::AVIRHTTPLoadBalanceForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
