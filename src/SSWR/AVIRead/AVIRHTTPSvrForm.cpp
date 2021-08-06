#include "Stdafx.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIRHTTPSvrForm.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/MessageDialog.h"

SSWR::AVIRead::AVIRHTTPLog::AVIRHTTPLog(UOSInt logCnt)
{
	this->logCnt = logCnt;
	this->entries = MemAlloc(LogEntry, this->logCnt);
	this->currEnt = 0;
	NEW_CLASS(this->entMut, Sync::Mutex());
	UOSInt i = this->logCnt;
	while (i-- > 0)
	{
		this->entries[i].reqTime = 0;
		NEW_CLASS(this->entries[i].headerName, Data::ArrayListStrUTF8());
		NEW_CLASS(this->entries[i].headerVal, Data::ArrayList<const UTF8Char *>());
		this->entries[i].reqURI = 0;
		this->entries[i].cliAddr.addrType = Net::SocketUtil::AT_UNKNOWN;
		this->entries[i].cliPort = 0;
	}
}

SSWR::AVIRead::AVIRHTTPLog::~AVIRHTTPLog()
{
	UOSInt j;
	UOSInt i = this->logCnt;
	while (i-- > 0)
	{
		SDEL_TEXT(this->entries[i].reqURI);
		j = this->entries[i].headerName->GetCount();
		while (j-- > 0)
		{
			Text::StrDelNew(this->entries[i].headerName->GetItem(j));
			Text::StrDelNew(this->entries[i].headerVal->GetItem(j));
		}
		DEL_CLASS(this->entries[i].headerName);
		DEL_CLASS(this->entries[i].headerVal);
	}
	MemFree(this->entries);
	DEL_CLASS(this->entMut);
}

void SSWR::AVIRead::AVIRHTTPLog::LogRequest(Net::WebServer::IWebRequest *req)
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
	SDEL_TEXT(this->entries[i].reqURI);
	this->entries[i].reqURI = Text::StrCopyNew(req->GetRequestURI());
	this->entries[i].cliAddr = *req->GetClientAddr();
	this->entries[i].cliPort = req->GetClientPort();
	this->entries[i].reqTime = dt.ToTicks();
	j = this->entries[i].headerName->GetCount();
	while (j-- > 0)
	{
		Text::StrDelNew(this->entries[i].headerName->GetItem(j));
		Text::StrDelNew(this->entries[i].headerVal->GetItem(j));
	}
	this->entries[i].headerName->Clear();
	this->entries[i].headerVal->Clear();
	Data::ArrayList<const UTF8Char *> names;
	Text::StringBuilderUTF8 sb;
	req->GetHeaderNames(&names);
	j = names.GetCount();
	k = 0;
	while (k < j)
	{
		this->entries[i].headerName->Add(Text::StrCopyNew(names.GetItem(k)));
		sb.ClearStr();
		req->GetHeader(&sb, names.GetItem(k));
		this->entries[i].headerVal->Add(Text::StrCopyNew(sb.ToString()));
		k++;
	}
	mutUsage.EndUse();
}

UOSInt SSWR::AVIRead::AVIRHTTPLog::GetNextIndex()
{
	return this->currEnt;
}

void SSWR::AVIRead::AVIRHTTPLog::Use(Sync::MutexUsage *mutUsage)
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
	Text::StringBuilderUTF8 *sb;
	NEW_CLASS(sb, Text::StringBuilderUTF8());
	me->txtPort->GetText(sb);
	Text::StrToUInt16S(sb->ToString(), &port, 0);
	sb->ClearStr();
	me->txtDocDir->GetText(sb);
	Net::SSLEngine *ssl = 0;

	if (me->chkSSL->IsChecked())
	{
		ssl = me->core->GetSSLEngine();
/*		if (!ssl->SetServerCerts((const UTF8Char*)"C:\\Progs\\SSWR\\AVIRead2017\\test.crt", (const UTF8Char*)"C:\\Progs\\SSWR\\AVIRead2017\\test.key"))
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in setting certs", (const UTF8Char*)"HTTP Server", me);
			return;
		}*/
		
		Text::StringBuilderUTF8 sbCountry;
		Text::StringBuilderUTF8 sbCompany;
		Text::StringBuilderUTF8 sbCommonName;
		me->txtSSLCountry->GetText(&sbCountry);
		me->txtSSLCompany->GetText(&sbCompany);
		me->txtSSLCommonName->GetText(&sbCommonName);
		if (sbCountry.GetLength() != 2)
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Country must be 2 characters", (const UTF8Char*)"HTTP Server", me);
			return;
		}
		if (sbCompany.GetLength() == 0)
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter company", (const UTF8Char*)"HTTP Server", me);
			return;
		}
		if (sbCommonName.GetLength() == 0)
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter common name", (const UTF8Char*)"HTTP Server", me);
			return;
		}
		Crypto::X509File *certASN1;
		Crypto::X509File *keyASN1;
		if (ssl->GenerateCert(sbCountry.ToString(), sbCompany.ToString(), sbCommonName.ToString(), &certASN1, &keyASN1))
		{
			ssl->SetServerCertsASN1(certASN1, keyASN1);
			SDEL_CLASS(certASN1);
			SDEL_CLASS(keyASN1);
		}
		else
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in generating certs", (const UTF8Char*)"HTTP Server", me);
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
		NEW_CLASS(me->dirHdlr, Net::WebServer::HTTPDirectoryHandler(sb->ToString(), me->chkAllowBrowse->IsChecked(), cacheSize, true));
		NEW_CLASS(me->svr, Net::WebServer::WebListener(me->core->GetSocketFactory(), ssl, me->dirHdlr, port, 120, Sync::Thread::GetThreadCnt(), (const UTF8Char*)"sswr", me->chkAllowProxy->IsChecked(), me->chkAllowKA->IsChecked()));
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
			sb->Append((const UTF8Char*)"Acccess");

			if (!me->chkSkipLog->IsChecked())
			{
				NEW_CLASS(me->log, IO::LogTool());
				me->log->AddFileLog(sb->ToString(), IO::ILogHandler::LOG_TYPE_PER_DAY, IO::ILogHandler::LOG_GROUP_TYPE_PER_MONTH, IO::ILogHandler::LOG_LEVEL_RAW, "yyyy-MM-dd HH:mm:ss.fff", false);
				me->svr->SetAccessLog(me->log, IO::ILogHandler::LOG_LEVEL_RAW);
				me->svr->SetRequestLog(me->reqLog);
				NEW_CLASS(me->logger, UI::ListBoxLogger(me, me->lbLog, 500, true));
				me->log->AddLogHandler(me->logger, IO::ILogHandler::LOG_LEVEL_RAW);
			}
			if (me->chkPackageFile->IsChecked())
			{
				me->dirHdlr->ExpandPackageFiles(me->core->GetParserList());
			}
			if (me->chkCrossOrigin->IsChecked())
			{
				me->dirHdlr->SetAllowOrigin((const UTF8Char*)"*");
			}
			if (me->chkDownloadCnt->IsChecked())
			{
				me->dirHdlr->EnableStats();
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
		me->chkPackageFile->SetEnabled(false);
		me->chkSkipLog->SetEnabled(false);
		me->chkAllowKA->SetEnabled(false);
		me->chkCrossOrigin->SetEnabled(false);
		me->chkDownloadCnt->SetEnabled(false);
		me->chkSSL->SetEnabled(false);
		me->txtSSLCountry->SetReadOnly(true);
		me->txtSSLCompany->SetReadOnly(true);
		me->txtSSLCommonName->SetReadOnly(true);
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
	DEL_CLASS(sb);
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
	me->chkPackageFile->SetEnabled(true);
	me->chkSkipLog->SetEnabled(true);
	me->chkAllowKA->SetEnabled(true);
	me->chkCrossOrigin->SetEnabled(true);
	me->chkDownloadCnt->SetEnabled(true);
	me->chkSSL->SetEnabled(true);
	me->txtSSLCountry->SetReadOnly(false);
	me->txtSSLCompany->SetReadOnly(false);
	me->txtSSLCommonName->SetReadOnly(false);
}

void __stdcall SSWR::AVIRead::AVIRHTTPSvrForm::OnLogSel(void *userObj)
{
	const UTF8Char *t;
	SSWR::AVIRead::AVIRHTTPSvrForm *me = (SSWR::AVIRead::AVIRHTTPSvrForm*)userObj;
	t = me->lbLog->GetSelectedItemTextNew();
	me->txtLog->SetText(t);
	me->lbLog->DelTextNew(t);
}

void __stdcall SSWR::AVIRead::AVIRHTTPSvrForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRHTTPSvrForm *me = (SSWR::AVIRead::AVIRHTTPSvrForm*)userObj;
	UOSInt i;
	UOSInt j;
	UTF8Char sbuff[128];
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
			sb.Append((const UTF8Char*)" ");
			Net::SocketUtil::GetAddrName(sbuff, &log->cliAddr, log->cliPort);
			sb.Append(sbuff);

			me->lbAccess->AddItem(sb.ToString(), (void*)(OSInt)logIndex.GetItem(i));
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
	me->reqLog->Use(&mutUsage);
	UOSInt i = (UOSInt)me->lbAccess->GetSelectedItem();
	UOSInt j;
	SSWR::AVIRead::AVIRHTTPLog::LogEntry *log;
	log = me->reqLog->GetEntry(i);
	Data::DateTime dt;
	dt.SetTicks(log->reqTime);
	dt.ToLocalTime();
	sb.AppendDate(&dt);
	sb.Append((const UTF8Char*)" ");
	Net::SocketUtil::GetAddrName(sbuff, &log->cliAddr, log->cliPort);
	sb.Append(sbuff);
	sb.Append((const UTF8Char*)"\r\n");
	sb.Append(log->reqURI);
	sb.Append((const UTF8Char*)"\r\n\r\nHeaders:");
	i = 0;
	j = log->headerName->GetCount();
	while (i < j)
	{
		sb.Append((const UTF8Char*)"\r\n");
		sb.Append(log->headerName->GetItem(i));
		sb.Append((const UTF8Char*)"\t");
		sb.Append(log->headerVal->GetItem(i));
		i++;
	}
	me->txtAccess->SetText(sb.ToString());
}

SSWR::AVIRead::AVIRHTTPSvrForm::AVIRHTTPSvrForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	UTF8Char sbuff[512];
	UOSInt i;
	this->core = core;
	this->SetText((const UTF8Char*)"HTTP Server");
	this->SetFont(0, 8.25, false);
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
	this->tpControl = this->tcMain->AddTabPage((const UTF8Char*)"Control");
	this->tpStatus = this->tcMain->AddTabPage((const UTF8Char*)"Status");
	this->tpAccess = this->tcMain->AddTabPage((const UTF8Char*)"Access");
	this->tpLog = this->tcMain->AddTabPage((const UTF8Char*)"Log");

	NEW_CLASS(this->grpParam, UI::GUIGroupBox(ui, this->tpControl, (const UTF8Char*)"Parameters"));
	this->grpParam->SetRect(0, 0, 620, 296, false);
	this->grpParam->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblPort, UI::GUILabel(ui, this->grpParam, (const UTF8Char*)"Port"));
	this->lblPort->SetRect(8, 8, 100, 23, false);
	NEW_CLASS(this->txtPort, UI::GUITextBox(ui, this->grpParam, (const UTF8Char*)"12345"));
	this->txtPort->SetRect(108, 8, 50, 23, false);
	NEW_CLASS(this->lblDocDir, UI::GUILabel(ui, this->grpParam, (const UTF8Char*)"Doc Path"));
	this->lblDocDir->SetRect(8, 32, 100, 23, false);
	IO::Path::GetProcessFileName(sbuff);
	i = Text::StrLastIndexOf(sbuff, IO::Path::PATH_SEPERATOR);
	if (i != INVALID_INDEX)
	{
		sbuff[i] = 0;
	}
	NEW_CLASS(this->txtDocDir, UI::GUITextBox(ui, this->grpParam, sbuff));
	this->txtDocDir->SetRect(108, 32, 500, 23, false);
	NEW_CLASS(this->lblLogDir, UI::GUILabel(ui, this->grpParam, (const UTF8Char*)"Log Path"));
	this->lblLogDir->SetRect(8, 56, 100, 23, false);
	sbuff[i] = IO::Path::PATH_SEPERATOR;
	Text::StrConcat(&sbuff[i+1], (const UTF8Char*)"log");
	NEW_CLASS(this->txtLogDir, UI::GUITextBox(ui, this->grpParam, sbuff));
	this->txtLogDir->SetRect(108, 56, 500, 23, false);
	NEW_CLASS(this->lblAllowBrowse, UI::GUILabel(ui, this->grpParam, (const UTF8Char*)"Directory Browsing"));
	this->lblAllowBrowse->SetRect(8, 80, 100, 23, false);
	NEW_CLASS(this->chkAllowBrowse, UI::GUICheckBox(ui, this->grpParam, (const UTF8Char*)"Allow", true));
	this->chkAllowBrowse->SetRect(108, 80, 100, 23, false);
	NEW_CLASS(this->lblAllowProxy, UI::GUILabel(ui, this->grpParam, (const UTF8Char*)"Proxy Connection"));
	this->lblAllowProxy->SetRect(8, 104, 100, 23, false);
	NEW_CLASS(this->chkAllowProxy, UI::GUICheckBox(ui, this->grpParam, (const UTF8Char*)"Allow", true));
	this->chkAllowProxy->SetRect(108, 104, 100, 23, false);
	NEW_CLASS(this->lblCacheFile, UI::GUILabel(ui, this->grpParam, (const UTF8Char*)"Cache File"));
	this->lblCacheFile->SetRect(8, 128, 100, 23, false);
	NEW_CLASS(this->chkCacheFile, UI::GUICheckBox(ui, this->grpParam, (const UTF8Char*)"Enable", true));
	this->chkCacheFile->SetRect(108, 128, 100, 23, false);
	NEW_CLASS(this->lblPackageFile, UI::GUILabel(ui, this->grpParam, (const UTF8Char*)"Expand Packages"));
	this->lblPackageFile->SetRect(8, 152, 100, 23, false);
	NEW_CLASS(this->chkPackageFile, UI::GUICheckBox(ui, this->grpParam, (const UTF8Char*)"Enable", false));
	this->chkPackageFile->SetRect(108, 152, 100, 23, false);
	NEW_CLASS(this->lblSkipLog, UI::GUILabel(ui, this->grpParam, (const UTF8Char*)"Skip Logging"));
	this->lblSkipLog->SetRect(8, 176, 100, 23, false);
	NEW_CLASS(this->chkSkipLog, UI::GUICheckBox(ui, this->grpParam, (const UTF8Char*)"Enable", false));
	this->chkSkipLog->SetRect(108, 176, 100, 23, false);
	NEW_CLASS(this->lblAllowKA, UI::GUILabel(ui, this->grpParam, (const UTF8Char*)"Allow KA"));
	this->lblAllowKA->SetRect(8, 200, 100, 23, false);
	NEW_CLASS(this->chkAllowKA, UI::GUICheckBox(ui, this->grpParam, (const UTF8Char*)"Enable", true));
	this->chkAllowKA->SetRect(108, 200, 100, 23, false);
	NEW_CLASS(this->lblCrossOrigin, UI::GUILabel(ui, this->grpParam, (const UTF8Char*)"Cross Origin"));
	this->lblCrossOrigin->SetRect(8, 224, 100, 23, false);
	NEW_CLASS(this->chkCrossOrigin, UI::GUICheckBox(ui, this->grpParam, (const UTF8Char*)"Allow", false));
	this->chkCrossOrigin->SetRect(108, 224, 100, 23, false);
	NEW_CLASS(this->lblDownloadCnt, UI::GUILabel(ui, this->grpParam, (const UTF8Char*)"Download Count"));
	this->lblDownloadCnt->SetRect(8, 248, 100, 23, false);
	NEW_CLASS(this->chkDownloadCnt, UI::GUICheckBox(ui, this->grpParam, (const UTF8Char*)"Enable", false));
	this->chkDownloadCnt->SetRect(108, 248, 100, 23, false);

	NEW_CLASS(this->grpSSL, UI::GUIGroupBox(ui, this->grpParam, (const UTF8Char*)"SSL"));
	this->grpSSL->SetRect(208, 80, 224, 120, false);
	NEW_CLASS(this->chkSSL, UI::GUICheckBox(ui, this->grpSSL, (const UTF8Char*)"Enable", false));
	this->chkSSL->SetRect(104, 4, 100, 23, false);
	NEW_CLASS(this->lblSSLCountry, UI::GUILabel(ui, this->grpSSL, (const UTF8Char*)"Country"));
	this->lblSSLCountry->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtSSLCountry, UI::GUITextBox(ui, this->grpSSL, (const UTF8Char*)"HK"));
	this->txtSSLCountry->SetRect(104, 28, 100, 23, false);
	NEW_CLASS(this->lblSSLCompany, UI::GUILabel(ui, this->grpSSL, (const UTF8Char*)"Company"));
	this->lblSSLCompany->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtSSLCompany, UI::GUITextBox(ui, this->grpSSL, (const UTF8Char*)"Test"));
	this->txtSSLCompany->SetRect(104, 52, 100, 23, false);
	NEW_CLASS(this->lblSSLCommonName, UI::GUILabel(ui, this->grpSSL, (const UTF8Char*)"Common Name"));
	this->lblSSLCommonName->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtSSLCommonName, UI::GUITextBox(ui, this->grpSSL, (const UTF8Char*)"localhost"));
	this->txtSSLCommonName->SetRect(104, 76, 100, 23, false);

	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this->tpControl, (const UTF8Char*)"Start"));
	this->btnStart->SetRect(200, 308, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClick, this);
	NEW_CLASS(this->btnStop, UI::GUIButton(ui, this->tpControl, (const UTF8Char*)"Stop"));
	this->btnStop->SetRect(300, 308, 75, 23, false);
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
}

void SSWR::AVIRead::AVIRHTTPSvrForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRHTTPSvrForm::SetPort(Int32 port)
{
	UTF8Char sbuff[16];
	Text::StrInt32(sbuff, port);
	this->txtPort->SetText(sbuff);
}

void SSWR::AVIRead::AVIRHTTPSvrForm::SetDocPath(const UTF8Char *docPath)
{
	this->txtDocDir->SetText(docPath);
}

void SSWR::AVIRead::AVIRHTTPSvrForm::SetLogPath(const UTF8Char *logPath)
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
