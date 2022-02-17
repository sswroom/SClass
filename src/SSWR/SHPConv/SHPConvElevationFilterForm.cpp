#include "Stdafx.h"
#include "SSWR/SHPConv/ElevationFilter.h"
#include "SSWR/SHPConv/SHPConvElevationFilterForm.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::SHPConv::SHPConvElevationFilterForm::OnOKClicked(void *userObj)
{
	SSWR::SHPConv::SHPConvElevationFilterForm *me = (SSWR::SHPConv::SHPConvElevationFilterForm*)userObj;
	Text::StringBuilderUTF8 sb;
	Int32 height;
	me->txtHeight->GetText(&sb);
	if (sb.ToInt32(&height))
	{
		if (height > 0)
		{
			NEW_CLASS(me->filter, SSWR::SHPConv::ElevationFilter((UOSInt)me->cboCol->GetSelectedIndex(), height));
			me->SetDialogResult(UI::GUIForm::DR_OK);
		}
	}
}

void __stdcall SSWR::SHPConv::SHPConvElevationFilterForm::OnCancelClicked(void *userObj)
{
	SSWR::SHPConv::SHPConvElevationFilterForm *me = (SSWR::SHPConv::SHPConvElevationFilterForm*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::SHPConv::SHPConvElevationFilterForm::SHPConvElevationFilterForm(UI::GUIClientControl *parent, UI::GUICore *ui, DB::DBFFile *dbf) : UI::GUIForm(parent, 294, 104, ui)
{
	this->SetText(CSTR("Elevation Filter"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	NEW_CLASS(this->monMgr, Media::MonitorMgr());
	this->SetDPI(this->monMgr->GetMonitorHDPI(this->GetHMonitor()), this->monMgr->GetMonitorDDPI(this->GetHMonitor()));
	this->dbf = dbf;
	this->filter = 0;
	
	NEW_CLASS(this->lblCol, UI::GUILabel(ui, this, (const UTF8Char*)"Column"));
	this->lblCol->SetRect(8, 8, 56, 23, false);
	NEW_CLASS(this->cboCol, UI::GUIComboBox(ui, this, false));
	this->cboCol->SetRect(72, 8, 176, 21, false);
	NEW_CLASS(this->lblHeight, UI::GUILabel(ui, this, (const UTF8Char*)"Height"));
	this->lblHeight->SetRect(8, 32, 56, 23, false);
	NEW_CLASS(this->txtHeight, UI::GUITextBox(ui, this, CSTR("20")));
	this->txtHeight->SetRect(72, 32, 80, 20, false);
	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this, CSTR("OK")));
	this->btnOK->SetRect(72, 56, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this, CSTR("Cancel")));
	this->btnCancel->SetRect(152, 56, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);

	UTF8Char sbuff[256];
	UTF8Char *sptr;
	UOSInt i = 0;
	UOSInt j = this->dbf->GetColCount();
	this->cboCol->ClearItems();
	while (i < j)
	{
		sptr = this->dbf->GetColumnName(i, sbuff);
		this->cboCol->AddItem(CSTRP(sbuff, sptr), 0);
		i++;
	}
	this->cboCol->SetSelectedIndex(0);
}

SSWR::SHPConv::SHPConvElevationFilterForm::~SHPConvElevationFilterForm()
{
	DEL_CLASS(this->monMgr);
}

void SSWR::SHPConv::SHPConvElevationFilterForm::OnMonitorChanged()
{
	this->SetDPI(this->monMgr->GetMonitorHDPI(this->GetHMonitor()), this->monMgr->GetMonitorDDPI(this->GetHMonitor()));
}

SSWR::SHPConv::MapFilter *SSWR::SHPConv::SHPConvElevationFilterForm::GetFilter()
{
	return this->filter;
}
