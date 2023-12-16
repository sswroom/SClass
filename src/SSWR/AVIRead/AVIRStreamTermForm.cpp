#include "Stdafx.h"
#include "IO/SerialPort.h"
#include "SSWR/AVIRead/AVIRStreamTermForm.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/Encoding.h"

void __stdcall SSWR::AVIRead::AVIRStreamTermForm::OnStreamClicked(void *userObj)
{
	SSWR::AVIRead::AVIRStreamTermForm *me = (SSWR::AVIRead::AVIRStreamTermForm *)userObj;
	if (me->stm)
	{
		me->StopStream(true);
	}
	else
	{
		IO::StreamType st;
		me->stm = me->core->OpenStream(&st, me, 0, false);
		if (me->stm)
		{
			me->txtStream->SetText(IO::StreamTypeGetName(st));
			me->btnStream->SetText(CSTR("&Close"));
			me->remoteClosed = false;
			me->threadRunning = false;
			me->threadToStop = false;
			me->recvUpdated = false;
			me->sendBuff.Clear();
			me->recvBuff.Clear();
			me->UpdateSendDisp();
			me->UpdateRecvDisp();

			Sync::ThreadUtil::Create(RecvThread, me);
			while (!me->threadRunning && !me->remoteClosed)
			{
				Sync::SimpleThread::Sleep(10);
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRStreamTermForm::OnSendClicked(void *userObj)
{
	SSWR::AVIRead::AVIRStreamTermForm *me = (SSWR::AVIRead::AVIRStreamTermForm *)userObj;
	if (me->stm)
	{
		Text::StringBuilderUTF8 sb;
		UOSInt size;
		me->txtSendValue->GetText(sb);
		if (me->radSendText->IsSelected())
		{
			switch (me->cboSendLBreak->GetSelectedIndex())
			{
			case 0:
				sb.AppendC(UTF8STRC("\r\n"));
				break;
			case 1:
				sb.AppendC(UTF8STRC("\r"));
				break;
			case 2:
				sb.AppendC(UTF8STRC("\n"));
				break;
			case 3:
				break;
			}
			if (sb.GetLength() > 0)
			{
				me->stm->Write(sb.ToString(), sb.GetLength());
				me->sendBuff.Write(sb.ToString(), sb.GetLength());
				me->UpdateSendDisp();
			}
		}
		else
		{
			UInt8 *buff = MemAlloc(UInt8, (sb.GetLength() >> 1) + 2);
			size = sb.Hex2Bytes(buff);
			switch (me->cboSendLBreak->GetSelectedIndex())
			{
			case 0:
				buff[size] = '\r';
				buff[size + 1] = '\n';
				size += 2;
				break;
			case 1:
				buff[size] = '\r';
				size++;
				break;
			case 2:
				buff[size] = '\n';
				size++;
				break;
			case 3:
				break;
			}
			me->stm->Write(buff, size);
			me->sendBuff.Write(buff, size);
			MemFree(buff);
			me->UpdateSendDisp();
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRStreamTermForm::OnRecvTypeChg(void *userObj)
{
	SSWR::AVIRead::AVIRStreamTermForm *me = (SSWR::AVIRead::AVIRStreamTermForm *)userObj;
	me->UpdateRecvDisp();
}

void __stdcall SSWR::AVIRead::AVIRStreamTermForm::OnSendTypeChg(void *userObj)
{
	SSWR::AVIRead::AVIRStreamTermForm *me = (SSWR::AVIRead::AVIRStreamTermForm *)userObj;
	me->UpdateSendDisp();
}

void __stdcall SSWR::AVIRead::AVIRStreamTermForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRStreamTermForm *me = (SSWR::AVIRead::AVIRStreamTermForm *)userObj;
	if (me->remoteClosed)
	{
		me->remoteClosed = false;
		me->StopStream(true);
	}
	if (me->recvUpdated)
	{
		me->recvUpdated = false;
		me->UpdateRecvDisp();
	}
}

UInt32 __stdcall SSWR::AVIRead::AVIRStreamTermForm::RecvThread(void *userObj)
{
	SSWR::AVIRead::AVIRStreamTermForm *me = (SSWR::AVIRead::AVIRStreamTermForm *)userObj;
	UInt8 buff[2048];
	UOSInt recvSize;
	me->threadRunning = true;
	while (!me->threadToStop)
	{
		recvSize = me->stm->Read(BYTEARR(buff));
		if (recvSize <= 0)
		{
			me->remoteClosed = true;
		}
		else
		{
			Sync::MutexUsage mutUsage(me->recvMut);
			me->recvBuff.Write(buff, recvSize);
			mutUsage.EndUse();
			me->recvUpdated = true;
		}
	}
	me->threadRunning = false;
	return 0;
}

void SSWR::AVIRead::AVIRStreamTermForm::StopStream(Bool clearUI)
{
	if (this->stm)
	{
		this->stm->Close();
		this->threadToStop = true;
		while (this->threadRunning)
		{
			Sync::SimpleThread::Sleep(10);
		}
		this->threadToStop = false;
		DEL_CLASS(this->stm);
		this->stm = 0;
		if (clearUI)
		{
			this->txtStream->SetText(CSTR("-"));
			this->btnStream->SetText(CSTR("&Open"));
		}
		this->remoteClosed = false;
	}
}

void SSWR::AVIRead::AVIRStreamTermForm::UpdateRecvDisp()
{
	UInt8 *buff;
	UOSInt buffSize;
	UOSInt j;
	Sync::MutexUsage mutUsage(this->recvMut);
	buff = this->recvBuff.GetBuff(buffSize);
	UOSInt i = this->cboRecvType->GetSelectedIndex();
	if (buffSize > 0)
	{
		if (i == 0)
		{
			UTF8Char *sbuff = MemAlloc(UTF8Char, buffSize * 3 + 1 + (buffSize >> 4));
			UTF8Char *sptr = sbuff;
			j = 0;
			while ((buffSize - j) > 16)
			{
				sptr = Text::StrHexBytes(sptr, &buff[j], 16, ' ');
				*sptr++ = '\r';
				*sptr++ = '\n';
				j += 16;
			}
			sptr = Text::StrHexBytes(sptr, &buff[j], buffSize - j, ' ');
			this->txtRecvDisp->SetText(CSTRP(sbuff, sptr));
			MemFree(sbuff);
		}
		else
		{
			UTF8Char *sbuff = MemAlloc(UTF8Char, buffSize + 1);
			MemCopyNO(sbuff, buff, buffSize);
			sbuff[buffSize] = 0;
			this->txtRecvDisp->SetText({sbuff, buffSize});
			MemFree(sbuff);
		}
	}
	else
	{
		this->txtRecvDisp->SetText(CSTR(""));
	}
	mutUsage.EndUse();
}

void SSWR::AVIRead::AVIRStreamTermForm::UpdateSendDisp()
{
	UInt8 *buff;
	UOSInt buffSize;
	buff = this->sendBuff.GetBuff(buffSize);
	UOSInt i = this->cboSendType->GetSelectedIndex();
	UOSInt j;
	if (buffSize > 0)
	{
		if (i == 0)
		{
			UTF8Char *sbuff = MemAlloc(UTF8Char, buffSize * 3 + 1 + (buffSize >> 4));
			UTF8Char *sptr = sbuff;
			j = 0;
			while ((buffSize - j) > 16)
			{
				sptr = Text::StrHexBytes(sptr, &buff[j], 16, ' ');
				*sptr++ = '\r';
				*sptr++ = '\n';
				j += 16;
			}
			sptr = Text::StrHexBytes(sptr, &buff[j], buffSize - j, ' ');
			this->txtSendDisp->SetText(CSTRP(sbuff, sptr));
			MemFree(sbuff);
		}
		else
		{
			UTF8Char *sbuff = MemAlloc(UTF8Char, buffSize + 1);
			MemCopyNO(sbuff, buff, buffSize);
			sbuff[buffSize] = 0;
			this->txtSendDisp->SetText({sbuff, buffSize});
			MemFree(sbuff);
		}
	}
	else
	{
		this->txtSendDisp->SetText(CSTR(""));
	}
}

SSWR::AVIRead::AVIRStreamTermForm::AVIRStreamTermForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 456, 200, ui)
{
	this->SetText(CSTR("Stream Terminal"));
	this->SetFont(0, 0, 8.25, false);
	
	this->core = core;
	this->siLabDriver = this->core->GetSiLabDriver();
	this->stm = 0;
	this->threadToStop = false;
	this->threadRunning = false;
	this->recvUpdated = false;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->grpStream = ui->NewGroupBox(*this, CSTR("Stream"));
	this->grpStream->SetRect(0, 0, 100, 48, false);
	this->grpStream->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblStream, UI::GUILabel(ui, this->grpStream, CSTR("Stream Type")));
	this->lblStream->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtStream, UI::GUITextBox(ui, this->grpStream, CSTR("-")));
	this->txtStream->SetRect(104, 4, 200, 23, false);
	this->txtStream->SetReadOnly(true);
	NEW_CLASS(this->btnStream, UI::GUIButton(ui, this->grpStream, CSTR("&Open")));
	this->btnStream->SetRect(304, 4, 75, 23, false);
	this->btnStream->HandleButtonClick(OnStreamClicked, this);
	
	this->grpRecv = ui->NewGroupBox(*this, CSTR("Receive"));
	this->grpRecv->SetRect(0, 0, 256, 100, false);
	this->grpRecv->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->hspMain, UI::GUIHSplitter(ui, *this, 3, false));
	this->grpSend = ui->NewGroupBox(*this, CSTR("Send"));
	this->grpSend->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->cboRecvType, UI::GUIComboBox(ui, this->grpRecv, false));
	this->cboRecvType->SetRect(0, 0, 100, 23, false);
	this->cboRecvType->SetDockType(UI::GUIControl::DOCK_TOP);
	this->cboRecvType->AddItem(CSTR("Display as Hex"), 0);
	this->cboRecvType->AddItem(CSTR("Display as Text"), 0);
	this->cboRecvType->SetSelectedIndex(0);
	this->cboRecvType->HandleSelectionChange(OnRecvTypeChg, this);
	NEW_CLASS(this->txtRecvDisp, UI::GUITextBox(ui, this->grpRecv, CSTR(""), true));
	this->txtRecvDisp->SetReadOnly(true);
	this->txtRecvDisp->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASSNN(this->pnlSendOption, UI::GUIPanel(ui, this->grpSend));
	this->pnlSendOption->SetRect(0, 0, 100, 72, false);
	this->pnlSendOption->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASSNN(this->pnlSend, UI::GUIPanel(ui, this->pnlSendOption));
	this->pnlSend->SetRect(0, 0, 100, 24, false);
	this->pnlSend->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->btnSend, UI::GUIButton(ui, this->pnlSend, CSTR("&Send")));
	this->btnSend->SetRect(0, 0, 75, 23, false);
	this->btnSend->SetDockType(UI::GUIControl::DOCK_RIGHT);
	this->btnSend->HandleButtonClick(OnSendClicked, this);
	NEW_CLASS(this->lblSendLBreak, UI::GUILabel(ui, this->pnlSendOption, CSTR("Line Break")));
	this->lblSendLBreak->SetRect(0, 24, 100, 23, false);
	NEW_CLASS(this->cboSendLBreak, UI::GUIComboBox(ui, this->pnlSendOption, false));
	this->cboSendLBreak->SetRect(100, 24, 100, 23, false);
	this->cboSendLBreak->AddItem(CSTR("CR+LF"), 0);
	this->cboSendLBreak->AddItem(CSTR("CR"), 0);
	this->cboSendLBreak->AddItem(CSTR("LF"), 0);
	this->cboSendLBreak->AddItem(CSTR("<null>"), 0);
	this->cboSendLBreak->SetSelectedIndex(1);
	NEW_CLASS(this->radSendText, UI::GUIRadioButton(ui, this->pnlSendOption, CSTR("Text"), true));
	this->radSendText->SetRect(0, 0, 100, 23, false);
	NEW_CLASS(this->radSendHex, UI::GUIRadioButton(ui, this->pnlSendOption, CSTR("Hex"), false));
	this->radSendHex->SetRect(100, 0, 100, 23, false);
	NEW_CLASS(this->txtSendValue, UI::GUITextBox(ui, this->pnlSend, CSTR("")));
	this->txtSendValue->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->cboSendType, UI::GUIComboBox(ui, this->grpSend, false));
	this->cboSendType->SetRect(0, 0, 100, 23, false);
	this->cboSendType->SetDockType(UI::GUIControl::DOCK_TOP);
	this->cboSendType->AddItem(CSTR("Display as Hex"), 0);
	this->cboSendType->AddItem(CSTR("Display as Text"), 0);
	this->cboSendType->SetSelectedIndex(0);
	this->cboSendType->HandleSelectionChange(OnSendTypeChg, this);
	NEW_CLASS(this->txtSendDisp, UI::GUITextBox(ui, this->grpSend, CSTR(""), true));
	this->txtSendDisp->SetReadOnly(true);
	this->txtSendDisp->SetDockType(UI::GUIControl::DOCK_FILL);
	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRStreamTermForm::~AVIRStreamTermForm()
{
	StopStream(false);
}

void SSWR::AVIRead::AVIRStreamTermForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
