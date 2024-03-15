#include "Stdafx.h"
#include "IO/Registry.h"
#include "Math/Math.h"
#include "SSWR/AVIRead/AVIRImageGRForm.h"
#include "Text/MyString.h"

void __stdcall SSWR::AVIRead::AVIRImageGRForm::OnHOfstChanged(AnyType userObj, UOSInt newPos)
{
	NotNullPtr<SSWR::AVIRead::AVIRImageGRForm> me = userObj.GetNN<SSWR::AVIRead::AVIRImageGRForm>();
	OSInt hOfst;
	OSInt vOfst;
	OSInt level;
	Int32 status;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	if (!me->modifying && me->currLayer != INVALID_INDEX && me->grFilter->GetParameter(me->currLayer, &hOfst, &vOfst, &level, &status))
	{
		me->grFilter->SetParameter(me->currLayer, (OSInt)newPos - 100, vOfst, level, status);
		me->UpdatePreview();
		sptr = Text::StrOSInt(sbuff, (OSInt)newPos - 100);
		me->txtHOfst->SetText(CSTRP(sbuff, sptr));
	}
}

void __stdcall SSWR::AVIRead::AVIRImageGRForm::OnVOfstChanged(AnyType userObj, UOSInt newPos)
{
	NotNullPtr<SSWR::AVIRead::AVIRImageGRForm> me = userObj.GetNN<SSWR::AVIRead::AVIRImageGRForm>();
	OSInt hOfst;
	OSInt vOfst;
	OSInt level;
	Int32 status;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	if (!me->modifying && me->currLayer != INVALID_INDEX && me->grFilter->GetParameter(me->currLayer, &hOfst, &vOfst, &level, &status))
	{
		me->grFilter->SetParameter(me->currLayer, hOfst, (OSInt)newPos - 100, level, status);
		me->UpdatePreview();
		sptr = Text::StrOSInt(sbuff, (OSInt)newPos - 100);
		me->txtVOfst->SetText(CSTRP(sbuff, sptr));
	}
}

void __stdcall SSWR::AVIRead::AVIRImageGRForm::OnLevelChanged(AnyType userObj, UOSInt newPos)
{
	NotNullPtr<SSWR::AVIRead::AVIRImageGRForm> me = userObj.GetNN<SSWR::AVIRead::AVIRImageGRForm>();
	OSInt hOfst;
	OSInt vOfst;
	OSInt level;
	Int32 status;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	if (!me->modifying && me->currLayer != INVALID_INDEX && me->grFilter->GetParameter(me->currLayer, &hOfst, &vOfst, &level, &status))
	{
		me->grFilter->SetParameter(me->currLayer, hOfst, vOfst, (OSInt)newPos - 100, status);
		me->UpdatePreview();
		sptr=  Text::StrOSInt(sbuff, (OSInt)newPos - 100);
		me->txtLevel->SetText(CSTRP(sbuff, sptr));
	}
}

void __stdcall SSWR::AVIRead::AVIRImageGRForm::OnTypeChanged(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRImageGRForm> me = userObj.GetNN<SSWR::AVIRead::AVIRImageGRForm>();
	OSInt hOfst;
	OSInt vOfst;
	OSInt level;
	Int32 status;
	if (!me->modifying && me->currLayer != INVALID_INDEX && me->grFilter->GetParameter(me->currLayer, &hOfst, &vOfst, &level, &status))
	{
		status = ((status & ~3) | (Int32)(UInt32)me->cboType->GetSelectedIndex());
		me->grFilter->SetParameter(me->currLayer, hOfst, vOfst, level, status);
		me->UpdatePreview();
	}
}

void __stdcall SSWR::AVIRead::AVIRImageGRForm::OnEnableChanged(AnyType userObj, Bool newVal)
{
	NotNullPtr<SSWR::AVIRead::AVIRImageGRForm> me = userObj.GetNN<SSWR::AVIRead::AVIRImageGRForm>();
	OSInt hOfst;
	OSInt vOfst;
	OSInt level;
	Int32 status;
	if (!me->modifying && me->currLayer != INVALID_INDEX && me->grFilter->GetParameter(me->currLayer, &hOfst, &vOfst, &level, &status))
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

void __stdcall SSWR::AVIRead::AVIRImageGRForm::OnAddLayerClicked(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRImageGRForm> me = userObj.GetNN<SSWR::AVIRead::AVIRImageGRForm>();
	me->grFilter->AddLayer();
	me->UpdateLayers();
}

void __stdcall SSWR::AVIRead::AVIRImageGRForm::OnRemoveLayerClicked(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRImageGRForm> me = userObj.GetNN<SSWR::AVIRead::AVIRImageGRForm>();
	if (me->currLayer != INVALID_INDEX)
	{
		me->grFilter->RemoveLayer(me->currLayer);
		me->UpdateLayers();
	}
}

void __stdcall SSWR::AVIRead::AVIRImageGRForm::OnLayersChanged(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRImageGRForm> me = userObj.GetNN<SSWR::AVIRead::AVIRImageGRForm>();
	me->currLayer = (UOSInt)me->lbLayers->GetSelectedIndex();
	if (me->currLayer != INVALID_INDEX)
	{
		OSInt hOfst;
		OSInt vOfst;
		OSInt level;
		Int32 status;
		UTF8Char sbuff[32];
		UTF8Char *sptr;
		if (me->grFilter->GetParameter(me->currLayer, &hOfst, &vOfst, &level, &status))
		{
			me->modifying = true;
			sptr = Text::StrOSInt(sbuff, hOfst);
			me->txtHOfst->SetText(CSTRP(sbuff, sptr));
			me->hsbHOfst->SetPos((UOSInt)(hOfst + 100));
			sptr = Text::StrOSInt(sbuff, vOfst);
			me->txtVOfst->SetText(CSTRP(sbuff, sptr));
			me->hsbVOfst->SetPos((UOSInt)(vOfst + 100));
			sptr = Text::StrOSInt(sbuff, level);
			me->txtLevel->SetText(CSTRP(sbuff, sptr));
			me->hsbLevel->SetPos((UOSInt)(level + 100));
			me->cboType->SetSelectedIndex(status & 3);
			me->chkEnable->SetChecked((status & 4) == 0);
			me->modifying = false;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRImageGRForm::OnOKClicked(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRImageGRForm> me = userObj.GetNN<SSWR::AVIRead::AVIRImageGRForm>();
	if (me->srcImg->info.pf == Media::PF_B8G8R8)
	{
		me->srcImg->To32bpp();
		me->destImg->To32bpp();
	}
	if (me->srcImg->info.pf == Media::PF_B8G8R8A8)
	{
		me->grFilter->ProcessImage32(me->srcImg->data, me->destImg->data, me->srcImg->info.dispSize.x, me->srcImg->info.dispSize.y, (OSInt)(me->srcImg->info.storeSize.x * (me->srcImg->info.storeBPP >> 3)), (OSInt)(me->destImg->info.storeSize.x * (me->srcImg->info.storeBPP >> 3)));
		me->previewCtrl->SetImage(me->destImg.Ptr(), true);
	}
	else if (me->srcImg->info.pf == Media::PF_LE_B16G16R16A16)
	{
		me->grFilter->ProcessImage64(me->srcImg->data, me->destImg->data, me->srcImg->info.dispSize.x, me->srcImg->info.dispSize.y, (OSInt)(me->srcImg->info.storeSize.x * (me->srcImg->info.storeBPP >> 3)), (OSInt)(me->destImg->info.storeSize.x * (me->srcImg->info.storeBPP >> 3)));
		me->previewCtrl->SetImage(me->destImg.Ptr(), true);
	}
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRImageGRForm::OnCancelClicked(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRImageGRForm> me = userObj.GetNN<SSWR::AVIRead::AVIRImageGRForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

void SSWR::AVIRead::AVIRImageGRForm::UpdatePreview()
{
	if (this->srcPrevImg->info.pf == Media::PF_B8G8R8A8)
	{
		this->grFilter->ProcessImage32(this->srcPrevImg->data, this->destPrevImg->data, this->srcPrevImg->info.dispSize.x, this->srcPrevImg->info.dispSize.y, (OSInt)(this->srcPrevImg->info.storeSize.x * (this->srcPrevImg->info.storeBPP >> 3)), (OSInt)(this->destPrevImg->info.storeSize.x * (this->srcPrevImg->info.storeBPP >> 3)));
	}
	else if (this->srcPrevImg->info.pf == Media::PF_LE_B16G16R16A16)
	{
		this->grFilter->ProcessImage64(this->srcPrevImg->data, this->destPrevImg->data, this->srcPrevImg->info.dispSize.x, this->srcPrevImg->info.dispSize.y, (OSInt)(this->srcPrevImg->info.storeSize.x * (this->srcPrevImg->info.storeBPP >> 3)), (OSInt)(this->destPrevImg->info.storeSize.x * (this->srcPrevImg->info.storeBPP >> 3)));
	}
	this->previewCtrl->SetImage(this->destPrevImg.Ptr(), true);
}

void SSWR::AVIRead::AVIRImageGRForm::UpdateLayers()
{
	UOSInt i;
	UOSInt j;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	this->lbLayers->ClearItems();
	this->currLayer = INVALID_INDEX;
	i = 0;
	j = this->grFilter->GetLayerCount();
	while (i < j)
	{
		sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Layer ")), i + 1);
		this->lbLayers->AddItem(CSTRP(sbuff, sptr), (void*)i);
		i++;
	}
}

SSWR::AVIRead::AVIRImageGRForm::AVIRImageGRForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<Media::StaticImage> srcImg, NotNullPtr<Media::StaticImage> destImg, NotNullPtr<UI::GUIPictureBoxDD> previewCtrl) : UI::GUIForm(parent, 640, 480, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("GR Filter"));

	this->core = core;
	this->currLayer = INVALID_INDEX;
	this->srcImg = srcImg;
	this->destImg = destImg;
	this->previewCtrl = previewCtrl;
	NEW_CLASS(this->grFilter, Media::GRFilter());
	this->srcPrevImg = this->previewCtrl->CreatePreviewImage(this->srcImg);
	this->destPrevImg = this->srcPrevImg->CreateStaticImage();
	this->modifying = false;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlLayers = ui->NewPanel(*this);
	this->pnlLayers->SetRect(0, 0, 200, 23, false);
	this->pnlLayers->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->btnAddLayer = ui->NewButton(this->pnlLayers, CSTR("New Layer"));
	this->btnAddLayer->SetRect(0, 0, 100, 23, false);
	this->btnAddLayer->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->btnAddLayer->HandleButtonClick(OnAddLayerClicked, this);
	this->lbLayers = ui->NewListBox(this->pnlLayers, false);
	this->lbLayers->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLayers->HandleSelectionChange(OnLayersChanged, this);
	this->hspLayers = ui->NewHSplitter(*this, 3, false);
	this->pnlSetting = ui->NewPanel(*this);
	this->pnlSetting->SetDockType(UI::GUIControl::DOCK_FILL);
	this->btnRemoveLayer = ui->NewButton(this->pnlSetting, CSTR("Remove Layer"));
	this->btnRemoveLayer->SetRect(4, 4, 75, 23, false);
	this->btnRemoveLayer->HandleButtonClick(OnRemoveLayerClicked, this);
	this->lblHOfst = ui->NewLabel(this->pnlSetting, CSTR("H Offset"));
	this->lblHOfst->SetRect(4, 28, 100, 23, false);
	this->hsbHOfst = ui->NewHScrollBar(this->pnlSetting, 16);
	this->hsbHOfst->SetRect(104, 28, 300, 23, false);
	this->hsbHOfst->InitScrollBar(0, 210, 100, 10);
	this->hsbHOfst->HandlePosChanged(OnHOfstChanged, this);
	this->txtHOfst = ui->NewTextBox(this->pnlSetting, CSTR("0"));
	this->txtHOfst->SetRect(404, 28, 100, 23, false);
	this->txtHOfst->SetReadOnly(true);
	this->lblVOfst = ui->NewLabel(this->pnlSetting, CSTR("V Offset"));
	this->lblVOfst->SetRect(4, 52, 100, 23, false);
	this->hsbVOfst = ui->NewHScrollBar(this->pnlSetting, 16);
	this->hsbVOfst->SetRect(104, 52, 300, 23, false);
	this->hsbVOfst->InitScrollBar(0, 210, 100, 10);
	this->hsbVOfst->HandlePosChanged(OnVOfstChanged, this);
	this->txtVOfst = ui->NewTextBox(this->pnlSetting, CSTR("0"));
	this->txtVOfst->SetRect(404, 52, 100, 23, false);
	this->txtVOfst->SetReadOnly(true);
	this->lblLevel = ui->NewLabel(this->pnlSetting, CSTR("Level"));
	this->lblLevel->SetRect(4, 76, 100, 23, false);
	this->hsbLevel = ui->NewHScrollBar(this->pnlSetting, 16);
	this->hsbLevel->SetRect(104, 76, 300, 23, false);
	this->hsbLevel->InitScrollBar(0, 210, 100, 10);
	this->hsbLevel->HandlePosChanged(OnLevelChanged, this);
	this->txtLevel = ui->NewTextBox(this->pnlSetting, CSTR("0"));
	this->txtLevel->SetRect(404, 76, 100, 23, false);
	this->txtLevel->SetReadOnly(true);
	this->lblType = ui->NewLabel(this->pnlSetting, CSTR("Type"));
	this->lblType->SetRect(4, 100, 100, 23, false);
	this->cboType = ui->NewComboBox(this->pnlSetting, false);
	this->cboType->SetRect(104, 100, 150, 23, false);
	this->cboType->AddItem(CSTR("H GR"), 0);
	this->cboType->AddItem(CSTR("V GR"), 0);
	this->cboType->AddItem(CSTR("HV GR"), 0);
	this->cboType->HandleSelectionChange(OnTypeChanged, this);
	this->chkEnable = ui->NewCheckBox(this->pnlSetting, CSTR("Enabled"), true);
	this->chkEnable->SetRect(104, 124, 200, 23, false);
	this->chkEnable->HandleCheckedChange(OnEnableChanged, this);

	this->btnCancel = ui->NewButton(this->pnlSetting, CSTR("Cancel"));
	this->btnCancel->SetRect(104, 156, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	this->btnOK = ui->NewButton(this->pnlSetting, CSTR("OK"));
	this->btnOK->SetRect(184, 156, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);
}

SSWR::AVIRead::AVIRImageGRForm::~AVIRImageGRForm()
{
	DEL_CLASS(this->grFilter);
	this->srcPrevImg.Delete();
	this->destPrevImg.Delete();
}

void SSWR::AVIRead::AVIRImageGRForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
