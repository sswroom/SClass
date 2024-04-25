#include "Stdafx.h"
#include "Text/MyString.h"
#include "IO/FileStream.h"
#include "IO/SerialPort.h"
#include "IO/StreamWriter.h"
#include "Media/WaveInSource.h"
#include "SSWR/AVIRead/AVIRVoiceModemForm.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"
#include "UI/GUITabPage.h"

void __stdcall SSWR::AVIRead::AVIRVoiceModemForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRVoiceModemForm> me = userObj.GetNN<SSWR::AVIRead::AVIRVoiceModemForm>();
	if (me->toneChg)
	{
		me->toneChg = false;
		Sync::MutexUsage mutUsage(me->toneMut);
		me->txtDialTones->SetText(me->toneSb.ToCString());
	}

	if (me->hasEvt)
	{
		me->hasEvt = false;
		me->txtDialStatus->SetText(CSTR("Remote Disconnected"));
	}
}

void __stdcall SSWR::AVIRead::AVIRVoiceModemForm::OnDialClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRVoiceModemForm> me = userObj.GetNN<SSWR::AVIRead::AVIRVoiceModemForm>();
	if (!me->isConnected)
	{
		Text::StringBuilderUTF8 sb;
		UTF8Char phoneBuff[21];
		UTF8Char *cptr;
		UTF8Char *sptr;
		UTF8Char c;
		me->txtDialNum->GetText(sb);
		if (sb.GetLength() <= 0)
		{
			me->txtDialStatus->SetText(CSTR("Enter dial num"));
			return;
		}
		if (sb.GetLength() > 20)
		{
			me->txtDialStatus->SetText(CSTR("Dial num too long"));
			return;
		}
		sptr = sb.v;
		cptr = phoneBuff;
		while (true)
		{
			c = *sptr++;
			if (c == 0)
			{
				*cptr = 0;
				break;
			}
			else if (c == '*' || c == '#')
			{
				*cptr++ = c;
			}
			else if (c >= '0' && c <= '9')
			{
				*cptr++ = c;
			}
			else
			{
				me->txtDialStatus->SetText(CSTR("Dial num is not valid"));
				return;
			}
		}
		if (!me->modem->VoiceSetType(IO::Device::RockwellModemController::VT_VOICE))
		{
			me->txtDialStatus->SetText(CSTR("Failed to set Voice Type"));
			return;
		}
		if (!me->modem->VoiceSetVoiceLineType(IO::Device::RockwellModemController::VLT_EMULATION))
		{
			me->txtDialStatus->SetText(CSTR("Failed to set Voice Line Type"));
			return;
		}
		IO::ModemController::DialResult res = me->modem->VoiceToneDial(phoneBuff);
		if (res == IO::ModemController::DR_NO_CARRIER)
		{
			me->txtDialStatus->SetText(CSTR("No Carrier"));
		}
		else if (res == IO::ModemController::DR_NO_DIALTONE)
		{
			me->txtDialStatus->SetText(CSTR("No Dial Tone"));
		}
		else if (res == IO::ModemController::DR_CONNECT)
		{
			me->txtDialStatus->SetText(CSTR("Connected"));
			me->isConnected = true;
		}
		else if (res == IO::ModemController::DR_BUSY)
		{
			me->txtDialStatus->SetText(CSTR("Busy"));
		}
		else if (res == IO::ModemController::DR_ERROR)
		{
			me->txtDialStatus->SetText(CSTR("Error"));
		}
		else
		{
			me->txtDialStatus->SetText(CSTR("Unknown Error"));
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRVoiceModemForm::OnHangUpClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRVoiceModemForm> me = userObj.GetNN<SSWR::AVIRead::AVIRVoiceModemForm>();
	if (me->isConnected)
	{
		if (me->modem->HangUp())
		{
			me->txtDialStatus->SetText(CSTR("Hang Up Successfully"));
		}
		else
		{
			me->txtDialStatus->SetText(CSTR("Failed to Hang Up"));
		}
		me->isConnected = false;
	}
}

void __stdcall SSWR::AVIRead::AVIRVoiceModemForm::OnModemEvent(AnyType userObj, UInt8 evtType)
{
	NN<SSWR::AVIRead::AVIRVoiceModemForm> me = userObj.GetNN<SSWR::AVIRead::AVIRVoiceModemForm>();
	switch (evtType)
	{
	case 'b':
		if (me->isConnected)
		{
			me->modem->HangUp();
			me->evtType = 'b';
			me->isConnected = false;
			me->hasEvt = true;
		}
		break;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '*':
	case '#':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
		{
			Sync::MutexUsage mutUsage(me->toneMut);
			me->toneSb.AppendUTF8Char(evtType);
			me->toneChg = true;
		}
		break;
	}
}

SSWR::AVIRead::AVIRVoiceModemForm::AVIRVoiceModemForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<IO::Device::RockwellModemController> modem, NN<IO::ATCommandChannel> channel, NN<IO::Stream> port) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("Voice Modem"));
	this->SetFont(0, 0, 8.25, false);
	this->core = core;
	this->modem = modem;
	this->channel = channel;
	this->port = port;
	this->hasEvt = false;
	this->evtType = 0;
	this->channel->SetEventHandler(OnModemEvent, this);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->toneChg = false;
	this->isConnected = false;

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpInfo = this->tcMain->AddTabPage(CSTR("Info"));
	this->lblModemManu = ui->NewLabel(this->tpInfo, CSTR("Manufacture"));
	this->lblModemManu->SetRect(8, 8, 100, 23, false);
	this->txtModemManu = ui->NewTextBox(this->tpInfo, CSTR(""));
	this->txtModemManu->SetRect(108, 8, 200, 23, false);
	this->txtModemManu->SetReadOnly(true);
	this->lblModemModel = ui->NewLabel(this->tpInfo, CSTR("Model"));
	this->lblModemModel->SetRect(8, 32, 100, 23, false);
	this->txtModemModel = ui->NewTextBox(this->tpInfo, CSTR(""));
	this->txtModemModel->SetRect(108, 32, 200, 23, false);
	this->txtModemModel->SetReadOnly(true);
	this->lblModemRev = ui->NewLabel(this->tpInfo, CSTR("Revision"));
	this->lblModemRev->SetRect(8, 56, 100, 23, false);
	this->txtModemRev = ui->NewTextBox(this->tpInfo, CSTR(""));
	this->txtModemRev->SetRect(108, 56, 200, 23, false);
	this->txtModemRev->SetReadOnly(true);

	this->tpDial = this->tcMain->AddTabPage(CSTR("Dial"));
	this->lblDialNum = ui->NewLabel(this->tpDial, CSTR("Dial Num"));
	this->lblDialNum->SetRect(4, 4, 100, 23, false);
	this->txtDialNum = ui->NewTextBox(this->tpDial, CSTR(""));
	this->txtDialNum->SetRect(104, 4, 200, 23, false);
	this->btnDial = ui->NewButton(this->tpDial, CSTR("Dial"));
	this->btnDial->SetRect(104, 28, 75, 23, false);
	this->btnDial->HandleButtonClick(OnDialClicked, this);
	this->btnHangUp = ui->NewButton(this->tpDial, CSTR("Hang Up"));
	this->btnHangUp->SetRect(184, 28, 75, 23, false);
	this->btnHangUp->HandleButtonClick(OnHangUpClicked, this);
	this->lblDialStatus = ui->NewLabel(this->tpDial, CSTR("Status"));
	this->lblDialStatus->SetRect(4, 52, 250, 23, false);
	this->txtDialStatus = ui->NewTextBox(this->tpDial, CSTR(""));
	this->txtDialStatus->SetReadOnly(true);
	this->txtDialStatus->SetRect(104, 52, 200, 23, false);
	this->lblDialTones = ui->NewLabel(this->tpDial, CSTR("Tones"));
	this->lblDialTones->SetRect(4, 76, 100, 23, false);
	this->txtDialTones = ui->NewTextBox(this->tpDial, CSTR(""));
	this->txtDialTones->SetReadOnly(true);
	this->txtDialTones->SetRect(104, 76, 200, 23, false);

	UTF8Char sbuff[128];
	UTF8Char *sptr;
	if ((sptr = this->modem->VoiceGetManufacturer(sbuff)) != 0)
	{
		this->txtModemManu->SetText(CSTRP(sbuff, sptr));
	}
	else
	{
		this->txtModemManu->SetText(CSTR("Unknown"));
	}
	if ((sptr = this->modem->VoiceGetModel(sbuff)) != 0)
	{
		this->txtModemModel->SetText(CSTRP(sbuff, sptr));
	}
	else
	{
		this->txtModemModel->SetText(CSTR("Unknown"));
	}
	if ((sptr = this->modem->VoiceGetRevision(sbuff)) != 0)
	{
		this->txtModemRev->SetText(CSTRP(sbuff, sptr));
	}
	else
	{
		this->txtModemRev->SetText(CSTR("Unknown"));
	}
	this->AddTimer(200, OnTimerTick, this);
}

SSWR::AVIRead::AVIRVoiceModemForm::~AVIRVoiceModemForm()
{
	this->port->Close();
	this->channel->Close();
	this->modem.Delete();
	this->channel.Delete();
	this->port.Delete();
}

void SSWR::AVIRead::AVIRVoiceModemForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
