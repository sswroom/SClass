#include "Stdafx.h"
#include "IO/Registry.h"
#include "Math/Math.h"
#include "SSWR/AVIRead/AVIRImageGRForm.h"
#include "Text/MyString.h"

void __stdcall SSWR::AVIRead::AVIRImageGRForm::OnHOfstChanged(void *userObj, OSInt newPos)
{
	SSWR::AVIRead::AVIRImageGRForm *me = (SSWR::AVIRead::AVIRImageGRForm*)userObj;
	OSInt hOfst;
	OSInt vOfst;
	OSInt level;
	Int32 status;
	UTF8Char sbuff[32];
	if (!me->modifying && me->currLayer != (UOSInt)-1 && me->grFilter->GetParameter(me->currLayer, &hOfst, &vOfst, &level, &status))
	{
		me->grFilter->SetParameter(me->currLayer, newPos - 100, vOfst, level, status);
		me->UpdatePreview();
		Text::StrOSInt(sbuff, newPos - 100);
		me->txtHOfst->SetText(sbuff);
	}
}

void __stdcall SSWR::AVIRead::AVIRImageGRForm::OnVOfstChanged(void *userObj, OSInt newPos)
{
	SSWR::AVIRead::AVIRImageGRForm *me = (SSWR::AVIRead::AVIRImageGRForm*)userObj;
	OSInt hOfst;
	OSInt vOfst;
	OSInt level;
	Int32 status;
	UTF8Char sbuff[32];
	if (!me->modifying && me->currLayer != (UOSInt)-1 && me->grFilter->GetParameter(me->currLayer, &hOfst, &vOfst, &level, &status))
	{
		me->grFilter->SetParameter(me->currLayer, hOfst, newPos - 100, level, status);
		me->UpdatePreview();
		Text::StrOSInt(sbuff, newPos - 100);
		me->txtVOfst->SetText(sbuff);
	}
}

void __stdcall SSWR::AVIRead::AVIRImageGRForm::OnLevelChanged(void *userObj, OSInt newPos)
{
	SSWR::AVIRead::AVIRImageGRForm *me = (SSWR::AVIRead::AVIRImageGRForm*)userObj;
	OSInt hOfst;
	OSInt vOfst;
	OSInt level;
	Int32 status;
	UTF8Char sbuff[32];
	if (!me->modifying && me->currLayer != (UOSInt)-1 && me->grFilter->GetParameter(me->currLayer, &hOfst, &vOfst, &level, &status))
	{
		me->grFilter->SetParameter(me->currLayer, hOfst, vOfst, newPos - 100, status);
		me->UpdatePreview();
		Text::StrOSInt(sbuff, newPos - 100);
		me->txtLevel->SetText(sbuff);
	}
}

void __stdcall SSWR::AVIRead::AVIRImageGRForm::OnTypeChanged(void *userObj)
{
	SSWR::AVIRead::AVIRImageGRForm *me = (SSWR::AVIRead::AVIRImageGRForm*)userObj;
	OSInt hOfst;
	OSInt vOfst;
	OSInt level;
	Int32 status;
	if (!me->modifying && me->currLayer != (UOSInt)-1 && me->grFilter->GetParameter(me->currLayer, &hOfst, &vOfst, &level, &status))
	{
		status = ((status & ~3) | (Int32)(UInt32)me->cboType->GetSelectedIndex());
		me->grFilter->SetParameter(me->currLayer, hOfst, vOfst, level, status);
		me->UpdatePreview();
	}
}

void __stdcall SSWR::AVIRead::AVIRImageGRForm::OnEnableChanged(void *userObj, Bool newVal)
{
	SSWR::AVIRead::AVIRImageGRForm *me = (SSWR::AVIRead::AVIRImageGRForm*)userObj;
	OSInt hOfst;
	OSInt vOfst;
	OSInt level;
	Int32 status;
	if (!me->modifying && me->currLayer != (UOSInt)-1 && me->grFilter->GetParameter(me->currLayer, &hOfst, &vOfst, &level, &status))
	{
		if (newVal)
		{
			status = status & ~4;
		}
		else
		{
			status = status | 4;
		}
		me->grFilter->SetParameter(me->currLayer, hOfst, vOfst, level, status);
		me->UpdatePreview();
	}
}

void __stdcall SSWR::AVIRead::AVIRImageGRForm::OnAddLayerClicked(void *userObj)
{
	SSWR::AVIRead::AVIRImageGRForm *me = (SSWR::AVIRead::AVIRImageGRForm*)userObj;
	me->grFilter->AddLayer();
	me->UpdateLayers();
}

void __stdcall SSWR::AVIRead::AVIRImageGRForm::OnRemoveLayerClicked(void *userObj)
{
	SSWR::AVIRead::AVIRImageGRForm *me = (SSWR::AVIRead::AVIRImageGRForm*)userObj;
	if (me->currLayer != (UOSInt)-1)
	{
		me->grFilter->RemoveLayer(me->currLayer);
		me->UpdateLayers();
	}
}

void __stdcall SSWR::AVIRead::AVIRImageGRForm::OnLayersChanged(void *userObj)
{
	SSWR::AVIRead::AVIRImageGRForm *me = (SSWR::AVIRead::AVIRImageGRForm*)userObj;
	me->currLayer = (UOSInt)me->lbLayers->GetSelectedIndex();
	if (me->currLayer != (UOSInt)-1)
	{
		OSInt hOfst;
		OSInt vOfst;
		OSInt level;
		Int32 status;
		UTF8Char sbuff[32];
		if (me->grFilter->GetParameter(me->currLayer, &hOfst, &vOfst, &level, &status))
		{
			me->modifying = true;
			Text::StrOSInt(sbuff, hOfst);
			me->txtHOfst->SetText(sbuff);
			me->hsbHOfst->SetPos(hOfst + 100);
			Text::StrOSInt(sbuff, vOfst);
			me->txtVOfst->SetText(sbuff);
			me->hsbVOfst->SetPos(vOfst + 100);
			Text::StrOSInt(sbuff, level);
			me->txtLevel->SetText(sbuff);
			me->hsbLevel->SetPos(level + 100);
			me->cboType->SetSelectedIndex(status & 3);
			me->chkEnable->SetChecked((status & 4) == 0);
			me->modifying = false;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRImageGRForm::OnOKClicked(void *userObj)
{
	SSWR::AVIRead::AVIRImageGRForm *me = (SSWR::AVIRead::AVIRImageGRForm*)userObj;
	if (me->srcImg->info->pf == Media::PF_B8G8R8)
	{
		me->srcImg->To32bpp();
		me->destImg->To32bpp();
	}
	if (me->srcImg->info->pf == Media::PF_B8G8R8A8)
	{
		me->grFilter->ProcessImage32(me->srcImg->data, me->destImg->data, me->srcImg->info->dispWidth, me->srcImg->info->dispHeight, (OSInt)(me->srcImg->info->storeWidth * (me->srcImg->info->storeBPP >> 3)), (OSInt)(me->destImg->info->storeWidth * (me->srcImg->info->storeBPP >> 3)));
		me->previewCtrl->SetImage(me->destImg, true);
	}
	else if (me->srcImg->info->pf == Media::PF_LE_B16G16R16A16)
	{
		me->grFilter->ProcessImage64(me->srcImg->data, me->destImg->data, me->srcImg->info->dispWidth, me->srcImg->info->dispHeight, (OSInt)(me->srcImg->info->storeWidth * (me->srcImg->info->storeBPP >> 3)), (OSInt)(me->destImg->info->storeWidth * (me->srcImg->info->storeBPP >> 3)));
		me->previewCtrl->SetImage(me->destImg, true);
	}
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRImageGRForm::OnCancelClicked(void *userObj)
{
	SSWR::AVIRead::AVIRImageGRForm *me = (SSWR::AVIRead::AVIRImageGRForm*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

void SSWR::AVIRead::AVIRImageGRForm::UpdatePreview()
{
	if (this->srcPrevImg->info->pf == Media::PF_B8G8R8A8)
	{
		this->grFilter->ProcessImage32(this->srcPrevImg->data, this->destPrevImg->data, this->srcPrevImg->info->dispWidth, this->srcPrevImg->info->dispHeight, (OSInt)(this->srcPrevImg->info->storeWidth * (this->srcPrevImg->info->storeBPP >> 3)), (OSInt)(this->destPrevImg->info->storeWidth * (this->srcPrevImg->info->storeBPP >> 3)));
	}
	else if (this->srcPrevImg->info->pf == Media::PF_LE_B16G16R16A16)
	{
		this->grFilter->ProcessImage64(this->srcPrevImg->data, this->destPrevImg->data, this->srcPrevImg->info->dispWidth, this->srcPrevImg->info->dispHeight, (OSInt)(this->srcPrevImg->info->storeWidth * (this->srcPrevImg->info->storeBPP >> 3)), (OSInt)(this->destPrevImg->info->storeWidth * (this->srcPrevImg->info->storeBPP >> 3)));
	}
	this->previewCtrl->SetImage(this->destPrevImg, true);
}

void SSWR::AVIRead::AVIRImageGRForm::UpdateLayers()
{
	UOSInt i;
	UOSInt j;
	UTF8Char sbuff[32];
	this->lbLayers->ClearItems();
	this->currLayer = (UOSInt)-1;
	i = 0;
	j = this->grFilter->GetLayerCount();
	while (i < j)
	{
		Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"Layer "), i + 1);
		this->lbLayers->AddItem(sbuff, (void*)i);
		i++;
	}
}

SSWR::AVIRead::AVIRImageGRForm::AVIRImageGRForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Media::StaticImage *srcImg, Media::StaticImage *destImg, UI::GUIPictureBoxDD *previewCtrl) : UI::GUIForm(parent, 640, 480, ui)
{
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"GR Filter");

	this->core = core;
	this->currLayer = (UOSInt)-1;
	this->srcImg = srcImg;
	this->destImg = destImg;
	this->previewCtrl = previewCtrl;
	NEW_CLASS(this->grFilter, Media::GRFilter());
	this->srcPrevImg = 0;
	this->destPrevImg = 0;
	this->srcPrevImg = this->previewCtrl->CreatePreviewImage(this->srcImg);
	this->destPrevImg = this->srcPrevImg->CreateStaticImage();
	this->modifying = false;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlLayers, UI::GUIPanel(ui, this));
	this->pnlLayers->SetRect(0, 0, 200, 23, false);
	this->pnlLayers->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->btnAddLayer, UI::GUIButton(ui, this->pnlLayers, (const UTF8Char*)"New Layer"));
	this->btnAddLayer->SetRect(0, 0, 100, 23, false);
	this->btnAddLayer->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->btnAddLayer->HandleButtonClick(OnAddLayerClicked, this);
	NEW_CLASS(this->lbLayers, UI::GUIListBox(ui, this->pnlLayers, false));
	this->lbLayers->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLayers->HandleSelectionChange(OnLayersChanged, this);
	NEW_CLASS(this->hspLayers, UI::GUIHSplitter(ui, this, 3, false));
	NEW_CLASS(this->pnlSetting, UI::GUIPanel(ui, this));
	this->pnlSetting->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->btnRemoveLayer, UI::GUIButton(ui, this->pnlSetting, (const UTF8Char*)"Remove Layer"));
	this->btnRemoveLayer->SetRect(4, 4, 75, 23, false);
	this->btnRemoveLayer->HandleButtonClick(OnRemoveLayerClicked, this);
	NEW_CLASS(this->lblHOfst, UI::GUILabel(ui, this->pnlSetting, (const UTF8Char*)"H Offset"));
	this->lblHOfst->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->hsbHOfst, UI::GUIHScrollBar(ui, this->pnlSetting, 16));
	this->hsbHOfst->SetRect(104, 28, 300, 23, false);
	this->hsbHOfst->InitScrollBar(0, 210, 100, 10);
	this->hsbHOfst->HandlePosChanged(OnHOfstChanged, this);
	NEW_CLASS(this->txtHOfst, UI::GUITextBox(ui, this->pnlSetting, (const UTF8Char*)"0"));
	this->txtHOfst->SetRect(404, 28, 100, 23, false);
	this->txtHOfst->SetReadOnly(true);
	NEW_CLASS(this->lblVOfst, UI::GUILabel(ui, this->pnlSetting, (const UTF8Char*)"V Offset"));
	this->lblVOfst->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->hsbVOfst, UI::GUIHScrollBar(ui, this->pnlSetting, 16));
	this->hsbVOfst->SetRect(104, 52, 300, 23, false);
	this->hsbVOfst->InitScrollBar(0, 210, 100, 10);
	this->hsbVOfst->HandlePosChanged(OnVOfstChanged, this);
	NEW_CLASS(this->txtVOfst, UI::GUITextBox(ui, this->pnlSetting, (const UTF8Char*)"0"));
	this->txtVOfst->SetRect(404, 52, 100, 23, false);
	this->txtVOfst->SetReadOnly(true);
	NEW_CLASS(this->lblLevel, UI::GUILabel(ui, this->pnlSetting, (const UTF8Char*)"Level"));
	this->lblLevel->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->hsbLevel, UI::GUIHScrollBar(ui, this->pnlSetting, 16));
	this->hsbLevel->SetRect(104, 76, 300, 23, false);
	this->hsbLevel->InitScrollBar(0, 210, 100, 10);
	this->hsbLevel->HandlePosChanged(OnLevelChanged, this);
	NEW_CLASS(this->txtLevel, UI::GUITextBox(ui, this->pnlSetting, (const UTF8Char*)"0"));
	this->txtLevel->SetRect(404, 76, 100, 23, false);
	this->txtLevel->SetReadOnly(true);
	NEW_CLASS(this->lblType, UI::GUILabel(ui, this->pnlSetting, (const UTF8Char*)"Type"));
	this->lblType->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->cboType, UI::GUIComboBox(ui, this->pnlSetting, false));
	this->cboType->SetRect(104, 100, 150, 23, false);
	this->cboType->AddItem((const UTF8Char*)"H GR", 0);
	this->cboType->AddItem((const UTF8Char*)"V GR", 0);
	this->cboType->AddItem((const UTF8Char*)"HV GR", 0);
	this->cboType->HandleSelectionChange(OnTypeChanged, this);
	NEW_CLASS(this->chkEnable, UI::GUICheckBox(ui, this->pnlSetting, (const UTF8Char*)"Enabled", true));
	this->chkEnable->SetRect(104, 124, 200, 23, false);
	this->chkEnable->HandleCheckedChange(OnEnableChanged, this);

	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this->pnlSetting, (const UTF8Char*)"Cancel"));
	this->btnCancel->SetRect(104, 156, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this->pnlSetting, (const UTF8Char*)"OK"));
	this->btnOK->SetRect(184, 156, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);
}

SSWR::AVIRead::AVIRImageGRForm::~AVIRImageGRForm()
{
	DEL_CLASS(this->grFilter);
	SDEL_CLASS(this->srcPrevImg);
	SDEL_CLASS(this->destPrevImg);
}

void SSWR::AVIRead::AVIRImageGRForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
