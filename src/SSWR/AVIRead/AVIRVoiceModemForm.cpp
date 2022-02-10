#include "Stdafx.h"
#include "Text/MyString.h"
#include "IO/FileStream.h"
#include "IO/SerialPort.h"
#include "IO/StreamWriter.h"
#include "Media/WaveInSource.h"
#include "SSWR/AVIRead/AVIRVoiceModemForm.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "UI/FileDialog.h"
#include "UI/GUITabPage.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRVoiceModemForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRVoiceModemForm *me = (SSWR::AVIRead::AVIRVoiceModemForm*)userObj;
	if (me->toneChg)
	{
		me->toneChg = false;
		Sync::MutexUsage mutUsage(me->toneMut);
		me->txtDialTones->SetText(me->toneSb->ToString());
		mutUsage.EndUse();
	}

	if (me->hasEvt)
	{
		me->hasEvt = false;
		me->txtDialStatus->SetText((const UTF8Char*)"Remote Disconnected");
	}
}

void __stdcall SSWR::AVIRead::AVIRVoiceModemForm::OnDialClicked(void *userObj)
{
	SSWR::AVIRead::AVIRVoiceModemForm *me = (SSWR::AVIRead::AVIRVoiceModemForm*)userObj;
	if (!me->isConnected)
	{
		Text::StringBuilderUTF8 sb;
		UTF8Char phoneBuff[21];
		UTF8Char *cptr;
		UTF8Char *sptr;
		UTF8Char c;
		me->txtDialNum->GetText(&sb);
		if (sb.GetLength() <= 0)
		{
			me->txtDialStatus->SetText((const UTF8Char*)"Enter dial num");
			return;
		}
		if (sb.GetLength() > 20)
		{
			me->txtDialStatus->SetText((const UTF8Char*)"Dial num too long");
			return;
		}
		sptr = sb.ToString();
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
				me->txtDialStatus->SetText((const UTF8Char*)"Dial num is not valid");
				return;
			}
		}
		if (!me->modem->VoiceSetType(IO::Device::RockwellModemController::VT_VOICE))
		{
			me->txtDialStatus->SetText((const UTF8Char*)"Failed to set Voice Type");
			return;
		}
		if (!me->modem->VoiceSetVoiceLineType(IO::Device::RockwellModemController::VLT_EMULATION))
		{
			me->txtDialStatus->SetText((const UTF8Char*)"Failed to set Voice Line Type");
			return;
		}
		IO::ModemController::DialResult res = me->modem->VoiceToneDial(phoneBuff);
		if (res == IO::ModemController::DR_NO_CARRIER)
		{
			me->txtDialStatus->SetText((const UTF8Char*)"No Carrier");
		}
		else if (res == IO::ModemController::DR_NO_DIALTONE)
		{
			me->txtDialStatus->SetText((const UTF8Char*)"No Dial Tone");
		}
		else if (res == IO::ModemController::DR_CONNECT)
		{
			me->txtDialStatus->SetText((const UTF8Char*)"Connected");
			me->isConnected = true;
		}
		else if (res == IO::ModemController::DR_BUSY)
		{
			me->txtDialStatus->SetText((const UTF8Char*)"Busy");
		}
		else if (res == IO::ModemController::DR_ERROR)
		{
			me->txtDialStatus->SetText((const UTF8Char*)"Error");
		}
		else
		{
			me->txtDialStatus->SetText((const UTF8Char*)"Unknown Error");
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRVoiceModemForm::OnHangUpClicked(void *userObj)
{
	SSWR::AVIRead::AVIRVoiceModemForm *me = (SSWR::AVIRead::AVIRVoiceModemForm*)userObj;
	if (me->isConnected)
	{
		if (me->modem->HangUp())
		{
			me->txtDialStatus->SetText((const UTF8Char*)"Hang Up Successfully");
		}
		else
		{
			me->txtDialStatus->SetText((const UTF8Char*)"Failed to Hang Up");
		}
		me->isConnected = false;
	}
}

void __stdcall SSWR::AVIRead::AVIRVoiceModemForm::OnModemEvent(void *userObj, UInt8 evtType)
{
	SSWR::AVIRead::AVIRVoiceModemForm *me = (SSWR::AVIRead::AVIRVoiceModemForm*)userObj;
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
			me->toneSb->AppendChar(evtType, 1);
			me->toneChg = true;
			mutUsage.EndUse();
		}
		break;
	}
}

SSWR::AVIRead::AVIRVoiceModemForm::AVIRVoiceModemForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, IO::Device::RockwellModemController *modem, IO::ATCommandChannel *channel, IO::Stream *port) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText((const UTF8Char*)"Voice Modem");
	this->SetFont(0, 0, 8.25, false);
	this->core = core;
	this->modem = modem;
	this->channel = channel;
	this->port = port;
	this->hasEvt = false;
	this->evtType = 0;
	this->channel->SetEventHandler(OnModemEvent, this);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	NEW_CLASS(this->toneMut, Sync::Mutex());
	NEW_CLASS(this->toneSb, Text::StringBuilderUTF8());
	this->toneChg = false;
	this->isConnected = false;

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpInfo = this->tcMain->AddTabPage(CSTR("Info"));
	NEW_CLASS(this->lblModemManu, UI::GUILabel(ui, this->tpInfo, (const UTF8Char*)"Manufacture"));
	this->lblModemManu->SetRect(8, 8, 100, 23, false);
	NEW_CLASS(this->txtModemManu, UI::GUITextBox(ui, this->tpInfo, CSTR("")));
	this->txtModemManu->SetRect(108, 8, 200, 23, false);
	this->txtModemManu->SetReadOnly(true);
	NEW_CLASS(this->lblModemModel, UI::GUILabel(ui, this->tpInfo, (const UTF8Char*)"Model"));
	this->lblModemModel->SetRect(8, 32, 100, 23, false);
	NEW_CLASS(this->txtModemModel, UI::GUITextBox(ui, this->tpInfo, CSTR("")));
	this->txtModemModel->SetRect(108, 32, 200, 23, false);
	this->txtModemModel->SetReadOnly(true);
	NEW_CLASS(this->lblModemRev, UI::GUILabel(ui, this->tpInfo, (const UTF8Char*)"Revision"));
	this->lblModemRev->SetRect(8, 56, 100, 23, false);
	NEW_CLASS(this->txtModemRev, UI::GUITextBox(ui, this->tpInfo, CSTR("")));
	this->txtModemRev->SetRect(108, 56, 200, 23, false);
	this->txtModemRev->SetReadOnly(true);

	this->tpDial = this->tcMain->AddTabPage(CSTR("Dial"));
	NEW_CLASS(this->lblDialNum, UI::GUILabel(ui, this->tpDial, (const UTF8Char*)"Dial Num"));
	this->lblDialNum->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtDialNum, UI::GUITextBox(ui, this->tpDial, CSTR("")));
	this->txtDialNum->SetRect(104, 4, 200, 23, false);
	NEW_CLASS(this->btnDial, UI::GUIButton(ui, this->tpDial, (const UTF8Char*)"Dial"));
	this->btnDial->SetRect(104, 28, 75, 23, false);
	this->btnDial->HandleButtonClick(OnDialClicked, this);
	NEW_CLASS(this->btnHangUp, UI::GUIButton(ui, this->tpDial, (const UTF8Char*)"Hang Up"));
	this->btnHangUp->SetRect(184, 28, 75, 23, false);
	this->btnHangUp->HandleButtonClick(OnHangUpClicked, this);
	NEW_CLASS(this->lblDialStatus, UI::GUILabel(ui, this->tpDial, (const UTF8Char*)"Status"));
	this->lblDialStatus->SetRect(4, 52, 250, 23, false);
	NEW_CLASS(this->txtDialStatus, UI::GUITextBox(ui, this->tpDial, CSTR("")));
	this->txtDialStatus->SetReadOnly(true);
	this->txtDialStatus->SetRect(104, 52, 200, 23, false);
	NEW_CLASS(this->lblDialTones, UI::GUILabel(ui, this->tpDial, (const UTF8Char*)"Tones"));
	this->lblDialTones->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtDialTones, UI::GUITextBox(ui, this->tpDial, CSTR("")));
	this->txtDialTones->SetReadOnly(true);
	this->txtDialTones->SetRect(104, 76, 200, 23, false);

	UTF8Char sbuff[128];
	if (this->modem->VoiceGetManufacturer(sbuff))
	{
		this->txtModemManu->SetText(sbuff);
	}
	else
	{
		this->txtModemManu->SetText((const UTF8Char*)"Unknown");
	}
	if (this->modem->VoiceGetModel(sbuff))
	{
		this->txtModemModel->SetText(sbuff);
	}
	else
	{
		this->txtModemModel->SetText((const UTF8Char*)"Unknown");
	}
	if (this->modem->VoiceGetRevision(sbuff))
	{
		this->txtModemRev->SetText(sbuff);
	}
	else
	{
		this->txtModemRev->SetText((const UTF8Char*)"Unknown");
	}
	this->AddTimer(200, OnTimerTick, this);
}

SSWR::AVIRead::AVIRVoiceModemForm::~AVIRVoiceModemForm()
{
	this->port->Close();
	this->channel->Close();
	DEL_CLASS(this->modem);
	DEL_CLASS(this->channel);
	DEL_CLASS(this->port);
	DEL_CLASS(this->toneMut);
	DEL_CLASS(this->toneSb);
}

void SSWR::AVIRead::AVIRVoiceModemForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
