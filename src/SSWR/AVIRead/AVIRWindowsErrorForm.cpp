#include "Stdafx.h"
#include "IO/WindowsError.h"
#include "SSWR/AVIRead/AVIRWindowsErrorForm.h"

void __stdcall SSWR::AVIRead::AVIRWindowsErrorForm::OnErrorCodeChanged(void *userObj)
{
	SSWR::AVIRead::AVIRWindowsErrorForm *me = (SSWR::AVIRead::AVIRWindowsErrorForm*)userObj;
	UInt32 errCode;
	Text::StringBuilderUTF8 sb;
	me->txtErrorCode->GetText(sb);
	if (sb.ToUInt32(errCode))
	{
		me->txtErrorName->SetText(IO::WindowsError::GetString(errCode));
	}
	else
	{
		me->txtErrorName->SetText(CSTR(""));
	}
}

SSWR::AVIRead::AVIRWindowsErrorForm::AVIRWindowsErrorForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 456, 200, ui)
{
	this->SetText(CSTR("Windows Error"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblErrorCode, UI::GUILabel(ui, *this, CSTR("Error Code")));
	this->lblErrorCode->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtErrorCode, UI::GUITextBox(ui, *this, CSTR("0")));
	this->txtErrorCode->SetRect(104, 4, 150, 23, false);
	this->txtErrorCode->HandleTextChanged(OnErrorCodeChanged, this);

	NEW_CLASS(this->lblErrorName, UI::GUILabel(ui, *this, CSTR("Name")));
	this->lblErrorName->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtErrorName, UI::GUITextBox(ui, *this, IO::WindowsError::GetString(0)));
	this->txtErrorName->SetRect(104, 28, 200, 23, false);
	this->txtErrorName->SetReadOnly(true);
}

SSWR::AVIRead::AVIRWindowsErrorForm::~AVIRWindowsErrorForm()
{
}

void SSWR::AVIRead::AVIRWindowsErrorForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
