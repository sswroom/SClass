#include "Stdafx.h"
#include "IO/SerialPort.h"
#include "SSWR/AVIRead/AVIRStreamTermForm.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/Encoding.h"

void __stdcall SSWR::AVIRead::AVIRStreamTermForm::OnStreamClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRStreamTermForm> me = userObj.GetNN<SSWR::AVIRead::AVIRStreamTermForm>();
	if (me->stm.NotNull())
	{
		me->StopStream(true);
	}
	else
	{
		IO::StreamType st;
		me->stm = me->core->OpenStream(st, me, 0, false);
		if (me->stm.NotNull())
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

			Sync::ThreadUtil::Create(RecvThread, me.Ptr());
			while (!me->threadRunning && !me->remoteClosed)
			{
				Sync::SimpleThread::Sleep(10);
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRStreamTermForm::OnSendClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRStreamTermForm> me = userObj.GetNN<SSWR::AVIRead::AVIRStreamTermForm>();
	NN<IO::Stream> stm;
	if (me->stm.SetTo(stm))
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
				stm->Write(sb.ToByteArray());
				me->sendBuff.Write(sb.ToByteArray());
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
			stm->Write(Data::ByteArrayR(buff, size));
			me->sendBuff.Write(Data::ByteArrayR(buff, size));
			MemFree(buff);
			me->UpdateSendDisp();
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRStreamTermForm::OnRecvTypeChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRStreamTermForm> me = userObj.GetNN<SSWR::AVIRead::AVIRStreamTermForm>();
	me->UpdateRecvDisp();
}

void __stdcall SSWR::AVIRead::AVIRStreamTermForm::OnSendTypeChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRStreamTermForm> me = userObj.GetNN<SSWR::AVIRead::AVIRStreamTermForm>();
	me->UpdateSendDisp();
}

void __stdcall SSWR::AVIRead::AVIRStreamTermForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRStreamTermForm> me = userObj.GetNN<SSWR::AVIRead::AVIRStreamTermForm>();
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

UInt32 __stdcall SSWR::AVIRead::AVIRStreamTermForm::RecvThread(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRStreamTermForm> me = userObj.GetNN<SSWR::AVIRead::AVIRStreamTermForm>();
	UInt8 buff[2048];
	UOSInt recvSize;
	NN<IO::Stream> stm;
	me->threadRunning = true;
	if (me->stm.SetTo(stm))
	{
		while (!me->threadToStop)
		{
			recvSize = stm->Read(BYTEARR(buff));
			if (recvSize <= 0)
			{
				me->remoteClosed = true;
			}
			else
			{
				Sync::MutexUsage mutUsage(me->recvMut);
				me->recvBuff.Write(Data::ByteArrayR(buff, recvSize));
				mutUsage.EndUse();
				me->recvUpdated = true;
			}
		}
	}
	me->threadRunning = false;
	return 0;
}

void SSWR::AVIRead::AVIRStreamTermForm::StopStream(Bool clearUI)
{
	NN<IO::Stream> stm;
	if (this->stm.SetTo(stm))
	{
		stm->Close();
		this->threadToStop = true;
		while (this->threadRunning)
		{
			Sync::SimpleThread::Sleep(10);
		}
		this->threadToStop = false;
		stm.Delete();
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
	UnsafeArray<UInt8> buff;
	UOSInt buffSize;
	UOSInt j;
	Sync::MutexUsage mutUsage(this->recvMut);
	buff = this->recvBuff.GetBuff(buffSize);
	UOSInt i = this->cboRecvType->GetSelectedIndex();
	if (buffSize > 0)
	{
		if (i == 0)
		{
			UnsafeArray<UTF8Char> sbuff = MemAllocArr(UTF8Char, buffSize * 3 + 1 + (buffSize >> 4));
			UnsafeArray<UTF8Char> sptr = sbuff;
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
			MemFreeArr(sbuff);
		}
		else
		{
			UnsafeArray<UTF8Char> sbuff = MemAllocArr(UTF8Char, buffSize + 1);
			MemCopyNO(sbuff.Ptr(), buff.Ptr(), buffSize);
			sbuff[buffSize] = 0;
			this->txtRecvDisp->SetText({sbuff, buffSize});
			MemFreeArr(sbuff);
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
	UnsafeArray<UInt8> buff;
	UOSInt buffSize;
	buff = this->sendBuff.GetBuff(buffSize);
	UOSInt i = this->cboSendType->GetSelectedIndex();
	UOSInt j;
	if (buffSize > 0)
	{
		if (i == 0)
		{
			UnsafeArray<UTF8Char> sbuff = MemAllocArr(UTF8Char, buffSize * 3 + 1 + (buffSize >> 4));
			UnsafeArray<UTF8Char> sptr = sbuff;
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
			MemFreeArr(sbuff);
		}
		else
		{
			UnsafeArray<UTF8Char> sbuff = MemAllocArr(UTF8Char, buffSize + 1);
			MemCopyNO(sbuff.Ptr(), buff.Ptr(), buffSize);
			sbuff[buffSize] = 0;
			this->txtSendDisp->SetText({sbuff, buffSize});
			MemFreeArr(sbuff);
		}
	}
	else
	{
		this->txtSendDisp->SetText(CSTR(""));
	}
}

SSWR::AVIRead::AVIRStreamTermForm::AVIRStreamTermForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 456, 200, ui)
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
	this->lblStream = ui->NewLabel(this->grpStream, CSTR("Stream Type"));
	this->lblStream->SetRect(4, 4, 100, 23, false);
	this->txtStream = ui->NewTextBox(this->grpStream, CSTR("-"));
	this->txtStream->SetRect(104, 4, 200, 23, false);
	this->txtStream->SetReadOnly(true);
	this->btnStream = ui->NewButton(this->grpStream, CSTR("&Open"));
	this->btnStream->SetRect(304, 4, 75, 23, false);
	this->btnStream->HandleButtonClick(OnStreamClicked, this);
	
	this->grpRecv = ui->NewGroupBox(*this, CSTR("Receive"));
	this->grpRecv->SetRect(0, 0, 256, 100, false);
	this->grpRecv->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->hspMain = ui->NewHSplitter(*this, 3, false);
	this->grpSend = ui->NewGroupBox(*this, CSTR("Send"));
	this->grpSend->SetDockType(UI::GUIControl::DOCK_FILL);
	this->cboRecvType = ui->NewComboBox(this->grpRecv, false);
	this->cboRecvType->SetRect(0, 0, 100, 23, false);
	this->cboRecvType->SetDockType(UI::GUIControl::DOCK_TOP);
	this->cboRecvType->AddItem(CSTR("Display as Hex"), 0);
	this->cboRecvType->AddItem(CSTR("Display as Text"), 0);
	this->cboRecvType->SetSelectedIndex(0);
	this->cboRecvType->HandleSelectionChange(OnRecvTypeChg, this);
	this->txtRecvDisp = ui->NewTextBox(this->grpRecv, CSTR(""), true);
	this->txtRecvDisp->SetReadOnly(true);
	this->txtRecvDisp->SetDockType(UI::GUIControl::DOCK_FILL);
	this->pnlSendOption = ui->NewPanel(this->grpSend);
	this->pnlSendOption->SetRect(0, 0, 100, 72, false);
	this->pnlSendOption->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->pnlSend = ui->NewPanel(this->pnlSendOption);
	this->pnlSend->SetRect(0, 0, 100, 24, false);
	this->pnlSend->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->btnSend = ui->NewButton(this->pnlSend, CSTR("&Send"));
	this->btnSend->SetRect(0, 0, 75, 23, false);
	this->btnSend->SetDockType(UI::GUIControl::DOCK_RIGHT);
	this->btnSend->HandleButtonClick(OnSendClicked, this);
	this->lblSendLBreak = ui->NewLabel(this->pnlSendOption, CSTR("Line Break"));
	this->lblSendLBreak->SetRect(0, 24, 100, 23, false);
	this->cboSendLBreak = ui->NewComboBox(this->pnlSendOption, false);
	this->cboSendLBreak->SetRect(100, 24, 100, 23, false);
	this->cboSendLBreak->AddItem(CSTR("CR+LF"), 0);
	this->cboSendLBreak->AddItem(CSTR("CR"), 0);
	this->cboSendLBreak->AddItem(CSTR("LF"), 0);
	this->cboSendLBreak->AddItem(CSTR("<null>"), 0);
	this->cboSendLBreak->SetSelectedIndex(1);
	this->radSendText = ui->NewRadioButton(this->pnlSendOption, CSTR("Text"), true);
	this->radSendText->SetRect(0, 0, 100, 23, false);
	this->radSendHex = ui->NewRadioButton(this->pnlSendOption, CSTR("Hex"), false);
	this->radSendHex->SetRect(100, 0, 100, 23, false);
	this->txtSendValue = ui->NewTextBox(this->pnlSend, CSTR(""));
	this->txtSendValue->SetDockType(UI::GUIControl::DOCK_FILL);
	this->cboSendType = ui->NewComboBox(this->grpSend, false);
	this->cboSendType->SetRect(0, 0, 100, 23, false);
	this->cboSendType->SetDockType(UI::GUIControl::DOCK_TOP);
	this->cboSendType->AddItem(CSTR("Display as Hex"), 0);
	this->cboSendType->AddItem(CSTR("Display as Text"), 0);
	this->cboSendType->SetSelectedIndex(0);
	this->cboSendType->HandleSelectionChange(OnSendTypeChg, this);
	this->txtSendDisp = ui->NewTextBox(this->grpSend, CSTR(""), true);
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
