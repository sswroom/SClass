#include "Stdafx.h"
#include "IO/SystemInfo.h"
#include "Manage/CPUInfo.h"
#include "SSWR/AVIRead/AVIRGPUInfoForm.h"
#include "Text/MyStringFloat.h"

void __stdcall SSWR::AVIRead::AVIRGPUInfoForm::OnGPUSelChange(void *userObj)
{
	SSWR::AVIRead::AVIRGPUInfoForm *me = (SSWR::AVIRead::AVIRGPUInfoForm*)userObj;
	IO::IGPUControl *gpu = (IO::IGPUControl*)me->lbGPU->GetSelectedItem();
	if (gpu == 0)
	{
		me->lvMain->ClearItems();
	}
	else
	{
		WChar sbuff[32];
		Double val;
		UOSInt i;
		me->lvMain->ClearItems();
		if (gpu->GetTemperature(&val))
		{
			Text::StrDouble(sbuff, val);
			i = me->lvMain->AddItem((const UTF8Char*)"Temperature", 0);
			me->lvMain->SetSubItem(i, 1, sbuff);
		}
		if (gpu->GetCoreClock(&val))
		{
			Text::StrDouble(sbuff, val);
			i = me->lvMain->AddItem((const UTF8Char*)"Core Clock (MHz)", 0);
			me->lvMain->SetSubItem(i, 1, sbuff);
		}
		if (gpu->GetMemoryClock(&val))
		{
			Text::StrDouble(sbuff, val);
			i = me->lvMain->AddItem((const UTF8Char*)"Memory Clock (MHz)", 0);
			me->lvMain->SetSubItem(i, 1, sbuff);
		}
		if (gpu->GetVoltage(&val))
		{
			Text::StrDouble(sbuff, val);
			i = me->lvMain->AddItem((const UTF8Char*)"Core Voltage (V)", 0);
			me->lvMain->SetSubItem(i, 1, sbuff);
		}
	}
}

SSWR::AVIRead::AVIRGPUInfoForm::AVIRGPUInfoForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 800, 600, ui)
{
	this->SetText((const UTF8Char*)"GPU Info");
	this->SetFont(0, 0, 8.25, false);
	
	this->core = core;
	NEW_CLASS(this->gpuMgr, IO::GPUManager());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	
	NEW_CLASS(this->lbGPU, UI::GUIListBox(ui, this, false));
	this->lbGPU->SetRect(0, 0, 250, 23, false);
	this->lbGPU->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbGPU->HandleSelectionChange(OnGPUSelChange, this);
	NEW_CLASS(this->hspGPU, UI::GUIHSplitter(ui, this, 3, false));
	NEW_CLASS(this->lvMain, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvMain->SetShowGrid(true);
	this->lvMain->SetFullRowSelect(true);
	this->lvMain->AddColumn((const UTF8Char*)"Name", 150);
	this->lvMain->AddColumn((const UTF8Char*)"Value", 300);

	UOSInt i;
	UOSInt j;
	IO::IGPUControl *gpu;
	i = 0;
	j = this->gpuMgr->GetGPUCount();
	while (i < j)
	{
		gpu = this->gpuMgr->GetGPUControl(i);
		this->lbGPU->AddItem(gpu->GetName(), gpu);
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
