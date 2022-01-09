#include "Stdafx.h"
#include "SSWR/SHPConv/SHPConvAddFilterForm.h"
#include "SSWR/SHPConv/SHPConvCurrFilterForm.h"

void __stdcall SSWR::SHPConv::SHPConvCurrFilterForm::OnAddClicked(void *userObj)
{
	SSWR::SHPConv::SHPConvCurrFilterForm *me = (SSWR::SHPConv::SHPConvCurrFilterForm*)userObj;
	SSWR::SHPConv::SHPConvAddFilterForm *frm;
	NEW_CLASS(frm, SSWR::SHPConv::SHPConvAddFilterForm(0, me->ui, me->dbf, me->deng));
	if (frm->ShowDialog(me) == UI::GUIForm::DR_OK)
	{
		UTF8Char sbuff[512];
		MapFilter *filter = frm->GetFilter();
		me->filters->Add(filter);
		filter->ToString(sbuff);
		me->lbFilters->AddItem(sbuff, filter);
	}
	DEL_CLASS(frm);
}

void __stdcall SSWR::SHPConv::SHPConvCurrFilterForm::OnDeleteClicked(void *userObj)
{
	SSWR::SHPConv::SHPConvCurrFilterForm *me = (SSWR::SHPConv::SHPConvCurrFilterForm*)userObj;
	UOSInt i = me->lbFilters->GetSelectedIndex();
	if (i != INVALID_INDEX)
	{
		MapFilter *filter = me->filters->RemoveAt(i);
		me->lbFilters->RemoveItem(i);
		DEL_CLASS(filter);
	}
}

void __stdcall SSWR::SHPConv::SHPConvCurrFilterForm::OnOkClicked(void *userObj)
{
	SSWR::SHPConv::SHPConvCurrFilterForm *me = (SSWR::SHPConv::SHPConvCurrFilterForm*)userObj;
	me->Close();
}

SSWR::SHPConv::SHPConvCurrFilterForm::SHPConvCurrFilterForm(UI::GUIClientControl *parent, UI::GUICore *ui, DB::DBFFile *dbf, Data::ArrayList<MapFilter*> *filters, Media::DrawEngine *deng) : UI::GUIForm(parent, 414, 298, ui)
{
	this->SetText((const UTF8Char*)"Current Filters");
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	NEW_CLASS(this->monMgr, Media::MonitorMgr());
	this->SetDPI(this->monMgr->GetMonitorHDPI(this->GetHMonitor()), this->monMgr->GetMonitorDDPI(this->GetHMonitor()));
	this->deng = deng;
	this->dbf = dbf;
	this->filters = filters;
	
	NEW_CLASS(this->lbFilters, UI::GUIListBox(ui, this, false));
	this->lbFilters->SetRect(8, 8, 392, 199, false);
	NEW_CLASS(this->btnAdd, UI::GUIButton(ui, this, (const UTF8Char*)"Add Filter"));
	this->btnAdd->SetRect(8, 224, 96, 23, false);
	this->btnAdd->HandleButtonClick(OnAddClicked, this);
	NEW_CLASS(this->btnDelete, UI::GUIButton(ui, this, (const UTF8Char*)"Delete Filter"));
	this->btnDelete->SetRect(144, 224, 96, 23, false);
	this->btnDelete->HandleButtonClick(OnDeleteClicked, this);
	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this, (const UTF8Char*)"OK"));
	this->btnOK->SetRect(320, 224, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOkClicked, this);

	UTF8Char sbuff[512];
	UOSInt i;
	UOSInt j;
	MapFilter *filter;
	i = 0;
	j = this->filters->GetCount();
	while (i < j)
	{
		filter = this->filters->GetItem(i);
		filter->ToString(sbuff);
		this->lbFilters->AddItem(sbuff, filter);
		i++;
	}
}

SSWR::SHPConv::SHPConvCurrFilterForm::~SHPConvCurrFilterForm()
{
	DEL_CLASS(this->monMgr);
}

void SSWR::SHPConv::SHPConvCurrFilterForm::OnMonitorChanged()
{
	this->SetDPI(this->monMgr->GetMonitorHDPI(this->GetHMonitor()), this->monMgr->GetMonitorDDPI(this->GetHMonitor()));
}

