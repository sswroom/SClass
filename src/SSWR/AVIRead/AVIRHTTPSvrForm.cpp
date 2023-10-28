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
#include "UI/MessageDialog.h"

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
		NEW_CLASS(this->entries[i].headerVal, Data::ArrayListNN<Text::String>());
		this->entries[i].reqURI = 0;
		this->entries[i].cliAddr.addrType = Net::AddrType::Unknown;
		this->entries[i].cliPort = 0;
	}
}

SSWR::AVIRead::AVIRHTTPLog::~AVIRHTTPLog()
{
	UOSInt j;
	UOSInt i = this->logCnt;
	while (i-- > 0)
	{
		SDEL_STRING(this->entries[i].reqURI);
		j = this->entries[i].headerName->GetCount();
		while (j-- > 0)
		{
			this->entries[i].headerName->GetItem(j)->Release();
			this->entries[i].headerVal->GetItem(j)->Release();
		}
		DEL_CLASS(this->entries[i].headerName);
		DEL_CLASS(this->entries[i].headerVal);
	}
	MemFree(this->entries);
}

void SSWR::AVIRead::AVIRHTTPLog::LogRequest(NotNullPtr<Net::WebServer::IWebRequest> req)
{
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	UOSInt i;
	UOSInt j;
	UOSInt k;
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
	j = this->entries[i].headerName->GetCount();
	while (j-- > 0)
	{
		this->entries[i].headerName->GetItem(j)->Release();
		this->entries[i].headerVal->GetItem(j)->Release();
	}
	this->entries[i].headerName->Clear();
	this->entries[i].headerVal->Clear();
	Data::ArrayList<Text::String *> names;
	Text::String *name;
	Text::StringBuilderUTF8 sb;
	req->GetHeaderNames(names);
	j = names.GetCount();
	k = 0;
	while (k < j)
	{
		this->entries[i].headerName->Add(names.GetItem(k)->Clone());
		sb.ClearStr();
		name = names.GetItem(k);
		req->GetHeaderC(sb, name->ToCString());
		this->entries[i].headerVal->Add(Text::String::New(sb.ToString(), sb.GetLength()));
		k++;
	}
	mutUsage.EndUse();
}

UOSInt SSWR::AVIRead::AVIRHTTPLog::GetNextIndex()
{
	return this->currEnt;
}

void SSWR::AVIRead::AVIRHTTPLog::Use(NotNullPtr<Sync::MutexUsage> mutUsage)
{
	mutUsage->ReplaceMutex(this->entMut);
}

void SSWR::AVIRead::AVIRHTTPLog::GetEntries(Data::ArrayList<LogEntry*> *logs, Data::ArrayList<UOSInt> *logIndex)
{
	UOSInt i;
	if (this->entries[this->currEnt].reqTime == 0)
	{
		i = 0;
		while (i < this->currEnt)
		{
			logs->Add(&this->entries[i]);
			logIndex->Add(i);
			i++;
		}
	}
	else
	{
		i = this->currEnt;
		while (i < this->logCnt)
		{
			logs->Add(&this->entries[i]);
			logIndex->Add(i);
			i++;
		}
		i = 0;
		while (i < this->currEnt)
		{
			logs->Add(&this->entries[i]);
			logIndex->Add(i);
			i++;
		}
	}
}

SSWR::AVIRead::AVIRHTTPLog::LogEntry *SSWR::AVIRead::AVIRHTTPLog::GetEntry(UOSInt index)
{
	return &this->entries[index];
}

void __stdcall SSWR::AVIRead::AVIRHTTPSvrForm::OnStartClick(void *userObj)
{
	SSWR::AVIRead::AVIRHTTPSvrForm *me = (SSWR::AVIRead::AVIRHTTPSvrForm*)userObj;
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
	me->txtDocDir->GetText(sb);
	Net::SSLEngine *ssl = 0;

	if (me->chkSSL->IsChecked())
	{
		NotNullPtr<Crypto::Cert::X509Cert> sslCert;
		NotNullPtr<Crypto::Cert::X509File> sslKey;
		if (!sslCert.Set(me->sslCert) || !sslKey.Set(me->sslKey))
		{
			UI::MessageDialog::ShowDialog(CSTR("Please select SSL Cert/Key First"), CSTR("HTTP Server"), me);
			return;
		}
		ssl = me->ssl;
		if (!ssl->ServerSetCertsASN1(sslCert, sslKey, me->caCerts))
		{
			UI::MessageDialog::ShowDialog(CSTR("Error in initializing Cert/Key"), CSTR("HTTP Server"), me);
			return;
		}
	}
	if (port > 0 && port < 65535)
	{
		UInt64 cacheSize = 0;
		if (me->chkCacheFile->IsChecked())
		{
			cacheSize = 4096;
		}
		UTF8Char sbuff[128];
		UTF8Char *sptr;
		Data::DateTime dt;
		IO::BuildTime::GetBuildTime(&dt);
		dt.ToUTCTime();
		sptr = dt.ToString(Text::StrConcatC(sbuff, UTF8STRC("AVIRead/")), "yyyyMMddHHmmss");
		NotNullPtr<Net::WebServer::HTTPDirectoryHandler> dirHdlr;
		NEW_CLASSNN(dirHdlr, Net::WebServer::HTTPDirectoryHandler(sb.ToCString(), me->chkAllowBrowse->IsChecked(), cacheSize, true));
		NEW_CLASS(me->svr, Net::WebServer::WebListener(me->core->GetSocketFactory(), ssl, dirHdlr, port, 120, Sync::ThreadUtil::GetThreadCnt(), CSTRP(sbuff, sptr), me->chkAllowProxy->IsChecked(), (Net::WebServer::KeepAlive)(OSInt)me->cboKeepAlive->GetSelectedItem(), false));
		if (me->svr->IsError())
		{
			valid = false;
			SDEL_CLASS(me->svr);
			dirHdlr->Release();
			UI::MessageDialog::ShowDialog(CSTR("Error in listening to port"), CSTR("HTTP Server"), me);
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
				NotNullPtr<UI::ListBoxLogger> logger;
				NEW_CLASSNN(logger, UI::ListBoxLogger(*me, me->lbLog, 500, true));
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
				UI::MessageDialog::ShowDialog(CSTR("Error in starting HTTP Server"), CSTR("HTTP Server"), me);
			}
		}
	}

	if (valid)
	{
		me->txtPort->SetReadOnly(true);
		me->txtDocDir->SetReadOnly(true);
		me->txtLogDir->SetReadOnly(true);
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
		if (me->dirHdlr)
		{
			me->dirHdlr->Release();
			me->dirHdlr = 0;
		}
		SDEL_CLASS(me->log);
		SDEL_CLASS(me->logger);
	}
}

void __stdcall SSWR::AVIRead::AVIRHTTPSvrForm::OnStopClick(void *userObj)
{
	SSWR::AVIRead::AVIRHTTPSvrForm *me = (SSWR::AVIRead::AVIRHTTPSvrForm*)userObj;
	if (me->svr == 0)
	{
		return;
	}
	SDEL_CLASS(me->svr);
	if (me->dirHdlr)
	{
		me->dirHdlr->Release();
		me->dirHdlr = 0;
	}
	SDEL_CLASS(me->log);
	SDEL_CLASS(me->logger);
	me->txtPort->SetReadOnly(false);
	me->txtDocDir->SetReadOnly(false);
	me->txtLogDir->SetReadOnly(false);
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

void __stdcall SSWR::AVIRead::AVIRHTTPSvrForm::OnLogSel(void *userObj)
{
	Text::String *s;
	SSWR::AVIRead::AVIRHTTPSvrForm *me = (SSWR::AVIRead::AVIRHTTPSvrForm*)userObj;
	s = me->lbLog->GetSelectedItemTextNew();
	me->txtLog->SetText(s->ToCString());
	s->Release();
}

void __stdcall SSWR::AVIRead::AVIRHTTPSvrForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRHTTPSvrForm *me = (SSWR::AVIRead::AVIRHTTPSvrForm*)userObj;
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
			sb.AppendTS(Data::Timestamp(log->reqTime, Data::DateTimeUtil::GetLocalTzQhr()));
			sb.AppendC(UTF8STRC(" "));
			sptr = Net::SocketUtil::GetAddrName(sbuff, log->cliAddr, log->cliPort);
			sb.AppendP(sbuff, sptr);

			me->lbAccess->AddItem(sb.ToCString(), (void*)(OSInt)logIndex.GetItem(i));
			i++;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRHTTPSvrForm::OnAccessSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRHTTPSvrForm *me = (SSWR::AVIRead::AVIRHTTPSvrForm*)userObj;
	Text::StringBuilderUTF8 sb;
	Sync::MutexUsage mutUsage;
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	me->reqLog->Use(mutUsage);
	UOSInt i = (UOSInt)me->lbAccess->GetSelectedItem();
	UOSInt j;
	SSWR::AVIRead::AVIRHTTPLog::LogEntry *log;
	log = me->reqLog->GetEntry(i);
	sb.AppendTS(Data::Timestamp(log->reqTime, Data::DateTimeUtil::GetLocalTzQhr()));
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
		sb.Append(log->headerName->GetItem(i));
		sb.AppendC(UTF8STRC("\t"));
		sb.Append(log->headerVal->GetItem(i));
		i++;
	}
	me->txtAccess->SetText(sb.ToCString());
}

void __stdcall SSWR::AVIRead::AVIRHTTPSvrForm::OnSSLCertClicked(void *userObj)
{
	SSWR::AVIRead::AVIRHTTPSvrForm *me = (SSWR::AVIRead::AVIRHTTPSvrForm*)userObj;
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

void SSWR::AVIRead::AVIRHTTPSvrForm::ClearCACerts()
{
	UOSInt i = this->caCerts.GetCount();
	while (i-- > 0)
	{
		Crypto::Cert::X509Cert *cert = this->caCerts.GetItem(i);
		DEL_CLASS(cert);
	}
	this->caCerts.Clear();
}

SSWR::AVIRead::AVIRHTTPSvrForm::AVIRHTTPSvrForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
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

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpControl = this->tcMain->AddTabPage(CSTR("Control"));
	this->tpStatus = this->tcMain->AddTabPage(CSTR("Status"));
	this->tpAccess = this->tcMain->AddTabPage(CSTR("Access"));
	this->tpLog = this->tcMain->AddTabPage(CSTR("Log"));

	NEW_CLASS(this->grpParam, UI::GUIGroupBox(ui, this->tpControl, CSTR("Parameters")));
	this->grpParam->SetRect(0, 0, 620, 320, false);
	this->grpParam->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblPort, UI::GUILabel(ui, this->grpParam, CSTR("Port")));
	this->lblPort->SetRect(8, 8, 100, 23, false);
	NEW_CLASS(this->txtPort, UI::GUITextBox(ui, this->grpParam, CSTR("12345")));
	this->txtPort->SetRect(108, 8, 50, 23, false);
	NEW_CLASS(this->lblDocDir, UI::GUILabel(ui, this->grpParam, CSTR("Doc Path")));
	this->lblDocDir->SetRect(8, 32, 100, 23, false);
	sptr = IO::Path::GetProcessFileName(sbuff);
	i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
	if (i != INVALID_INDEX)
	{
		sbuff[i] = 0;
		sptr = &sbuff[i];
	}
	NEW_CLASS(this->txtDocDir, UI::GUITextBox(ui, this->grpParam, CSTRP(sbuff, sptr)));
	this->txtDocDir->SetRect(108, 32, 500, 23, false);
	NEW_CLASS(this->lblLogDir, UI::GUILabel(ui, this->grpParam, CSTR("Log Path")));
	this->lblLogDir->SetRect(8, 56, 100, 23, false);
	sbuff[i] = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrConcatC(&sbuff[i+1], UTF8STRC("log"));
	NEW_CLASS(this->txtLogDir, UI::GUITextBox(ui, this->grpParam, CSTRP(sbuff, sptr)));
	this->txtLogDir->SetRect(108, 56, 500, 23, false);
	NEW_CLASS(this->lblSSL, UI::GUILabel(ui, this->grpParam, CSTR("SSL")));
	this->lblSSL->SetRect(8, 80, 100, 23, false);
	NEW_CLASS(this->chkSSL, UI::GUICheckBox(ui, this->grpParam, CSTR("Enable"), false));
	this->chkSSL->SetRect(108, 80, 100, 23, false);
	NEW_CLASS(this->btnSSLCert, UI::GUIButton(ui, this->grpParam, CSTR("Cert/Key")));
	this->btnSSLCert->SetRect(208, 80, 75,23, false);
	this->btnSSLCert->HandleButtonClick(OnSSLCertClicked, this);
	NEW_CLASS(this->lblSSLCert, UI::GUILabel(ui, this->grpParam, CSTR("")));
	this->lblSSLCert->SetRect(288, 80, 200, 23, false);
	NEW_CLASS(this->lblAllowBrowse, UI::GUILabel(ui, this->grpParam, CSTR("Directory Browsing")));
	this->lblAllowBrowse->SetRect(8, 104, 100, 23, false);
	NEW_CLASS(this->chkAllowBrowse, UI::GUICheckBox(ui, this->grpParam, CSTR("Allow"), true));
	this->chkAllowBrowse->SetRect(108, 104, 100, 23, false);
	NEW_CLASS(this->lblAllowProxy, UI::GUILabel(ui, this->grpParam, CSTR("Proxy Connection")));
	this->lblAllowProxy->SetRect(8, 128, 100, 23, false);
	NEW_CLASS(this->chkAllowProxy, UI::GUICheckBox(ui, this->grpParam, CSTR("Allow"), false));
	this->chkAllowProxy->SetRect(108, 128, 100, 23, false);
	NEW_CLASS(this->lblCacheFile, UI::GUILabel(ui, this->grpParam, CSTR("Cache File")));
	this->lblCacheFile->SetRect(8, 152, 100, 23, false);
	NEW_CLASS(this->chkCacheFile, UI::GUICheckBox(ui, this->grpParam, CSTR("Enable"), false));
	this->chkCacheFile->SetRect(108, 152, 100, 23, false);
	NEW_CLASS(this->lblSPKPackageFile, UI::GUILabel(ui, this->grpParam, CSTR("Expand SPK Packages")));
	this->lblSPKPackageFile->SetRect(8, 176, 100, 23, false);
	NEW_CLASS(this->chkSPKPackageFile, UI::GUICheckBox(ui, this->grpParam, CSTR("Enable"), false));
	this->chkSPKPackageFile->SetRect(108, 176, 100, 23, false);
	NEW_CLASS(this->lblZIPPackageFile, UI::GUILabel(ui, this->grpParam, CSTR("Expand ZIP Packages")));
	this->lblZIPPackageFile->SetRect(8, 200, 100, 23, false);
	NEW_CLASS(this->chkZIPPackageFile, UI::GUICheckBox(ui, this->grpParam, CSTR("Enable"), false));
	this->chkZIPPackageFile->SetRect(108, 200, 100, 23, false);
	NEW_CLASS(this->lblSkipLog, UI::GUILabel(ui, this->grpParam, CSTR("Skip Logging")));
	this->lblSkipLog->SetRect(8, 224, 100, 23, false);
	NEW_CLASS(this->chkSkipLog, UI::GUICheckBox(ui, this->grpParam, CSTR("Enable"), true));
	this->chkSkipLog->SetRect(108, 224, 100, 23, false);
	NEW_CLASS(this->lblKeepAlive, UI::GUILabel(ui, this->grpParam, CSTR("Keep Alive")));
	this->lblKeepAlive->SetRect(8, 248, 100, 23, false);
	NEW_CLASS(this->cboKeepAlive, UI::GUIComboBox(ui, this->grpParam, false));
	this->cboKeepAlive->SetRect(108, 248, 100, 23, false);
	this->cboKeepAlive->AddItem(CSTR("Always"), (void*)(OSInt)Net::WebServer::KeepAlive::Always);
	this->cboKeepAlive->AddItem(CSTR("Default"), (void*)(OSInt)Net::WebServer::KeepAlive::Default);
	this->cboKeepAlive->AddItem(CSTR("No"), (void*)(OSInt)Net::WebServer::KeepAlive::No);
	this->cboKeepAlive->SetSelectedIndex(1);
	NEW_CLASS(this->lblCrossOrigin, UI::GUILabel(ui, this->grpParam, CSTR("Cross Origin")));
	this->lblCrossOrigin->SetRect(8, 272, 100, 23, false);
	NEW_CLASS(this->chkCrossOrigin, UI::GUICheckBox(ui, this->grpParam, CSTR("Allow"), false));
	this->chkCrossOrigin->SetRect(108, 272, 100, 23, false);
	NEW_CLASS(this->lblDownloadCnt, UI::GUILabel(ui, this->grpParam, CSTR("Download Count")));
	this->lblDownloadCnt->SetRect(8, 320, 100, 23, false);
	NEW_CLASS(this->chkDownloadCnt, UI::GUICheckBox(ui, this->grpParam, CSTR("Enable"), false));
	this->chkDownloadCnt->SetRect(108, 320, 100, 23, false);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this->tpControl, CSTR("Start")));
	this->btnStart->SetRect(200, 356, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClick, this);
	NEW_CLASS(this->btnStop, UI::GUIButton(ui, this->tpControl, CSTR("Stop")));
	this->btnStop->SetRect(300, 356, 75, 23, false);
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
	NEW_CLASS(this->hspAccess, UI::GUIHSplitter(ui, this->tpAccess, 3, false));
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

SSWR::AVIRead::AVIRHTTPSvrForm::~AVIRHTTPSvrForm()
{
	SDEL_CLASS(this->svr);
	if (this->dirHdlr)
	{
		this->dirHdlr->Release();
		this->dirHdlr = 0;
	}
	SDEL_CLASS(this->log);
	SDEL_CLASS(this->logger);
	SDEL_CLASS(this->reqLog);
	SDEL_CLASS(this->ssl);
	SDEL_CLASS(this->sslCert);
	SDEL_CLASS(this->sslKey);
	this->ClearCACerts();
}

void SSWR::AVIRead::AVIRHTTPSvrForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRHTTPSvrForm::SetPort(Int32 port)
{
	UTF8Char sbuff[16];
	UTF8Char *sptr;
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
