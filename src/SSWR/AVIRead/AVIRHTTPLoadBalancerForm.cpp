#include "Stdafx.h"
#include "Crypto/Cert/CertUtil.h"
#include "IO/Path.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRHTTPSvrForm.h"
#include "SSWR/AVIRead/AVIRHTTPLoadBalancerForm.h"
#include "SSWR/AVIRead/AVIRSSLCertKeyForm.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

#define TITLE CSTR("HTTP Load Balancer")

void __stdcall SSWR::AVIRead::AVIRHTTPLoadBalancerForm::OnStartClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHTTPLoadBalancerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHTTPLoadBalancerForm>();
	if (me->svr.NotNull())
	{
		return;
	}
	UInt16 port = 0;
	Bool valid = true;
	Text::StringBuilderUTF8 sb;
	me->txtPort->GetText(sb);
	Text::StrToUInt16S(sb.ToString(), port, 0);
	Optional<Net::SSLEngine> ssl = nullptr;
	UOSInt i;
	UOSInt j;
	if (me->targets.GetCount() <= 0)
	{
		me->ui->ShowMsgOK(CSTR("Please add at least 1 target URL"), TITLE, me);
		return;
	}

	if (me->chkSSL->IsChecked())
	{
		NN<Crypto::Cert::X509Cert> sslCert;
		NN<Crypto::Cert::X509File> sslKey;
		if (!me->sslCert.SetTo(sslCert) || !me->sslKey.SetTo(sslKey))
		{
			me->ui->ShowMsgOK(CSTR("Please select SSL Cert/Key First"), TITLE, me);
			return;
		}
		ssl = me->ssl;
		NN<Net::SSLEngine> nnssl;
		if (ssl.SetTo(nnssl))
		{
			nnssl->ServerSetCertsASN1(sslCert, sslKey, me->caCerts);
		}
	}
	if (port > 0 && port < 65535)
	{
		NN<Net::WebServer::WebListener> svr;
		NN<Net::WebServer::HTTPForwardHandler> fwdHdlr;
		NEW_CLASSNN(fwdHdlr, Net::WebServer::HTTPForwardHandler(me->core->GetTCPClientFactory(), me->ssl, me->targets.GetItemNoCheck(0)->ToCString(), (Net::WebServer::HTTPForwardHandler::ForwardType)me->cboFwdType->GetSelectedItem().GetOSInt()));
		i = 1;
		j = me->targets.GetCount();
		while (i < j)
		{
			fwdHdlr->AddForwardURL(me->targets.GetItemNoCheck(i)->ToCString());
			i++;
		}
		NEW_CLASSNN(svr, Net::WebServer::WebListener(me->core->GetTCPClientFactory(), ssl, fwdHdlr, port, 120, 2, Sync::ThreadUtil::GetThreadCnt(), CSTR("sswr"), me->chkAllowProxy->IsChecked(), me->chkAllowKA->IsChecked()?Net::WebServer::KeepAlive::Always:Net::WebServer::KeepAlive::Default, false));
		if (svr->IsError())
		{
			valid = false;
			svr.Delete();
			fwdHdlr.Delete();
		}
		else
		{
			me->fwdHdlr = fwdHdlr;
			me->svr = svr;
			sb.ClearStr();
			me->txtLogDir->GetText(sb);
			if (sb.GetEndPtr()[-1] != IO::Path::PATH_SEPERATOR)
			{
				sb.AppendUTF8Char(IO::Path::PATH_SEPERATOR);
			}
			sb.AppendC(UTF8STRC("Acccess"));

			if (!me->chkSkipLog->IsChecked())
			{
				NN<IO::LogTool> log;
				NEW_CLASSNN(log, IO::LogTool());
				log->AddFileLog(sb.ToCString(), IO::LogHandler::LogType::PerDay, IO::LogHandler::LogGroup::PerMonth, IO::LogHandler::LogLevel::Raw, "yyyy-MM-dd HH:mm:ss.fff", false);
				svr->SetAccessLog(me->log, IO::LogHandler::LogLevel::Raw);
				svr->SetRequestLog(me->reqLog);
				NN<UI::ListBoxLogger> logger;
				NEW_CLASSNN(logger, UI::ListBoxLogger(me, me->lbLog, 500, true));
				me->logger = logger;
				me->log = log;
				log->AddLogHandler(logger, IO::LogHandler::LogLevel::Raw);
			}
			if (!svr->Start())
			{
				valid = false;
			}
		}
	}

	if (valid)
	{
		me->txtPort->SetReadOnly(true);
		me->txtTargetURL->SetReadOnly(true);
		me->btnTargetAdd->SetEnabled(false);
		me->txtLogDir->SetReadOnly(true);
		me->chkAllowProxy->SetEnabled(false);
		me->chkSkipLog->SetEnabled(false);
		me->chkAllowKA->SetEnabled(false);
		me->chkSSL->SetEnabled(false);
		me->btnSSLCert->SetEnabled(false);
	}
	else
	{
		me->svr.Delete();
		me->fwdHdlr.Delete();
		me->log.Delete();
		me->logger.Delete();
	}
}

void __stdcall SSWR::AVIRead::AVIRHTTPLoadBalancerForm::OnStopClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHTTPLoadBalancerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHTTPLoadBalancerForm>();
	if (me->svr.IsNull())
	{
		return;
	}
	me->svr.Delete();
	me->fwdHdlr.Delete();
	me->log.Delete();
	me->logger.Delete();
	me->txtPort->SetReadOnly(false);
	me->txtTargetURL->SetReadOnly(false);
	me->btnTargetAdd->SetEnabled(true);
	me->txtLogDir->SetReadOnly(false);
	me->chkAllowProxy->SetEnabled(true);
	me->chkSkipLog->SetEnabled(true);
	me->chkAllowKA->SetEnabled(true);
	me->chkSSL->SetEnabled(true);
	me->btnSSLCert->SetEnabled(true);
}

void __stdcall SSWR::AVIRead::AVIRHTTPLoadBalancerForm::OnLogSel(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHTTPLoadBalancerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHTTPLoadBalancerForm>();
	NN<Text::String> s;
	if (me->lbLog->GetSelectedItemTextNew().SetTo(s))
	{
		me->txtLog->SetText(s->ToCString());
		s->Release();
	}
}

void __stdcall SSWR::AVIRead::AVIRHTTPLoadBalancerForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHTTPLoadBalancerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHTTPLoadBalancerForm>();
	UOSInt i;
	UOSInt j;
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	NN<Net::WebServer::WebListener> svr;
	if (me->svr.SetTo(svr))
	{
		Net::WebServer::WebListener::SERVER_STATUS status;
		svr->GetStatus(status);

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
		Data::ArrayListNN<SSWR::AVIRead::AVIRHTTPLog::LogEntry> logs;
		Data::ArrayListNative<UOSInt> logIndex;
		NN<SSWR::AVIRead::AVIRHTTPLog::LogEntry> log;
		Text::StringBuilderUTF8 sb;
		Sync::MutexUsage mutUsage;

		me->lastAccessIndex = i;
		me->reqLog->Use(mutUsage);
		me->reqLog->GetEntries(logs, logIndex);
		me->lbAccess->ClearItems();
		me->txtAccess->SetText(CSTR(""));
		i = 0;
		j = logs.GetCount();
		while (i < j)
		{
			log = logs.GetItemNoCheck(i);
			sb.ClearStr();
			sb.AppendTSNoZone(Data::Timestamp(log->reqTime, Data::DateTimeUtil::GetLocalTzQhr()));
			sb.AppendC(UTF8STRC(" "));
			sptr = Net::SocketUtil::GetAddrName(sbuff, log->cliAddr, log->cliPort).Or(sbuff);
			sb.AppendP(sbuff, sptr);

			me->lbAccess->AddItem(sb.ToCString(), (void*)(OSInt)logIndex.GetItem(i));
			i++;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRHTTPLoadBalancerForm::OnAccessSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHTTPLoadBalancerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHTTPLoadBalancerForm>();
	Text::StringBuilderUTF8 sb;
	Sync::MutexUsage mutUsage;
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	me->reqLog->Use(mutUsage);
	UOSInt i = (UOSInt)me->lbAccess->GetSelectedItem().p;
	UOSInt j;
	NN<SSWR::AVIRead::AVIRHTTPLog::LogEntry> log;
	log = me->reqLog->GetEntry(i);
	sb.AppendTSNoZone(Data::Timestamp(log->reqTime, Data::DateTimeUtil::GetLocalTzQhr()));
	sb.AppendC(UTF8STRC(" "));
	sptr = Net::SocketUtil::GetAddrName(sbuff, log->cliAddr, log->cliPort).Or(sbuff);
	sb.AppendP(sbuff, sptr);
	sb.AppendC(UTF8STRC("\r\n"));
	sb.AppendOpt(log->reqURI);
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

void __stdcall SSWR::AVIRead::AVIRHTTPLoadBalancerForm::OnSSLCertClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHTTPLoadBalancerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHTTPLoadBalancerForm>();
	SSWR::AVIRead::AVIRSSLCertKeyForm frm(nullptr, me->ui, me->core, me->ssl, me->sslCert, me->sslKey, me->caCerts);
	if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
	{
		me->sslCert.Delete();
		me->sslKey.Delete();
		me->ClearCACerts();
		me->sslCert = frm.GetCert();
		me->sslKey = frm.GetKey();
		frm.GetCACerts(me->caCerts);
		Text::StringBuilderUTF8 sb;
		NN<Crypto::Cert::X509Cert> nnCert;
		NN<Crypto::Cert::X509File> nnKey;
		if (me->sslCert.SetTo(nnCert)) nnCert->ToShortString(sb);
		sb.AppendC(UTF8STRC(", "));
		if (me->sslKey.SetTo(nnKey)) nnKey->ToShortString(sb);
		me->lblSSLCert->SetText(sb.ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRHTTPLoadBalancerForm::OnTargetAddClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHTTPLoadBalancerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHTTPLoadBalancerForm>();
	Text::StringBuilderUTF8 sb;
	me->txtTargetURL->GetText(sb);
	if (!sb.StartsWith(UTF8STRC("http://")) && !sb.StartsWith(UTF8STRC("https://")))
	{
		me->ui->ShowMsgOK(CSTR("Invalid Target URL, must be starts with http::// or https://"), TITLE, me);
		return;
	}
	UOSInt i = me->targets.GetCount();
	while (i-- > 0)
	{
		if (me->targets.GetItemNoCheck(i)->Equals(sb.ToCString()))
		{
			me->ui->ShowMsgOK(CSTR("Target URL already exists"), TITLE, me);
			return;
		}
	}
	me->targets.Add(Text::String::New(sb.ToCString()));
	me->lbTarget->AddItem(sb.ToCString(), 0);
}

void SSWR::AVIRead::AVIRHTTPLoadBalancerForm::ClearCACerts()
{
	UOSInt i = this->caCerts.GetCount();
	while (i-- > 0)
	{
		this->caCerts.GetItem(i).Delete();
	}
	this->caCerts.Clear();
}

SSWR::AVIRead::AVIRHTTPLoadBalancerForm::AVIRHTTPLoadBalancerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	this->core = core;
	this->SetText(TITLE);
	this->SetFont(nullptr, 8.25, false);
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetTCPClientFactory(), true);
	this->sslCert = nullptr;
	this->sslKey = nullptr;
	this->svr = nullptr;
	this->log = nullptr;
	this->fwdHdlr = nullptr;
	this->logger = nullptr;
	this->lastAccessIndex = 0;
	this->lastStatus.connCnt = 0;
	this->lastStatus.currConn = 0;
	this->lastStatus.reqCnt = 0;
	this->lastStatus.totalRead = 0;
	this->lastStatus.totalWrite = 0;
	NEW_CLASSNN(this->reqLog, SSWR::AVIRead::AVIRHTTPLog(100));
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpControl = this->tcMain->AddTabPage(CSTR("Control"));
	this->tpStatus = this->tcMain->AddTabPage(CSTR("Status"));
	this->tpAccess = this->tcMain->AddTabPage(CSTR("Access"));
	this->tpLog = this->tcMain->AddTabPage(CSTR("Log"));

	this->grpParam = ui->NewGroupBox(this->tpControl, CSTR("Parameters"));
	this->grpParam->SetRect(0, 0, 620, 256, false);
	this->grpParam->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblPort = ui->NewLabel(this->grpParam, CSTR("Port"));
	this->lblPort->SetRect(8, 8, 100, 23, false);
	this->txtPort = ui->NewTextBox(this->grpParam, CSTR("12345"));
	this->txtPort->SetRect(108, 8, 50, 23, false);
	this->lblLogDir = ui->NewLabel(this->grpParam, CSTR("Log Path"));
	this->lblLogDir->SetRect(8, 32, 100, 23, false);
	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
	sptr = Text::StrConcatC(&sbuff[i + 1], UTF8STRC("log"));
	this->txtLogDir = ui->NewTextBox(this->grpParam, CSTRP(sbuff, sptr));
	this->txtLogDir->SetRect(108, 32, 500, 23, false);
	this->lblSSL = ui->NewLabel(this->grpParam, CSTR("SSL"));
	this->lblSSL->SetRect(8, 56, 100, 23, false);
	this->chkSSL = ui->NewCheckBox(this->grpParam, CSTR("Enable"), false);
	this->chkSSL->SetRect(108, 56, 100, 23, false);
	this->btnSSLCert = ui->NewButton(this->grpParam, CSTR("Cert/Key"));
	this->btnSSLCert->SetRect(208, 56, 75,23, false);
	this->btnSSLCert->HandleButtonClick(OnSSLCertClicked, this);
	this->lblSSLCert = ui->NewLabel(this->grpParam, CSTR(""));
	this->lblSSLCert->SetRect(288, 56, 200, 23, false);
	this->lblAllowProxy = ui->NewLabel(this->grpParam, CSTR("Proxy Connection"));
	this->lblAllowProxy->SetRect(8, 80, 100, 23, false);
	this->chkAllowProxy = ui->NewCheckBox(this->grpParam, CSTR("Allow"), false);
	this->chkAllowProxy->SetRect(108, 80, 100, 23, false);
	this->lblSkipLog = ui->NewLabel(this->grpParam, CSTR("Skip Logging"));
	this->lblSkipLog->SetRect(8, 104, 100, 23, false);
	this->chkSkipLog = ui->NewCheckBox(this->grpParam, CSTR("Enable"), true);
	this->chkSkipLog->SetRect(108, 104, 100, 23, false);
	this->lblAllowKA = ui->NewLabel(this->grpParam, CSTR("Allow KA"));
	this->lblAllowKA->SetRect(8, 128, 100, 23, false);
	this->chkAllowKA = ui->NewCheckBox(this->grpParam, CSTR("Enable"), false);
	this->chkAllowKA->SetRect(108, 128, 100, 23, false);
	this->lblFwdType = ui->NewLabel(this->grpParam, CSTR("Forward Type"));
	this->lblFwdType->SetRect(8, 152, 100, 23, false);
	this->cboFwdType = ui->NewComboBox(this->grpParam,false);
	this->cboFwdType->SetRect(108, 152, 100, 23, false);
	this->cboFwdType->AddItem(CSTR("Normal"), (void*)(OSInt)Net::WebServer::HTTPForwardHandler::ForwardType::Normal);
	this->cboFwdType->AddItem(CSTR("Transparent"), (void*)(OSInt)Net::WebServer::HTTPForwardHandler::ForwardType::Transparent);
	this->cboFwdType->SetSelectedIndex(0);
	this->btnStart = ui->NewButton(this->grpParam, CSTR("Start"));
	this->btnStart->SetRect(200, 176, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClick, this);
	this->btnStop = ui->NewButton(this->grpParam, CSTR("Stop"));
	this->btnStop->SetRect(300, 176, 75, 23, false);
	this->btnStop->HandleButtonClick(OnStopClick, this);
	this->grpTarget = ui->NewGroupBox(this->tpControl, CSTR("Targets"));
	this->grpTarget->SetDockType(UI::GUIControl::DOCK_FILL);
	this->pnlTarget = ui->NewPanel(this->grpTarget);
	this->pnlTarget->SetRect(0, 0, 100, 31, false);
	this->pnlTarget->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblTargetURL = ui->NewLabel(this->pnlTarget, CSTR("Forward URL"));
	this->lblTargetURL->SetRect(8, 4, 100, 23, false);
	this->txtTargetURL = ui->NewTextBox(this->pnlTarget, CSTR("http://sswroom.no-ip.org:5080/"));
	this->txtTargetURL->SetRect(108, 4, 500, 23, false);
	this->btnTargetAdd = ui->NewButton(this->pnlTarget, CSTR("Add"));
	this->btnTargetAdd->SetRect(608, 4, 75, 23, false);
	this->btnTargetAdd->HandleButtonClick(OnTargetAddClicked, this);
	this->lbTarget = ui->NewListBox(this->grpTarget, false);
	this->lbTarget->SetDockType(UI::GUIControl::DOCK_FILL);

	this->lblConnCurr = ui->NewLabel(this->tpStatus, CSTR("Conn Curr"));
	this->lblConnCurr->SetRect(4, 4, 100, 23, false);
	this->txtConnCurr = ui->NewTextBox(this->tpStatus, CSTR("0"));
	this->txtConnCurr->SetRect(104, 4, 100, 23, false);
	this->txtConnCurr->SetReadOnly(true);
	this->lblConnTotal = ui->NewLabel(this->tpStatus, CSTR("Conn Total"));
	this->lblConnTotal->SetRect(4, 28, 100, 23, false);
	this->txtConnTotal = ui->NewTextBox(this->tpStatus, CSTR("0"));
	this->txtConnTotal->SetRect(104, 28, 100, 23, false);
	this->txtConnTotal->SetReadOnly(true);
	this->lblDataRateR = ui->NewLabel(this->tpStatus, CSTR("Data Rate(R)"));
	this->lblDataRateR->SetRect(4, 52, 100, 23, false);
	this->txtDataRateR = ui->NewTextBox(this->tpStatus, CSTR("0"));
	this->txtDataRateR->SetRect(104, 52, 100, 23, false);
	this->txtDataRateR->SetReadOnly(true);
	this->lblDataRateW = ui->NewLabel(this->tpStatus, CSTR("Data Rate(W)"));
	this->lblDataRateW->SetRect(4, 76, 100, 23, false);
	this->txtDataRateW = ui->NewTextBox(this->tpStatus, CSTR("0"));
	this->txtDataRateW->SetRect(104, 76, 100, 23, false);
	this->txtDataRateW->SetReadOnly(true);
	this->lblDataTotalR = ui->NewLabel(this->tpStatus, CSTR("Total Data(R)"));
	this->lblDataTotalR->SetRect(4, 100, 100, 23, false);
	this->txtDataTotalR = ui->NewTextBox(this->tpStatus, CSTR("0"));
	this->txtDataTotalR->SetRect(104, 100, 100, 23, false);
	this->txtDataTotalR->SetReadOnly(true);
	this->lblDataTotalW = ui->NewLabel(this->tpStatus, CSTR("Total Data(W)"));
	this->lblDataTotalW->SetRect(4, 124, 100, 23, false);
	this->txtDataTotalW = ui->NewTextBox(this->tpStatus, CSTR("0"));
	this->txtDataTotalW->SetRect(104, 124, 100, 23, false);
	this->txtDataTotalW->SetReadOnly(true);
	this->lblReqRate = ui->NewLabel(this->tpStatus, CSTR("Request Rate"));
	this->lblReqRate->SetRect(4, 148, 100, 23, false);
	this->txtReqRate = ui->NewTextBox(this->tpStatus, CSTR("0"));
	this->txtReqRate->SetRect(104, 148, 100, 23, false);
	this->txtReqRate->SetReadOnly(true);
	this->lblReqTotal = ui->NewLabel(this->tpStatus, CSTR("Request Total"));
	this->lblReqTotal->SetRect(4, 172, 100, 23, false);
	this->txtReqTotal = ui->NewTextBox(this->tpStatus, CSTR("0"));
	this->txtReqTotal->SetRect(104, 172, 100, 23, false);
	this->txtReqTotal->SetReadOnly(true);

	this->lbAccess = ui->NewListBox(this->tpAccess, false);
	this->lbAccess->SetRect(0, 0, 200, 100, false);
	this->lbAccess->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbAccess->HandleSelectionChange(OnAccessSelChg, this);
	this->hspAccess = ui->NewHSplitter(this->tpAccess, 3, false);
	this->txtAccess = ui->NewTextBox(this->tpAccess, CSTR(""), true);
	this->txtAccess->SetReadOnly(true);
	this->txtAccess->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtLog = ui->NewTextBox(this->tpLog, CSTR(""));
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetReadOnly(true);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lbLog = ui->NewListBox(this->tpLog, false);
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLog->HandleSelectionChange(OnLogSel, this);

	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRHTTPLoadBalancerForm::~AVIRHTTPLoadBalancerForm()
{
	this->svr.Delete();
	this->fwdHdlr.Delete();
	this->log.Delete();
	this->logger.Delete();
	this->reqLog.Delete();
	this->ssl.Delete();
	this->sslCert.Delete();
	this->sslKey.Delete();
	this->ClearCACerts();
}

void SSWR::AVIRead::AVIRHTTPLoadBalancerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
