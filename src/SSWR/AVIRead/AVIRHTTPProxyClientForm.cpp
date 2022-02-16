#include "Stdafx.h"
#include "Manage/HiResClock.h"
#include "Net/HTTPProxyClient.h"
#include "SSWR/AVIRead/AVIRHTTPProxyClientForm.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRHTTPProxyClientForm::OnRequestClicked(void *userObj)
{
	SSWR::AVIRead::AVIRHTTPProxyClientForm *me = (SSWR::AVIRead::AVIRHTTPProxyClientForm*)userObj;
	Text::StringBuilderUTF8 sb;
	UInt32 ip;
	UInt16 port;
	me->txtProxyPort->GetText(&sb);
	port = 0;
	sb.ToUInt16(&port);
	if (port <= 0 || port >= 65536)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter valid proxy port", (const UTF8Char*)"Request", me);
		return;
	}
	sb.ClearStr();
	me->txtProxySvr->GetText(&sb);
	ip = me->sockf->DNSResolveIPv4(sb.ToString());
	if (ip == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter valid proxy server", (const UTF8Char*)"Request", me);
		return;
	}

	sb.ClearStr();
	me->txtURL->GetText(&sb);
	if (!sb.StartsWith(UTF8STRC("http://")))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter valid http URL", (const UTF8Char*)"Request", me);
		return;
	}

	me->proxyIP = ip;
	me->proxyPort = port;
	me->reqURL = Text::String::New(sb.ToString(), sb.GetLength());
	me->threadEvt->Set();
	while (me->threadRunning && me->reqURL)
	{
		Sync::Thread::Sleep(1);
	}
}

UInt32 __stdcall SSWR::AVIRead::AVIRHTTPProxyClientForm::ProcessThread(void *userObj)
{
	SSWR::AVIRead::AVIRHTTPProxyClientForm *me = (SSWR::AVIRead::AVIRHTTPProxyClientForm*)userObj;
	Text::String *currURL;
	Net::HTTPClient *cli;
	UInt8 buff[4096];
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
			me->reqURL = 0;

			NEW_CLASS(cli, Net::HTTPProxyClient(me->core->GetSocketFactory(), false, me->proxyIP, me->proxyPort));
			cli->Connect(currURL->ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, &me->respTimeDNS, &me->respTimeConn, false);
			cli->AddHeaderC(CSTR("User-Agent"), CSTR("Test/1.0"));
			cli->AddHeaderC(CSTR("Accept"), CSTR("*/*"));
			cli->AddHeaderC(CSTR("Accept-Charset"), CSTR("*"));
			cli->EndRequest(&me->respTimeReq, &me->respTimeResp);
			while (cli->Read(buff, 4096) > 0);
			me->respTimeTotal = cli->GetTotalTime();
			me->ClearHeaders();
			i = 0;
			j = cli->GetRespHeaderCnt();
			while (i < j)
			{
				sptr = cli->GetRespHeader(i, sbuff);
				me->respHeaders->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				i++;
			}
			me->respSvrAddr = *cli->GetSvrAddr();

			DEL_CLASS(cli);
			me->respChanged = true;

			currURL->Release();
		}
		me->threadEvt->Wait(1000);
	}
	MemFree(sbuff);
	SDEL_STRING(me->reqURL);
	me->threadToStop = false;
	me->threadRunning = false;
	return 0;
}

void __stdcall SSWR::AVIRead::AVIRHTTPProxyClientForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRHTTPProxyClientForm *me = (SSWR::AVIRead::AVIRHTTPProxyClientForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	if (me->respChanged)
	{
		sptr = Net::SocketUtil::GetAddrName(sbuff, &me->respSvrAddr);
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
		j = me->respHeaders->GetCount();
		while (i < j)
		{
			me->lvHeaders->AddItem(me->respHeaders->GetItem(i), 0);
			i++;
		}
		me->respChanged = false;
	}
}

void SSWR::AVIRead::AVIRHTTPProxyClientForm::ClearHeaders()
{
	UOSInt i;
	i = this->respHeaders->GetCount();
	while (i-- > 0)
	{
		this->respHeaders->RemoveAt(i)->Release();
	}
}

SSWR::AVIRead::AVIRHTTPProxyClientForm::AVIRHTTPProxyClientForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
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
	NEW_CLASS(this->threadEvt, Sync::Event(true));
	NEW_CLASS(this->respHeaders, Data::ArrayList<Text::String *>());

	NEW_CLASS(this->pnlRequest, UI::GUIPanel(ui, this));
	this->pnlRequest->SetRect(0, 0, 100, 79, false);
	this->pnlRequest->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblURL, UI::GUILabel(ui, this->pnlRequest, (const UTF8Char*)"URL"));
	this->lblURL->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtURL, UI::GUITextBox(ui, this->pnlRequest, CSTR("http://")));
	this->txtURL->SetRect(104, 4, 400, 23, false);
	NEW_CLASS(this->lblProxySvr, UI::GUILabel(ui, this->pnlRequest, (const UTF8Char*)"Proxy Server"));
	this->lblProxySvr->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtProxySvr, UI::GUITextBox(ui, this->pnlRequest, CSTR("")));
	this->txtProxySvr->SetRect(104, 28, 200, 23, false);
	NEW_CLASS(this->lblProxyPort, UI::GUILabel(ui, this->pnlRequest, (const UTF8Char*)"Port"));
	this->lblProxyPort->SetRect(304, 28, 50, 23, false);
	NEW_CLASS(this->txtProxyPort, UI::GUITextBox(ui, this->pnlRequest, CSTR("80")));
	this->txtProxyPort->SetRect(354, 28, 60, 23, false);
	NEW_CLASS(this->btnRequest, UI::GUIButton(ui, this->pnlRequest, CSTR("Request")));
	this->btnRequest->SetRect(104, 56, 75, 23, false);
	this->btnRequest->HandleButtonClick(OnRequestClicked, this);
	NEW_CLASS(this->grpResponse, UI::GUIGroupBox(ui, this, (const UTF8Char*)"Response"));
	this->grpResponse->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->pnlResponse, UI::GUIPanel(ui, this->grpResponse));
	this->pnlResponse->SetRect(0, 0, 100, 151, false);
	this->pnlResponse->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblSvrIP, UI::GUILabel(ui, this->pnlResponse, (const UTF8Char*)"Server IP"));
	this->lblSvrIP->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtSvrIP, UI::GUITextBox(ui, this->pnlResponse, CSTR("")));
	this->txtSvrIP->SetRect(104, 4, 150, 23, false);
	this->txtSvrIP->SetReadOnly(true);
	NEW_CLASS(this->lblTimeDNS, UI::GUILabel(ui, this->pnlResponse, (const UTF8Char*)"DNS Time"));
	this->lblTimeDNS->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtTimeDNS, UI::GUITextBox(ui, this->pnlResponse, CSTR("")));
	this->txtTimeDNS->SetRect(104, 28, 150, 23, false);
	this->txtTimeDNS->SetReadOnly(true);
	NEW_CLASS(this->lblTimeConn, UI::GUILabel(ui, this->pnlResponse, (const UTF8Char*)"Conn Time"));
	this->lblTimeConn->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtTimeConn, UI::GUITextBox(ui, this->pnlResponse, CSTR("")));
	this->txtTimeConn->SetRect(104, 52, 150, 23, false);
	this->txtTimeConn->SetReadOnly(true);
	NEW_CLASS(this->lblTimeSendHdr, UI::GUILabel(ui, this->pnlResponse, (const UTF8Char*)"Request Time"));
	this->lblTimeSendHdr->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtTimeSendHdr, UI::GUITextBox(ui, this->pnlResponse, CSTR("")));
	this->txtTimeSendHdr->SetRect(104, 76, 150, 23, false);
	this->txtTimeSendHdr->SetReadOnly(true);
	NEW_CLASS(this->lblTimeResp, UI::GUILabel(ui, this->pnlResponse, (const UTF8Char*)"Response Time"));
	this->lblTimeResp->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtTimeResp, UI::GUITextBox(ui, this->pnlResponse, CSTR("")));
	this->txtTimeResp->SetRect(104, 100, 150, 23, false);
	this->txtTimeResp->SetReadOnly(true);
	NEW_CLASS(this->lblTimeTotal, UI::GUILabel(ui, this->pnlResponse, (const UTF8Char*)"Download Time"));
	this->lblTimeTotal->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->txtTimeTotal, UI::GUITextBox(ui, this->pnlResponse, CSTR("")));
	this->txtTimeTotal->SetRect(104, 124, 150, 23, false);
	this->txtTimeTotal->SetReadOnly(true);
	NEW_CLASS(this->lvHeaders, UI::GUIListView(ui, this->grpResponse, UI::GUIListView::LVSTYLE_TABLE, 1));
	this->lvHeaders->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvHeaders->SetShowGrid(true);
	this->lvHeaders->SetFullRowSelect(true);
	this->lvHeaders->AddColumn((const UTF8Char*)"Header", 1000);

	this->SetDefaultButton(this->btnRequest);
	this->txtURL->Focus();
	Sync::Thread::Create(ProcessThread, this, 0);
	this->AddTimer(100, OnTimerTick, this);
}

SSWR::AVIRead::AVIRHTTPProxyClientForm::~AVIRHTTPProxyClientForm()
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
}

void SSWR::AVIRead::AVIRHTTPProxyClientForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
