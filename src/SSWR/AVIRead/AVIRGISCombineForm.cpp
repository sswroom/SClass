#include "Stdafx.h"
#include "IO/Path.h"
#include "Map/LayerTools.h"
#include "SSWR/AVIRead/AVIRGISCombineForm.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRGISCombineForm::OnCheckAllClick(void *userObj)
{
	SSWR::AVIRead::AVIRGISCombineForm *me = (SSWR::AVIRead::AVIRGISCombineForm*)userObj;
	UOSInt i = me->layers->GetCount();
	while (i-- > 0)
	{
		me->lbLayers->SetItemChecked(i, true);
	}
}

void __stdcall SSWR::AVIRead::AVIRGISCombineForm::OnUncheckAllClick(void *userObj)
{
	SSWR::AVIRead::AVIRGISCombineForm *me = (SSWR::AVIRead::AVIRGISCombineForm*)userObj;
	UOSInt i = me->layers->GetCount();
	while (i-- > 0)
	{
		me->lbLayers->SetItemChecked(i, false);
	}
}

void __stdcall SSWR::AVIRead::AVIRGISCombineForm::OnCombineClick(void *userObj)
{
	SSWR::AVIRead::AVIRGISCombineForm *me = (SSWR::AVIRead::AVIRGISCombineForm*)userObj;
	UOSInt i;
	UOSInt j;
	me->selLayers->Clear();
	i = 0;
	j = me->layers->GetCount();
	while (i < j)
	{
		if (me->lbLayers->GetItemChecked(i))
		{
			me->selLayers->Add(me->layers->GetItem(i));
		}
		i++;
	}

	if (me->selLayers->GetCount() > 0)
	{
		me->SetDialogResult(UI::GUIForm::DR_OK);
	}
	else
	{
		UI::MessageDialog::ShowDialog((const UTF8Char *)"Error in combining layers", (const UTF8Char *)"Combine Layer", me);
	}
}

void __stdcall SSWR::AVIRead::AVIRGISCombineForm::OnCancelClick(void *userObj)
{
	SSWR::AVIRead::AVIRGISCombineForm *me = (SSWR::AVIRead::AVIRGISCombineForm*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIRGISCombineForm::AVIRGISCombineForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Data::ArrayList<Map::IMapDrawLayer*> *layers) : UI::GUIForm(parent, 456, 300, ui)
{
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->layers = layers;
	SetText((const UTF8Char*)"Combine Layers");
	SetFont(0, 8.25, false);

	NEW_CLASS(this->selLayers, Data::ArrayList<Map::IMapDrawLayer*>());

	UI::GUILabel *lbl;
	UI::GUIPanel *pnl;
	NEW_CLASS(pnl, UI::GUIPanel(ui, this));
	pnl->SetRect(0, 0, 448, 48, false);
	pnl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(lbl, UI::GUILabel(ui, pnl, (const UTF8Char*)"Select layers to combine"));
	lbl->SetRect(0, 0, 280, 23, false);
	NEW_CLASS(this->btnCheckAll, UI::GUIButton(ui, pnl, (const UTF8Char*)"Check All"));
	this->btnCheckAll->SetRect(0, 24, 80, 23, false);
	this->btnCheckAll->HandleButtonClick(OnCheckAllClick, this);
	NEW_CLASS(this->btnUncheckAll, UI::GUIButton(ui, pnl, (const UTF8Char*)"Uncheck All"));
	this->btnUncheckAll->SetRect(88, 24, 80, 23, false);
	this->btnUncheckAll->HandleButtonClick(OnUncheckAllClick, this);

	NEW_CLASS(pnl, UI::GUIPanel(ui, this));
	pnl->SetRect(0, 0, 448, 40, false);
	pnl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->btnCombine, UI::GUIButton(ui, pnl, (const UTF8Char*)"Combine"));
	this->btnCombine->SetRect(64, 8, 75, 23, false);
	this->btnCombine->HandleButtonClick(OnCombineClick, this);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, pnl, (const UTF8Char*)"Cancel"));
	this->btnCancel->SetRect(160, 8, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClick, this);
	
	NEW_CLASS(this->lbLayers, UI::GUICheckedListBox(ui, this));
	this->lbLayers->SetDockType(UI::GUIControl::DOCK_FILL);


	UOSInt i = 0;
	UOSInt cnt = this->layers->GetCount();
	OSInt j;
	Map::IMapDrawLayer *lyr;
	const UTF8Char *name;
	this->lbLayers->ClearItems();
	while (i < cnt)
	{
		lyr = this->layers->GetItem(i);
		name = lyr->GetName();
		j = ::Text::StrLastIndexOf(name, IO::Path::PATH_SEPERATOR);
		if (j >= 0)
		{
			name = &name[j + 1];
		}
		this->lbLayers->AddItem(name, 0);
		i++;
	}
}

SSWR::AVIRead::AVIRGISCombineForm::~AVIRGISCombineForm()
{
	DEL_CLASS(this->selLayers);
}

void SSWR::AVIRead::AVIRGISCombineForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

Map::IMapDrawLayer *SSWR::AVIRead::AVIRGISCombineForm::GetCombinedLayer()
{
	Map::VectorLayer *layer = 0;
	layer = Map::LayerTools::CombineLayers(this->selLayers, (const UTF8Char*)"CombinedLayer");
	return layer;
}
