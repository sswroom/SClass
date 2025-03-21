#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Manage/HiResClock.h"
#include "Net/HTTPOSClient.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRHTTPDownloaderForm.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/PString.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRHTTPDownloaderForm::OnRequestClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHTTPDownloaderForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHTTPDownloaderForm>();
	Text::StringBuilderUTF8 sb;
	me->txtDownloadDir->GetText(sb);
	if (IO::Path::GetPathType(sb.ToCString()) != IO::Path::PathType::Directory)
	{
		me->ui->ShowMsgOK(CSTR("Please enter valid download path"), CSTR("Request"), me);
		return;
	}
	OPTSTR_DEL(me->downPath);
	me->downPath = Text::String::New(sb.ToCString());

	sb.ClearStr();
	me->txtHeaders->GetText(sb);
	OPTSTR_DEL(me->reqHeader);
	if (sb.GetLength() > 0)
	{
		me->reqHeader = Text::String::New(sb.ToCString());
	}

	sb.ClearStr();
	me->txtURL->GetText(sb);
	if (!sb.StartsWith(UTF8STRC("http://")) && !sb.StartsWith(UTF8STRC("https://")))
	{
		me->ui->ShowMsgOK(CSTR("Please enter valid http URL"), CSTR("Request"), me);
		return;
	}


	me->reqURL = Text::String::New(sb.ToCString());
	me->threadEvt.Set();
	while (me->threadRunning && me->reqURL.NotNull())
	{
		Sync::SimpleThread::Sleep(1);
	}
}

UInt32 __stdcall SSWR::AVIRead::AVIRHTTPDownloaderForm::ProcessThread(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHTTPDownloaderForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHTTPDownloaderForm>();
	NN<Text::String> currURL;
	NN<Text::String> currPath;
	Optional<Text::String> currHeader;
	NN<Text::String> s;
	UInt8 buff[4096];
	Text::PString sarr[2];
	Text::PString sarr2[2];
	UnsafeArray<UTF8Char> sbuff;
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptrEnd;
	UOSInt i;
	UOSInt j;
	me->threadRunning = true;
	sbuff = MemAllocArr(UTF8Char, 65536);
	while (!me->threadToStop)
	{
		if (me->reqURL.SetTo(currURL) && me->downPath.SetTo(currPath) && !me->respChanged)
		{
			currHeader = Text::String::CopyOrNull(me->reqHeader);
			currPath = currPath->Clone();
			me->reqURL = 0;

			sptr = currPath->ConcatTo(sbuff);
			if (sptr[-1] != IO::Path::PATH_SEPERATOR)
			{
				*sptr++ = IO::Path::PATH_SEPERATOR;
			}
			i = currURL->LastIndexOf('/');
			sptrEnd = Text::StrConcatC(sptr, &currURL->v[i + 1], currURL->leng - i - 1);
			i = Text::StrIndexOfCharC(sptr, (UOSInt)(sptrEnd - sptr), '?');
			if (i == 0)
			{
				sptrEnd = Text::StrConcatC(sptr, UTF8STRC("download.dat"));
			}
			else if (i != INVALID_INDEX)
			{
				sptr[i] = 0;
				sptrEnd = &sptr[i];
			}
			IO::FileStream fs({sbuff, (UOSInt)(sptrEnd - sbuff)}, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer);
			NN<Net::HTTPClient> cli = Net::HTTPClient::CreateClient(me->core->GetTCPClientFactory(), me->ssl, CSTR_NULL, false, currURL->StartsWith(UTF8STRC("https://")));
			cli->Connect(currURL->ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, me->respTimeDNS, me->respTimeConn, false);
			if (currHeader.SetTo(s))
			{
				sptr = s->ConcatTo(sbuff);
				sarr[1].v = sbuff;
				sarr[1].leng = (UOSInt)(sptr - sbuff);
				while (true)
				{
					i = Text::StrSplitP(sarr, 2, sarr[1], '\r');
					j = Text::StrSplitTrimP(sarr2, 2, sarr[0], ':');
					if (j == 2)
					{
						if (sarr2[0].EqualsICase(UTF8STRC("HOST")))
						{
						}
						else
						{
							cli->AddHeaderC(sarr2[0].ToCString(), sarr2[1].ToCString());
						}
					}
					if (i == 1)
						break;
					while (sarr[1].v[0] == '\n' ||sarr[1].v[0] == '\r')
					{
						sarr[1].v = &sarr[1].v[1];
						sarr[1].leng--;
					}
				}
			}

			cli->EndRequest(me->respTimeReq, me->respTimeResp);
			me->ClearHeaders();
			i = 0;
			j = cli->GetRespHeaderCnt();
			while (i < j)
			{
				sptr = cli->GetRespHeader(i, buff).Or(buff);
				me->respHeaders.Add(Text::String::New(buff, (UOSInt)(sptr - buff)));
				i++;
			}
			me->respHdrChanged = true;

			while ((i = cli->Read(BYTEARR(buff))) > 0)
			{
				fs.Write(Data::ByteArrayR(buff, i));
				me->currSize += i;
			}
			me->respTimeTotal = cli->GetTotalTime();
			me->respSvrAddr = cli->GetSvrAddr().Ptr()[0];

			cli.Delete();
			me->respChanged = true;

			currURL->Release();
			currPath->Release();
			OPTSTR_DEL(currHeader);
		}
	}
	MemFreeArr(sbuff);
	OPTSTR_DEL(me->reqURL);
	me->threadToStop = false;
	me->threadRunning = false;
	return 0;
}

void __stdcall SSWR::AVIRead::AVIRHTTPDownloaderForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHTTPDownloaderForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHTTPDownloaderForm>();
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	Text::StringBuilderUTF8 sb;
	if (me->respHdrChanged)
	{
		me->lvHeaders->ClearItems();
		Data::ArrayIterator<NN<Text::String>> it = me->respHeaders.Iterator();
		while (it.HasNext())
		{
			me->lvHeaders->AddItem(it.Next(), 0);
		}
		me->respHdrChanged = false;
	}

	if (me->respChanged)
	{
		sptr = Net::SocketUtil::GetAddrName(sbuff, me->respSvrAddr).Or(sbuff);
		me->txtSvrIP->SetText(CSTRP(sbuff, sptr));
		if (me->respTimeDNS == -1)
		{
			me->txtTimeDNS->SetText(CSTR("-1"));
		}
		else
		{
			sptr = Text::StrDoubleFmt(sbuff, me->respTimeDNS, "0.0000000000");
			me->txtTimeDNS->SetText(CSTRP(sbuff, sptr));
		}
		if (me->respTimeConn == -1)
		{
			me->txtTimeConn->SetText(CSTR("-1"));
		}
		else
		{
			sptr = Text::StrDoubleFmt(sbuff, me->respTimeConn - me->respTimeDNS, "0.0000000000");
			me->txtTimeConn->SetText(CSTRP(sbuff, sptr));
		}
		if (me->respTimeReq == -1)
		{
			me->txtTimeSendHdr->SetText(CSTR("-1"));
		}
		else
		{
			sptr = Text::StrDoubleFmt(sbuff, me->respTimeReq - me->respTimeConn, "0.0000000000");
			me->txtTimeSendHdr->SetText(CSTRP(sbuff, sptr));
		}
		if (me->respTimeResp == -1)
		{
			me->txtTimeResp->SetText(CSTR("-1"));
		}
		else
		{
			sptr = Text::StrDoubleFmt(sbuff, me->respTimeResp - me->respTimeReq, "0.0000000000");
			me->txtTimeResp->SetText(CSTRP(sbuff, sptr));
		}
		if (me->respTimeTotal == -1)
		{
			me->txtTimeTotal->SetText(CSTR("-1"));
		}
		else
		{
			sptr = Text::StrDoubleFmt(sbuff, me->respTimeTotal - me->respTimeResp, "0.0000000000");
			me->txtTimeTotal->SetText(CSTRP(sbuff, sptr));
		}

		me->respChanged = false;
	}

	Data::DateTime t;
	UInt64 currSize = me->currSize;
	t.SetCurrTimeUTC();
	sptr = Text::StrUInt64(sbuff, currSize);
	me->txtTotalSize->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrDouble(sbuff, (Double)(currSize - me->lastSize) / ((Double)(t.ToTicks() - me->lastT) * 0.001));
	me->txtCurrSpeed->SetText(CSTRP(sbuff, sptr));
	me->lastT = t.ToTicks();
	me->lastSize = currSize;
}

void SSWR::AVIRead::AVIRHTTPDownloaderForm::ClearHeaders()
{
	UOSInt i;
	i = this->respHeaders.GetCount();
	while (i-- > 0)
	{
		OPTSTR_DEL(this->respHeaders.RemoveAt(i));
	}
}

SSWR::AVIRead::AVIRHTTPDownloaderForm::AVIRHTTPDownloaderForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("HTTP Downloader"));

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->clif = core->GetTCPClientFactory();
	this->ssl = Net::SSLEngineFactory::Create(this->clif, true);
	this->respChanged = false;
	this->respHdrChanged = false;
	this->threadRunning = false;
	this->threadToStop = false;
	this->reqURL = 0;
	this->reqHeader = 0;
	this->downPath = 0;

	Data::DateTime t;
	t.SetCurrTimeUTC();
	this->lastT = t.ToTicks();
	this->lastSize = 0;
	this->currSize = 0;

	this->pnlRequest = ui->NewPanel(*this);
	this->pnlRequest->SetRect(0, 0, 100, 151, false);
	this->pnlRequest->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblURL = ui->NewLabel(this->pnlRequest, CSTR("URL"));
	this->lblURL->SetRect(4, 4, 100, 23, false);
	this->txtURL = ui->NewTextBox(this->pnlRequest, CSTR("http://"));
	this->txtURL->SetRect(104, 4, 400, 23, false);
	this->lblDownloadDir = ui->NewLabel(this->pnlRequest, CSTR("Down Path"));
	this->lblDownloadDir->SetRect(4, 28, 100, 23, false);
	this->txtDownloadDir = ui->NewTextBox(this->pnlRequest, CSTR(""));
	this->txtDownloadDir->SetRect(104, 28, 400, 23, false);
	this->lblHeaders = ui->NewLabel(this->pnlRequest, CSTR("Headers\r\n(Except Host)"));
	this->lblHeaders->SetRect(4, 52, 100, 47, false);
	this->txtHeaders = ui->NewTextBox(this->pnlRequest, CSTR(""), true);
	this->txtHeaders->SetRect(104, 52, 400, 71, false);
	this->btnRequest = ui->NewButton(this->pnlRequest, CSTR("Request"));
	this->btnRequest->SetRect(104, 128, 75, 23, false);
	this->btnRequest->HandleButtonClick(OnRequestClicked, this);
	this->grpStatus = ui->NewGroupBox(*this, CSTR("Status"));
	this->grpStatus->SetRect(0, 0, 100, 67, false);
	this->grpStatus->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblCurrSpeed = ui->NewLabel(this->grpStatus, CSTR("Curr Speed"));
	this->lblCurrSpeed->SetRect(4, 0, 100, 23, false);
	this->txtCurrSpeed = ui->NewTextBox(this->grpStatus, CSTR(""));
	this->txtCurrSpeed->SetRect(104, 0, 200, 23, false);
	this->txtCurrSpeed->SetReadOnly(true);
	this->lblTotalSize = ui->NewLabel(this->grpStatus, CSTR("Total Size"));
	this->lblTotalSize->SetRect(4, 24, 100, 23, false);
	this->txtTotalSize = ui->NewTextBox(this->grpStatus, CSTR(""));
	this->txtTotalSize->SetRect(104, 24, 200, 23, false);
	this->txtTotalSize->SetReadOnly(true);
	this->grpResponse = ui->NewGroupBox(*this, CSTR("Response"));
	this->grpResponse->SetDockType(UI::GUIControl::DOCK_FILL);
	this->pnlResponse = ui->NewPanel(this->grpResponse);
	this->pnlResponse->SetRect(0, 0, 100, 151, false);
	this->pnlResponse->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblSvrIP = ui->NewLabel(this->pnlResponse, CSTR("Server IP"));
	this->lblSvrIP->SetRect(4, 4, 100, 23, false);
	this->txtSvrIP = ui->NewTextBox(this->pnlResponse, CSTR(""));
	this->txtSvrIP->SetRect(104, 4, 150, 23, false);
	this->txtSvrIP->SetReadOnly(true);
	this->lblTimeDNS = ui->NewLabel(this->pnlResponse, CSTR("DNS Time"));
	this->lblTimeDNS->SetRect(4, 28, 100, 23, false);
	this->txtTimeDNS = ui->NewTextBox(this->pnlResponse, CSTR(""));
	this->txtTimeDNS->SetRect(104, 28, 150, 23, false);
	this->txtTimeDNS->SetReadOnly(true);
	this->lblTimeConn = ui->NewLabel(this->pnlResponse, CSTR("Conn Time"));
	this->lblTimeConn->SetRect(4, 52, 100, 23, false);
	this->txtTimeConn = ui->NewTextBox(this->pnlResponse, CSTR(""));
	this->txtTimeConn->SetRect(104, 52, 150, 23, false);
	this->txtTimeConn->SetReadOnly(true);
	this->lblTimeSendHdr = ui->NewLabel(this->pnlResponse, CSTR("Request Time"));
	this->lblTimeSendHdr->SetRect(4, 76, 100, 23, false);
	this->txtTimeSendHdr = ui->NewTextBox(this->pnlResponse, CSTR(""));
	this->txtTimeSendHdr->SetRect(104, 76, 150, 23, false);
	this->txtTimeSendHdr->SetReadOnly(true);
	this->lblTimeResp = ui->NewLabel(this->pnlResponse, CSTR("Response Time"));
	this->lblTimeResp->SetRect(4, 100, 100, 23, false);
	this->txtTimeResp = ui->NewTextBox(this->pnlResponse, CSTR(""));
	this->txtTimeResp->SetRect(104, 100, 150, 23, false);
	this->txtTimeResp->SetReadOnly(true);
	this->lblTimeTotal = ui->NewLabel(this->pnlResponse, CSTR("Download Time"));
	this->lblTimeTotal->SetRect(4, 124, 100, 23, false);
	this->txtTimeTotal = ui->NewTextBox(this->pnlResponse, CSTR(""));
	this->txtTimeTotal->SetRect(104, 124, 150, 23, false);
	this->txtTimeTotal->SetReadOnly(true);
	this->lvHeaders = ui->NewListView(this->grpResponse, UI::ListViewStyle::Table, 1);
	this->lvHeaders->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvHeaders->SetShowGrid(true);
	this->lvHeaders->SetFullRowSelect(true);
	this->lvHeaders->AddColumn(CSTR("Header"), 1000);
	
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	sbuff[0] = 0;
	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
	if (i != INVALID_INDEX)
	{
		sbuff[i] = 0;
		sptr = &sbuff[i];
	}
	this->txtDownloadDir->SetText(CSTRP(sbuff, sptr));

	this->SetDefaultButton(this->btnRequest);
	this->txtURL->Focus();
	Sync::ThreadUtil::Create(ProcessThread, this, 0);
	this->AddTimer(100, OnTimerTick, this);
}

SSWR::AVIRead::AVIRHTTPDownloaderForm::~AVIRHTTPDownloaderForm()
{
	this->threadToStop = true;
	this->threadEvt.Set();
	while (this->threadRunning)
	{
		Sync::SimpleThread::Sleep(1);
	}
	this->ClearHeaders();
	OPTSTR_DEL(this->downPath);
	OPTSTR_DEL(this->reqHeader);
	this->ssl.Delete();
}

void SSWR::AVIRead::AVIRHTTPDownloaderForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
