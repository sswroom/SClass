#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRGISImageForm.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRGISImageForm::OnOKClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISImageForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISImageForm>();
	UOSInt selIndex = me->plIcons->GetSelectedIndex();
	if (selIndex != INVALID_INDEX)
	{
		me->imgIndex = selIndex;
		me->SetDialogResult(UI::GUIForm::DR_OK);
	}
	else
	{
		me->ui->ShowMsgOK(CSTR("Select an image first"), CSTR("Select Image"), me);
	}
}

void __stdcall SSWR::AVIRead::AVIRGISImageForm::OnCancelClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISImageForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISImageForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

void __stdcall SSWR::AVIRead::AVIRGISImageForm::OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> fileNames)
{
	NN<SSWR::AVIRead::AVIRGISImageForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISImageForm>();
	Bool loaded = false;
	UOSInt i;
	UOSInt fileCnt = fileNames.GetCount();
	i = 0;
	while (i < fileCnt)
	{
		if (me->env->AddImage(fileNames[i]->ToCString(), me->parsers) >= 0)
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
	NN<Media::StaticImage> simg;
	UOSInt i = 0;
	UOSInt j = this->env->GetImageCnt();
	this->plIcons->Clear();
	while (i < j)
	{
		if (this->env->GetImage(i, 0).SetTo(simg))
			this->plIcons->Add(simg);
		i++;
	}
}

SSWR::AVIRead::AVIRGISImageForm::AVIRGISImageForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Map::MapEnv> env, UOSInt imgIndex) : UI::GUIForm(parent, 456, 405, ui)
{
	this->SetText(CSTR("Select Image"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->lbl = ui->NewLabel(*this, CSTR("Drag and drop to add icons"));
	this->lbl->SetRect(0, 0, 100, 23, false);
	this->lbl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->pnlButtons = ui->NewPanel(*this);
	this->pnlButtons->SetRect(0, 0, 456, 48, false);
	this->pnlButtons->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->btnOK = ui->NewButton(this->pnlButtons, CSTR("&OK"));
	this->btnOK->SetRect(96, 8, 100, 23, false);
	this->btnOK->HandleButtonClick(OnOKClick, this);
	this->btnCancel = ui->NewButton(this->pnlButtons, CSTR("&Cancel"));
	this->btnCancel->SetRect(264, 8, 100, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClick, this);
	NEW_CLASS(this->plIcons, UI::GUIPictureList(ui, *this, core->GetDrawEngine(), true, Math::Size2D<UOSInt>(48, 48)));
	this->plIcons->SetDockType(UI::GUIControl::DOCK_FILL);
	this->plIcons->HandleDblClk(OnOKClick, this);

	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);
	this->HandleDropFiles(OnFileDrop, this);

	this->imgIndex = imgIndex;
	this->env = env;
	Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
	NEW_CLASS(resizer, Media::Resizer::LanczosResizer8_C8(4, 3, color, color, 0, Media::AT_NO_ALPHA));
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
