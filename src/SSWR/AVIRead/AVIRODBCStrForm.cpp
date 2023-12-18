#include "Stdafx.h"
#include "DB/ODBCConn.h"
#include "SSWR/AVIRead/AVIRODBCStrForm.h"
#include "SSWR/AVIRead/AVIRTableMsgForm.h"
#include "Text/MyString.h"

void __stdcall SSWR::AVIRead::AVIRODBCStrForm::OnDriverSelChg(void *userObj)
{
	Text::StringBuilderUTF8 sb;
	SSWR::AVIRead::AVIRODBCStrForm *me = (SSWR::AVIRead::AVIRODBCStrForm*)userObj;
	me->cboDriver->GetText(sb);
	if (sb.GetCharCnt() > 0)
	{
		Text::StringBuilderUTF8 sb2;
		sb2.AppendC(UTF8STRC("Driver={"));
		sb2.AppendC(sb.ToString(), sb.GetLength());
		sb2.AppendUTF8Char('}');
		me->txtConnStr->SetText(sb2.ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRODBCStrForm::OnDriverInfoClicked(void *userObj)
{
	Text::StringBuilderUTF8 sb;
	SSWR::AVIRead::AVIRODBCStrForm *me = (SSWR::AVIRead::AVIRODBCStrForm*)userObj;
	me->cboDriver->GetText(sb);
	if (sb.GetCharCnt() > 0)
	{
		IO::ConfigFile *driver = DB::ODBCConn::GetDriverInfo(sb.ToCString());
		if (driver)
		{
			Data::ArrayListStringNN keys;
			const UTF8Char *sarr[2];
			sarr[0] = (const UTF8Char*)"Name";
			sarr[1] = (const UTF8Char*)"Value";
			SSWR::AVIRead::AVIRTableMsgForm frm(0, me->ui, me->core, sb.ToCString(), 2, sarr);
			driver->GetKeys(CSTR(""), keys);
			Data::ArrayIterator<NotNullPtr<Text::String>> it = keys.Iterator();
			while (it.HasNext())
			{
				NotNullPtr<Text::String> s = it.Next();
				sarr[0] = s->v;
				sarr[1] = Text::String::OrEmpty(driver->GetValue(s))->v;
				frm.AddRow(sarr);
			}
			frm.ShowDialog(me);
			DEL_CLASS(driver);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRODBCStrForm::OnOKClicked(void *userObj)
{
	Text::StringBuilderUTF8 sb;
	SSWR::AVIRead::AVIRODBCStrForm *me = (SSWR::AVIRead::AVIRODBCStrForm*)userObj;
	me->txtConnStr->GetText(sb);

	DB::ODBCConn *conn;
	NEW_CLASS(conn, DB::ODBCConn(sb.ToCString(), CSTR("ODBC"), me->core->GetLog()));
	if (conn->GetConnError() != DB::ODBCConn::CE_NONE)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Error in opening ODBC connection\r\n"));
		conn->GetLastErrorMsg(sb);
		me->ui->ShowMsgOK(sb.ToCString(), CSTR("ODBC String Connection"), me);
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

SSWR::AVIRead::AVIRODBCStrForm::AVIRODBCStrForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 108, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("ODBC String Connection"));
	this->SetNoResize(true);

	this->core = core;
	this->conn = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblDriver = ui->NewLabel(*this, CSTR("Driver"));
	this->lblDriver->SetRect(4, 4, 100, 23, false);
	this->cboDriver = ui->NewComboBox(*this, false);
	this->cboDriver->SetRect(104, 4, 200, 23, false);
	this->cboDriver->HandleSelectionChange(OnDriverSelChg, this);
	this->btnDriverInfo = ui->NewButton(*this, CSTR("Info"));
	this->btnDriverInfo->SetRect(304, 4, 75, 23, false);
	this->btnDriverInfo->HandleButtonClick(OnDriverInfoClicked, this);
	this->lblConnStr = ui->NewLabel(*this, CSTR("Conn Str"));
	this->lblConnStr->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtConnStr, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtConnStr->SetRect(104, 28, 800, 23, false);
	this->btnOK = ui->NewButton(*this, CSTR("OK"));
	this->btnOK->SetRect(104, 56, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	this->btnCancel = ui->NewButton(*this, CSTR("Cancel"));
	this->btnCancel->SetRect(184, 56, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);

	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);

	Data::ArrayListStringNN driverList;
	UOSInt i = 0;
	UOSInt j = DB::ODBCConn::GetDriverList(driverList);
	while (i < j)
	{
		this->cboDriver->AddItem(Text::String::OrEmpty(driverList.GetItem(i)), 0);
		OPTSTR_DEL(driverList.GetItem(i));
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
