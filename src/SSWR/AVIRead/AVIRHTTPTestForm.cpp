#include "Stdafx.h"
#include "Manage/HiResClock.h"
#include "Net/HTTPClient.h"
#include "Net/SSLEngineFactory.h"
#include "Net/JMeter/JMeterHTTPSamplerProxy.h"
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
	if (me->connSteps.GetCount() <= 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter at least 1 step"), CSTR("Start"), me);
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
	
	UnsafeArray<NN<Sync::Thread>> threads;
	UOSInt i;
	me->connCnt = 0;
	me->failCnt = 0;
	me->threadCurrCnt = 0;
	me->totalSize = 0;
	me->t = 0;
	me->threads = threads = MemAllocArr(NN<Sync::Thread>, me->threadCnt);
	me->clk.Start();
	i = me->threadCnt;
	while (i-- > 0)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("HTTPTest"));
		sb.AppendUOSInt(i);
		NEW_CLASSNN(threads[i], Sync::Thread(ProcessThread, me, sb.ToCString()));
		threads[i]->Start();
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
	NN<Net::JMeter::JMeterHTTPSamplerProxy> httpRequest;
	Net::WebUtil::RequestMethod method = (Net::WebUtil::RequestMethod)me->cboMethod->GetSelectedItem().GetOSInt();
	if (sb.StartsWith(UTF8STRC("http://")) || sb.StartsWith(UTF8STRC("https://")))
	{
		NEW_CLASSNN(httpRequest, Net::JMeter::JMeterHTTPSamplerProxy(CSTR("HTTP Request"), method, sb.ToCString(), false, me->chkKAConn->IsChecked(), me->chkGZip->IsChecked()));
		if (method == Net::WebUtil::RequestMethod::HTTP_POST)
		{
			Text::StringBuilderUTF8 sbData;
			sb.ClearStr();
			me->cboPostType->GetText(sb);
			me->txtPostData->GetText(sbData);
			if (sb.leng == 0)
			{
				me->ui->ShowMsgOK(CSTR("Please select post type"), CSTR("Add"), me);
			}
			else if (sbData.leng == 0)
			{
				me->ui->ShowMsgOK(CSTR("Please input post data"), CSTR("Add"), me);
			}
			else
			{
				httpRequest->SetPostContent(sb.ToCString(), sbData.ToByteArray());
				me->connSteps.Add(httpRequest);
				sb.ClearStr();
				httpRequest->ToString(sb);
				me->lbURL->AddItem(sb.ToCString(), 0);
				me->txtURL->SetText(CSTR(""));
			}
		}
		else
		{
			me->connSteps.Add(httpRequest);
			sb.ClearStr();
			httpRequest->ToString(sb);
			me->lbURL->AddItem(sb.ToCString(), 0);
			me->txtURL->SetText(CSTR(""));
		}
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
	me->ClearSteps();
}

void __stdcall SSWR::AVIRead::AVIRHTTPTestForm::ProcessThread(NN<Sync::Thread> thread)
{
	NN<SSWR::AVIRead::AVIRHTTPTestForm> me = thread->GetUserObj().GetNN<SSWR::AVIRead::AVIRHTTPTestForm>();
	UOSInt i;
	UOSInt j;
	UInt32 cnt;
	Data::ArrayListNN<Net::JMeter::JMeterStep> steps;
	NN<Net::JMeter::JMeterStep> step;
	Sync::Interlocked::IncrementU32(me->threadCurrCnt);
	steps.AddAll(me->connSteps);
	while (!thread->IsStopping())
	{
		{
			Sync::MutexUsage mutUsage(me->connMut);
			if (me->connLeftCnt <= 0)
				break;
			me->connLeftCnt--;
		}
		Net::JMeter::JMeterIteration iter(me->clif, me->ssl);
		iter.AddListener(me->cookieManager);
		Net::JMeter::JMeterResult result;
		i = 0;
		j = steps.GetCount();
		while (i < j)
		{
			step = steps.GetItemNoCheck(i);
			if (step->Step(iter, result))
			{
				Sync::Interlocked::IncrementU32(me->connCnt);
				Sync::Interlocked::AddU64(me->totalSize, result.dataSize);
			}
			else
			{
				Sync::Interlocked::IncrementU32(me->failCnt);
			}
			i++;
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
	sptr = Text::StrUInt64(sbuff, me->totalSize);
	me->txtTotalSize->SetText(CSTRP(sbuff, sptr));
	if (me->t <= 0 || me->connSteps.GetCount() == 0)
	{
		me->txtReqPerSec->SetText(CSTR("0"));
		me->txtDataRate->SetText(CSTR("0"));
	}
	else
	{
		sptr = Text::StrDouble(sbuff, me->connCnt / me->t / (Double)me->connSteps.GetCount());
		me->txtReqPerSec->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, (Double)me->totalSize / me->t);
		me->txtDataRate->SetText(CSTRP(sbuff, sptr));
	}
}

void SSWR::AVIRead::AVIRHTTPTestForm::StopThreads()
{
	UnsafeArray<NN<Sync::Thread>> threads;
	if (this->threads.SetTo(threads))
	{
		if (this->threadCurrCnt > 0)
		{
			UOSInt i = this->threadCnt;
			while (i-- > 0)
			{
				threads[i]->BeginStop();
			}
			i = this->threadCnt;
			while (i-- > 0)
			{
				threads[i]->WaitForEnd();
			}
		}
		UOSInt i = this->threadCnt;
		while (i-- > 0)
		{
			threads[i].Delete();
		}
		MemFreeArr(threads);
		this->threads = nullptr;
		this->threadCnt = 0;
	}
}

void SSWR::AVIRead::AVIRHTTPTestForm::ClearSteps()
{
	this->connSteps.DeleteAll();
	if (this->children.GetCount() > 0)
	{
		this->lbURL->ClearItems();
	}
}

SSWR::AVIRead::AVIRHTTPTestForm::AVIRHTTPTestForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui), cookieManager(CSTR("HTTP Cookie Manager"))
{
	this->SetFont(nullptr, 8.25, false);
	this->SetText(CSTR("HTTP Test"));

	this->core = core;
	this->clif = core->GetTCPClientFactory();
	this->ssl = Net::SSLEngineFactory::Create(this->clif, true);
	this->threads = nullptr;
	this->connCurrIndex = 0;
	this->connLeftCnt = 0;
	this->threadCnt = 0;
	this->threadCurrCnt = 0;
	this->connCnt = 0;
	this->failCnt = 0;
	this->totalSize = 0;
	this->t = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->grpStatus = ui->NewGroupBox(*this, CSTR("Status"));
	this->grpStatus->SetRect(0, 0, 100, 208, false);
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
	this->lblTotalSize = ui->NewLabel(this->grpStatus, CSTR("Total Size"));
	this->lblTotalSize->SetRect(4, 124, 100, 23, false);
	this->txtTotalSize = ui->NewTextBox(this->grpStatus, CSTR(""));
	this->txtTotalSize->SetRect(104, 124, 150, 23, false);
	this->txtTotalSize->SetReadOnly(true);
	this->lblReqPerSec = ui->NewLabel(this->grpStatus, CSTR("Req/s"));
	this->lblReqPerSec->SetRect(4, 148, 100, 23, false);
	this->txtReqPerSec = ui->NewTextBox(this->grpStatus, CSTR(""));
	this->txtReqPerSec->SetRect(104, 148, 150, 23, false);
	this->txtReqPerSec->SetReadOnly(true);
	this->lblDataRate = ui->NewLabel(this->grpStatus, CSTR("Data Rate(Byte/s)"));
	this->lblDataRate->SetRect(4, 172, 100, 23, false);
	this->txtDataRate = ui->NewTextBox(this->grpStatus, CSTR(""));
	this->txtDataRate->SetRect(104, 172, 150, 23, false);
	this->txtDataRate->SetReadOnly(true);

	this->pnlRequest = ui->NewPanel(*this);
	this->pnlRequest->SetRect(0, 0, 100, 80, false);
	this->pnlRequest->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lblConcurrCnt = ui->NewLabel(this->pnlRequest, CSTR("Concurrent Count"));
	this->lblConcurrCnt->SetRect(4, 4, 100, 23, false);
	this->txtConcurrCnt = ui->NewTextBox(this->pnlRequest, CSTR("10"));
	this->txtConcurrCnt->SetRect(104, 4, 100, 23, false);
	this->lblTotalConnCnt = ui->NewLabel(this->pnlRequest, CSTR("Total Conn Count"));
	this->lblTotalConnCnt->SetRect(4, 28, 100, 23, false);
	this->txtTotalConnCnt = ui->NewTextBox(this->pnlRequest, CSTR("100000"));
	this->txtTotalConnCnt->SetRect(104, 28, 100, 23, false);
	this->btnStart = ui->NewButton(this->pnlRequest, CSTR("Start"));
	this->btnStart->SetRect(104, 52, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	
	this->grpURL = ui->NewGroupBox(*this, CSTR("URL"));
	this->grpURL->SetDockType(UI::GUIControl::DOCK_FILL);
	this->pnlURL = ui->NewPanel(this->grpURL);
	this->pnlURL->SetRect(0, 0, 100, 71, false);
	this->pnlURL->SetDockType(UI::GUIControl::DOCK_TOP);
	this->pnlURLCtrl = ui->NewPanel(this->grpURL);
	this->pnlURLCtrl->SetRect(0, 0, 100, 31, false);
	this->pnlURLCtrl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->cboMethod = ui->NewComboBox(this->pnlURL, false);
	this->cboMethod->SetRect(4, 0, 100, 23, false);
	this->cboMethod->AddItem(CSTR("GET"), (void*)(OSInt)Net::WebUtil::RequestMethod::HTTP_GET);
	this->cboMethod->AddItem(CSTR("POST"), (void*)(OSInt)Net::WebUtil::RequestMethod::HTTP_POST);
	this->cboMethod->SetSelectedIndex(0);
	this->txtURL = ui->NewTextBox(this->pnlURL, CSTR(""));
	this->txtURL->SetRect(104, 0, 700, 23, false);
	this->lblPostData = ui->NewLabel(this->pnlURL, CSTR("POST Data"));
	this->lblPostData->SetRect(4, 24, 100, 23, false);
	this->cboPostType = ui->NewComboBox(this->pnlURL, false);
	this->cboPostType->SetRect(104, 24, 200, 23, false);
	this->cboPostType->AddItem(CSTR("application/x-www-form-urlencoded"), 0);
	this->cboPostType->SetSelectedIndex(0);
	this->txtPostData = ui->NewTextBox(this->pnlURL, CSTR(""));
	this->txtPostData->SetRect(304, 24, 500, 23, false);
	this->chkKAConn = ui->NewCheckBox(this->pnlURL, CSTR("KA Conn"), false);
	this->chkKAConn->SetRect(4, 48, 100, 23, false);
	this->chkGZip = ui->NewCheckBox(this->pnlURL, CSTR("GZip"), false);
	this->chkGZip->SetRect(104, 48, 100, 23, false);
	this->btnURLAdd = ui->NewButton(this->pnlURL, CSTR("&Add"));
	this->btnURLAdd->SetRect(204, 48, 75, 23, false);
	this->btnURLAdd->HandleButtonClick(OnURLAddClicked, this);

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
	this->ClearSteps();
	this->ssl.Delete();
}

void SSWR::AVIRead::AVIRHTTPTestForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
