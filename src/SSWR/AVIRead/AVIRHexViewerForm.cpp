#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRHexViewerForm.h"

void __stdcall SSWR::AVIRead::AVIRHexViewerForm::OnFilesDrop(void *userObj, const UTF8Char **files, UOSInt nFiles)
{
	SSWR::AVIRead::AVIRHexViewerForm *me = (SSWR::AVIRead::AVIRHexViewerForm*)userObj;
	UOSInt i = 0;
	while (i < nFiles)
	{
		if (me->hexView->LoadFile(files[i]))
		{
			break;
		}
		i++;
	}
}

SSWR::AVIRead::AVIRHexViewerForm::AVIRHexViewerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 800, 600, ui)
{
	this->SetText((const UTF8Char*)"Hex Viewer");
	this->SetFont(0, 8.25, false);
	
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->hexView, UI::GUIHexFileView(ui, this, this->core->GetDrawEngine()));
	this->hexView->SetDockType(UI::GUIControl::DOCK_FILL);
	
	this->HandleDropFiles(OnFilesDrop, this);
}

SSWR::AVIRead::AVIRHexViewerForm::~AVIRHexViewerForm()
{
}

void SSWR::AVIRead::AVIRHexViewerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
