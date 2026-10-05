#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "SSWR/AVIRead/AVIRConfigForm.h"

void __stdcall SSWR::AVIRead::AVIRConfigForm::OnCategorySelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRConfigForm> me = userObj.GetNN<SSWR::AVIRead::AVIRConfigForm>();
	me->lvKeyValues->ClearItems();
	Data::ArrayListStringNN keyList;
	NN<Text::String> s;
	NN<Text::String> k;
	NN<Text::String> v;
	if (!me->lbCategory->GetSelectedItemTextNew().SetTo(s))
	{
		return;
	}
	me->cfg->GetKeys(s, keyList);
	Data::Sort::ArtificialQuickSort::Sort<NN<Text::String>>(keyList, keyList);
	UIntOS i = 0;
	UIntOS j = keyList.GetCount();
	while (i < j)
	{
		k = keyList.GetItemNoCheck(i);
		me->lvKeyValues->AddItem(k, 0);
		if (me->cfg->GetCateValue(s, k).SetTo(v))
		{
			me->lvKeyValues->SetSubItem(i, 1, v);
		}
		i++;
	}
	s->Release();
}

SSWR::AVIRead::AVIRConfigForm::AVIRConfigForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<IO::ConfigFile> cfg) : UI::GUIForm(parent, 1024, 768, ui)
{
	Text::StringBuilderUTF8 sb;
	sb.Append(CSTR("Config file - "));
	sb.Append(cfg->GetSourceNameObj());
	this->SetText(sb.ToCString());
	this->SetFont(nullptr, 8.25, false);

	this->core = core;
	this->cfg = cfg;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lbCategory = ui->NewListBox(*this, false);
	this->lbCategory->SetRect(0, 0, 150, 23, false);
	this->lbCategory->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbCategory->HandleSelectionChange(OnCategorySelChg, this);
	this->hspMain = ui->NewHSplitter(*this, 3, false);
	this->lvKeyValues = ui->NewListView(*this, UI::ListViewStyle::Table, 2);
	this->lvKeyValues->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvKeyValues->AddColumn(CSTR("Key"), 200);
	this->lvKeyValues->AddColumn(CSTR("Value"), 400);
	this->lvKeyValues->SetShowGrid(true);
	this->lvKeyValues->SetFullRowSelect(true);

	Data::ArrayListStringNN cateList;
	cfg->GetCateList(cateList, true);
	UIntOS i = 0;
	UIntOS j = cateList.GetCount();
	while (i < j)
	{
		this->lbCategory->AddItem(cateList.GetItemNoCheck(i), 0);
		i++;
	}
	if (j > 0)
	{
		this->lbCategory->SetSelectedIndex(0);
	}
}

SSWR::AVIRead::AVIRConfigForm::~AVIRConfigForm()
{
	this->cfg.Delete();
}

void SSWR::AVIRead::AVIRConfigForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
