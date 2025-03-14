#include "Stdafx.h"
#include "DB/OLEDBConn.h"
#include "SSWR/AVIRead/AVIROLEDBForm.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIROLEDBForm::OnOKClicked(AnyType userObj)
{
	Text::StringBuilderUTF8 sb;
	NN<SSWR::AVIRead::AVIROLEDBForm> me = userObj.GetNN<SSWR::AVIRead::AVIROLEDBForm>();
	me->txtConnStr->GetText(sb);

	DB::OLEDBConn *conn;
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(sb.ToString());
	NEW_CLASS(conn, DB::OLEDBConn(wptr, me->core->GetLog()));
	Text::StrDelNew(wptr);
	if (conn->GetConnError() != DB::OLEDBConn::CE_NONE)
	{
		DEL_CLASS(conn);
		me->ui->ShowMsgOK(CSTR("Error in opening OLEDB connection"), CSTR("OLEDB Connection"), me);
		return;
	}
	me->conn = conn;
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIROLEDBForm::OnCancelClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIROLEDBForm> me = userObj.GetNN<SSWR::AVIRead::AVIROLEDBForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIROLEDBForm::AVIROLEDBForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 92, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("OLEDB Connection"));
	this->SetNoResize(true);

	this->core = core;
	this->conn = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblConnStr = ui->NewLabel(*this, CSTR("Conn String"));
	this->lblConnStr->SetRect(4, 4, 100, 23, false);
	this->txtConnStr = ui->NewTextBox(*this, CSTR(""));
	this->txtConnStr->SetRect(104, 4, 800, 23, false);
	this->btnOK = ui->NewButton(*this, CSTR("OK"));
	this->btnOK->SetRect(104, 28, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	this->btnCancel = ui->NewButton(*this, CSTR("Cancel"));
	this->btnCancel->SetRect(184, 28, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);

	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);
	this->txtConnStr->Focus();
}

SSWR::AVIRead::AVIROLEDBForm::~AVIROLEDBForm()
{
}

void SSWR::AVIRead::AVIROLEDBForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

Optional<DB::DBConn> SSWR::AVIRead::AVIROLEDBForm::GetDBConn()
{
	return this->conn;
}