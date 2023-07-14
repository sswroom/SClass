#include "Stdafx.h"
#include "IO/Device/AXCAN.h"
#include "IO/FileStream.h"
#include "SSWR/AVIRead/AVIRSelCANForm.h"
#include "SSWR/AVIRead/AVIRSelStreamForm.h"
#include "Text/UTF8Reader.h"
#include "UI/FileDialog.h"

void __stdcall SSWR::AVIRead::AVIRSelCANForm::OnAXCANSerialClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSelCANForm *me = (SSWR::AVIRead::AVIRSelCANForm*)userObj;
	SSWR::AVIRead::AVIRSelStreamForm dlg(0, me->ui, me->core, false, me->ssl);
	if (dlg.ShowDialog(me) == DR_OK)
	{
		IO::Device::AXCAN *can;
		NEW_CLASS(can, IO::Device::AXCAN(me->hdlr));
		can->OpenStream(dlg.GetStream(), (IO::Device::AXCAN::CANBitRate)(OSInt)me->cboAXCANBitRate->GetSelectedItem());
		me->listener = can;
		me->SetDialogResult(DialogResult::DR_OK);
	}
}

void __stdcall SSWR::AVIRead::AVIRSelCANForm::OnAXCANFileClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSelCANForm *me = (SSWR::AVIRead::AVIRSelCANForm*)userObj;
	UI::FileDialog dlg(L"SSWR", L"AVIRead", L"AXCANFile", false);
	dlg.AddFilter(CSTR("*.txt"), CSTR("Text File"));
	if (dlg.ShowDialog(me->GetHandle()))
	{
		IO::FileStream fs(dlg.GetFileName(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		Text::UTF8Reader reader(fs);
		IO::Device::AXCAN *can;
		NEW_CLASS(can, IO::Device::AXCAN(me->hdlr));
		can->ParseReader(&reader);
		me->listener = can;
		me->SetDialogResult(DialogResult::DR_OK);
	}
}

SSWR::AVIRead::AVIRSelCANForm::AVIRSelCANForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Net::SSLEngine *ssl, IO::CANHandler *hdlr) : UI::GUIForm(parent, 640, 480, ui)
{
	this->SetText(CSTR("Select CAN Bus"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->hdlr = hdlr;
	this->listener = 0;
	this->ssl = ssl;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpAXCAN = this->tcMain->AddTabPage(CSTR("AXCAN"));
	NEW_CLASS(this->lblAXCANBitRate, UI::GUILabel(ui, this->tpAXCAN, CSTR("BitRate")));
	this->lblAXCANBitRate->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->cboAXCANBitRate, UI::GUIComboBox(ui, this->tpAXCAN, false));
	this->cboAXCANBitRate->SetRect(104, 4, 100, 23, false);
	this->cboAXCANBitRate->AddItem(CSTR("5K"), (void*)IO::Device::AXCAN::CANBitRate::BR_5K);
	this->cboAXCANBitRate->AddItem(CSTR("10K"), (void*)IO::Device::AXCAN::CANBitRate::BR_10K);
	this->cboAXCANBitRate->AddItem(CSTR("20K"), (void*)IO::Device::AXCAN::CANBitRate::BR_20K);
	this->cboAXCANBitRate->AddItem(CSTR("40K"), (void*)IO::Device::AXCAN::CANBitRate::BR_40K);
	this->cboAXCANBitRate->AddItem(CSTR("50K"), (void*)IO::Device::AXCAN::CANBitRate::BR_50K);
	this->cboAXCANBitRate->AddItem(CSTR("80K"), (void*)IO::Device::AXCAN::CANBitRate::BR_80K);
	this->cboAXCANBitRate->AddItem(CSTR("100K"), (void*)IO::Device::AXCAN::CANBitRate::BR_100K);
	this->cboAXCANBitRate->AddItem(CSTR("125K"), (void*)IO::Device::AXCAN::CANBitRate::BR_125K);
	this->cboAXCANBitRate->AddItem(CSTR("200K"), (void*)IO::Device::AXCAN::CANBitRate::BR_200K);
	this->cboAXCANBitRate->AddItem(CSTR("250K"), (void*)IO::Device::AXCAN::CANBitRate::BR_250K);
	this->cboAXCANBitRate->AddItem(CSTR("400K"), (void*)IO::Device::AXCAN::CANBitRate::BR_400K);
	this->cboAXCANBitRate->AddItem(CSTR("500K"), (void*)IO::Device::AXCAN::CANBitRate::BR_500K);
	this->cboAXCANBitRate->AddItem(CSTR("600K"), (void*)IO::Device::AXCAN::CANBitRate::BR_600K);
	this->cboAXCANBitRate->AddItem(CSTR("800K"), (void*)IO::Device::AXCAN::CANBitRate::BR_800K);
	this->cboAXCANBitRate->AddItem(CSTR("1000K"), (void*)IO::Device::AXCAN::CANBitRate::BR_1000K);
	this->cboAXCANBitRate->SetSelectedIndex(9);
	NEW_CLASS(this->btnAXCANSerial, UI::GUIButton(ui, this->tpAXCAN, CSTR("Open Serial")));
	this->btnAXCANSerial->SetRect(204, 4, 75, 23, false);
	this->btnAXCANSerial->HandleButtonClick(OnAXCANSerialClicked, this);
	NEW_CLASS(this->btnAXCANFile, UI::GUIButton(ui, this->tpAXCAN, CSTR("Open File")));
	this->btnAXCANFile->SetRect(4, 28, 75, 23, false);
	this->btnAXCANFile->HandleButtonClick(OnAXCANFileClicked, this);
}

SSWR::AVIRead::AVIRSelCANForm::~AVIRSelCANForm()
{
}

void SSWR::AVIRead::AVIRSelCANForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

IO::CANListener *SSWR::AVIRead::AVIRSelCANForm::GetListener() const
{
	return this->listener;
}
