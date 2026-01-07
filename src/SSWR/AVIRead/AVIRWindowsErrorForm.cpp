#include "Stdafx.h"
#include "IO/WindowsError.h"
#include "SSWR/AVIRead/AVIRWindowsErrorForm.h"

void __stdcall SSWR::AVIRead::AVIRWindowsErrorForm::OnErrorCodeChanged(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRWindowsErrorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWindowsErrorForm>();
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

SSWR::AVIRead::AVIRWindowsErrorForm::AVIRWindowsErrorForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 456, 200, ui)
{
	this->SetText(CSTR("Windows Error"));
	this->SetFont(nullptr, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblErrorCode = ui->NewLabel(*this, CSTR("Error Code"));
	this->lblErrorCode->SetRect(4, 4, 100, 23, false);
	this->txtErrorCode = ui->NewTextBox(*this, CSTR("0"));
	this->txtErrorCode->SetRect(104, 4, 150, 23, false);
	this->txtErrorCode->HandleTextChanged(OnErrorCodeChanged, this);

	this->lblErrorName = ui->NewLabel(*this, CSTR("Name"));
	this->lblErrorName->SetRect(4, 28, 100, 23, false);
	this->txtErrorName = ui->NewTextBox(*this, IO::WindowsError::GetString(0));
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
