#include "Stdafx.h"
#include "IO/Path.h"
#include "Map/LayerTools.h"
#include "SSWR/AVIRead/AVIRGISCombineForm.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRGISCombineForm::OnCheckAllClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISCombineForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISCombineForm>();
	UOSInt i = me->layers->GetCount();
	while (i-- > 0)
	{
		me->lbLayers->SetItemChecked(i, true);
	}
}

void __stdcall SSWR::AVIRead::AVIRGISCombineForm::OnUncheckAllClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISCombineForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISCombineForm>();
	UOSInt i = me->layers->GetCount();
	while (i-- > 0)
	{
		me->lbLayers->SetItemChecked(i, false);
	}
}

void __stdcall SSWR::AVIRead::AVIRGISCombineForm::OnCombineClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISCombineForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISCombineForm>();
	UOSInt i;
	UOSInt j;
	NN<Map::MapDrawLayer> lyr;
	me->selLayers.Clear();
	i = 0;
	j = me->layers->GetCount();
	while (i < j)
	{
		if (me->lbLayers->GetItemChecked(i) && me->layers->GetItem(i).SetTo(lyr))
		{
			me->selLayers.Add(lyr);
		}
		i++;
	}

	if (me->selLayers.GetCount() > 0)
	{
		me->SetDialogResult(UI::GUIForm::DR_OK);
	}
	else
	{
		me->ui->ShowMsgOK(CSTR("Error in combining layers"), CSTR("Combine Layer"), me);
	}
}

void __stdcall SSWR::AVIRead::AVIRGISCombineForm::OnCancelClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISCombineForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISCombineForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIRGISCombineForm::AVIRGISCombineForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Data::ArrayListNN<Map::MapDrawLayer>> layers) : UI::GUIForm(parent, 456, 300, ui)
{
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->layers = layers;
	this->SetText(CSTR("Combine Layers"));
	this->SetFont(nullptr, 8.25, false);

	NN<UI::GUILabel> lbl;
	NN<UI::GUIPanel> pnl;
	pnl = ui->NewPanel(*this);
	pnl->SetRect(0, 0, 448, 48, false);
	pnl->SetDockType(UI::GUIControl::DOCK_TOP);
	lbl = ui->NewLabel(pnl, CSTR("Select layers to combine"));
	lbl->SetRect(0, 0, 280, 23, false);
	this->btnCheckAll = ui->NewButton(pnl, CSTR("Check All"));
	this->btnCheckAll->SetRect(0, 24, 80, 23, false);
	this->btnCheckAll->HandleButtonClick(OnCheckAllClick, this);
	this->btnUncheckAll = ui->NewButton(pnl, CSTR("Uncheck All"));
	this->btnUncheckAll->SetRect(88, 24, 80, 23, false);
	this->btnUncheckAll->HandleButtonClick(OnUncheckAllClick, this);

	pnl = ui->NewPanel(*this);
	pnl->SetRect(0, 0, 448, 40, false);
	pnl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->btnCombine = ui->NewButton(pnl, CSTR("Combine"));
	this->btnCombine->SetRect(64, 8, 75, 23, false);
	this->btnCombine->HandleButtonClick(OnCombineClick, this);
	this->btnCancel = ui->NewButton(pnl, CSTR("Cancel"));
	this->btnCancel->SetRect(160, 8, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClick, this);
	
	this->lbLayers = ui->NewCheckedListBox(*this);
	this->lbLayers->SetDockType(UI::GUIControl::DOCK_FILL);


	UOSInt j;
	NN<Map::MapDrawLayer> lyr;
	NN<Text::String> name;
	this->lbLayers->ClearItems();
	Data::ArrayIterator<NN<Map::MapDrawLayer>> it = this->layers->Iterator();
	while (it.HasNext())
	{
		lyr = it.Next();
		name = lyr->GetName();
		j = name->LastIndexOf(IO::Path::PATH_SEPERATOR);
		if (j != INVALID_INDEX)
		{
			this->lbLayers->AddItem({&name->v[j + 1], name->leng - j - 1}, 0);
		}
		else
		{
			this->lbLayers->AddItem(name, 0);
		}
	}
}

SSWR::AVIRead::AVIRGISCombineForm::~AVIRGISCombineForm()
{
}

void SSWR::AVIRead::AVIRGISCombineForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

Optional<Map::MapDrawLayer> SSWR::AVIRead::AVIRGISCombineForm::GetCombinedLayer()
{
	Optional<Map::VectorLayer> layer = nullptr;
	NN<Text::String> s = Text::String::New(UTF8STRC("CombinedLayer"));
	layer = Map::LayerTools::CombineLayers(this->selLayers, s);
	s->Release();
	return layer;
}
