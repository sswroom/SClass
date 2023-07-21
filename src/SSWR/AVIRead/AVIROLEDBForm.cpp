#include "Stdafx.h"
#include "DB/OLEDBConn.h"
#include "SSWR/AVIRead/AVIROLEDBForm.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIROLEDBForm::OnOKClicked(void *userObj)
{
	Text::StringBuilderUTF8 sb;
	SSWR::AVIRead::AVIROLEDBForm *me = (SSWR::AVIRead::AVIROLEDBForm*)userObj;
	me->txtConnStr->GetText(&sb);

	DB::OLEDBConn *conn;
	const WChar *wptr = Text::StrToWCharNew(sb.ToString());
	NEW_CLASS(conn, DB::OLEDBConn(wptr, me->core->GetLog()));
	Text::StrDelNew(wptr);
	if (conn->GetConnError() != DB::OLEDBConn::CE_NONE)
	{
		DEL_CLASS(conn);
		UI::MessageDialog::ShowDialog(CSTR("Error in opening OLEDB connection"), CSTR("OLEDB Connection"), me);
		return;
	}
	me->conn = conn;
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIROLEDBForm::OnCancelClicked(void *userObj)
{
	SSWR::AVIRead::AVIROLEDBForm *me = (SSWR::AVIRead::AVIROLEDBForm*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIROLEDBForm::AVIROLEDBForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 92, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("OLEDB Connection"));
	this->SetNoResize(true);

	this->core = core;
	this->conn = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblConnStr, UI::GUILabel(ui, this, CSTR("Conn String")));
	this->lblConnStr->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtConnStr, UI::GUITextBox(ui, this, CSTR("")));
	this->txtConnStr->SetRect(104, 4, 800, 23, false);
	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this, CSTR("OK")));
	this->btnOK->SetRect(104, 28, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this, CSTR("Cancel")));
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

DB::DBConn *SSWR::AVIRead::AVIROLEDBForm::GetDBConn()
{
	return this->conn;
}