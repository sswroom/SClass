#include "Stdafx.h"
#include "IO/LogZipper.h"
#include "IO/Path.h"
#include "Manage/HiResClock.h"
#include "SSWR/AVIRead/AVIRLogZipperForm.h"

void __stdcall SSWR::AVIRead::AVIRLogZipperForm::OnStartClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRLogZipperForm> me = userObj.GetNN<SSWR::AVIRead::AVIRLogZipperForm>();
	Text::StringBuilderUTF8 sb;

	me->txtLogDir->GetText(sb);
	if (sb.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter Log Dir"), CSTR("Log Zipper"), me);
		return;
	}
	if (sb.GetLength() >= 500)
	{
		me->ui->ShowMsgOK(CSTR("Log Dir is too long"), CSTR("Log Zipper"), me);
		return;
	}
	if (IO::Path::GetPathType(sb.ToCString()) != IO::Path::PathType::Directory)
	{
		me->ui->ShowMsgOK(CSTR("Invalid Log Dir"), CSTR("Log Zipper"), me);
		return;
	}
	Manage::HiResClock clk;
	Bool succ = IO::LogZipper::ZipLogs(sb.ToCString());
	Double t = clk.GetTimeDiff();
	sb.ClearStr();
	if (succ)
	{
		sb.Append(CSTR("Success, Time taken: "));
		sb.AppendDouble(t);
		sb.Append(CSTR(" seconds"));
	}
	else
	{
		sb.Append(CSTR("Failed"));
	}
	me->txtStatus->SetText(sb.ToCString());
}

SSWR::AVIRead::AVIRLogZipperForm::AVIRLogZipperForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 200, ui)
{
	this->SetText(CSTR("Log Zipper"));
	this->SetFont(nullptr, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblLogDir = ui->NewLabel(*this, CSTR("Log Dir"));
	this->lblLogDir->SetRect(4, 4, 100, 23, false);
	this->txtLogDir = ui->NewTextBox(*this, CSTR(""), false);
	this->txtLogDir->SetRect(104, 4, 600, 23, false);
	this->btnStart = ui->NewButton(*this, CSTR("&Start"));
	this->btnStart->SetRect(104, 28, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	this->lblStatus = ui->NewLabel(*this, CSTR("Status"));
	this->lblStatus->SetRect(4, 52, 100, 23, false);
	this->txtStatus = ui->NewTextBox(*this, CSTR(""), false);
	this->txtStatus->SetRect(104, 52, 600, 23, false);
	this->txtStatus->SetReadOnly(true);
}

SSWR::AVIRead::AVIRLogZipperForm::~AVIRLogZipperForm()
{
}

void SSWR::AVIRead::AVIRLogZipperForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
