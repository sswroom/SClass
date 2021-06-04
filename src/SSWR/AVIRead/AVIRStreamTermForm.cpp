#include "Stdafx.h"
#include "IO/SerialPort.h"
#include "SSWR/AVIRead/AVIRStreamTermForm.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/Encoding.h"

void __stdcall SSWR::AVIRead::AVIRStreamTermForm::OnStreamClicked(void *userObj)
{
	SSWR::AVIRead::AVIRStreamTermForm *me = (SSWR::AVIRead::AVIRStreamTermForm *)userObj;
	if (me->stm)
	{
		me->StopStream();
	}
	else
	{
		SSWR::AVIRead::AVIRCore::StreamType st;
		me->stm = me->core->OpenStream(&st, me, 0, false);
		if (me->stm)
		{
			me->txtStream->SetText(SSWR::AVIRead::AVIRCore::GetStreamTypeName(st));
			me->btnStream->SetText((const UTF8Char*)"&Close");
			me->remoteClosed = false;
			me->threadRunning = false;
			me->threadToStop = false;
			me->recvUpdated = false;
			me->sendBuff->Clear();
			me->recvBuff->Clear();
			me->UpdateSendDisp();
			me->UpdateRecvDisp();

			Sync::Thread::Create(RecvThread, me);
			while (!me->threadRunning && !me->remoteClosed)
			{
				Sync::Thread::Sleep(10);
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
		OSInt size;
		me->txtSendValue->GetText(&sb);
		if (me->radSendText->IsSelected())
		{
			switch (me->cboSendLBreak->GetSelectedIndex())
			{
			case 0:
				sb.Append((const UTF8Char*)"\r\n");
				break;
			case 1:
				sb.Append((const UTF8Char*)"\r");
				break;
			case 2:
				sb.Append((const UTF8Char*)"\n");
				break;
			case 3:
				break;
			}
			if (sb.GetLength() > 0)
			{
				me->stm->Write(sb.ToString(), sb.GetLength());
				me->sendBuff->Write(sb.ToString(), sb.GetLength());
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
			me->sendBuff->Write(buff, size);
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
		me->StopStream();
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
	OSInt recvSize;
	me->threadRunning = true;
	while (!me->threadToStop)
	{
		recvSize = me->stm->Read(buff, 2048);
		if (recvSize <= 0)
		{
			me->remoteClosed = true;
		}
		else
		{
			Sync::MutexUsage mutUsage(me->recvMut);
			me->recvBuff->Write(buff, recvSize);
			mutUsage.EndUse();
			me->recvUpdated = true;
		}
	}
	me->threadRunning = false;
	return 0;
}

void SSWR::AVIRead::AVIRStreamTermForm::StopStream()
{
	if (this->stm)
	{
		this->stm->Close();
		this->threadToStop = true;
		while (this->threadRunning)
		{
			Sync::Thread::Sleep(10);
		}
		this->threadToStop = false;
		DEL_CLASS(this->stm);
		this->stm = 0;
		this->txtStream->SetText((const UTF8Char*)"-");
		this->btnStream->SetText((const UTF8Char*)"&Open");
		this->remoteClosed = false;
	}
}

void SSWR::AVIRead::AVIRStreamTermForm::UpdateRecvDisp()
{
	UInt8 *buff;
	UOSInt buffSize;
	UOSInt j;
	Sync::MutexUsage mutUsage(this->recvMut);
	buff = this->recvBuff->GetBuff(&buffSize);
	OSInt i = this->cboRecvType->GetSelectedIndex();
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
			this->txtRecvDisp->SetText(sbuff);
			MemFree(sbuff);
		}
		else
		{
			UTF8Char *u8buff = MemAlloc(UTF8Char, buffSize + 1);
			MemCopyNO(u8buff, buff, buffSize);
			u8buff[buffSize] = 0;
			this->txtRecvDisp->SetText(u8buff);
			MemFree(u8buff);
		}
	}
	else
	{
		this->txtRecvDisp->SetText((const UTF8Char*)"");
	}
	mutUsage.EndUse();
}

void SSWR::AVIRead::AVIRStreamTermForm::UpdateSendDisp()
{
	UInt8 *buff;
	UOSInt buffSize;
	buff = this->sendBuff->GetBuff(&buffSize);
	OSInt i = this->cboSendType->GetSelectedIndex();
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
			this->txtSendDisp->SetText(sbuff);
			MemFree(sbuff);
		}
		else
		{
			UTF8Char *u8buff = MemAlloc(UTF8Char, buffSize + 1);
			MemCopyNO(u8buff, buff, buffSize);
			u8buff[buffSize] = 0;
			this->txtSendDisp->SetText(u8buff);
			MemFree(u8buff);
		}
	}
	else
	{
		this->txtSendDisp->SetText((const UTF8Char*)"");
	}
}

SSWR::AVIRead::AVIRStreamTermForm::AVIRStreamTermForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 456, 200, ui)
{
	this->SetText((const UTF8Char*)"Stream Terminal");
	this->SetFont(0, 8.25, false);
	
	this->core = core;
	this->siLabDriver = this->core->GetSiLabDriver();
	this->stm = 0;
	this->threadToStop = false;
	this->threadRunning = false;
	this->recvUpdated = false;
	NEW_CLASS(this->recvBuff, IO::MemoryStream((const UTF8Char*)"SSWR.AVIRead.AVIRStreamTermForm.recvBuff"));
	NEW_CLASS(this->sendBuff, IO::MemoryStream((const UTF8Char*)"SSWR.AVIRead.AVIRStreamTermForm.sendBuff"));
	NEW_CLASS(this->recvMut, Sync::Mutex());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->grpStream, UI::GUIGroupBox(ui, this, (const UTF8Char*)"Stream"));
	this->grpStream->SetRect(0, 0, 100, 48, false);
	this->grpStream->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblStream, UI::GUILabel(ui, this->grpStream, (const UTF8Char*)"Stream Type"));
	this->lblStream->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtStream, UI::GUITextBox(ui, this->grpStream, (const UTF8Char*)"-"));
	this->txtStream->SetRect(104, 4, 200, 23, false);
	this->txtStream->SetReadOnly(true);
	NEW_CLASS(this->btnStream, UI::GUIButton(ui, this->grpStream, (const UTF8Char*)"&Open"));
	this->btnStream->SetRect(304, 4, 75, 23, false);
	this->btnStream->HandleButtonClick(OnStreamClicked, this);
	
	NEW_CLASS(this->grpRecv, UI::GUIGroupBox(ui, this, (const UTF8Char*)"Receive"));
	this->grpRecv->SetRect(0, 0, 256, 100, false);
	this->grpRecv->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->hspMain, UI::GUIHSplitter(ui, this, 3, false));
	NEW_CLASS(this->grpSend, UI::GUIGroupBox(ui, this, (const UTF8Char*)"Send"));
	this->grpSend->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->cboRecvType, UI::GUIComboBox(ui, this->grpRecv, false));
	this->cboRecvType->SetRect(0, 0, 100, 23, false);
	this->cboRecvType->SetDockType(UI::GUIControl::DOCK_TOP);
	this->cboRecvType->AddItem((const UTF8Char*)"Display as Hex", 0);
	this->cboRecvType->AddItem((const UTF8Char*)"Display as Text", 0);
	this->cboRecvType->SetSelectedIndex(0);
	this->cboRecvType->HandleSelectionChange(OnRecvTypeChg, this);
	NEW_CLASS(this->txtRecvDisp, UI::GUITextBox(ui, this->grpRecv, (const UTF8Char*)"", true));
	this->txtRecvDisp->SetReadOnly(true);
	this->txtRecvDisp->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->pnlSendOption, UI::GUIPanel(ui, this->grpSend));
	this->pnlSendOption->SetRect(0, 0, 100, 72, false);
	this->pnlSendOption->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->pnlSend, UI::GUIPanel(ui, this->pnlSendOption));
	this->pnlSend->SetRect(0, 0, 100, 24, false);
	this->pnlSend->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->btnSend, UI::GUIButton(ui, this->pnlSend, (const UTF8Char*)"&Send"));
	this->btnSend->SetRect(0, 0, 75, 23, false);
	this->btnSend->SetDockType(UI::GUIControl::DOCK_RIGHT);
	this->btnSend->HandleButtonClick(OnSendClicked, this);
	NEW_CLASS(this->lblSendLBreak, UI::GUILabel(ui, this->pnlSendOption, (const UTF8Char*)"Line Break"));
	this->lblSendLBreak->SetRect(0, 24, 100, 23, false);
	NEW_CLASS(this->cboSendLBreak, UI::GUIComboBox(ui, this->pnlSendOption, false));
	this->cboSendLBreak->SetRect(100, 24, 100, 23, false);
	this->cboSendLBreak->AddItem((const UTF8Char*)"CR+LF", 0);
	this->cboSendLBreak->AddItem((const UTF8Char*)"CR", 0);
	this->cboSendLBreak->AddItem((const UTF8Char*)"LF", 0);
	this->cboSendLBreak->AddItem((const UTF8Char*)"<null>", 0);
	this->cboSendLBreak->SetSelectedIndex(1);
	NEW_CLASS(this->radSendText, UI::GUIRadioButton(ui, this->pnlSendOption, (const UTF8Char*)"Text", true));
	this->radSendText->SetRect(0, 0, 100, 23, false);
	NEW_CLASS(this->radSendHex, UI::GUIRadioButton(ui, this->pnlSendOption, (const UTF8Char*)"Hex", false));
	this->radSendHex->SetRect(100, 0, 100, 23, false);
	NEW_CLASS(this->txtSendValue, UI::GUITextBox(ui, this->pnlSend, (const UTF8Char*)""));
	this->txtSendValue->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->cboSendType, UI::GUIComboBox(ui, this->grpSend, false));
	this->cboSendType->SetRect(0, 0, 100, 23, false);
	this->cboSendType->SetDockType(UI::GUIControl::DOCK_TOP);
	this->cboSendType->AddItem((const UTF8Char*)"Display as Hex", 0);
	this->cboSendType->AddItem((const UTF8Char*)"Display as Text", 0);
	this->cboSendType->SetSelectedIndex(0);
	this->cboSendType->HandleSelectionChange(OnSendTypeChg, this);
	NEW_CLASS(this->txtSendDisp, UI::GUITextBox(ui, this->grpSend, (const UTF8Char*)"", true));
	this->txtSendDisp->SetReadOnly(true);
	this->txtSendDisp->SetDockType(UI::GUIControl::DOCK_FILL);
	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRStreamTermForm::~AVIRStreamTermForm()
{
	StopStream();
	DEL_CLASS(this->recvBuff);
	DEL_CLASS(this->sendBuff);
	DEL_CLASS(this->recvMut);
}

void SSWR::AVIRead::AVIRStreamTermForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
