#include "Stdafx.h"
#include "Crypto/Cert/CertUtil.h"
#include "IO/BuildTime.h"
#include "IO/Path.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRHTTPSvrForm.h"
#include "SSWR/AVIRead/AVIRSSLCertKeyForm.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

SSWR::AVIRead::AVIRHTTPLog::AVIRHTTPLog(UOSInt logCnt)
{
	this->logCnt = logCnt;
	this->entries = MemAlloc(LogEntry, this->logCnt);
	this->currEnt = 0;
	UOSInt i = this->logCnt;
	while (i-- > 0)
	{
		this->entries[i].reqTime = 0;
		NEW_CLASS(this->entries[i].headerName, Data::ArrayListStringNN());
		NEW_CLASS(this->entries[i].headerVal, Data::ArrayListStringNN());
		this->entries[i].reqURI = 0;
		this->entries[i].cliAddr.addrType = Net::AddrType::Unknown;
		this->entries[i].cliPort = 0;
	}
}

SSWR::AVIRead::AVIRHTTPLog::~AVIRHTTPLog()
{
	UOSInt i = this->logCnt;
	while (i-- > 0)
	{
		SDEL_STRING(this->entries[i].reqURI);
		this->entries[i].headerName->FreeAll();
		this->entries[i].headerVal->FreeAll();
		DEL_CLASS(this->entries[i].headerName);
		DEL_CLASS(this->entries[i].headerVal);
	}
	MemFree(this->entries);
}

void SSWR::AVIRead::AVIRHTTPLog::LogRequest(NN<Net::WebServer::IWebRequest> req)
{
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	UOSInt i;
	Sync::MutexUsage mutUsage(this->entMut);
	i = this->currEnt;
	this->currEnt++;
	if (this->currEnt >= this->logCnt)
	{
		this->currEnt = 0;
	}
	SDEL_STRING(this->entries[i].reqURI);
	this->entries[i].reqURI = req->GetRequestURI()->Clone().Ptr();
	this->entries[i].cliAddr = req->GetClientAddr().Ptr()[0];
	this->entries[i].cliPort = req->GetClientPort();
	this->entries[i].reqTime = dt.ToTicks();
	this->entries[i].headerName->FreeAll();
	this->entries[i].headerVal->FreeAll();
	Data::ArrayListStringNN names;
	NN<Text::String> name;
	Text::StringBuilderUTF8 sb;
	req->GetHeaderNames(names);
	Data::ArrayIterator<NN<Text::String>> it = names.Iterator();
	while (it.HasNext())
	{
		name = it.Next();
		this->entries[i].headerName->Add(name->Clone());
		sb.ClearStr();
		req->GetHeaderC(sb, name->ToCString());
		this->entries[i].headerVal->Add(Text::String::New(sb.ToString(), sb.GetLength()));
	}
	mutUsage.EndUse();
}

UOSInt SSWR::AVIRead::AVIRHTTPLog::GetNextIndex()
{
	return this->currEnt;
}

void SSWR::AVIRead::AVIRHTTPLog::Use(NN<Sync::MutexUsage> mutUsage)
{
	mutUsage->ReplaceMutex(this->entMut);
}

void SSWR::AVIRead::AVIRHTTPLog::GetEntries(NN<Data::ArrayListNN<LogEntry>> logs, NN<Data::ArrayList<UOSInt>> logIndex)
{
	UOSInt i;
	if (this->entries[this->currEnt].reqTime == 0)
	{
		i = 0;
		while (i < this->currEnt)
		{
			logs->Add(this->entries[i]);
			logIndex->Add(i);
			i++;
		}
	}
	else
	{
		i = this->currEnt;
		while (i < this->logCnt)
		{
			logs->Add(this->entries[i]);
			logIndex->Add(i);
			i++;
		}
		i = 0;
		while (i < this->currEnt)
		{
			logs->Add(this->entries[i]);
			logIndex->Add(i);
			i++;
		}
	}
}

NN<SSWR::AVIRead::AVIRHTTPLog::LogEntry> SSWR::AVIRead::AVIRHTTPLog::GetEntry(UOSInt index)
{
	return this->entries[index];
}

void __stdcall SSWR::AVIRead::AVIRHTTPSvrForm::OnStartClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHTTPSvrForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHTTPSvrForm>();
	if (me->svr)
	{
		return;
	}
	UInt16 port = 0;
	Bool valid = true;
	UOSInt workerCnt;
	Text::StringBuilderUTF8 sb;
	me->txtPort->GetText(sb);
	Text::StrToUInt16S(sb.ToString(), port, 0);
	sb.ClearStr();
	me->txtWorkerCnt->GetText(sb);
	if (!sb.ToUOSInt(workerCnt))
	{
		me->ui->ShowMsgOK(CSTR("Please enter valid worker count"), CSTR("HTTP Server"), me);
		return;
	}
	sb.ClearStr();
	me->txtDocDir->GetText(sb);
	Optional<Net::SSLEngine> ssl = 0;

	if (me->chkSSL->IsChecked())
	{
		NN<Crypto::Cert::X509Cert> sslCert;
		NN<Crypto::Cert::X509File> sslKey;
		if (!me->sslCert.SetTo(sslCert) || !me->sslKey.SetTo(sslKey))
		{
			me->ui->ShowMsgOK(CSTR("Please select SSL Cert/Key First"), CSTR("HTTP Server"), me);
			return;
		}
		ssl = me->ssl;
		NN<Net::SSLEngine> nnssl;
		if (!ssl.SetTo(nnssl) || !nnssl->ServerSetCertsASN1(sslCert, sslKey, me->caCerts))
		{
			me->ui->ShowMsgOK(CSTR("Error in initializing Cert/Key"), CSTR("HTTP Server"), me);
			return;
		}
	}
	if (port < 65535)
	{
		UInt64 cacheSize = 0;
		if (me->chkCacheFile->IsChecked())
		{
			cacheSize = 4096;
		}
		UTF8Char sbuff[128];
		UnsafeArray<UTF8Char> sptr;
		Data::DateTime dt;
		IO::BuildTime::GetBuildTime(&dt);
		dt.ToUTCTime();
		sptr = dt.ToString(Text::StrConcatC(sbuff, UTF8STRC("AVIRead/")), "yyyyMMddHHmmss");
		NN<Net::WebServer::HTTPDirectoryHandler> dirHdlr;
		NEW_CLASSNN(dirHdlr, Net::WebServer::HTTPDirectoryHandler(sb.ToCString(), me->chkAllowBrowse->IsChecked(), cacheSize, true));
		NEW_CLASS(me->svr, Net::WebServer::WebListener(me->core->GetSocketFactory(), ssl, dirHdlr, port, 120, 2, Sync::ThreadUtil::GetThreadCnt(), CSTRP(sbuff, sptr), me->chkAllowProxy->IsChecked(), (Net::WebServer::KeepAlive)me->cboKeepAlive->GetSelectedItem().GetOSInt(), false));
		if (me->svr->IsError())
		{
			valid = false;
			SDEL_CLASS(me->svr);
			dirHdlr.Delete();
			me->ui->ShowMsgOK(CSTR("Error in listening to port"), CSTR("HTTP Server"), me);
		}
		else
		{
			me->dirHdlr = dirHdlr.Ptr();
			sb.ClearStr();
			me->txtLogDir->GetText(sb);
			if (sb.GetEndPtr()[-1] != IO::Path::PATH_SEPERATOR)
			{
				sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
			}
			sb.AppendC(UTF8STRC("Acccess"));

			if (!me->chkSkipLog->IsChecked())
			{
				NEW_CLASS(me->log, IO::LogTool());
				me->log->AddFileLog(sb.ToCString(), IO::LogHandler::LogType::PerDay, IO::LogHandler::LogGroup::PerMonth, IO::LogHandler::LogLevel::Raw, "yyyy-MM-dd HH:mm:ss.fff", false);
				me->svr->SetAccessLog(me->log, IO::LogHandler::LogLevel::Raw);
				me->svr->SetRequestLog(me->reqLog);
				NN<UI::ListBoxLogger> logger;
				NEW_CLASSNN(logger, UI::ListBoxLogger(me, me->lbLog, 500, true));
				me->logger = logger.Ptr();
				me->log->AddLogHandler(logger, IO::LogHandler::LogLevel::Raw);
			}
			if (me->chkSPKPackageFile->IsChecked())
			{
				me->dirHdlr->ExpandPackageFiles(me->core->GetParserList(), CSTR("*.spk"));
			}
			if (me->chkZIPPackageFile->IsChecked())
			{
				me->dirHdlr->ExpandPackageFiles(me->core->GetParserList(), CSTR("*.zip"));
			}
			if (me->chkCrossOrigin->IsChecked())
			{
				me->dirHdlr->SetAllowOrigin(CSTR("*"));
			}
			if (me->chkDownloadCnt->IsChecked())
			{
				me->dirHdlr->EnableStats();
			}
			if (!me->svr->Start())
			{
				valid = false;
				me->ui->ShowMsgOK(CSTR("Error in starting HTTP Server"), CSTR("HTTP Server"), me);
			}
			else
			{
				if (port == 0)
				{
					sb.ClearStr();
					sb.AppendU16(me->svr->GetListenPort());
					me->txtPort->SetText(sb.ToCString());
				}
			}
		}
	}
	else
	{
		valid = false;
		me->ui->ShowMsgOK(CSTR("Port number out of range"), CSTR("HTTP Server"), me);
	}

	if (valid)
	{
		me->txtPort->SetReadOnly(true);
		me->txtDocDir->SetReadOnly(true);
		me->txtLogDir->SetReadOnly(true);
		me->txtWorkerCnt->SetReadOnly(true);
		me->chkAllowBrowse->SetEnabled(false);
		me->chkAllowProxy->SetEnabled(false);
		me->chkCacheFile->SetEnabled(false);
		me->chkSPKPackageFile->SetEnabled(false);
		me->chkZIPPackageFile->SetEnabled(false);
		me->chkSkipLog->SetEnabled(false);
		me->cboKeepAlive->SetEnabled(false);
		me->chkCrossOrigin->SetEnabled(false);
		me->chkDownloadCnt->SetEnabled(false);
		me->chkSSL->SetEnabled(false);
		me->btnSSLCert->SetEnabled(false);
	}
	else
	{
		SDEL_CLASS(me->svr);
		SDEL_CLASS(me->dirHdlr);
		SDEL_CLASS(me->log);
		SDEL_CLASS(me->logger);
	}
}

void __stdcall SSWR::AVIRead::AVIRHTTPSvrForm::OnStopClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHTTPSvrForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHTTPSvrForm>();
	if (me->svr == 0)
	{
		return;
	}
	SDEL_CLASS(me->svr);
	SDEL_CLASS(me->dirHdlr);
	SDEL_CLASS(me->log);
	SDEL_CLASS(me->logger);
	me->txtPort->SetReadOnly(false);
	me->txtDocDir->SetReadOnly(false);
	me->txtLogDir->SetReadOnly(false);
	me->txtWorkerCnt->SetReadOnly(false);
	me->chkAllowBrowse->SetEnabled(true);
	me->chkAllowProxy->SetEnabled(true);
	me->chkCacheFile->SetEnabled(true);
	me->chkSPKPackageFile->SetEnabled(true);
	me->chkZIPPackageFile->SetEnabled(true);
	me->chkSkipLog->SetEnabled(true);
	me->cboKeepAlive->SetEnabled(true);
	me->chkCrossOrigin->SetEnabled(true);
	me->chkDownloadCnt->SetEnabled(true);
	me->chkSSL->SetEnabled(true);
	me->btnSSLCert->SetEnabled(true);
}

void __stdcall SSWR::AVIRead::AVIRHTTPSvrForm::OnLogSel(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHTTPSvrForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHTTPSvrForm>();
	Optional<Text::String> s;
	s = me->lbLog->GetSelectedItemTextNew();
	me->txtLog->SetText(Text::String::OrEmpty(s)->ToCString());
	OPTSTR_DEL(s);
}

void __stdcall SSWR::AVIRead::AVIRHTTPSvrForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHTTPSvrForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHTTPSvrForm>();
	UOSInt i;
	UOSInt j;
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
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
			sptr = Text::StrUInt64(sbuff, status.totalRead);
			me->txtDataTotalR->SetText(CSTRP(sbuff, sptr));
		}
		if (me->lastStatus.totalWrite != status.totalWrite)
		{
			me->lastStatus.totalWrite = status.totalWrite;
			sptr = Text::StrUInt64(sbuff, status.totalWrite);
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
		Data::ArrayList<UOSInt> logIndex;
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

void __stdcall SSWR::AVIRead::AVIRHTTPSvrForm::OnAccessSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHTTPSvrForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHTTPSvrForm>();
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
	sb.Append(log->reqURI);
	sb.AppendC(UTF8STRC("\r\n\r\nHeaders:"));
	i = 0;
	j = log->headerName->GetCount();
	while (i < j)
	{
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendOpt(log->headerName->GetItem(i));
		sb.AppendC(UTF8STRC("\t"));
		sb.AppendOpt(log->headerVal->GetItem(i));
		i++;
	}
	me->txtAccess->SetText(sb.ToCString());
}

void __stdcall SSWR::AVIRead::AVIRHTTPSvrForm::OnSSLCertClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHTTPSvrForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHTTPSvrForm>();
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

void SSWR::AVIRead::AVIRHTTPSvrForm::ClearCACerts()
{
	this->caCerts.DeleteAll();
}

SSWR::AVIRead::AVIRHTTPSvrForm::AVIRHTTPSvrForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	this->core = core;
	this->SetText(CSTR("HTTP Server"));
	this->SetFont(0, 0, 8.25, false);
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetSocketFactory(), true);
	this->sslCert = 0;
	this->sslKey = 0;
	this->svr = 0;
	this->log = 0;
	this->dirHdlr = 0;
	this->logger = 0;
	this->lastAccessIndex = 0;
	this->lastStatus.connCnt = 0;
	this->lastStatus.currConn = 0;
	this->lastStatus.reqCnt = 0;
	this->lastStatus.totalRead = 0;
	this->lastStatus.totalWrite = 0;
	NEW_CLASS(this->reqLog, SSWR::AVIRead::AVIRHTTPLog(100));
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpControl = this->tcMain->AddTabPage(CSTR("Control"));
	this->tpStatus = this->tcMain->AddTabPage(CSTR("Status"));
	this->tpAccess = this->tcMain->AddTabPage(CSTR("Access"));
	this->tpLog = this->tcMain->AddTabPage(CSTR("Log"));

	this->grpParam = ui->NewGroupBox(this->tpControl, CSTR("Parameters"));
	this->grpParam->SetRect(0, 0, 620, 344, false);
	this->grpParam->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblPort = ui->NewLabel(this->grpParam, CSTR("Port"));
	this->lblPort->SetRect(8, 8, 100, 23, false);
	this->txtPort = ui->NewTextBox(this->grpParam, CSTR("12345"));
	this->txtPort->SetRect(108, 8, 50, 23, false);
	this->lblDocDir = ui->NewLabel(this->grpParam, CSTR("Doc Path"));
	this->lblDocDir->SetRect(8, 32, 100, 23, false);
	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
	if (i != INVALID_INDEX)
	{
		sbuff[i] = 0;
		sptr = &sbuff[i];
	}
	this->txtDocDir = ui->NewTextBox(this->grpParam, CSTRP(sbuff, sptr));
	this->txtDocDir->SetRect(108, 32, 500, 23, false);
	this->lblLogDir = ui->NewLabel(this->grpParam, CSTR("Log Path"));
	this->lblLogDir->SetRect(8, 56, 100, 23, false);
	sbuff[i] = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrConcatC(&sbuff[i+1], UTF8STRC("log"));
	this->txtLogDir = ui->NewTextBox(this->grpParam, CSTRP(sbuff, sptr));
	this->txtLogDir->SetRect(108, 56, 500, 23, false);
	this->lblSSL = ui->NewLabel(this->grpParam, CSTR("SSL"));
	this->lblSSL->SetRect(8, 80, 100, 23, false);
	this->chkSSL = ui->NewCheckBox(this->grpParam, CSTR("Enable"), false);
	this->chkSSL->SetRect(108, 80, 100, 23, false);
	this->btnSSLCert = ui->NewButton(this->grpParam, CSTR("Cert/Key"));
	this->btnSSLCert->SetRect(208, 80, 75,23, false);
	this->btnSSLCert->HandleButtonClick(OnSSLCertClicked, this);
	this->lblSSLCert = ui->NewLabel(this->grpParam, CSTR(""));
	this->lblSSLCert->SetRect(288, 80, 200, 23, false);
	this->lblWorkerCnt = ui->NewLabel(this->grpParam, CSTR("Worker Count"));
	this->lblWorkerCnt->SetRect(8, 104, 100, 23, false);
	sptr = Text::StrUOSInt(sbuff, Sync::ThreadUtil::GetThreadCnt());
	this->txtWorkerCnt = ui->NewTextBox(this->grpParam, CSTRP(sbuff, sptr));
	this->txtWorkerCnt->SetRect(108, 104, 100, 23, false);
	this->lblAllowBrowse = ui->NewLabel(this->grpParam, CSTR("Directory Browsing"));
	this->lblAllowBrowse->SetRect(8, 128, 100, 23, false);
	this->chkAllowBrowse = ui->NewCheckBox(this->grpParam, CSTR("Allow"), true);
	this->chkAllowBrowse->SetRect(108, 128, 100, 23, false);
	this->lblAllowProxy = ui->NewLabel(this->grpParam, CSTR("Proxy Connection"));
	this->lblAllowProxy->SetRect(8, 152, 100, 23, false);
	this->chkAllowProxy = ui->NewCheckBox(this->grpParam, CSTR("Allow"), false);
	this->chkAllowProxy->SetRect(108, 152, 100, 23, false);
	this->lblCacheFile = ui->NewLabel(this->grpParam, CSTR("Cache File"));
	this->lblCacheFile->SetRect(8, 176, 100, 23, false);
	this->chkCacheFile = ui->NewCheckBox(this->grpParam, CSTR("Enable"), false);
	this->chkCacheFile->SetRect(108, 176, 100, 23, false);
	this->lblSPKPackageFile = ui->NewLabel(this->grpParam, CSTR("Expand SPK Packages"));
	this->lblSPKPackageFile->SetRect(8, 200, 100, 23, false);
	this->chkSPKPackageFile = ui->NewCheckBox(this->grpParam, CSTR("Enable"), false);
	this->chkSPKPackageFile->SetRect(108, 200, 100, 23, false);
	this->lblZIPPackageFile = ui->NewLabel(this->grpParam, CSTR("Expand ZIP Packages"));
	this->lblZIPPackageFile->SetRect(8, 224, 100, 23, false);
	this->chkZIPPackageFile = ui->NewCheckBox(this->grpParam, CSTR("Enable"), false);
	this->chkZIPPackageFile->SetRect(108, 224, 100, 23, false);
	this->lblSkipLog = ui->NewLabel(this->grpParam, CSTR("Skip Logging"));
	this->lblSkipLog->SetRect(8, 248, 100, 23, false);
	this->chkSkipLog = ui->NewCheckBox(this->grpParam, CSTR("Enable"), true);
	this->chkSkipLog->SetRect(108, 248, 100, 23, false);
	this->lblKeepAlive = ui->NewLabel(this->grpParam, CSTR("Keep Alive"));
	this->lblKeepAlive->SetRect(8, 272, 100, 23, false);
	this->cboKeepAlive = ui->NewComboBox(this->grpParam, false);
	this->cboKeepAlive->SetRect(108, 272, 100, 23, false);
	this->cboKeepAlive->AddItem(CSTR("Always"), (void*)(OSInt)Net::WebServer::KeepAlive::Always);
	this->cboKeepAlive->AddItem(CSTR("Default"), (void*)(OSInt)Net::WebServer::KeepAlive::Default);
	this->cboKeepAlive->AddItem(CSTR("No"), (void*)(OSInt)Net::WebServer::KeepAlive::No);
	this->cboKeepAlive->SetSelectedIndex(1);
	this->lblCrossOrigin = ui->NewLabel(this->grpParam, CSTR("Cross Origin"));
	this->lblCrossOrigin->SetRect(8, 296, 100, 23, false);
	this->chkCrossOrigin = ui->NewCheckBox(this->grpParam, CSTR("Allow"), false);
	this->chkCrossOrigin->SetRect(108, 296, 100, 23, false);
	this->lblDownloadCnt = ui->NewLabel(this->grpParam, CSTR("Download Count"));
	this->lblDownloadCnt->SetRect(8, 344, 100, 23, false);
	this->chkDownloadCnt = ui->NewCheckBox(this->grpParam, CSTR("Enable"), false);
	this->chkDownloadCnt->SetRect(108, 344, 100, 23, false);
	this->btnStart = ui->NewButton(this->tpControl, CSTR("Start"));
	this->btnStart->SetRect(200, 380, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClick, this);
	this->btnStop = ui->NewButton(this->tpControl, CSTR("Stop"));
	this->btnStop->SetRect(300, 380, 75, 23, false);
	this->btnStop->HandleButtonClick(OnStopClick, this);

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

SSWR::AVIRead::AVIRHTTPSvrForm::~AVIRHTTPSvrForm()
{
	SDEL_CLASS(this->svr);
	SDEL_CLASS(this->dirHdlr);
	SDEL_CLASS(this->log);
	SDEL_CLASS(this->logger);
	SDEL_CLASS(this->reqLog);
	this->ssl.Delete();
	this->sslCert.Delete();
	this->sslKey.Delete();
	this->ClearCACerts();
}

void SSWR::AVIRead::AVIRHTTPSvrForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRHTTPSvrForm::SetPort(Int32 port)
{
	UTF8Char sbuff[16];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrInt32(sbuff, port);
	this->txtPort->SetText(CSTRP(sbuff, sptr));
}

void SSWR::AVIRead::AVIRHTTPSvrForm::SetDocPath(Text::CStringNN docPath)
{
	this->txtDocDir->SetText(docPath);
}

void SSWR::AVIRead::AVIRHTTPSvrForm::SetLogPath(Text::CStringNN logPath)
{
	this->txtLogDir->SetText(logPath);
}

void SSWR::AVIRead::AVIRHTTPSvrForm::SetBrowseDir(Bool browseDir)
{
	this->chkAllowBrowse->SetChecked(browseDir);
}

void SSWR::AVIRead::AVIRHTTPSvrForm::SetAllowProxy(Bool allowProxy)
{
	this->chkAllowProxy->SetChecked(allowProxy);
}

void SSWR::AVIRead::AVIRHTTPSvrForm::StartListen()
{
	OnStartClick(this);
}
