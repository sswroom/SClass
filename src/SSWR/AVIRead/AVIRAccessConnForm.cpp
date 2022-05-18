#include "Stdafx.h"
#include "DB/MDBFileConn.h"
#include "SSWR/AVIRead/AVIRAccessConnForm.h"
#include "Text/MyString.h"
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRAccessConnForm::OnBrowseClicked(void *userObj)
{
	SSWR::AVIRead::AVIRAccessConnForm *me = (SSWR::AVIRead::AVIRAccessConnForm*)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtFileName->GetText(&sb);
	UI::FileDialog dlg(L"SSWR", L"AVIRead", L"AccessConn", false);
	dlg.AddFilter(CSTR("*.mdb"), CSTR("MDB File"));
	dlg.AddFilter(CSTR("*.accdb"), CSTR("Access DB File"));
	dlg.SetFileName(sb.ToCString());
	dlg.SetAllowMultiSel(false);
	if (dlg.ShowDialog(me->GetHandle()))
	{
		me->txtFileName->SetText(dlg.GetFileName()->ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRAccessConnForm::OnOKClicked(void *userObj)
{
	SSWR::AVIRead::AVIRAccessConnForm *me = (SSWR::AVIRead::AVIRAccessConnForm*)userObj;
	Text::StringBuilderUTF8 sbFileName;
	me->txtFileName->GetText(&sbFileName);
	if (sbFileName.GetCharCnt() == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Please enter file name"), CSTR("Access Conn"), me);
		return;
	}
	NEW_CLASS(me->conn, DB::MDBFileConn(sbFileName.ToCString(), me->core->GetLog(), 0, 0, 0));
	if (((DB::MDBFileConn*)me->conn)->GetConnError() == DB::ODBCConn::CE_NONE)
	{
		me->SetDialogResult(UI::GUIForm::DR_OK);
	}
	else
	{
		sbFileName.ClearStr();
		sbFileName.AppendC(UTF8STRC("Error in opening ODBC connection\r\n"));
		me->conn->GetErrorMsg(&sbFileName);
		UI::MessageDialog::ShowDialog(sbFileName.ToCString(), CSTR("Access Conn"), me);
		DEL_CLASS(me->conn);
	}
}

void __stdcall SSWR::AVIRead::AVIRAccessConnForm::OnCancelClicked(void *userObj)
{
	SSWR::AVIRead::AVIRAccessConnForm *me = (SSWR::AVIRead::AVIRAccessConnForm*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIRAccessConnForm::AVIRAccessConnForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 100, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Access Connection"));
	this->SetNoResize(true);

	this->core = core;
	this->conn = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblFileName, UI::GUILabel(ui, this, CSTR("File Name")));
	this->lblFileName->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtFileName, UI::GUITextBox(ui, this, CSTR("")));
	this->txtFileName->SetRect(104, 4, 500, 23, false);
	NEW_CLASS(this->btnBrowse, UI::GUIButton(ui, this, CSTR("B&rowse")));
	this->btnBrowse->SetRect(604, 4, 75, 23, false);
	this->btnBrowse->HandleButtonClick(OnBrowseClicked, this);
	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this, CSTR("OK")));
	this->btnOK->SetRect(104, 28, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this, CSTR("Cancel")));
	this->btnCancel->SetRect(184, 28, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);

	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);
}

SSWR::AVIRead::AVIRAccessConnForm::~AVIRAccessConnForm()
{
}

void SSWR::AVIRead::AVIRAccessConnForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

DB::DBConn *SSWR::AVIRead::AVIRAccessConnForm::GetDBConn()
{
	return this->conn;
}
