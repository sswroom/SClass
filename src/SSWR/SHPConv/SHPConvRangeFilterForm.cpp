#include "Stdafx.h"
#include "SSWR/SHPConv/RangeFilter.h"
#include "SSWR/SHPConv/SHPConvRangeFilterForm.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::SHPConv::SHPConvRangeFilterForm::OnOKClicked(void *userObj)
{
	SSWR::SHPConv::SHPConvRangeFilterForm *me = (SSWR::SHPConv::SHPConvRangeFilterForm*)userObj;
	Double x1;
	Double y1;
	Double x2;
	Double y2;
	Text::StringBuilderUTF8 sb;

	sb.ClearStr();
	me->txtX1->GetText(sb);
	if (!Text::StrToDouble(sb.ToString(), x1))
	{
		me->ui->ShowMsgOK(CSTR("Invalid input values"), CSTR("Error"), me);
		return;
	}
	sb.ClearStr();
	me->txtY1->GetText(sb);
	if (!Text::StrToDouble(sb.ToString(), y1))
	{
		me->ui->ShowMsgOK(CSTR("Invalid input values"), CSTR("Error"), me);
		return;
	}
	sb.ClearStr();
	me->txtX2->GetText(sb);
	if (!Text::StrToDouble(sb.ToString(), x2))
	{
		me->ui->ShowMsgOK(CSTR("Invalid input values"), CSTR("Error"), me);
		return;
	}
	sb.ClearStr();
	me->txtY2->GetText(sb);
	if (!Text::StrToDouble(sb.ToString(), y2))
	{
		me->ui->ShowMsgOK(CSTR("Invalid input values"), CSTR("Error"), me);
		return;
	}

	if (x1 >= x2)
	{
		me->ui->ShowMsgOK(CSTR("X value is not valid"), CSTR("Error"), me);
		return;
	}
	if (y1 >= y2)
	{
		me->ui->ShowMsgOK(CSTR("Y value is not valid"), CSTR("Error"), me);
		return;
	}
	Int32 typ;
	if (me->radInRange->IsSelected())
	{
		typ = 1;
	}
	else
	{
		typ = 0;
	}
	NEW_CLASS(me->filter, RangeFilter(x1, y1, x2, y2, typ));
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::SHPConv::SHPConvRangeFilterForm::OnCancelClicked(void *userObj)
{
	SSWR::SHPConv::SHPConvRangeFilterForm *me = (SSWR::SHPConv::SHPConvRangeFilterForm*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::SHPConv::SHPConvRangeFilterForm::SHPConvRangeFilterForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<Media::DrawEngine> deng) : UI::GUIForm(parent, 350, 232, ui)
{
	this->SetText(CSTR("Range Filter"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	NEW_CLASS(this->monMgr, Media::MonitorMgr());
	this->SetDPI(this->monMgr->GetMonitorHDPI(this->GetHMonitor()), this->monMgr->GetMonitorDDPI(this->GetHMonitor()));
	this->deng = deng;
	this->filter = 0;
	
	NEW_CLASS(this->radInRange, UI::GUIRadioButton(ui, *this, CSTR("In Range"), true));
	this->radInRange->SetRect(8, 8, 100, 23, false);
	NEW_CLASS(this->radOutRange, UI::GUIRadioButton(ui, *this, CSTR("Out Range"), false));
	this->radOutRange->SetRect(112, 8, 100, 23, false);
	this->lblX1 = ui->NewLabel(*this, CSTR("X"));
	this->lblX1->SetRect(8, 32, 16, 23, false);
	this->txtX1 = ui->NewTextBox(*this, CSTR("0"));
	this->txtX1->SetRect(24, 32, 80, 20, false);
	this->lblY1 = ui->NewLabel(*this, CSTR("Y"));
	this->lblY1->SetRect(112, 32, 16, 23, false);
	this->txtY1 = ui->NewTextBox(*this, CSTR("0"));
	this->txtY1->SetRect(128, 32, 80, 20, false);
	NEW_CLASS(this->pbBox, UI::GUIPictureBoxSimple(ui, *this, this->deng, true));
	this->pbBox->SetRect(56, 64, 216, 96, false);
	this->lblX2 = ui->NewLabel(*this, CSTR("X"));
	this->lblX2->SetRect(144, 176, 16, 23, false);
	this->txtX2 = ui->NewTextBox(*this, CSTR("0"));
	this->txtX2->SetRect(160, 176, 80, 20, false);
	this->lblY2 = ui->NewLabel(*this, CSTR("Y"));
	this->lblY2->SetRect(248, 176, 16, 23, false);
	this->txtY2 = ui->NewTextBox(*this, CSTR("0"));
	this->txtY2->SetRect(264, 176, 80, 20, false);
	this->btnOK = ui->NewButton(*this, CSTR("OK"));
	this->btnOK->SetRect(264, 8, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	this->btnCancel = ui->NewButton(*this, CSTR("Cancel"));
	this->btnCancel->SetRect(264, 32, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);
}

SSWR::SHPConv::SHPConvRangeFilterForm::~SHPConvRangeFilterForm()
{
	DEL_CLASS(this->monMgr);
}

void SSWR::SHPConv::SHPConvRangeFilterForm::OnMonitorChanged()
{
	this->SetDPI(this->monMgr->GetMonitorHDPI(this->GetHMonitor()), this->monMgr->GetMonitorDDPI(this->GetHMonitor()));
}

SSWR::SHPConv::MapFilter *SSWR::SHPConv::SHPConvRangeFilterForm::GetFilter()
{
	return this->filter;
}
