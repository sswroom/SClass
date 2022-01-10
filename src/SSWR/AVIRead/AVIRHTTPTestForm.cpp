#include "Stdafx.h"
#include "Manage/HiResClock.h"
#include "Net/HTTPClient.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRHTTPTestForm.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRHTTPTestForm::OnStartClicked(void *userObj)
{
	SSWR::AVIRead::AVIRHTTPTestForm *me = (SSWR::AVIRead::AVIRHTTPTestForm*)userObj;
	if (me->threadCurrCnt > 0)
	{
		me->StopThreads();
		return;
	}
	me->StopThreads();
	Text::StringBuilderUTF8 sb;
	if (me->connURLs->GetCount() <= 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter at least 1 URL", (const UTF8Char*)"Start", me);
		return;
	}
	me->txtConcurrCnt->GetText(&sb);
	if (!sb.ToUInt32(&me->threadCnt) || me->threadCnt <= 0 || me->threadCnt >= 1000)
	{
		me->threadCnt = 0;
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter valid Concurrent Count", (const UTF8Char*)"Start", me);
		return;
	}
	sb.ClearStr();
	me->txtTotalConnCnt->GetText(&sb);
	if (!sb.ToUInt32(&me->connLeftCnt) || me->connLeftCnt < me->threadCnt || me->connLeftCnt >= 1000000000)
	{
		me->threadCnt = 0;
		me->connLeftCnt = 0;
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter valid Total Connection Count", (const UTF8Char*)"Start", me);
		return;
	}
	me->kaConn = me->chkKAConn->IsChecked();
	if (me->cboMethod->GetSelectedIndex() == 1)
	{
		me->method = "POST";
		sb.ClearStr();
		me->txtPostSize->GetText(&sb);
		if (!sb.ToUInt32(&me->postSize) || me->postSize <= 0)
		{
			me->threadCnt = 0;
			me->connLeftCnt = 0;
			UI::MessageDialog::ShowDialog((const UTF8Char*)"POST Size must be > 0", (const UTF8Char*)"Start", me);
			return;
		}
	}
	else
	{
		me->method = "GET";
		me->postSize = 0;
	}
	
	
	UOSInt i;
	me->connCnt = 0;
	me->failCnt = 0;
	me->threadCurrCnt = 0;
	me->threadStatus = MemAlloc(ThreadStatus, me->threadCnt);
	me->clk->Start();
	i = me->threadCnt;
	while (i-- > 0)
	{
		me->threadStatus[i].me = me;
		me->threadStatus[i].threadRunning = false;
		me->threadStatus[i].threadToStop = false;
		NEW_CLASS(me->threadStatus[i].evt, Sync::Event(true, (const UTF8Char*)"SSWR.AVIRead.AVIRTCPTestForm.threadStatus.evt"));
		Sync::Thread::Create(ProcessThread, &me->threadStatus[i]);
	}
}

void __stdcall SSWR::AVIRead::AVIRHTTPTestForm::OnURLAddClicked(void *userObj)
{
	SSWR::AVIRead::AVIRHTTPTestForm *me = (SSWR::AVIRead::AVIRHTTPTestForm*)userObj;
	Text::StringBuilderUTF8 sb;
	if (me->threadCurrCnt > 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"You cannot add URL while running", (const UTF8Char*)"Add", me);
		return;
	}
	me->txtURL->GetText(&sb);
	if (sb.StartsWith((const UTF8Char*)"http://") || sb.StartsWith((const UTF8Char*)"https://"))
	{
		me->connURLs->Add(Text::String::New(sb.ToString(), sb.GetLength()));
		me->lbURL->AddItem(sb.ToString(), 0);
		me->txtURL->SetText((const UTF8Char*)"");
	}
	else
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter valid HTTP url", (const UTF8Char*)"Add", me);
		return;
	}
}

void __stdcall SSWR::AVIRead::AVIRHTTPTestForm::OnURLClearClicked(void *userObj)
{
	SSWR::AVIRead::AVIRHTTPTestForm *me = (SSWR::AVIRead::AVIRHTTPTestForm*)userObj;
	if (me->threadCurrCnt > 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"You cannot add URL while running", (const UTF8Char*)"Add", me);
		return;
	}
	me->ClearURLs();
}

UInt32 __stdcall SSWR::AVIRead::AVIRHTTPTestForm::ProcessThread(void *userObj)
{
	ThreadStatus *status = (ThreadStatus*)userObj;
	Net::HTTPClient *cli = 0;
//	UInt8 buff[2048];
	Text::String *url;
	Double timeDNS;
	Double timeConn;
	Double timeReq;
	Double timeResp;
	UInt8 buff[2048];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	UInt32 cnt;
	Sync::Interlocked::Increment(&status->me->threadCurrCnt);
	status->threadRunning = true;
	if (status->me->kaConn)
	{
		cli = Net::HTTPClient::CreateClient(status->me->sockf, status->me->ssl, 0, 0, true, false);
		while (!status->threadToStop)
		{
			url = status->me->GetNextURL();
			if (url == 0)
				break;
			if (cli->Connect(url->v, url->leng, status->me->method, &timeDNS, &timeConn, false))
			{
				cli->AddHeaderC(UTF8STRC("Connection"), UTF8STRC("keep-alive"));
				if (Text::StrEquals(status->me->method, "POST"))
				{
					i = status->me->postSize;
					sptr = Text::StrUOSInt(buff, i);
					cli->AddHeaderC(UTF8STRC("Content-Length"), buff, (UOSInt)(sptr - buff));
					while (i >= 2048)
					{
						j = cli->Write(buff, 2048);
						if (j <= 0)
						{
							break;
						}
						i -= j;
					}
					if (i > 0)
					{
						cli->Write(buff, i);
					}
				}
				cli->EndRequest(&timeReq, &timeResp);
				if (timeResp >= 0)
				{
					Sync::Interlocked::Increment(&status->me->connCnt);
					if (timeResp > 0.5)
					{
						if (timeConn > 0.5)
						{
							i = 0;
						}
						else
						{
							i = 0;
						}
					}
					while (cli->Read(buff, 2048));
				}
				else
				{
					Sync::Interlocked::Increment(&status->me->failCnt);
				}
				if (cli->IsError())
				{
					DEL_CLASS(cli);
					cli = Net::HTTPClient::CreateClient(status->me->sockf, status->me->ssl, 0, 0, true, url->StartsWith((const UTF8Char*)"https://"));
				}
			}
			else
			{
				DEL_CLASS(cli);
				cli = Net::HTTPClient::CreateClient(status->me->sockf, status->me->ssl, 0, 0, true, url->StartsWith((const UTF8Char*)"https://"));
				Sync::Interlocked::Increment(&status->me->failCnt);
			}
		}
		DEL_CLASS(cli);
	}
	else
	{
		while (!status->threadToStop)
		{
			url = status->me->GetNextURL();
			if (url == 0)
				break;
			cli = Net::HTTPClient::CreateClient(status->me->sockf, status->me->ssl, 0, 0, true, url->StartsWith((const UTF8Char*)"https://"));
			if (cli->Connect(url->v, url->leng, "GET", &timeDNS, &timeConn, false))
			{
				cli->AddHeaderC(UTF8STRC("Connection"), UTF8STRC("keep-alive"));
				cli->EndRequest(&timeReq, &timeResp);
				if (timeResp >= 0)
				{
					Sync::Interlocked::Increment(&status->me->connCnt);
				}
				else
				{
					Sync::Interlocked::Increment(&status->me->failCnt);
				}
			}
			else
			{
				Sync::Interlocked::Increment(&status->me->failCnt);
			}
			DEL_CLASS(cli);
		}
	}
	status->threadToStop = false;
	status->threadRunning = false;
	cnt = Sync::Interlocked::Decrement(&status->me->threadCurrCnt);
	if (cnt == 0)
	{
		status->me->t = status->me->clk->GetTimeDiff();
	}
	return 0;
}

void __stdcall SSWR::AVIRead::AVIRHTTPTestForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRHTTPTestForm *me = (SSWR::AVIRead::AVIRHTTPTestForm*)userObj;
	UTF8Char sbuff[32];
	Text::StrUInt32(sbuff, me->connLeftCnt);
	me->txtConnLeftCnt->SetText(sbuff);
	Text::StrUInt32(sbuff, me->threadCurrCnt);
	me->txtThreadCnt->SetText(sbuff);
	Text::StrUInt32(sbuff, me->connCnt);
	me->txtSuccCnt->SetText(sbuff);
	Text::StrUInt32(sbuff, me->failCnt);
	me->txtFailCnt->SetText(sbuff);
	Text::StrDouble(sbuff, me->t);
	me->txtTimeUsed->SetText(sbuff);
}

void SSWR::AVIRead::AVIRHTTPTestForm::StopThreads()
{
	if (this->threadCurrCnt > 0)
	{
		UOSInt i = this->threadCnt;
		while (i-- > 0)
		{
			this->threadStatus[i].threadToStop = true;
			this->threadStatus[i].evt->Set();
		}
		while (this->threadCurrCnt > 0)
		{
			Sync::Thread::Sleep(10);
		}
	}
	if (this->threadStatus)
	{
		UOSInt i = this->threadCnt;
		while (i-- > 0)
		{
			DEL_CLASS(this->threadStatus[i].evt);
		}
		MemFree(this->threadStatus);
		this->threadStatus = 0;
		this->threadCnt = 0;
	}
}

void SSWR::AVIRead::AVIRHTTPTestForm::ClearURLs()
{
	UOSInt i;
	i = this->connURLs->GetCount();
	while (i-- > 0)
	{
		this->connURLs->RemoveAt(i)->Release();
	}
	if (this->children->GetCount() > 0)
	{
		this->lbURL->ClearItems();
	}
}

Text::String *SSWR::AVIRead::AVIRHTTPTestForm::GetNextURL()
{
	Text::String *url;
	Sync::MutexUsage mutUsage(this->connMut);
	if (this->connLeftCnt <= 0)
	{
		mutUsage.EndUse();
		return 0;
	}
	url = this->connURLs->GetItem(this->connCurrIndex);
	if ((++this->connCurrIndex) >= this->connURLs->GetCount())
		this->connCurrIndex = 0;

	this->connLeftCnt -= 1;
	mutUsage.EndUse();
	return url;
}

SSWR::AVIRead::AVIRHTTPTestForm::AVIRHTTPTestForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText((const UTF8Char*)"HTTP Test");

	this->core = core;
	this->sockf = core->GetSocketFactory();
	this->ssl = Net::SSLEngineFactory::Create(this->sockf, true);
	this->threadStatus = 0;
	NEW_CLASS(this->connMut, Sync::Mutex());
	NEW_CLASS(this->connURLs, Data::ArrayList<Text::String*>());
	NEW_CLASS(this->clk, Manage::HiResClock());
	this->connCurrIndex = 0;
	this->connLeftCnt = 0;
	this->threadCnt = 0;
	this->threadCurrCnt = 0;
	this->connCnt = 0;
	this->failCnt = 0;
	this->t = 0;
	this->kaConn = false;
	this->method = "GET";
	this->postSize = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->grpStatus, UI::GUIGroupBox(ui, this, (const UTF8Char*)"Status"));
	this->grpStatus->SetRect(0, 0, 100, 136, false);
	this->grpStatus->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->lblConnLeftCnt, UI::GUILabel(ui, this->grpStatus, (const UTF8Char*)"Conn Left"));
	this->lblConnLeftCnt->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtConnLeftCnt, UI::GUITextBox(ui, this->grpStatus, (const UTF8Char*)""));
	this->txtConnLeftCnt->SetRect(104, 4, 150, 23, false);
	this->txtConnLeftCnt->SetReadOnly(true);
	NEW_CLASS(this->lblThreadCnt, UI::GUILabel(ui, this->grpStatus, (const UTF8Char*)"Thread Count"));
	this->lblThreadCnt->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtThreadCnt, UI::GUITextBox(ui, this->grpStatus, (const UTF8Char*)""));
	this->txtThreadCnt->SetRect(104, 28, 150, 23, false);
	this->txtThreadCnt->SetReadOnly(true);
	NEW_CLASS(this->lblSuccCnt, UI::GUILabel(ui, this->grpStatus, (const UTF8Char*)"Success Count"));
	this->lblSuccCnt->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtSuccCnt, UI::GUITextBox(ui, this->grpStatus, (const UTF8Char*)""));
	this->txtSuccCnt->SetRect(104, 52, 150, 23, false);
	this->txtSuccCnt->SetReadOnly(true);
	NEW_CLASS(this->lblFailCnt, UI::GUILabel(ui, this->grpStatus, (const UTF8Char*)"Fail Count"));
	this->lblFailCnt->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtFailCnt, UI::GUITextBox(ui, this->grpStatus, (const UTF8Char*)""));
	this->txtFailCnt->SetRect(104, 76, 150, 23, false);
	this->txtFailCnt->SetReadOnly(true);
	NEW_CLASS(this->lblTimeUsed, UI::GUILabel(ui, this->grpStatus, (const UTF8Char*)"Time Used"));
	this->lblTimeUsed->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtTimeUsed, UI::GUITextBox(ui, this->grpStatus, (const UTF8Char*)""));
	this->txtTimeUsed->SetRect(104, 100, 150, 23, false);
	this->txtTimeUsed->SetReadOnly(true);

	NEW_CLASS(this->pnlRequest, UI::GUIPanel(ui, this));
	this->pnlRequest->SetRect(0, 0, 100, 127, false);
	this->pnlRequest->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->lblConcurrCnt, UI::GUILabel(ui, this->pnlRequest, (const UTF8Char*)"Concurrent Count"));
	this->lblConcurrCnt->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtConcurrCnt, UI::GUITextBox(ui, this->pnlRequest, (const UTF8Char*)"10"));
	this->txtConcurrCnt->SetRect(104, 4, 100, 23, false);
	NEW_CLASS(this->lblTotalConnCnt, UI::GUILabel(ui, this->pnlRequest, (const UTF8Char*)"Total Conn Count"));
	this->lblTotalConnCnt->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtTotalConnCnt, UI::GUITextBox(ui, this->pnlRequest, (const UTF8Char*)"100000"));
	this->txtTotalConnCnt->SetRect(104, 28, 100, 23, false);
	NEW_CLASS(this->lblMethod, UI::GUILabel(ui, this->pnlRequest, (const UTF8Char*)"Method"));
	this->lblMethod->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->cboMethod, UI::GUIComboBox(ui, this->pnlRequest, false));
	this->cboMethod->SetRect(104, 52, 100, 23, false);
	this->cboMethod->AddItem((const UTF8Char*)"GET", (void*)0);
	this->cboMethod->AddItem((const UTF8Char*)"POST", (void*)1);
	this->cboMethod->SetSelectedIndex(0);
	NEW_CLASS(this->lblPostSize, UI::GUILabel(ui, this->pnlRequest, (const UTF8Char*)"POST Size"));
	this->lblPostSize->SetRect(204, 52, 100, 23, false);
	NEW_CLASS(this->txtPostSize, UI::GUITextBox(ui, this->pnlRequest, (const UTF8Char*)"1048576"));
	this->txtPostSize->SetRect(304, 52, 100, 23, false);
	NEW_CLASS(this->chkKAConn, UI::GUICheckBox(ui, this->pnlRequest, (const UTF8Char*)"KA Conn", false));
	this->chkKAConn->SetRect(104, 76, 100, 23, false);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this->pnlRequest, (const UTF8Char*)"Start"));
	this->btnStart->SetRect(104, 100, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	
	NEW_CLASS(this->grpURL, UI::GUIGroupBox(ui, this, (const UTF8Char*)"URL"));
	this->grpURL->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->pnlURL, UI::GUIPanel(ui, this->grpURL));
	this->pnlURL->SetRect(0, 0, 100, 23, false);
	this->pnlURL->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->pnlURLCtrl, UI::GUIPanel(ui, this->grpURL));
	this->pnlURLCtrl->SetRect(0, 0, 100, 31, false);
	this->pnlURLCtrl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->btnURLAdd, UI::GUIButton(ui, this->pnlURL, (const UTF8Char*)"&Add"));
	this->btnURLAdd->SetRect(0, 0, 75, 23, false);
	this->btnURLAdd->SetDockType(UI::GUIControl::DOCK_RIGHT);
	this->btnURLAdd->HandleButtonClick(OnURLAddClicked, this);
	NEW_CLASS(this->txtURL, UI::GUITextBox(ui, this->pnlURL, (const UTF8Char*)""));
	this->txtURL->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->btnURLClear, UI::GUIButton(ui, this->pnlURLCtrl, (const UTF8Char*)"&Clear"));
	this->btnURLClear->SetRect(4, 4, 75, 23, false);
	this->btnURLClear->HandleButtonClick(OnURLClearClicked, this);
	NEW_CLASS(this->lbURL, UI::GUIListBox(ui, this->grpURL, false));
	this->lbURL->SetDockType(UI::GUIControl::DOCK_FILL);

	this->SetDefaultButton(this->btnURLAdd);
	this->txtURL->Focus();
	this->AddTimer(100, OnTimerTick, this);
}

SSWR::AVIRead::AVIRHTTPTestForm::~AVIRHTTPTestForm()
{
	this->StopThreads();
	this->ClearURLs();
	DEL_CLASS(this->connURLs);
	DEL_CLASS(this->connMut);
	DEL_CLASS(this->clk);
	SDEL_CLASS(this->ssl);
}

void SSWR::AVIRead::AVIRHTTPTestForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
