#include "Stdafx.h"
#include "DB/ODBCConn.h"
#include "SSWR/AVIRead/AVIRODBCStrForm.h"
#include "SSWR/AVIRead/AVIRTableMsgForm.h"
#include "Text/MyString.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRODBCStrForm::OnDriverSelChg(void *userObj)
{
	Text::StringBuilderUTF8 sb;
	SSWR::AVIRead::AVIRODBCStrForm *me = (SSWR::AVIRead::AVIRODBCStrForm*)userObj;
	me->cboDriver->GetText(&sb);
	if (sb.GetCharCnt() > 0)
	{
		Text::StringBuilderUTF8 sb2;
		sb2.Append((const UTF8Char*)"Driver={");
		sb2.Append(sb.ToString());
		sb2.AppendChar('}', 1);
		me->txtConnStr->SetText(sb2.ToString());
	}
}

void __stdcall SSWR::AVIRead::AVIRODBCStrForm::OnDriverInfoClicked(void *userObj)
{
	Text::StringBuilderUTF8 sb;
	SSWR::AVIRead::AVIRODBCStrForm *me = (SSWR::AVIRead::AVIRODBCStrForm*)userObj;
	me->cboDriver->GetText(&sb);
	if (sb.GetCharCnt() > 0)
	{
		IO::ConfigFile *driver = DB::ODBCConn::GetDriverInfo(sb.ToString());
		if (driver)
		{
			SSWR::AVIRead::AVIRTableMsgForm *frm;
			Data::ArrayList<const UTF8Char*> keys;
			UOSInt i;
			UOSInt j;
			const UTF8Char *sarr[2];
			sarr[0] = (const UTF8Char*)"Name";
			sarr[1] = (const UTF8Char*)"Value";
			NEW_CLASS(frm, SSWR::AVIRead::AVIRTableMsgForm(0, me->ui, me->core, sb.ToString(), 2, sarr));
			driver->GetKeys(0, &keys);
			i = 0;
			j = keys.GetCount();
			while (i < j)
			{
				sarr[0] = keys.GetItem(i);
				sarr[1] = driver->GetValue(sarr[0]);
				frm->AddRow(sarr);
				i++;
			}
			frm->ShowDialog(me);
			DEL_CLASS(frm);
			DEL_CLASS(driver);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRODBCStrForm::OnOKClicked(void *userObj)
{
	Text::StringBuilderUTF8 sb;
	SSWR::AVIRead::AVIRODBCStrForm *me = (SSWR::AVIRead::AVIRODBCStrForm*)userObj;
	me->txtConnStr->GetText(&sb);

	DB::ODBCConn *conn;
	NEW_CLASS(conn, DB::ODBCConn(sb.ToString(), (const UTF8Char*)"ODBC", me->core->GetLog()));
	if (conn->GetConnError() != DB::ODBCConn::CE_NONE)
	{
		sb.ClearStr();
		sb.Append((const UTF8Char *)"Error in opening ODBC connection\r\n");
		conn->GetErrorMsg(&sb);
		UI::MessageDialog::ShowDialog(sb.ToString(), (const UTF8Char *)"ODBC String Connection", me);
		DEL_CLASS(conn);
		return;
	}
	me->conn = conn;
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRODBCStrForm::OnCancelClicked(void *userObj)
{
	SSWR::AVIRead::AVIRODBCStrForm *me = (SSWR::AVIRead::AVIRODBCStrForm*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIRODBCStrForm::AVIRODBCStrForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 108, ui)
{
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"ODBC String Connection");
	this->SetNoResize(true);

	this->core = core;
	this->conn = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblDriver, UI::GUILabel(ui, this, (const UTF8Char*)"Driver"));
	this->lblDriver->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->cboDriver, UI::GUIComboBox(ui, this, false));
	this->cboDriver->SetRect(104, 4, 200, 23, false);
	this->cboDriver->HandleSelectionChange(OnDriverSelChg, this);
	NEW_CLASS(this->btnDriverInfo, UI::GUIButton(ui, this, (const UTF8Char*)"Info"));
	this->btnDriverInfo->SetRect(304, 4, 75, 23, false);
	this->btnDriverInfo->HandleButtonClick(OnDriverInfoClicked, this);
	NEW_CLASS(this->lblConnStr, UI::GUILabel(ui, this, (const UTF8Char*)"Conn Str"));
	this->lblConnStr->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtConnStr, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtConnStr->SetRect(104, 28, 800, 23, false);
	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this, (const UTF8Char*)"OK"));
	this->btnOK->SetRect(104, 56, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this, (const UTF8Char*)"Cancel"));
	this->btnCancel->SetRect(184, 56, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);

	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);

	Data::ArrayList<const UTF8Char *> driverList;
	UOSInt i = 0;
	UOSInt j = DB::ODBCConn::GetDriverList(&driverList);
	while (i < j)
	{
		this->cboDriver->AddItem(driverList.GetItem(i), 0);
		Text::StrDelNew(driverList.GetItem(i));
		i++;
	}
	if (j > 0)
	{
		this->cboDriver->SetSelectedIndex(0);
	}
	this->cboDriver->Focus();
}

SSWR::AVIRead::AVIRODBCStrForm::~AVIRODBCStrForm()
{
}

void SSWR::AVIRead::AVIRODBCStrForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

DB::DBConn *SSWR::AVIRead::AVIRODBCStrForm::GetDBConn()
{
	return this->conn;
}
