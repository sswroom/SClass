#include "Stdafx.h"
#include "Manage/HiResClock.h"
#include "Net/HTTPProxyClient.h"
#include "SSWR/AVIRead/AVIRHTTPProxyClientForm.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRHTTPProxyClientForm::OnRequestClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHTTPProxyClientForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHTTPProxyClientForm>();
	Text::StringBuilderUTF8 sb;
	UInt32 ip;
	UInt16 port;
	me->txtProxyPort->GetText(sb);
	port = 0;
	sb.ToUInt16(port);
	if (port <= 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter valid proxy port"), CSTR("Request"), me);
		return;
	}
	sb.ClearStr();
	me->txtProxySvr->GetText(sb);
	ip = me->sockf->DNSResolveIPv4(sb.ToCString());
	if (ip == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter valid proxy server"), CSTR("Request"), me);
		return;
	}

	sb.ClearStr();
	me->txtURL->GetText(sb);
	if (!sb.StartsWith(UTF8STRC("http://")))
	{
		me->ui->ShowMsgOK(CSTR("Please enter valid http URL"), CSTR("Request"), me);
		return;
	}

	me->proxyIP = ip;
	me->proxyPort = port;
	me->reqURL = Text::String::New(sb.ToString(), sb.GetLength());
	me->threadEvt->Set();
	while (me->threadRunning && me->reqURL.NotNull())
	{
		Sync::SimpleThread::Sleep(1);
	}
}

UInt32 __stdcall SSWR::AVIRead::AVIRHTTPProxyClientForm::ProcessThread(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHTTPProxyClientForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHTTPProxyClientForm>();
	NN<Text::String> currURL;
	NN<Net::HTTPClient> cli;
	UInt8 buff[4096];
	UnsafeArray<UTF8Char> sbuff;
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UOSInt j;
	me->threadRunning = true;
	sbuff = MemAllocArr(UTF8Char, 65536);
	while (!me->threadToStop)
	{
		if (me->reqURL.SetTo(currURL) && !me->respChanged)
		{
			me->reqURL = 0;

			NEW_CLASSNN(cli, Net::HTTPProxyClient(me->core->GetTCPClientFactory(), false, me->proxyIP, me->proxyPort));
			cli->Connect(currURL->ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, me->respTimeDNS, me->respTimeConn, false);
			cli->AddHeaderC(CSTR("User-Agent"), CSTR("Test/1.0"));
			cli->AddHeaderC(CSTR("Accept"), CSTR("*/*"));
			cli->AddHeaderC(CSTR("Accept-Charset"), CSTR("*"));
			cli->EndRequest(me->respTimeReq, me->respTimeResp);
			while (cli->Read(BYTEARR(buff)) > 0);
			me->respTimeTotal = cli->GetTotalTime();
			me->ClearHeaders();
			i = 0;
			j = cli->GetRespHeaderCnt();
			while (i < j)
			{
				sptr = cli->GetRespHeader(i, sbuff).Or(sbuff);
				me->respHeaders.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				i++;
			}
			me->respSvrAddr = cli->GetSvrAddr().Ptr()[0];

			cli.Delete();
			me->respChanged = true;

			currURL->Release();
		}
		me->threadEvt->Wait(1000);
	}
	MemFreeArr(sbuff);
	OPTSTR_DEL(me->reqURL);
	me->threadToStop = false;
	me->threadRunning = false;
	return 0;
}

void __stdcall SSWR::AVIRead::AVIRHTTPProxyClientForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHTTPProxyClientForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHTTPProxyClientForm>();
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UOSInt j;
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

		me->lvHeaders->ClearItems();
		i = 0;
		j = me->respHeaders.GetCount();
		while (i < j)
		{
			me->lvHeaders->AddItem(Text::String::OrEmpty(me->respHeaders.GetItem(i)), 0);
			i++;
		}
		me->respChanged = false;
	}
}

void SSWR::AVIRead::AVIRHTTPProxyClientForm::ClearHeaders()
{
	UOSInt i;
	i = this->respHeaders.GetCount();
	while (i-- > 0)
	{
		OPTSTR_DEL(this->respHeaders.RemoveAt(i));
	}
}

SSWR::AVIRead::AVIRHTTPProxyClientForm::AVIRHTTPProxyClientForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("HTTP Proxy Client"));

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->sockf = core->GetSocketFactory();
	this->respChanged = false;
	this->threadRunning = false;
	this->threadToStop = false;
	this->reqURL = 0;
	NEW_CLASSNN(this->threadEvt, Sync::Event(true));

	this->pnlRequest = ui->NewPanel(*this);
	this->pnlRequest->SetRect(0, 0, 100, 79, false);
	this->pnlRequest->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblURL = ui->NewLabel(this->pnlRequest, CSTR("URL"));
	this->lblURL->SetRect(4, 4, 100, 23, false);
	this->txtURL = ui->NewTextBox(this->pnlRequest, CSTR("http://"));
	this->txtURL->SetRect(104, 4, 400, 23, false);
	this->lblProxySvr = ui->NewLabel(this->pnlRequest, CSTR("Proxy Server"));
	this->lblProxySvr->SetRect(4, 28, 100, 23, false);
	this->txtProxySvr = ui->NewTextBox(this->pnlRequest, CSTR(""));
	this->txtProxySvr->SetRect(104, 28, 200, 23, false);
	this->lblProxyPort = ui->NewLabel(this->pnlRequest, CSTR("Port"));
	this->lblProxyPort->SetRect(304, 28, 50, 23, false);
	this->txtProxyPort = ui->NewTextBox(this->pnlRequest, CSTR("80"));
	this->txtProxyPort->SetRect(354, 28, 60, 23, false);
	this->btnRequest = ui->NewButton(this->pnlRequest, CSTR("Request"));
	this->btnRequest->SetRect(104, 56, 75, 23, false);
	this->btnRequest->HandleButtonClick(OnRequestClicked, this);
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

	this->SetDefaultButton(this->btnRequest);
	this->txtURL->Focus();
	Sync::ThreadUtil::Create(ProcessThread, this, 0);
	this->AddTimer(100, OnTimerTick, this);
}

SSWR::AVIRead::AVIRHTTPProxyClientForm::~AVIRHTTPProxyClientForm()
{
	this->threadToStop = true;
	this->threadEvt->Set();
	while (this->threadRunning)
	{
		Sync::SimpleThread::Sleep(1);
	}
	this->threadEvt.Delete();
	this->ClearHeaders();
}

void SSWR::AVIRead::AVIRHTTPProxyClientForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
