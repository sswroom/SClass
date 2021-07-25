#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Manage/HiResClock.h"
#include "Net/HTTPOSClient.h"
#include "SSWR/AVIRead/AVIRHTTPDownloaderForm.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRHTTPDownloaderForm::OnRequestClicked(void *userObj)
{
	SSWR::AVIRead::AVIRHTTPDownloaderForm *me = (SSWR::AVIRead::AVIRHTTPDownloaderForm*)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtDownloadDir->GetText(&sb);
	if (IO::Path::GetPathType(sb.ToString()) != IO::Path::PT_DIRECTORY)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char *)"Please enter valid download path", (const UTF8Char *)"Request", me);
		return;
	}
	SDEL_TEXT(me->downPath);
	me->downPath = Text::StrCopyNew(sb.ToString());

	sb.ClearStr();
	me->txtHeaders->GetText(&sb);
	SDEL_TEXT(me->reqHeader);
	if (sb.GetLength() > 0)
	{
		me->reqHeader = Text::StrCopyNew(sb.ToString());
	}

	sb.ClearStr();
	me->txtURL->GetText(&sb);
	if (!sb.StartsWith((const UTF8Char*)"http://") && !sb.StartsWith((const UTF8Char*)"https://") )
	{
		UI::MessageDialog::ShowDialog((const UTF8Char *)"Please enter valid http URL", (const UTF8Char *)"Request", me);
		return;
	}


	me->reqURL = Text::StrCopyNew(sb.ToString());
	me->threadEvt->Set();
	while (me->threadRunning && me->reqURL)
	{
		Sync::Thread::Sleep(1);
	}
}

UInt32 __stdcall SSWR::AVIRead::AVIRHTTPDownloaderForm::ProcessThread(void *userObj)
{
	SSWR::AVIRead::AVIRHTTPDownloaderForm *me = (SSWR::AVIRead::AVIRHTTPDownloaderForm*)userObj;
	const UTF8Char *currURL;
	const UTF8Char *currPath;
	const UTF8Char *currHeader;
	IO::FileStream *fs;
	UInt8 buff[4096];
	UTF8Char *sarr[2];
	UTF8Char *sarr2[2];
	UTF8Char *sbuff;
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	me->threadRunning = true;
	sbuff = MemAlloc(UTF8Char, 65536);
	while (!me->threadToStop)
	{
		if (me->reqURL && !me->respChanged)
		{
			currURL = me->reqURL;
			if (me->reqHeader)
			{
				currHeader = Text::StrCopyNew(me->reqHeader);
			}
			else
			{
				currHeader = 0;
			}
			currPath = Text::StrCopyNew(me->downPath);
			me->reqURL = 0;

			sptr = Text::StrConcat(sbuff, currPath);
			if (sptr[-1] != IO::Path::PATH_SEPERATOR)
			{
				*sptr++ = IO::Path::PATH_SEPERATOR;
			}
			i = Text::StrLastIndexOf(currURL, '/');
			Text::StrConcat(sptr, &currURL[i + 1]);
			i = Text::StrIndexOf(sptr, '?');
			if (i == 0)
			{
				Text::StrConcat(sptr, (const UTF8Char*)"download.dat");
			}
			else if (i != INVALID_INDEX)
			{
				sptr[i] = 0;
			}
			Net::HTTPClient *cli;
			NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NO_WRITE_BUFFER));
			cli = Net::HTTPClient::CreateClient(me->core->GetSocketFactory(), 0, false, Text::StrStartsWith(currURL, (const UTF8Char*)"https://"));
			cli->Connect(currURL, "GET", &me->respTimeDNS, &me->respTimeConn, false);
			if (currHeader)
			{
				Text::StrConcat(sbuff, currHeader);
				sarr[1] = sbuff;
				while (true)
				{
					i = Text::StrSplit(sarr, 2, sarr[1], '\r');
					j = Text::StrSplitTrim(sarr2, 2, sarr[0], ':');
					if (j == 2)
					{
						if (Text::StrCompareICase(sarr2[0], (const UTF8Char*)"HOST") == 0)
						{
						}
						else
						{
							cli->AddHeader(sarr2[0], sarr2[1]);
						}
					}
					if (i == 1)
						break;
					while (sarr[1][0] == '\n' ||sarr[1][0] == '\r')
					{
						sarr[1] = &sarr[1][1];
					}
				}
			}

			cli->EndRequest(&me->respTimeReq, &me->respTimeResp);
			me->ClearHeaders();
			i = 0;
			j = cli->GetRespHeaderCnt();
			while (i < j)
			{
				cli->GetRespHeader(i, (UTF8Char*)buff);
				me->respHeaders->Add(Text::StrCopyNew((UTF8Char*)buff));
				i++;
			}
			me->respHdrChanged = true;

			while ((i = cli->Read(buff, 4096)) > 0)
			{
				fs->Write(buff, i);
				me->currSize += i;
			}
			me->respTimeTotal = cli->GetTotalTime();
			me->respSvrAddr = *cli->GetSvrAddr();

			DEL_CLASS(cli);
			DEL_CLASS(fs);
			me->respChanged = true;

			Text::StrDelNew(currURL);
			Text::StrDelNew(currPath);
			SDEL_TEXT(currHeader);
		}
	}
	MemFree(sbuff);
	SDEL_TEXT(me->reqURL);
	me->threadToStop = false;
	me->threadRunning = false;
	return 0;
}

void __stdcall SSWR::AVIRead::AVIRHTTPDownloaderForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRHTTPDownloaderForm *me = (SSWR::AVIRead::AVIRHTTPDownloaderForm*)userObj;
	UTF8Char sbuff[32];
	UOSInt i;
	UOSInt j;
	Text::StringBuilderUTF8 sb;
	if (me->respHdrChanged)
	{
		me->lvHeaders->ClearItems();
		i = 0;
		j = me->respHeaders->GetCount();
		while (i < j)
		{
			sb.ClearStr();
			sb.Append(me->respHeaders->GetItem(i));
			me->lvHeaders->AddItem(sb.ToString(), 0);
			i++;
		}
		me->respHdrChanged = false;
	}

	if (me->respChanged)
	{
		Net::SocketUtil::GetAddrName(sbuff, &me->respSvrAddr);
		me->txtSvrIP->SetText(sbuff);
		if (me->respTimeDNS == -1)
		{
			me->txtTimeDNS->SetText((const UTF8Char*)"-1");
		}
		else
		{
			Text::StrDoubleFmt(sbuff, me->respTimeDNS, "0.0000000000");
			me->txtTimeDNS->SetText(sbuff);
		}
		if (me->respTimeConn == -1)
		{
			me->txtTimeConn->SetText((const UTF8Char*)"-1");
		}
		else
		{
			Text::StrDoubleFmt(sbuff, me->respTimeConn - me->respTimeDNS, "0.0000000000");
			me->txtTimeConn->SetText(sbuff);
		}
		if (me->respTimeReq == -1)
		{
			me->txtTimeSendHdr->SetText((const UTF8Char*)"-1");
		}
		else
		{
			Text::StrDoubleFmt(sbuff, me->respTimeReq - me->respTimeConn, "0.0000000000");
			me->txtTimeSendHdr->SetText(sbuff);
		}
		if (me->respTimeResp == -1)
		{
			me->txtTimeResp->SetText((const UTF8Char*)"-1");
		}
		else
		{
			Text::StrDoubleFmt(sbuff, me->respTimeResp - me->respTimeReq, "0.0000000000");
			me->txtTimeResp->SetText(sbuff);
		}
		if (me->respTimeTotal == -1)
		{
			me->txtTimeTotal->SetText((const UTF8Char*)"-1");
		}
		else
		{
			Text::StrDoubleFmt(sbuff, me->respTimeTotal - me->respTimeResp, "0.0000000000");
			me->txtTimeTotal->SetText(sbuff);
		}

		me->respChanged = false;
	}

	Data::DateTime t;
	UInt64 currSize = me->currSize;
	t.SetCurrTimeUTC();
	Text::StrUInt64(sbuff, currSize);
	me->txtTotalSize->SetText(sbuff);
	Text::StrDouble(sbuff, (Double)(currSize - me->lastSize) / ((Double)(t.ToTicks() - me->lastT) * 0.001));
	me->txtCurrSpeed->SetText(sbuff);
	me->lastT = t.ToTicks();
	me->lastSize = currSize;
}

void SSWR::AVIRead::AVIRHTTPDownloaderForm::ClearHeaders()
{
	UOSInt i;
	i = this->respHeaders->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->respHeaders->RemoveAt(i));
	}
}

SSWR::AVIRead::AVIRHTTPDownloaderForm::AVIRHTTPDownloaderForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"HTTP Downloader");

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->sockf = core->GetSocketFactory();
	this->respChanged = false;
	this->respHdrChanged = false;
	this->threadRunning = false;
	this->threadToStop = false;
	this->reqURL = 0;
	this->reqHeader = 0;
	this->downPath = 0;
	NEW_CLASS(this->threadEvt, Sync::Event(true, (const UTF8Char*)"SSWR.AVIRead.AVIRHTTPDownloaderForm.threadEvt"));
	NEW_CLASS(this->respHeaders, Data::ArrayList<const UTF8Char *>());

	Data::DateTime t;
	t.SetCurrTimeUTC();
	this->lastT = t.ToTicks();
	this->lastSize = 0;
	this->currSize = 0;

	NEW_CLASS(this->pnlRequest, UI::GUIPanel(ui, this));
	this->pnlRequest->SetRect(0, 0, 100, 151, false);
	this->pnlRequest->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblURL, UI::GUILabel(ui, this->pnlRequest, (const UTF8Char*)"URL"));
	this->lblURL->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtURL, UI::GUITextBox(ui, this->pnlRequest, (const UTF8Char*)"http://"));
	this->txtURL->SetRect(104, 4, 400, 23, false);
	NEW_CLASS(this->lblDownloadDir, UI::GUILabel(ui, this->pnlRequest, (const UTF8Char*)"Down Path"));
	this->lblDownloadDir->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtDownloadDir, UI::GUITextBox(ui, this->pnlRequest, (const UTF8Char*)""));
	this->txtDownloadDir->SetRect(104, 28, 400, 23, false);
	NEW_CLASS(this->lblHeaders, UI::GUILabel(ui, this->pnlRequest, (const UTF8Char*)"Headers\r\n(Except Host)"));
	this->lblHeaders->SetRect(4, 52, 100, 47, false);
	NEW_CLASS(this->txtHeaders, UI::GUITextBox(ui, this->pnlRequest, (const UTF8Char*)"", true));
	this->txtHeaders->SetRect(104, 52, 400, 71, false);
	NEW_CLASS(this->btnRequest, UI::GUIButton(ui, this->pnlRequest, (const UTF8Char*)"Request"));
	this->btnRequest->SetRect(104, 128, 75, 23, false);
	this->btnRequest->HandleButtonClick(OnRequestClicked, this);
	NEW_CLASS(this->grpStatus, UI::GUIGroupBox(ui, this, (const UTF8Char*)"Status"));
	this->grpStatus->SetRect(0, 0, 100, 67, false);
	this->grpStatus->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblCurrSpeed, UI::GUILabel(ui, this->grpStatus, (const UTF8Char*)"Curr Speed"));
	this->lblCurrSpeed->SetRect(4, 0, 100, 23, false);
	NEW_CLASS(this->txtCurrSpeed, UI::GUITextBox(ui, this->grpStatus, (const UTF8Char*)""));
	this->txtCurrSpeed->SetRect(104, 0, 200, 23, false);
	this->txtCurrSpeed->SetReadOnly(true);
	NEW_CLASS(this->lblTotalSize, UI::GUILabel(ui, this->grpStatus, (const UTF8Char*)"Total Size"));
	this->lblTotalSize->SetRect(4, 24, 100, 23, false);
	NEW_CLASS(this->txtTotalSize, UI::GUITextBox(ui, this->grpStatus, (const UTF8Char*)""));
	this->txtTotalSize->SetRect(104, 24, 200, 23, false);
	this->txtTotalSize->SetReadOnly(true);
	NEW_CLASS(this->grpResponse, UI::GUIGroupBox(ui, this, (const UTF8Char*)"Response"));
	this->grpResponse->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->pnlResponse, UI::GUIPanel(ui, this->grpResponse));
	this->pnlResponse->SetRect(0, 0, 100, 151, false);
	this->pnlResponse->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblSvrIP, UI::GUILabel(ui, this->pnlResponse, (const UTF8Char*)"Server IP"));
	this->lblSvrIP->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtSvrIP, UI::GUITextBox(ui, this->pnlResponse, (const UTF8Char*)""));
	this->txtSvrIP->SetRect(104, 4, 150, 23, false);
	this->txtSvrIP->SetReadOnly(true);
	NEW_CLASS(this->lblTimeDNS, UI::GUILabel(ui, this->pnlResponse, (const UTF8Char*)"DNS Time"));
	this->lblTimeDNS->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtTimeDNS, UI::GUITextBox(ui, this->pnlResponse, (const UTF8Char*)""));
	this->txtTimeDNS->SetRect(104, 28, 150, 23, false);
	this->txtTimeDNS->SetReadOnly(true);
	NEW_CLASS(this->lblTimeConn, UI::GUILabel(ui, this->pnlResponse, (const UTF8Char*)"Conn Time"));
	this->lblTimeConn->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtTimeConn, UI::GUITextBox(ui, this->pnlResponse, (const UTF8Char*)""));
	this->txtTimeConn->SetRect(104, 52, 150, 23, false);
	this->txtTimeConn->SetReadOnly(true);
	NEW_CLASS(this->lblTimeSendHdr, UI::GUILabel(ui, this->pnlResponse, (const UTF8Char*)"Request Time"));
	this->lblTimeSendHdr->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtTimeSendHdr, UI::GUITextBox(ui, this->pnlResponse, (const UTF8Char*)""));
	this->txtTimeSendHdr->SetRect(104, 76, 150, 23, false);
	this->txtTimeSendHdr->SetReadOnly(true);
	NEW_CLASS(this->lblTimeResp, UI::GUILabel(ui, this->pnlResponse, (const UTF8Char*)"Response Time"));
	this->lblTimeResp->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtTimeResp, UI::GUITextBox(ui, this->pnlResponse, (const UTF8Char*)""));
	this->txtTimeResp->SetRect(104, 100, 150, 23, false);
	this->txtTimeResp->SetReadOnly(true);
	NEW_CLASS(this->lblTimeTotal, UI::GUILabel(ui, this->pnlResponse, (const UTF8Char*)"Download Time"));
	this->lblTimeTotal->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->txtTimeTotal, UI::GUITextBox(ui, this->pnlResponse, (const UTF8Char*)""));
	this->txtTimeTotal->SetRect(104, 124, 150, 23, false);
	this->txtTimeTotal->SetReadOnly(true);
	NEW_CLASS(this->lvHeaders, UI::GUIListView(ui, this->grpResponse, UI::GUIListView::LVSTYLE_TABLE, 1));
	this->lvHeaders->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvHeaders->SetShowGrid(true);
	this->lvHeaders->SetFullRowSelect(true);
	this->lvHeaders->AddColumn((const UTF8Char*)"Header", 1000);
	
	UTF8Char sbuff[512];
	UOSInt i;
	IO::Path::GetProcessFileName(sbuff);
	i = Text::StrLastIndexOf(sbuff, IO::Path::PATH_SEPERATOR);
	if (i != INVALID_INDEX)
		sbuff[i] = 0;
	this->txtDownloadDir->SetText(sbuff);

	this->SetDefaultButton(this->btnRequest);
	this->txtURL->Focus();
	Sync::Thread::Create(ProcessThread, this, 0);
	this->AddTimer(100, OnTimerTick, this);
}

SSWR::AVIRead::AVIRHTTPDownloaderForm::~AVIRHTTPDownloaderForm()
{
	this->threadToStop = true;
	this->threadEvt->Set();
	while (this->threadRunning)
	{
		Sync::Thread::Sleep(1);
	}
	DEL_CLASS(this->threadEvt);
	this->ClearHeaders();
	DEL_CLASS(this->respHeaders);
	SDEL_TEXT(this->downPath);
	SDEL_TEXT(this->reqHeader);
}

void SSWR::AVIRead::AVIRHTTPDownloaderForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
