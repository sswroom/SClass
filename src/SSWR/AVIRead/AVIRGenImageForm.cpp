#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRGenImageForm.h"
#include "Text/MyString.h"
#include "UI/GUILabel.h"

void __stdcall SSWR::AVIRead::AVIRGenImageForm::GenerateClicked(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRGenImageForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGenImageForm>();
	UTF8Char sbuff[256];
	UInt32 width;
	UInt32 height;
	me->txtWidth->GetText(sbuff);
	width = Text::StrToUInt32(sbuff);
	me->txtHeight->GetText(sbuff);
	height = Text::StrToUInt32(sbuff);
	if (width <= 0 || height <= 0)
	{
		me->ui->ShowMsgOK(CSTR("Error in parsing output size"), CSTR("Error"), me);
		return;
	}

	UOSInt i = me->cboGenerator->GetSelectedIndex();
	if (i != INVALID_INDEX)
	{
		Media::ImageGenerator *imgGen = (Media::ImageGenerator*)me->cboGenerator->GetItem(i);
		Media::ColorProfile colorProfile((Media::ColorProfile::CommonProfileType)(OSInt)me->cboColorProfile->GetSelectedItem());

		Media::RasterImage *img = imgGen->GenerateImage(colorProfile, Math::Size2D<UOSInt>(width, height));
		if (img)
		{
			NotNullPtr<Media::ImageList> imgList;
			NEW_CLASSNN(imgList, Media::ImageList(imgGen->GetName()));
			imgList->AddImage(img, 0);
			me->core->OpenObject(imgList);
			me->SetDialogResult(UI::GUIForm::DR_OK);
		}
		else
		{
			me->ui->ShowMsgOK(CSTR("This parameters are not supported"), CSTR("Error"), me);
		}
	}
	else
	{
		me->ui->ShowMsgOK(CSTR("Please select a generator"), CSTR("Error"), me);
	}
}

void __stdcall SSWR::AVIRead::AVIRGenImageForm::CancelClicked(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRGenImageForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGenImageForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIRGenImageForm::AVIRGenImageForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 350, 296, ui)
{
	this->SetText(CSTR("Generate Image"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	
	NotNullPtr<UI::GUILabel> lbl;
	
	lbl = ui->NewLabel(*this, CSTR("Generator"));
	lbl->SetRect(8, 8, 100, 23, false);
	this->cboGenerator = ui->NewComboBox(*this, false);
	this->cboGenerator->SetRect(112, 8, 224, 23, false);
	lbl = ui->NewLabel(*this, CSTR("Color Profile"));
	lbl->SetRect(8, 32, 100, 23, false);
	this->cboColorProfile = ui->NewComboBox(*this, false);
	this->cboColorProfile->SetRect(112, 32, 224, 23, false);

	lbl = ui->NewLabel(*this, CSTR("Output Size"));
	lbl->SetRect(8, 200, 100, 23, false);
	this->txtWidth = ui->NewTextBox(*this, CSTR("640"));
	this->txtWidth->SetRect(112, 200, 56, 23, false);
	lbl = ui->NewLabel(*this, CSTR("x"));
	lbl->SetRect(168, 200, 24, 23, false);
	this->txtHeight = ui->NewTextBox(*this, CSTR("480"));
	this->txtHeight->SetRect(192, 200, 56, 23, false);

	btnGenerate = ui->NewButton(*this, CSTR("Generate"));
	btnGenerate->SetRect(88, 232, 75, 23, false);
	btnGenerate->HandleButtonClick(GenerateClicked, this);
	btnCancel = ui->NewButton(*this, CSTR("Cancel"));
	btnCancel->SetRect(184, 232, 75, 23, false);
	btnCancel->HandleButtonClick(CancelClicked, this);
	this->SetDefaultButton(btnGenerate);
	this->SetCancelButton(btnCancel);

	UOSInt i;
	UOSInt j;
	NEW_CLASS(this->imgGenMgr, Media::ImageGen::ImageGenMgr());
	i = 0;
	j = this->imgGenMgr->GetCount();
	while (i < j)
	{
		Media::ImageGenerator *imgGen = this->imgGenMgr->GetGenerator(i);
		this->cboGenerator->AddItem(imgGen->GetName(), imgGen);

		i++;
	}
	if (j > 0)
	{
		this->cboGenerator->SetSelectedIndex(0);
	}
	i = 0;
	j = Media::ColorProfile::CPT_LAST;
	while (i <= j)
	{
		this->cboColorProfile->AddItem(Media::ColorProfile::CommonProfileTypeGetName((Media::ColorProfile::CommonProfileType)i), (void*)i);
		i++;
	}
	this->cboColorProfile->SetSelectedIndex(0);
}

SSWR::AVIRead::AVIRGenImageForm::~AVIRGenImageForm()
{
	DEL_CLASS(this->imgGenMgr);
}

void SSWR::AVIRead::AVIRGenImageForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
