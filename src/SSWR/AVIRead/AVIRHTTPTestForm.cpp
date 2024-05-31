#include "Stdafx.h"
#include "Manage/HiResClock.h"
#include "Net/HTTPClient.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRHTTPTestForm.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

void __stdcall SSWR::AVIRead::AVIRHTTPTestForm::OnStartClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHTTPTestForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHTTPTestForm>();
	if (me->threadCurrCnt > 0)
	{
		me->StopThreads();
		return;
	}
	me->StopThreads();
	Text::StringBuilderUTF8 sb;
	if (me->connURLs.GetCount() <= 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter at least 1 URL"), CSTR("Start"), me);
		return;
	}
	me->txtConcurrCnt->GetText(sb);
	if (!sb.ToUInt32(me->threadCnt) || me->threadCnt <= 0 || me->threadCnt >= 1000)
	{
		me->threadCnt = 0;
		me->ui->ShowMsgOK(CSTR("Please enter valid Concurrent Count"), CSTR("Start"), me);
		return;
	}
	sb.ClearStr();
	me->txtTotalConnCnt->GetText(sb);
	if (!sb.ToUInt32(me->connLeftCnt) || me->connLeftCnt < me->threadCnt || me->connLeftCnt >= 1000000000)
	{
		me->threadCnt = 0;
		me->connLeftCnt = 0;
		me->ui->ShowMsgOK(CSTR("Please enter valid Total Connection Count"), CSTR("Start"), me);
		return;
	}
	me->kaConn = me->chkKAConn->IsChecked();
	me->enableGZip = me->chkGZip->IsChecked();
	if (me->cboMethod->GetSelectedIndex() == 1)
	{
		me->method = Net::WebUtil::RequestMethod::HTTP_POST;
		sb.ClearStr();
		me->txtPostSize->GetText(sb);
		if (!sb.ToUInt32(me->postSize) || me->postSize <= 0)
		{
			me->threadCnt = 0;
			me->connLeftCnt = 0;
			me->ui->ShowMsgOK(CSTR("POST Size must be > 0"), CSTR("Start"), me);
			return;
		}
	}
	else
	{
		me->method = Net::WebUtil::RequestMethod::HTTP_GET;
		me->postSize = 0;
	}
	
	
	UOSInt i;
	me->connCnt = 0;
	me->failCnt = 0;
	me->threadCurrCnt = 0;
	me->threads = MemAlloc(Sync::Thread*, me->threadCnt);
	me->clk.Start();
	i = me->threadCnt;
	while (i-- > 0)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("HTTPTest"));
		sb.AppendUOSInt(i);
		NEW_CLASS(me->threads[i], Sync::Thread(ProcessThread, me.Ptr(), sb.ToCString()));
		me->threads[i]->Start();
	}
}

void __stdcall SSWR::AVIRead::AVIRHTTPTestForm::OnURLAddClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHTTPTestForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHTTPTestForm>();
	Text::StringBuilderUTF8 sb;
	if (me->threadCurrCnt > 0)
	{
		me->ui->ShowMsgOK(CSTR("You cannot add URL while running"), CSTR("Add"), me);
		return;
	}
	me->txtURL->GetText(sb);
	if (sb.StartsWith(UTF8STRC("http://")) || sb.StartsWith(UTF8STRC("https://")))
	{
		me->connURLs.Add(Text::String::New(sb.ToCString()));
		me->lbURL->AddItem(sb.ToCString(), 0);
		me->txtURL->SetText(CSTR(""));
	}
	else
	{
		me->ui->ShowMsgOK(CSTR("Please enter valid HTTP url"), CSTR("Add"), me);
		return;
	}
}

void __stdcall SSWR::AVIRead::AVIRHTTPTestForm::OnURLClearClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHTTPTestForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHTTPTestForm>();
	if (me->threadCurrCnt > 0)
	{
		me->ui->ShowMsgOK(CSTR("You cannot add URL while running"), CSTR("Add"), me);
		return;
	}
	me->ClearURLs();
}

void __stdcall SSWR::AVIRead::AVIRHTTPTestForm::ProcessThread(NN<Sync::Thread> thread)
{
	NN<SSWR::AVIRead::AVIRHTTPTestForm> me = thread->GetUserObj().GetNN<SSWR::AVIRead::AVIRHTTPTestForm>();
//	UInt8 buff[2048];
	NN<Text::String> url;
	Double timeDNS;
	Double timeConn;
	Double timeReq;
	Double timeResp;
	UInt8 buff[2048];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UOSInt j;
	UInt32 cnt;
	Sync::Interlocked::IncrementU32(me->threadCurrCnt);
	if (me->kaConn)
	{
		NN<Net::HTTPClient> cli = Net::HTTPClient::CreateClient(me->sockf, me->ssl, CSTR_NULL, true, false);
		while (!thread->IsStopping())
		{
			if (!me->GetNextURL().SetTo(url))
				break;
			if (cli->Connect(url->ToCString(), me->method, timeDNS, timeConn, false))
			{
				if (me->enableGZip)
				{
					cli->AddHeaderC(CSTR("Accept-Encoding"), CSTR("gzip, deflate"));
				}
				cli->AddHeaderC(CSTR("Connection"), CSTR("keep-alive"));
				if (me->method == Net::WebUtil::RequestMethod::HTTP_POST)
				{
					i = me->postSize;
					sptr = Text::StrUOSInt(buff, i);
					cli->AddHeaderC(CSTR("Content-Length"), {buff, (UOSInt)(sptr - buff)});
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
				cli->EndRequest(timeReq, timeResp);
				if (timeResp >= 0)
				{
					Sync::Interlocked::IncrementU32(me->connCnt);
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
					while (cli->Read(BYTEARR(buff)));
				}
				else
				{
					Sync::Interlocked::IncrementU32(me->failCnt);
				}
				if (cli->IsError())
				{
					cli.Delete();
					cli = Net::HTTPClient::CreateClient(me->sockf, me->ssl, CSTR_NULL, true, url->StartsWith(UTF8STRC("https://")));
				}
			}
			else
			{
				cli.Delete();
				cli = Net::HTTPClient::CreateClient(me->sockf, me->ssl, CSTR_NULL, true, url->StartsWith(UTF8STRC("https://")));
				Sync::Interlocked::IncrementU32(me->failCnt);
			}
		}
		cli.Delete();
	}
	else
	{
		while (!thread->IsStopping())
		{
			if (!me->GetNextURL().SetTo(url))
				break;
			NN<Net::HTTPClient> cli = Net::HTTPClient::CreateClient(me->sockf, me->ssl, CSTR_NULL, true, url->StartsWith(UTF8STRC("https://")));
			if (cli->Connect(url->ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, timeDNS, timeConn, false))
			{
				if (me->enableGZip)
				{
					cli->AddHeaderC(CSTR("Accept-Encoding"), CSTR("gzip, deflate"));
				}
				cli->AddHeaderC(CSTR("Connection"), CSTR("keep-alive"));
				cli->EndRequest(timeReq, timeResp);
				if (timeResp >= 0)
				{
					Sync::Interlocked::IncrementU32(me->connCnt);
				}
				else
				{
					Sync::Interlocked::IncrementU32(me->failCnt);
				}
			}
			else
			{
				Sync::Interlocked::IncrementU32(me->failCnt);
			}
			cli.Delete();
		}
	}
	cnt = Sync::Interlocked::DecrementU32(me->threadCurrCnt);
	if (cnt == 0)
	{
		me->t = me->clk.GetTimeDiff();
	}
}

void __stdcall SSWR::AVIRead::AVIRHTTPTestForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHTTPTestForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHTTPTestForm>();
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrUInt32(sbuff, me->connLeftCnt);
	me->txtConnLeftCnt->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrUInt32(sbuff, me->threadCurrCnt);
	me->txtThreadCnt->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrUInt32(sbuff, me->connCnt);
	me->txtSuccCnt->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrUInt32(sbuff, me->failCnt);
	me->txtFailCnt->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrDouble(sbuff, me->t);
	me->txtTimeUsed->SetText(CSTRP(sbuff, sptr));
}

void SSWR::AVIRead::AVIRHTTPTestForm::StopThreads()
{
	if (this->threadCurrCnt > 0)
	{
		UOSInt i = this->threadCnt;
		while (i-- > 0)
		{
			this->threads[i]->BeginStop();
		}
		while (i-- > 0)
		{
			this->threads[i]->WaitForEnd();
		}
	}
	if (this->threads)
	{
		UOSInt i = this->threadCnt;
		while (i-- > 0)
		{
			DEL_CLASS(this->threads[i]);
		}
		MemFree(this->threads);
		this->threads = 0;
		this->threadCnt = 0;
	}
}

void SSWR::AVIRead::AVIRHTTPTestForm::ClearURLs()
{
	UOSInt i;
	i = this->connURLs.GetCount();
	while (i-- > 0)
	{
		OPTSTR_DEL(this->connURLs.RemoveAt(i));
	}
	if (this->children.GetCount() > 0)
	{
		this->lbURL->ClearItems();
	}
}

Optional<Text::String> SSWR::AVIRead::AVIRHTTPTestForm::GetNextURL()
{
	Optional<Text::String> url;
	Sync::MutexUsage mutUsage(this->connMut);
	if (this->connLeftCnt <= 0)
	{
		return 0;
	}
	url = this->connURLs.GetItem(this->connCurrIndex);
	if ((++this->connCurrIndex) >= this->connURLs.GetCount())
		this->connCurrIndex = 0;

	this->connLeftCnt -= 1;
	return url;
}

SSWR::AVIRead::AVIRHTTPTestForm::AVIRHTTPTestForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("HTTP Test"));

	this->core = core;
	this->sockf = core->GetSocketFactory();
	this->ssl = Net::SSLEngineFactory::Create(this->sockf, true);
	this->threads = 0;
	this->connCurrIndex = 0;
	this->connLeftCnt = 0;
	this->threadCnt = 0;
	this->threadCurrCnt = 0;
	this->connCnt = 0;
	this->failCnt = 0;
	this->t = 0;
	this->kaConn = false;
	this->enableGZip = false;
	this->method = Net::WebUtil::RequestMethod::HTTP_GET;
	this->postSize = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->grpStatus = ui->NewGroupBox(*this, CSTR("Status"));
	this->grpStatus->SetRect(0, 0, 100, 136, false);
	this->grpStatus->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lblConnLeftCnt = ui->NewLabel(this->grpStatus, CSTR("Conn Left"));
	this->lblConnLeftCnt->SetRect(4, 4, 100, 23, false);
	this->txtConnLeftCnt = ui->NewTextBox(this->grpStatus, CSTR(""));
	this->txtConnLeftCnt->SetRect(104, 4, 150, 23, false);
	this->txtConnLeftCnt->SetReadOnly(true);
	this->lblThreadCnt = ui->NewLabel(this->grpStatus, CSTR("Thread Count"));
	this->lblThreadCnt->SetRect(4, 28, 100, 23, false);
	this->txtThreadCnt = ui->NewTextBox(this->grpStatus, CSTR(""));
	this->txtThreadCnt->SetRect(104, 28, 150, 23, false);
	this->txtThreadCnt->SetReadOnly(true);
	this->lblSuccCnt = ui->NewLabel(this->grpStatus, CSTR("Success Count"));
	this->lblSuccCnt->SetRect(4, 52, 100, 23, false);
	this->txtSuccCnt = ui->NewTextBox(this->grpStatus, CSTR(""));
	this->txtSuccCnt->SetRect(104, 52, 150, 23, false);
	this->txtSuccCnt->SetReadOnly(true);
	this->lblFailCnt = ui->NewLabel(this->grpStatus, CSTR("Fail Count"));
	this->lblFailCnt->SetRect(4, 76, 100, 23, false);
	this->txtFailCnt = ui->NewTextBox(this->grpStatus, CSTR(""));
	this->txtFailCnt->SetRect(104, 76, 150, 23, false);
	this->txtFailCnt->SetReadOnly(true);
	this->lblTimeUsed = ui->NewLabel(this->grpStatus, CSTR("Time Used"));
	this->lblTimeUsed->SetRect(4, 100, 100, 23, false);
	this->txtTimeUsed = ui->NewTextBox(this->grpStatus, CSTR(""));
	this->txtTimeUsed->SetRect(104, 100, 150, 23, false);
	this->txtTimeUsed->SetReadOnly(true);

	this->pnlRequest = ui->NewPanel(*this);
	this->pnlRequest->SetRect(0, 0, 100, 127, false);
	this->pnlRequest->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lblConcurrCnt = ui->NewLabel(this->pnlRequest, CSTR("Concurrent Count"));
	this->lblConcurrCnt->SetRect(4, 4, 100, 23, false);
	this->txtConcurrCnt = ui->NewTextBox(this->pnlRequest, CSTR("10"));
	this->txtConcurrCnt->SetRect(104, 4, 100, 23, false);
	this->lblTotalConnCnt = ui->NewLabel(this->pnlRequest, CSTR("Total Conn Count"));
	this->lblTotalConnCnt->SetRect(4, 28, 100, 23, false);
	this->txtTotalConnCnt = ui->NewTextBox(this->pnlRequest, CSTR("100000"));
	this->txtTotalConnCnt->SetRect(104, 28, 100, 23, false);
	this->lblMethod = ui->NewLabel(this->pnlRequest, CSTR("Method"));
	this->lblMethod->SetRect(4, 52, 100, 23, false);
	this->cboMethod = ui->NewComboBox(this->pnlRequest, false);
	this->cboMethod->SetRect(104, 52, 100, 23, false);
	this->cboMethod->AddItem(CSTR("GET"), (void*)0);
	this->cboMethod->AddItem(CSTR("POST"), (void*)1);
	this->cboMethod->SetSelectedIndex(0);
	this->lblPostSize = ui->NewLabel(this->pnlRequest, CSTR("POST Size"));
	this->lblPostSize->SetRect(204, 52, 100, 23, false);
	this->txtPostSize = ui->NewTextBox(this->pnlRequest, CSTR("1048576"));
	this->txtPostSize->SetRect(304, 52, 100, 23, false);
	this->chkKAConn = ui->NewCheckBox(this->pnlRequest, CSTR("KA Conn"), false);
	this->chkKAConn->SetRect(104, 76, 100, 23, false);
	this->chkGZip = ui->NewCheckBox(this->pnlRequest, CSTR("GZip"), true);
	this->chkGZip->SetRect(204, 76, 100, 23, false);
	this->btnStart = ui->NewButton(this->pnlRequest, CSTR("Start"));
	this->btnStart->SetRect(104, 100, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	
	this->grpURL = ui->NewGroupBox(*this, CSTR("URL"));
	this->grpURL->SetDockType(UI::GUIControl::DOCK_FILL);
	this->pnlURL = ui->NewPanel(this->grpURL);
	this->pnlURL->SetRect(0, 0, 100, 23, false);
	this->pnlURL->SetDockType(UI::GUIControl::DOCK_TOP);
	this->pnlURLCtrl = ui->NewPanel(this->grpURL);
	this->pnlURLCtrl->SetRect(0, 0, 100, 31, false);
	this->pnlURLCtrl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->btnURLAdd = ui->NewButton(this->pnlURL, CSTR("&Add"));
	this->btnURLAdd->SetRect(0, 0, 75, 23, false);
	this->btnURLAdd->SetDockType(UI::GUIControl::DOCK_RIGHT);
	this->btnURLAdd->HandleButtonClick(OnURLAddClicked, this);
	this->txtURL = ui->NewTextBox(this->pnlURL, CSTR(""));
	this->txtURL->SetDockType(UI::GUIControl::DOCK_FILL);
	this->btnURLClear = ui->NewButton(this->pnlURLCtrl, CSTR("&Clear"));
	this->btnURLClear->SetRect(4, 4, 75, 23, false);
	this->btnURLClear->HandleButtonClick(OnURLClearClicked, this);
	this->lbURL = ui->NewListBox(this->grpURL, false);
	this->lbURL->SetDockType(UI::GUIControl::DOCK_FILL);

	this->SetDefaultButton(this->btnURLAdd);
	this->txtURL->Focus();
	this->AddTimer(100, OnTimerTick, this);
}

SSWR::AVIRead::AVIRHTTPTestForm::~AVIRHTTPTestForm()
{
	this->StopThreads();
	this->ClearURLs();
	this->ssl.Delete();
}

void SSWR::AVIRead::AVIRHTTPTestForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
