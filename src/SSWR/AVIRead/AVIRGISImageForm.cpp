#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRGISImageForm.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRGISImageForm::OnOKClick(void *userObj)
{
	SSWR::AVIRead::AVIRGISImageForm *me = (SSWR::AVIRead::AVIRGISImageForm*)userObj;
	UOSInt selIndex = me->plIcons->GetSelectedIndex();
	if (selIndex != INVALID_INDEX)
	{
		me->imgIndex = selIndex;
		me->SetDialogResult(UI::GUIForm::DR_OK);
	}
	else
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Select an image first", (const UTF8Char*)"Select Image", me);
	}
}

void __stdcall SSWR::AVIRead::AVIRGISImageForm::OnCancelClick(void *userObj)
{
	SSWR::AVIRead::AVIRGISImageForm *me = (SSWR::AVIRead::AVIRGISImageForm*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

void __stdcall SSWR::AVIRead::AVIRGISImageForm::OnFileDrop(void *userObj, const UTF8Char **fileNames, UOSInt fileCnt)
{
	SSWR::AVIRead::AVIRGISImageForm *me = (SSWR::AVIRead::AVIRGISImageForm *)userObj;
	Bool loaded = false;
	UOSInt i;
	i = 0;
	while (i < fileCnt)
	{
		if (me->env->AddImage(fileNames[i], me->parsers) >= 0)
		{
			loaded = true;
		}
		i++;
	}
	if (loaded)
	{
		me->UpdateImages();
		me->plIcons->Redraw();
	}
}

void SSWR::AVIRead::AVIRGISImageForm::UpdateImages()
{
	UOSInt i = 0;
	UOSInt j = this->env->GetImageCnt();
	UInt32 imgDurMS;

	this->plIcons->Clear();
	while (i < j)
	{
		this->plIcons->Add(this->env->GetImage(i, &imgDurMS));
		i++;
	}
}

SSWR::AVIRead::AVIRGISImageForm::AVIRGISImageForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Map::MapEnv *env, UOSInt imgIndex) : UI::GUIForm(parent, 456, 405, ui)
{
	this->SetText((const UTF8Char*)"Select Image");
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	NEW_CLASS(this->lbl, UI::GUILabel(ui, this, (const UTF8Char*)"Drag and drop to add icons"));
	this->lbl->SetRect(0, 0, 100, 23, false);
	this->lbl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->pnlButtons, UI::GUIPanel(ui, this));
	this->pnlButtons->SetRect(0, 0, 456, 48, false);
	this->pnlButtons->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this->pnlButtons, (const UTF8Char*)"&OK"));
	this->btnOK->SetRect(96, 8, 100, 23, false);
	this->btnOK->HandleButtonClick(OnOKClick, this);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this->pnlButtons, (const UTF8Char*)"&Cancel"));
	this->btnCancel->SetRect(264, 8, 100, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClick, this);
	NEW_CLASS(this->plIcons, UI::GUIPictureList(ui, this, core->GetDrawEngine(), true, 48, 48));
	this->plIcons->SetDockType(UI::GUIControl::DOCK_FILL);
	this->plIcons->HandleDblClk(OnOKClick, this);

	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);
	this->HandleDropFiles(OnFileDrop, this);

	this->imgIndex = imgIndex;
	this->env = env;
	Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
	NEW_CLASS(resizer, Media::Resizer::LanczosResizer8_C8(4, 3, &color, &color, 0, Media::AT_NO_ALPHA));
	this->parsers = core->GetParserList();
	this->UpdateImages();
	this->plIcons->SetSelectedIndex(imgIndex);
	this->plIcons->Redraw();
}

SSWR::AVIRead::AVIRGISImageForm::~AVIRGISImageForm()
{
	DEL_CLASS(resizer);
}

void SSWR::AVIRead::AVIRGISImageForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

UOSInt SSWR::AVIRead::AVIRGISImageForm::GetImgIndex()
{
	return this->imgIndex;
}
