#include "Stdafx.h"
#include "IO/SerialPort.h"
#include "IO/Device/ADAM4050.h"
#include "SSWR/AVIRead/AVIRADAMForm.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRADAMForm::OnStreamClicked(void *userObj)
{
	SSWR::AVIRead::AVIRADAMForm *me = (SSWR::AVIRead::AVIRADAMForm *)userObj;
	if (me->stm)
	{
		me->StopStream(true);
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		me->txtAddress->GetText(&sb);
		if (!sb.ToUInt8(&me->channelAddr))
		{
			UI::MessageDialog::ShowDialog(CSTR("Address out of range (0-255)"), CSTR("ADAM"), me);
			return;
		}
		SSWR::AVIRead::AVIRCore::StreamType st;
		me->stm = me->core->OpenStream(&st, me, 0, false);
		if (me->stm)
		{
			UTF8Char sbuff[64];
			UTF8Char *sptr;
			NEW_CLASS(me->channel, IO::AdvantechASCIIChannel(me->stm, true));
			me->channelModule = 0;
			me->txtStream->SetText(SSWR::AVIRead::AVIRCore::StreamTypeGetName(st));
			me->btnStream->SetText(CSTR("&Close"));
			me->txtAddress->SetReadOnly(true);

			if ((sptr = me->channel->GetModuleName(sbuff, me->channelAddr)) != 0)
			{
				me->txtModuleName->SetText(CSTRP(sbuff, sptr));
				if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("4050")))
				{
					me->channelModule = 4050;
				}
			}
			else
			{
				me->txtModuleName->SetText(CSTR("-"));
			}

			if ((sptr = me->channel->GetFirmwareVer(sbuff, me->channelAddr)) != 0)
			{
				me->txtFirmware->SetText(CSTRP(sbuff, sptr));
			}
			else
			{
				me->txtFirmware->SetText(CSTR("-"));
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRADAMForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRADAMForm *me = (SSWR::AVIRead::AVIRADAMForm *)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;		
	UOSInt i;
	me->lvData->ClearItems();
	if (me->channelModule == 4050)
	{
		IO::Device::ADAM4050 *adam = (IO::Device::ADAM4050*)me->channel;
		UInt16 inputs;
		UInt16 outputs;
		if (adam->GetIOStatus(me->channelAddr, &outputs, &inputs))
		{
			i = me->lvData->AddItem(CSTR("Inputs"), 0);
			sptr = Text::StrHexVal16(sbuff, inputs);
			me->lvData->SetSubItem(i, 1, CSTRP(sbuff, sptr));
			i = me->lvData->AddItem(CSTR("Outputs"), 0);
			sptr = Text::StrHexVal16(sbuff, outputs);
			me->lvData->SetSubItem(i, 1, CSTRP(sbuff, sptr));
		}
		else
		{
			i = me->lvData->AddItem(CSTR("Inputs"), 0);
			me->lvData->SetSubItem(i, 1, CSTR("-"));
			i = me->lvData->AddItem(CSTR("Outputs"), 0);
			me->lvData->SetSubItem(i, 1, CSTR("-"));
		}
	}
}

void SSWR::AVIRead::AVIRADAMForm::StopStream(Bool clearUI)
{
	if (this->stm)
	{
		DEL_CLASS(this->channel);
		this->stm = 0;
		this->channel = 0;
		this->channelModule = 0;
		if (clearUI)
		{
			this->txtStream->SetText(CSTR("-"));
			this->btnStream->SetText(CSTR("&Open"));
			this->txtAddress->SetReadOnly(false);
		}
	}
}

SSWR::AVIRead::AVIRADAMForm::AVIRADAMForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 576, 480, ui)
{
	this->SetText(CSTR("ADAM device"));
	this->SetFont(0, 0, 8.25, false);
	
	this->core = core;
	this->stm = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlMain, UI::GUIPanel(ui, this));
	this->pnlMain->SetRect(0, 0, 100, 124, false);
	this->pnlMain->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->grpStream, UI::GUIGroupBox(ui, this->pnlMain, CSTR("Stream")));
	this->grpStream->SetRect(0, 0, 100, 72, false);
	this->grpStream->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblAddress, UI::GUILabel(ui, this->grpStream, CSTR("Address")));
	this->lblAddress->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtAddress, UI::GUITextBox(ui, this->grpStream, CSTR("6")));
	this->txtAddress->SetRect(104, 4, 200, 23, false);
	NEW_CLASS(this->lblStream, UI::GUILabel(ui, this->grpStream, CSTR("Stream Type")));
	this->lblStream->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtStream, UI::GUITextBox(ui, this->grpStream, CSTR("-")));
	this->txtStream->SetRect(104, 28, 200, 23, false);
	this->txtStream->SetReadOnly(true);
	NEW_CLASS(this->btnStream, UI::GUIButton(ui, this->grpStream, CSTR("&Open")));
	this->btnStream->SetRect(304, 28, 75, 23, false);
	this->btnStream->HandleButtonClick(OnStreamClicked, this);
	NEW_CLASS(this->lblModuleName, UI::GUILabel(ui, this->pnlMain, CSTR("Module Name")));
	this->lblModuleName->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtModuleName, UI::GUITextBox(ui, this->pnlMain, CSTR("")));
	this->txtModuleName->SetRect(104, 76, 200, 23, false);
	this->txtModuleName->SetReadOnly(true);
	NEW_CLASS(this->lblFirmware, UI::GUILabel(ui, this->pnlMain, CSTR("Firmware")));
	this->lblFirmware->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtFirmware, UI::GUITextBox(ui, this->pnlMain, CSTR("")));
	this->txtFirmware->SetRect(104, 100, 200, 23, false);
	this->txtFirmware->SetReadOnly(true);
	NEW_CLASS(this->lvData, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvData->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvData->SetFullRowSelect(true);
	this->lvData->SetShowGrid(true);
	this->lvData->AddColumn(CSTR("Name"), 100);
	this->lvData->AddColumn(CSTR("Value"), 300);

	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRADAMForm::~AVIRADAMForm()
{
	this->StopStream(false);
}

void SSWR::AVIRead::AVIRADAMForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
