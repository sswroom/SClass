#include "Stdafx.h"
#include "IO/SystemInfo.h"
#include "Manage/CPUInfo.h"
#include "SSWR/AVIRead/AVIRGPUInfoForm.h"
#include "Text/MyStringFloat.h"

void __stdcall SSWR::AVIRead::AVIRGPUInfoForm::OnGPUSelChange(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGPUInfoForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGPUInfoForm>();
	IO::IGPUControl *gpu = (IO::IGPUControl*)me->lbGPU->GetSelectedItem().p;
	if (gpu == 0)
	{
		me->lvMain->ClearItems();
	}
	else
	{
		WChar wbuff[32];
		Double val;
		UOSInt i;
		me->lvMain->ClearItems();
		if (gpu->GetTemperature(&val))
		{
			Text::StrDouble(wbuff, val);
			i = me->lvMain->AddItem(CSTR("Temperature"), 0);
			me->lvMain->SetSubItem(i, 1, wbuff);
		}
		if (gpu->GetCoreClock(&val))
		{
			Text::StrDouble(wbuff, val);
			i = me->lvMain->AddItem(CSTR("Core Clock (MHz)"), 0);
			me->lvMain->SetSubItem(i, 1, wbuff);
		}
		if (gpu->GetMemoryClock(&val))
		{
			Text::StrDouble(wbuff, val);
			i = me->lvMain->AddItem(CSTR("Memory Clock (MHz)"), 0);
			me->lvMain->SetSubItem(i, 1, wbuff);
		}
		if (gpu->GetVoltage(&val))
		{
			Text::StrDouble(wbuff, val);
			i = me->lvMain->AddItem(CSTR("Core Voltage (V)"), 0);
			me->lvMain->SetSubItem(i, 1, wbuff);
		}
	}
}

SSWR::AVIRead::AVIRGPUInfoForm::AVIRGPUInfoForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 800, 600, ui)
{
	this->SetText(CSTR("GPU Info"));
	this->SetFont(0, 0, 8.25, false);
	
	this->core = core;
	NEW_CLASS(this->gpuMgr, IO::GPUManager());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	
	this->lbGPU = ui->NewListBox(*this, false);
	this->lbGPU->SetRect(0, 0, 250, 23, false);
	this->lbGPU->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbGPU->HandleSelectionChange(OnGPUSelChange, this);
	this->hspGPU = ui->NewHSplitter(*this, 3, false);
	this->lvMain = ui->NewListView(*this, UI::ListViewStyle::Table, 2);
	this->lvMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvMain->SetShowGrid(true);
	this->lvMain->SetFullRowSelect(true);
	this->lvMain->AddColumn(CSTR("Name"), 150);
	this->lvMain->AddColumn(CSTR("Value"), 300);

	UOSInt i;
	UOSInt j;
	IO::IGPUControl *gpu;
	i = 0;
	j = this->gpuMgr->GetGPUCount();
	while (i < j)
	{
		gpu = this->gpuMgr->GetGPUControl(i);
		const UTF8Char *name = gpu->GetName();
		this->lbGPU->AddItem({name, Text::StrCharCnt(name)}, gpu);
		i++;
	}
}

SSWR::AVIRead::AVIRGPUInfoForm::~AVIRGPUInfoForm()
{
	DEL_CLASS(this->gpuMgr);
}

void SSWR::AVIRead::AVIRGPUInfoForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
