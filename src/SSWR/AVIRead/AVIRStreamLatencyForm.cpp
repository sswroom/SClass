#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Data/DateTime.h"
#include "IO/SerialPort.h"
#include "SSWR/AVIRead/AVIRStreamLatencyForm.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/Encoding.h"

void __stdcall SSWR::AVIRead::AVIRStreamLatencyForm::OnStreamClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRStreamLatencyForm> me = userObj.GetNN<SSWR::AVIRead::AVIRStreamLatencyForm>();
	if (me->stm)
	{
		me->StopStream();
	}
	else
	{
		IO::StreamType st;
		me->stm = me->core->OpenStream(st, me, 0, false);
		if (me->stm)
		{
			Data::DateTime dt;
			dt.SetCurrTimeUTC();
			me->lastSentTime = dt.ToTicks();

			me->txtStream->SetText(IO::StreamTypeGetName(st));
			me->btnStream->SetText(CSTR("&Close"));
			me->remoteClosed = false;
			me->threadRunning = false;
			me->threadToStop = false;

			Sync::ThreadUtil::Create(RecvThread, me);
			while (!me->threadRunning && !me->remoteClosed)
			{
				Sync::SimpleThread::Sleep(10);
			}
			me->log->LogMessage(CSTR("Stream Started"), IO::LogHandler::LogLevel::Action);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRStreamLatencyForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRStreamLatencyForm> me = userObj.GetNN<SSWR::AVIRead::AVIRStreamLatencyForm>();
	if (me->remoteClosed)
	{
		me->remoteClosed = false;
		me->StopStream();
	}
	if (me->stm)
	{
		UInt8 buff[11];
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		Int64 currTime = dt.ToTicks();
		OSInt interval = me->cboReqInterval->GetSelectedItem().GetOSInt();
		if ((currTime - me->lastSentTime) >= interval)
		{
			me->lastSentTime = currTime;
			buff[0] = 's';
			buff[1] = 'l';
			WriteInt64(&buff[2], currTime);
			buff[10] = buff[0] ^ buff[1] ^ buff[2] ^ buff[3] ^ buff[4] ^ buff[5] ^ buff[6] ^ buff[7] ^ buff[8] ^ buff[9];
			me->stm->Write(buff, 11);
			me->sentCnt++;
		}
	}
	UTF8Char sbuff[24];
	UTF8Char *sptr;
	OSInt cnt = me->recvCnt;
	if (me->dispRecv != cnt)
	{
		me->dispRecv = cnt;
		sptr = Text::StrOSInt(sbuff, cnt);
		me->txtRecvCnt->SetText(CSTRP(sbuff, sptr));
	}
	cnt = me->sentCnt;
	if (me->dispSent != cnt)
	{
		me->dispSent = cnt;
		sptr = Text::StrOSInt(sbuff, cnt);
		me->txtSentCnt->SetText(CSTRP(sbuff, sptr));
	}
}

UInt32 __stdcall SSWR::AVIRead::AVIRStreamLatencyForm::RecvThread(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRStreamLatencyForm> me = userObj.GetNN<SSWR::AVIRead::AVIRStreamLatencyForm>();
	UInt8 buff[2060];
	UOSInt buffSize = 0;
	UOSInt recvSize;
	UOSInt i;
	UInt8 chk;
	Double diff;
	{
		me->threadRunning = true;
		Data::DateTime dt;
		Text::StringBuilderUTF8 sb;
		while (!me->threadToStop)
		{
			recvSize = me->stm->Read(Data::ByteArray(&buff[buffSize], 2048));
			if (recvSize <= 0)
			{
				me->remoteClosed = true;
			}
			else
			{
				buffSize += recvSize;
				recvSize = 0;
				while (recvSize < buffSize - 10)
				{
					if (buff[recvSize] == 's' && buff[recvSize + 1] == 'l')
					{
						chk = buff[recvSize + 10];
						i = 10;
						while (i-- > 0)
						{
							chk = (UInt8)(chk ^ buff[recvSize + i]);
						}
						if (chk == 0)
						{
							dt.SetCurrTimeUTC();
							Int64 currTime = dt.ToTicks();
							diff = (Double)(currTime - ReadInt64(&buff[recvSize + 2]));
							me->rlcLatency->AddSample(&diff);
							sb.ClearStr();
							sb.AppendC(UTF8STRC("Received packet: diff = "));
							sb.AppendI64(currTime - ReadInt64(&buff[recvSize + 2]));
							me->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Command);
							me->recvCnt++;
							recvSize += 10;
						}
					}
					recvSize++;
				}
				if (recvSize >= buffSize)
				{
					buffSize = 0;
				}
				else
				{
					MemCopyO(buff, &buff[recvSize], buffSize - recvSize);
					buffSize -= recvSize;
				}
			}
		}
	}
	me->threadRunning = false;
	return 0;
}

void SSWR::AVIRead::AVIRStreamLatencyForm::StopStream()
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
		this->txtStream->SetText(CSTR("-"));
		this->btnStream->SetText(CSTR("&Open"));
		this->remoteClosed = false;
		this->log->LogMessage(CSTR("Stream Stopped"), IO::LogHandler::LogLevel::Action);
	}
}

SSWR::AVIRead::AVIRStreamLatencyForm::AVIRStreamLatencyForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 456, 200, ui)
{
	this->SetText(CSTR("Stream Latency"));
	this->SetFont(0, 0, 8.25, false);
	
	this->core = core;
	this->siLabDriver = this->core->GetSiLabDriver();
	this->stm = 0;
	this->threadToStop = false;
	this->threadRunning = false;
	this->sentCnt = 0;
	this->recvCnt = 0;
	this->dispSent = 0;
	this->dispRecv = 0;
	NEW_CLASS(this->log, IO::LogTool());
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
	
	this->grpControl = ui->NewGroupBox(*this, CSTR("Control"));
	this->grpControl->SetRect(0, 0, 100, 48, false);
	this->grpControl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblReqInterval = ui->NewLabel(this->grpControl, CSTR("Req Interval"));
	this->lblReqInterval->SetRect(4, 4, 100, 23, false);
	this->cboReqInterval = ui->NewComboBox(this->grpControl, false);
	this->cboReqInterval->SetRect(104, 4, 100, 23, false);
	this->cboReqInterval->AddItem(CSTR("1s"), (void *)1000);
	this->cboReqInterval->AddItem(CSTR("2s"), (void *)2000);
	this->cboReqInterval->AddItem(CSTR("3s"), (void *)3000);
	this->cboReqInterval->AddItem(CSTR("4s"), (void *)4000);
	this->cboReqInterval->AddItem(CSTR("5s"), (void *)5000);
	this->cboReqInterval->AddItem(CSTR("10s"), (void *)10000);
	this->cboReqInterval->AddItem(CSTR("15s"), (void *)15000);
	this->cboReqInterval->AddItem(CSTR("20s"), (void *)20000);
	this->cboReqInterval->AddItem(CSTR("30s"), (void *)30000);
	this->cboReqInterval->AddItem(CSTR("45s"), (void *)45000);
	this->cboReqInterval->AddItem(CSTR("1min"), (void *)60000);
	this->cboReqInterval->AddItem(CSTR("2min"), (void *)120000);
	this->cboReqInterval->SetSelectedIndex(8);

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpLatency = this->tcMain->AddTabPage(CSTR("Latency (ms)"));
	this->rlcLatency = ui->NewRealtimeLineChart(this->tpLatency, this->core->GetDrawEngine(), 1, 256, 1000);
	this->rlcLatency->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpLog = this->tcMain->AddTabPage(CSTR("Log"));
	this->txtLog = ui->NewTextBox(this->tpLog, CSTR(""));
	this->txtLog->SetReadOnly(true);
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lbLog = ui->NewListBox(this->tpLog, false);
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpStatus = this->tcMain->AddTabPage(CSTR("Status"));
	this->lblSentCnt = ui->NewLabel(this->tpStatus, CSTR("Sent Cnt"));
	this->lblSentCnt->SetRect(0, 0, 100, 23, false);
	this->txtSentCnt = ui->NewTextBox(this->tpStatus, CSTR("0"));
	this->txtSentCnt->SetRect(100, 0, 150, 23, false);
	this->txtSentCnt->SetReadOnly(true);
	this->lblRecvCnt = ui->NewLabel(this->tpStatus, CSTR("Recv Cnt"));
	this->lblRecvCnt->SetRect(0, 24, 100, 23, false);
	this->txtRecvCnt = ui->NewTextBox(this->tpStatus, CSTR("0"));
	this->txtRecvCnt->SetRect(100, 24, 150, 23, false);
	this->txtRecvCnt->SetReadOnly(true);

	NEW_CLASSNN(this->logger, UI::ListBoxLogger(*this, this->lbLog, 256, false));
	this->log->AddLogHandler(this->logger, IO::LogHandler::LogLevel::Raw);

	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRStreamLatencyForm::~AVIRStreamLatencyForm()
{
	StopStream();
	DEL_CLASS(this->log);
	this->logger.Delete();
}

void SSWR::AVIRead::AVIRStreamLatencyForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
