#include "Stdafx.h"
#include "SSWR/SHPConv/SHPConvAddFilterForm.h"
#include "SSWR/SHPConv/SHPConvCurrFilterForm.h"

void __stdcall SSWR::SHPConv::SHPConvCurrFilterForm::OnAddClicked(AnyType userObj)
{
	NotNullPtr<SSWR::SHPConv::SHPConvCurrFilterForm> me = userObj.GetNN<SSWR::SHPConv::SHPConvCurrFilterForm>();
	SSWR::SHPConv::SHPConvAddFilterForm frm(0, me->ui, me->dbf, me->deng);
	if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
	{
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		MapFilter *filter = frm.GetFilter();
		me->filters->Add(filter);
		sptr = filter->ToString(sbuff);
		me->lbFilters->AddItem(CSTRP(sbuff, sptr), filter);
	}
}

void __stdcall SSWR::SHPConv::SHPConvCurrFilterForm::OnDeleteClicked(AnyType userObj)
{
	NotNullPtr<SSWR::SHPConv::SHPConvCurrFilterForm> me = userObj.GetNN<SSWR::SHPConv::SHPConvCurrFilterForm>();
	UOSInt i = me->lbFilters->GetSelectedIndex();
	if (i != INVALID_INDEX)
	{
		MapFilter *filter = me->filters->RemoveAt(i);
		me->lbFilters->RemoveItem(i);
		DEL_CLASS(filter);
	}
}

void __stdcall SSWR::SHPConv::SHPConvCurrFilterForm::OnOkClicked(AnyType userObj)
{
	NotNullPtr<SSWR::SHPConv::SHPConvCurrFilterForm> me = userObj.GetNN<SSWR::SHPConv::SHPConvCurrFilterForm>();
	me->Close();
}

SSWR::SHPConv::SHPConvCurrFilterForm::SHPConvCurrFilterForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, DB::DBFFile *dbf, Data::ArrayList<MapFilter*> *filters, NotNullPtr<Media::DrawEngine> deng) : UI::GUIForm(parent, 414, 298, ui)
{
	this->SetText(CSTR("Current Filters"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	NEW_CLASS(this->monMgr, Media::MonitorMgr());
	this->SetDPI(this->monMgr->GetMonitorHDPI(this->GetHMonitor()), this->monMgr->GetMonitorDDPI(this->GetHMonitor()));
	this->deng = deng;
	this->dbf = dbf;
	this->filters = filters;
	
	this->lbFilters = ui->NewListBox(*this, false);
	this->lbFilters->SetRect(8, 8, 392, 199, false);
	this->btnAdd = ui->NewButton(*this, CSTR("Add Filter"));
	this->btnAdd->SetRect(8, 224, 96, 23, false);
	this->btnAdd->HandleButtonClick(OnAddClicked, this);
	this->btnDelete = ui->NewButton(*this, CSTR("Delete Filter"));
	this->btnDelete->SetRect(144, 224, 96, 23, false);
	this->btnDelete->HandleButtonClick(OnDeleteClicked, this);
	this->btnOK = ui->NewButton(*this, CSTR("OK"));
	this->btnOK->SetRect(320, 224, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOkClicked, this);

	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	MapFilter *filter;
	i = 0;
	j = this->filters->GetCount();
	while (i < j)
	{
		filter = this->filters->GetItem(i);
		sptr = filter->ToString(sbuff);
		this->lbFilters->AddItem(CSTRP(sbuff, sptr), filter);
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

