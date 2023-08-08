#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRProfiledResizerForm.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRProfiledResizerForm::OnChgClicked(void *userObj)
{
	SSWR::AVIRead::AVIRProfiledResizerForm *me = (SSWR::AVIRead::AVIRProfiledResizerForm*)userObj;
	me->ChangeProfile();
}

void __stdcall SSWR::AVIRead::AVIRProfiledResizerForm::OnFileDrop(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles)
{
	SSWR::AVIRead::AVIRProfiledResizerForm *me = (SSWR::AVIRead::AVIRProfiledResizerForm*)userObj;
	UOSInt i = 0;
	while (i < nFiles)
	{
		me->resizer->ProcessFile(files[i]->ToCString());
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRProfiledResizerForm::OnClickedDelProfile(void *userObj)
{
	SSWR::AVIRead::AVIRProfiledResizerForm *me = (SSWR::AVIRead::AVIRProfiledResizerForm*)userObj;
	UOSInt i = me->lbProfile->GetSelectedIndex();
	if (i != INVALID_INDEX)
	{
		me->lbProfile->RemoveItem(i);
		if (me->resizer->GetCurrProfileIndex() == i)
		{
			me->resizer->RemoveProfile(i);
			me->UpdateProfileDisp();
		}
		else
		{
			me->resizer->RemoveProfile(i);
		}
		me->resizer->SaveProfile(CSTR(""));
	}
}

void __stdcall SSWR::AVIRead::AVIRProfiledResizerForm::OnClickedAddProfile(void *userObj)
{
	UTF8Char sbuff[256];
	UTF8Char sbuff2[16];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	Media::ProfiledResizer::SizeType sizeType;
	UInt32 targetSizeX;
	UInt32 targetSizeY;
	UInt32 outParam;
	Text::StringBuilderUTF8 sb;
	Media::ProfiledResizer::OutputType outType;
	SSWR::AVIRead::AVIRProfiledResizerForm *me = (SSWR::AVIRead::AVIRProfiledResizerForm*)userObj;
	sptr = me->txtProfileName->GetText(sbuff);
	if (sbuff[0] == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Please enter profile name"), CSTR("Error"), me);
		return;
	}
	if (me->radSize->IsSelected())
	{
		sizeType = Media::ProfiledResizer::ST_MAXSIZE;
		me->txtWidth->GetText(sbuff2);
		if (!Text::StrToUInt32(sbuff2, &targetSizeX))
		{
			UI::MessageDialog::ShowDialog(CSTR("Please enter max width"), CSTR("Error"), me);
			return;
		}
		me->txtHeight->GetText(sbuff2);
		if (!Text::StrToUInt32(sbuff2, &targetSizeY))
		{
			UI::MessageDialog::ShowDialog(CSTR("Please enter max height"), CSTR("Error"), me);
			return;
		}
		if (targetSizeX == 0 && targetSizeY == 0)
		{

		}
		else if (targetSizeX == 0 || targetSizeY == 0)
		{
			UI::MessageDialog::ShowDialog(CSTR("Max width and max height cannot be zero"), CSTR("Error"), me);
			return;
		}
	}
	else if (me->radDPI->IsSelected())
	{
		sizeType = Media::ProfiledResizer::ST_DPI;
		me->txtHDPI->GetText(sbuff2);
		targetSizeX = Text::StrToUInt32(sbuff2);
		if (targetSizeX <= 0)
		{
			UI::MessageDialog::ShowDialog(CSTR("Please enter H-DPI"), CSTR("Error"), me);
			return;
		}
		me->txtVDPI->GetText(sbuff2);
		targetSizeY = Text::StrToUInt32(sbuff2);
		if (targetSizeY <= 0)
		{
			UI::MessageDialog::ShowDialog(CSTR("Please enter V-DPI"), CSTR("Error"), me);
			return;
		}
	}
	else
	{
		UI::MessageDialog::ShowDialog(CSTR("Please select Size Type"), CSTR("Error"), me);
		return;
	}
	
	if (me->radTIFF->IsSelected())
	{
		outType = Media::ProfiledResizer::OT_TIFF;
		outParam = 0;
	}
	else if (me->radJPEGQ->IsSelected())
	{
		outType = Media::ProfiledResizer::OT_JPEGQUALITY;
		me->txtJPEGQuality->GetText(sbuff2);
		outParam = Text::StrToUInt32(sbuff2);
		if (outParam <= 0 || outParam > 100)
		{
			UI::MessageDialog::ShowDialog(CSTR("Please enter quality"), CSTR("Error"), me);
			return;
		}
	}
	else if (me->radJPEGSize->IsSelected())
	{
		outType = Media::ProfiledResizer::OT_JPEGSIZE;
		me->txtJPEGSize->GetText(sbuff2);
		outParam = Text::StrToUInt32(sbuff2);
		if (outParam <= 0 || outParam > 300)
		{
			UI::MessageDialog::ShowDialog(CSTR("Please enter size ratio"), CSTR("Error"), me);
			return;
		}
	}
	else if (me->radPNG->IsSelected())
	{
		outType = Media::ProfiledResizer::OT_PNG;
		outParam = 0;
	}
	else if (me->radWEBPQ->IsSelected())
	{
		outType = Media::ProfiledResizer::OT_WEBPQUALITY;
		me->txtWEBPQuality->GetText(sbuff2);
		outParam = Text::StrToUInt32(sbuff2);
		if (outParam <= 0 || outParam > 100)
		{
			UI::MessageDialog::ShowDialog(CSTR("Please enter quality"), CSTR("Error"), me);
			return;
		}
	}
	else
	{
		UI::MessageDialog::ShowDialog(CSTR("Please select output type"), CSTR("Error"), me);
		return;
	}
	sptr2 = me->txtSuffix->GetText(sbuff2);
	if (sbuff2[0] == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Please enter suffix"), CSTR("Error"), me);
		return;
	}
	me->txtWatermark->GetText(sb);
	if (me->resizer->AddProfile(CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2), targetSizeX, targetSizeY, outType, outParam, sb.ToCString(), sizeType))
	{
		me->lbProfile->AddItem(CSTRP(sbuff, sptr), 0);
		me->txtProfileName->SetText(CSTR(""));
		me->resizer->SaveProfile(CSTR(""));
	}
	else
	{
		UI::MessageDialog::ShowDialog(CSTR("Error in adding profile"), CSTR("Error"), me);
		return;
	}
}

void SSWR::AVIRead::AVIRProfiledResizerForm::ChangeProfile()
{
	if (this->resizer->GetProfileCount() > 0)
	{
		this->resizer->SetCurrentProfile((this->resizer->GetCurrProfileIndex() + 1) % this->resizer->GetProfileCount());
	}
	this->UpdateProfileDisp();
}

void SSWR::AVIRead::AVIRProfiledResizerForm::UpdateProfileDisp()
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	const Media::ProfiledResizer::ResizeProfile *profile = this->resizer->GetCurrProfile();
	if (profile == 0)
	{
		this->lblProfile->SetText(CSTR("Profile: --"));
	}
	else
	{
		sptr = profile->profileName->ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("Profile: ")));
		this->lblProfile->SetText(CSTRP(sbuff, sptr));
	}
}

void SSWR::AVIRead::AVIRProfiledResizerForm::UpdateProfileList()
{
	const Media::ProfiledResizer::ResizeProfile *profile;
	UOSInt i = 0;
	UOSInt j = this->resizer->GetProfileCount();
	this->lbProfile->ClearItems();
	while (i < j)
	{
		profile = this->resizer->GetProfile(i);
		this->lbProfile->AddItem(profile->profileName->ToCString(), (void*)profile);
		i++;
	}

}

SSWR::AVIRead::AVIRProfiledResizerForm::AVIRProfiledResizerForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 320, 320, ui)
{
	UI::GUILabel *lbl;
	this->SetText(CSTR("Profiled Resizer"));
	this->SetNoResize(true);
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->colorSess = this->core->GetColorMgr()->CreateSess(this->GetHMonitor());
	NEW_CLASS(resizer, Media::ProfiledResizer(this->core->GetParserList(), this->colorSess, this->core->GetDrawEngine()));
//	resizer->AddProfile(L"Facebook", L"fb", 960, 960, Media::ProfiledResizer::OT_JPEGQUALITY, 100);
//	resizer->AddProfile(L"HKWildlife", L"m", 800, 800, Media::ProfiledResizer::OT_JPEGSIZE, 27);
	resizer->LoadProfile(CSTR(""));
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpMain = this->tcMain->AddTabPage(CSTR("Main"));
	this->tpProfile = this->tcMain->AddTabPage(CSTR("Edit Profiles"));

	NEW_CLASS(this->btnChgProfile, UI::GUIButton(ui, this->tpMain, CSTR("&Change Profile")));
	this->btnChgProfile->SetRect(60, 200, 180, 22, false);
	this->btnChgProfile->HandleButtonClick(OnChgClicked, this);

	NEW_CLASS(this->lblProfile, UI::GUILabel(ui, this->tpMain, CSTR("Profile: ")));
	this->lblProfile->SetRect(60, 150, 180, 22, false);

	NEW_CLASS(lbl, UI::GUILabel(ui, this->tpMain, CSTR("sswroom presents")));
	lbl->SetRect(105, 0, 150, 22, false);
	
	NEW_CLASS(lbl, UI::GUILabel(ui, this->tpMain, CSTR("Add resize profile first")));
	lbl->SetRect(95, 30, 200, 19, false);
	NEW_CLASS(lbl, UI::GUILabel(ui, this->tpMain, CSTR("And then Drag and Drop file to resize")));
	lbl->SetRect(60, 50, 200, 19, false);

	
	NEW_CLASS(this->pnlProfile1, UI::GUIPanel(ui, this->tpProfile));
	this->pnlProfile1->SetRect(0, 0, 100, 100, false);
	this->pnlProfile1->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->btnProfileDel, UI::GUIButton(ui, this->pnlProfile1, CSTR("&Delete")));
	this->btnProfileDel->SetRect(0, 0, 100, 22, false);
	this->btnProfileDel->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->btnProfileDel->HandleButtonClick(OnClickedDelProfile, this);
	NEW_CLASS(this->lbProfile, UI::GUIListBox(ui, this->pnlProfile1, false));
	this->lbProfile->SetDockType(UI::GUIControl::DOCK_FILL);

	NEW_CLASS(this->pnlProfile2, UI::GUIPanel(ui, this->tpProfile));
	this->pnlProfile2->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(lbl, UI::GUILabel(ui, this->pnlProfile2, CSTR("Name")));
	lbl->SetRect(0, 0, 60, 19, false);
	NEW_CLASS(this->txtProfileName, UI::GUITextBox(ui, this->pnlProfile2, CSTR("")));
	this->txtProfileName->SetRect(60, 0, 140, 19, false);
	NEW_CLASS(lbl, UI::GUILabel(ui, this->pnlProfile2, CSTR("Suffix")));
	lbl->SetRect(0, 19, 60, 19, false);
	NEW_CLASS(this->txtSuffix, UI::GUITextBox(ui, this->pnlProfile2, CSTR("")));
	this->txtSuffix->SetRect(60, 19, 50, 19, false);
	NEW_CLASS(this->grpSize, UI::GUIGroupBox(ui, this->pnlProfile2, CSTR("Size")));
	this->grpSize->SetRect(0, 38, 200, 83, false);
	NEW_CLASS(this->radSize, UI::GUIRadioButton(ui, this->grpSize, CSTR("Size"), true));
	this->radSize->SetRect(0, 0, 60, 19, false);
	NEW_CLASS(lbl, UI::GUILabel(ui, this->grpSize, CSTR("Max W")));
	lbl->SetRect(0, 19, 40, 19, false);
	NEW_CLASS(this->txtWidth, UI::GUITextBox(ui, this->grpSize, CSTR("800")));
	this->txtWidth->SetRect(40, 19, 50, 19, false);
	NEW_CLASS(lbl, UI::GUILabel(ui, this->grpSize, CSTR("Max H")));
	lbl->SetRect(0, 38, 40, 19, false);
	NEW_CLASS(this->txtHeight, UI::GUITextBox(ui, this->grpSize, CSTR("800")));
	this->txtHeight->SetRect(40, 38, 50, 19, false);
	NEW_CLASS(this->radDPI, UI::GUIRadioButton(ui, this->grpSize, CSTR("DPI"), false));
	this->radDPI->SetRect(100, 0, 60, 19, false);
	NEW_CLASS(lbl, UI::GUILabel(ui, this->grpSize, CSTR("H-DPI")));
	lbl->SetRect(100, 19, 40, 19, false);
	NEW_CLASS(this->txtHDPI, UI::GUITextBox(ui, this->grpSize, CSTR("300")));
	this->txtHDPI->SetRect(140, 19, 50, 19, false);
	NEW_CLASS(lbl, UI::GUILabel(ui, this->grpSize, CSTR("V-DPI")));
	lbl->SetRect(100, 38, 40, 19, false);
	NEW_CLASS(this->txtVDPI, UI::GUITextBox(ui, this->grpSize, CSTR("300")));
	this->txtVDPI->SetRect(140, 38, 50, 19, false);
	NEW_CLASS(lbl, UI::GUILabel(ui, this->pnlProfile2, CSTR("Watermark")));
	lbl->SetRect(0, 121, 60, 19, false);
	NEW_CLASS(this->txtWatermark, UI::GUITextBox(ui, this->pnlProfile2, CSTR("")));
	this->txtWatermark->SetRect(60, 121, 140, 19, false);
	NEW_CLASS(this->radTIFF, UI::GUIRadioButton(ui, this->pnlProfile2, CSTR("TIFF"), false));
	this->radTIFF->SetRect(0, 140, 120, 19, false);
	NEW_CLASS(this->radJPEGQ, UI::GUIRadioButton(ui, this->pnlProfile2, CSTR("JPEG (% Quality)"), false))
	this->radJPEGQ->SetRect(0, 159, 120, 19, false);
	NEW_CLASS(this->txtJPEGQuality, UI::GUITextBox(ui, this->pnlProfile2, CSTR("100")));
	this->txtJPEGQuality->SetRect(120, 159, 60, 19, false);
	NEW_CLASS(this->radJPEGSize, UI::GUIRadioButton(ui, this->pnlProfile2, CSTR("JPEG (1 / Size)"), false))
	this->radJPEGSize->SetRect(0, 178, 120, 19, false);
	NEW_CLASS(this->txtJPEGSize, UI::GUITextBox(ui, this->pnlProfile2, CSTR("20")));
	this->txtJPEGSize->SetRect(120, 178, 60, 19, false);
	this->radJPEGSize->Select();
	NEW_CLASS(this->radPNG, UI::GUIRadioButton(ui, this->pnlProfile2, CSTR("PNG"), false));
	this->radPNG->SetRect(0, 197, 120, 19, false);
	NEW_CLASS(this->radWEBPQ, UI::GUIRadioButton(ui, this->pnlProfile2, CSTR("WEBP (% Quality)"), false))
	this->radWEBPQ->SetRect(0, 216, 120, 19, false);
	NEW_CLASS(this->txtWEBPQuality, UI::GUITextBox(ui, this->pnlProfile2, CSTR("100")));
	this->txtWEBPQuality->SetRect(120, 216, 60, 19, false);

	NEW_CLASS(this->btnProfileAdd, UI::GUIButton(ui, this->pnlProfile2, CSTR("&Add")));
	this->btnProfileAdd->SetRect(15, 235, 100, 22, false);
	this->btnProfileAdd->HandleButtonClick(OnClickedAddProfile, this);

	this->HandleDropFiles(OnFileDrop, this);

	UpdateProfileList();
	ChangeProfile();
}

SSWR::AVIRead::AVIRProfiledResizerForm::~AVIRProfiledResizerForm()
{
	DEL_CLASS(resizer);
	this->ClearChildren();
	this->core->GetColorMgr()->DeleteSess(this->colorSess);
}

void SSWR::AVIRead::AVIRProfiledResizerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
