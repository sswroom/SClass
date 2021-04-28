#include "Stdafx.h"
#include "IO/Registry.h"
#include "Math/Math.h"
#include "Media/Resizer/LanczosResizer8_C8.h"
#include "Media/Resizer/LanczosResizer16_C8.h"
#include "SSWR/AVIRead/AVIRImageResizeForm.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRImageResizeForm::OnOKClicked(void *userObj)
{
	SSWR::AVIRead::AVIRImageResizeForm *me = (SSWR::AVIRead::AVIRImageResizeForm*)userObj;
	Text::StringBuilderUTF8 sb;
	UOSInt outW = 0;
	UOSInt outH = 0;
	UOSInt nTap = 0;
	sb.ClearStr();
	me->txtOutW->GetText(&sb);
	sb.ToUOSInt(&outW);
	sb.ClearStr();
	me->txtOutH->GetText(&sb);
	sb.ToUOSInt(&outH);
	sb.ClearStr();
	me->txtNTap->GetText(&sb);
	sb.ToUOSInt(&nTap);
	if (outW == 0 || outH == 0 || nTap < 3 || nTap > 32)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Invalid input", (const UTF8Char*)"Error", me);
		return;
	}
	if (me->srcImg->info->pf == Media::PF_B8G8R8A8)
	{
		Media::Resizer::LanczosResizer8_C8 resizer(nTap, nTap, me->srcImg->info->color, me->srcImg->info->color, 0, Media::AT_NO_ALPHA);
		resizer.SetResizeAspectRatio(Media::IImgResizer::RAR_IGNOREAR);
		resizer.SetTargetWidth(outW);
		resizer.SetTargetHeight(outH);
		me->outImg = resizer.ProcessToNew(me->srcImg);
	}
	else if (me->srcImg->info->pf == Media::PF_LE_B16G16R16A16)
	{
		Media::Resizer::LanczosResizer16_C8 resizer(nTap, nTap, me->srcImg->info->color, me->srcImg->info->color, 0, Media::AT_NO_ALPHA);
		resizer.SetResizeAspectRatio(Media::IImgResizer::RAR_IGNOREAR);
		resizer.SetTargetWidth(outW);
		resizer.SetTargetHeight(outH);
		me->outImg = resizer.ProcessToNew(me->srcImg);
	}
	else
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Unsupported image", (const UTF8Char*)"Error", me);
		return;
	}
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRImageResizeForm::OnCancelClicked(void *userObj)
{
	SSWR::AVIRead::AVIRImageResizeForm *me = (SSWR::AVIRead::AVIRImageResizeForm*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIRImageResizeForm::AVIRImageResizeForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Media::Image *srcImg) : UI::GUIForm(parent, 480, 240, ui)
{
	this->SetText((const UTF8Char*)"Image Resize");
	this->SetFont(0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->srcImg = srcImg->CreateStaticImage();
	this->outImg = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	
	NEW_CLASS(this->lblOriW, UI::GUILabel(ui, this, (const UTF8Char*)"Ori Width"));
	this->lblOriW->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtOriW, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtOriW->SetRect(104, 4, 100, 23, false);
	this->txtOriW->SetReadOnly(true);
	NEW_CLASS(this->lblOriH, UI::GUILabel(ui, this, (const UTF8Char*)"Ori Height"));
	this->lblOriH->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtOriH, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtOriH->SetRect(104, 28, 100, 23, false);
	this->txtOriH->SetReadOnly(true);
	NEW_CLASS(this->lblOutW, UI::GUILabel(ui, this, (const UTF8Char*)"Out Width"));
	this->lblOutW->SetRect(4, 64, 100, 23, false);
	NEW_CLASS(this->txtOutW, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtOutW->SetRect(104, 64, 100, 23, false);
	NEW_CLASS(this->lblOutH, UI::GUILabel(ui, this, (const UTF8Char*)"Out Height"));
	this->lblOutH->SetRect(4, 88, 100, 23, false);
	NEW_CLASS(this->txtOutH, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtOutH->SetRect(104, 88, 100, 23, false);
	NEW_CLASS(this->lblNTap, UI::GUILabel(ui, this, (const UTF8Char*)"N-Tap"));
	this->lblNTap->SetRect(4, 112, 100, 23, false);
	NEW_CLASS(this->txtNTap, UI::GUITextBox(ui, this, (const UTF8Char*)"16"));
	this->txtNTap->SetRect(104, 112, 100, 23, false);

	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this, (const UTF8Char*)"Cancel"));
	this->btnCancel->SetRect(104, 144, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this, (const UTF8Char*)"OK"));
	this->btnOK->SetRect(184, 144, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);

	UTF8Char sbuff[32];
	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);
	this->txtOutW->Focus();
	if (this->srcImg)
	{
		Text::StrUOSInt(sbuff, this->srcImg->info->dispWidth);
		this->txtOriW->SetText(sbuff);
		this->txtOutW->SetText(sbuff);
		Text::StrUOSInt(sbuff, this->srcImg->info->dispHeight);
		this->txtOriH->SetText(sbuff);
		this->txtOutH->SetText(sbuff);
	}
}

SSWR::AVIRead::AVIRImageResizeForm::~AVIRImageResizeForm()
{
	SDEL_CLASS(this->srcImg);
}

void SSWR::AVIRead::AVIRImageResizeForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

Media::StaticImage *SSWR::AVIRead::AVIRImageResizeForm::GetNewImage()
{
	return this->outImg;
}
