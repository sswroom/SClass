#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRProfiledResizerForm.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRProfiledResizerForm::OnChgClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRProfiledResizerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRProfiledResizerForm>();
	me->ChangeProfile();
}

void __stdcall SSWR::AVIRead::AVIRProfiledResizerForm::OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIRProfiledResizerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRProfiledResizerForm>();
	UOSInt i = 0;
	UOSInt nFiles = files.GetCount();
	while (i < nFiles)
	{
		me->resizer->ProcessFile(files[i]->ToCString());
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRProfiledResizerForm::OnClickedDelProfile(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRProfiledResizerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRProfiledResizerForm>();
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

void __stdcall SSWR::AVIRead::AVIRProfiledResizerForm::OnClickedAddProfile(AnyType userObj)
{
	UTF8Char sbuff[256];
	UTF8Char sbuff2[16];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	Media::ProfiledResizer::SizeType sizeType;
	UInt32 targetSizeX;
	UInt32 targetSizeY;
	UInt32 outParam;
	Text::StringBuilderUTF8 sb;
	Media::ProfiledResizer::OutputType outType;
	NN<SSWR::AVIRead::AVIRProfiledResizerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRProfiledResizerForm>();
	if (!me->txtProfileName->GetText(sbuff).SetTo(sptr) || sbuff[0] == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter profile name"), CSTR("Error"), me);
		return;
	}
	if (me->radSize->IsSelected())
	{
		sizeType = Media::ProfiledResizer::ST_MAXSIZE;
		me->txtWidth->GetText(sbuff2);
		if (!Text::StrToUInt32(sbuff2, targetSizeX))
		{
			me->ui->ShowMsgOK(CSTR("Please enter max width"), CSTR("Error"), me);
			return;
		}
		me->txtHeight->GetText(sbuff2);
		if (!Text::StrToUInt32(sbuff2, targetSizeY))
		{
			me->ui->ShowMsgOK(CSTR("Please enter max height"), CSTR("Error"), me);
			return;
		}
		if (targetSizeX == 0 && targetSizeY == 0)
		{

		}
		else if (targetSizeX == 0 || targetSizeY == 0)
		{
			me->ui->ShowMsgOK(CSTR("Max width and max height cannot be zero"), CSTR("Error"), me);
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
			me->ui->ShowMsgOK(CSTR("Please enter H-DPI"), CSTR("Error"), me);
			return;
		}
		me->txtVDPI->GetText(sbuff2);
		targetSizeY = Text::StrToUInt32(sbuff2);
		if (targetSizeY <= 0)
		{
			me->ui->ShowMsgOK(CSTR("Please enter V-DPI"), CSTR("Error"), me);
			return;
		}
	}
	else
	{
		me->ui->ShowMsgOK(CSTR("Please select Size Type"), CSTR("Error"), me);
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
			me->ui->ShowMsgOK(CSTR("Please enter quality"), CSTR("Error"), me);
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
			me->ui->ShowMsgOK(CSTR("Please enter size ratio"), CSTR("Error"), me);
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
			me->ui->ShowMsgOK(CSTR("Please enter quality"), CSTR("Error"), me);
			return;
		}
	}
	else
	{
		me->ui->ShowMsgOK(CSTR("Please select output type"), CSTR("Error"), me);
		return;
	}
	if (!me->txtSuffix->GetText(sbuff2).SetTo(sptr2) || sbuff2[0] == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter suffix"), CSTR("Error"), me);
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
		me->ui->ShowMsgOK(CSTR("Error in adding profile"), CSTR("Error"), me);
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
	UnsafeArray<UTF8Char> sptr;
	NN<const Media::ProfiledResizer::ResizeProfile> profile;
	if (!this->resizer->GetCurrProfile().SetTo(profile))
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
	NN<const Media::ProfiledResizer::ResizeProfile> profile;
	UOSInt i = 0;
	UOSInt j = this->resizer->GetProfileCount();
	this->lbProfile->ClearItems();
	while (i < j)
	{
		profile = this->resizer->GetProfileNoCheck(i);
		this->lbProfile->AddItem(profile->profileName->ToCString(), NN<Media::ProfiledResizer::ResizeProfile>::ConvertFrom(profile));
		i++;
	}

}

SSWR::AVIRead::AVIRProfiledResizerForm::AVIRProfiledResizerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 320, 320, ui)
{
	NN<UI::GUILabel> lbl;
	this->SetText(CSTR("Profiled Resizer"));
	this->SetNoResize(true);
	this->SetFont(nullptr, 8.25, false);

	this->core = core;
	this->colorSess = this->core->GetColorManager()->CreateSess(this->GetHMonitor());
	NEW_CLASSNN(resizer, Media::ProfiledResizer(this->core->GetParserList(), this->colorSess.Ptr(), this->core->GetDrawEngine()));
//	resizer->AddProfile(L"Facebook", L"fb", 960, 960, Media::ProfiledResizer::OT_JPEGQUALITY, 100);
//	resizer->AddProfile(L"HKWildlife", L"m", 800, 800, Media::ProfiledResizer::OT_JPEGSIZE, 27);
	resizer->LoadProfile(CSTR(""));
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpMain = this->tcMain->AddTabPage(CSTR("Main"));
	this->tpProfile = this->tcMain->AddTabPage(CSTR("Edit Profiles"));

	this->btnChgProfile = ui->NewButton(this->tpMain, CSTR("&Change Profile"));
	this->btnChgProfile->SetRect(60, 200, 180, 22, false);
	this->btnChgProfile->HandleButtonClick(OnChgClicked, this);

	this->lblProfile = ui->NewLabel(this->tpMain, CSTR("Profile: "));
	this->lblProfile->SetRect(60, 150, 180, 22, false);

	lbl = ui->NewLabel(this->tpMain, CSTR("sswroom presents"));
	lbl->SetRect(105, 0, 150, 22, false);
	
	lbl = ui->NewLabel(this->tpMain, CSTR("Add resize profile first"));
	lbl->SetRect(95, 30, 200, 19, false);
	lbl = ui->NewLabel(this->tpMain, CSTR("And then Drag and Drop file to resize"));
	lbl->SetRect(60, 50, 200, 19, false);

	
	this->pnlProfile1 = ui->NewPanel(this->tpProfile);
	this->pnlProfile1->SetRect(0, 0, 100, 100, false);
	this->pnlProfile1->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->btnProfileDel = ui->NewButton(this->pnlProfile1, CSTR("&Delete"));
	this->btnProfileDel->SetRect(0, 0, 100, 22, false);
	this->btnProfileDel->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->btnProfileDel->HandleButtonClick(OnClickedDelProfile, this);
	this->lbProfile = ui->NewListBox(this->pnlProfile1, false);
	this->lbProfile->SetDockType(UI::GUIControl::DOCK_FILL);

	this->pnlProfile2 = ui->NewPanel(this->tpProfile);
	this->pnlProfile2->SetDockType(UI::GUIControl::DOCK_FILL);
	lbl = ui->NewLabel(this->pnlProfile2, CSTR("Name"));
	lbl->SetRect(0, 0, 60, 19, false);
	this->txtProfileName = ui->NewTextBox(this->pnlProfile2, CSTR(""));
	this->txtProfileName->SetRect(60, 0, 140, 19, false);
	lbl = ui->NewLabel(this->pnlProfile2, CSTR("Suffix"));
	lbl->SetRect(0, 19, 60, 19, false);
	this->txtSuffix = ui->NewTextBox(this->pnlProfile2, CSTR(""));
	this->txtSuffix->SetRect(60, 19, 50, 19, false);
	this->grpSize = ui->NewGroupBox(this->pnlProfile2, CSTR("Size"));
	this->grpSize->SetRect(0, 38, 200, 83, false);
	this->radSize = ui->NewRadioButton(this->grpSize, CSTR("Size"), true);
	this->radSize->SetRect(0, 0, 60, 19, false);
	lbl = ui->NewLabel(this->grpSize, CSTR("Max W"));
	lbl->SetRect(0, 19, 40, 19, false);
	this->txtWidth = ui->NewTextBox(this->grpSize, CSTR("800"));
	this->txtWidth->SetRect(40, 19, 50, 19, false);
	lbl = ui->NewLabel(this->grpSize, CSTR("Max H"));
	lbl->SetRect(0, 38, 40, 19, false);
	this->txtHeight = ui->NewTextBox(this->grpSize, CSTR("800"));
	this->txtHeight->SetRect(40, 38, 50, 19, false);
	this->radDPI = ui->NewRadioButton(this->grpSize, CSTR("DPI"), false);
	this->radDPI->SetRect(100, 0, 60, 19, false);
	lbl = ui->NewLabel(this->grpSize, CSTR("H-DPI"));
	lbl->SetRect(100, 19, 40, 19, false);
	this->txtHDPI = ui->NewTextBox(this->grpSize, CSTR("300"));
	this->txtHDPI->SetRect(140, 19, 50, 19, false);
	lbl = ui->NewLabel(this->grpSize, CSTR("V-DPI"));
	lbl->SetRect(100, 38, 40, 19, false);
	this->txtVDPI = ui->NewTextBox(this->grpSize, CSTR("300"));
	this->txtVDPI->SetRect(140, 38, 50, 19, false);
	lbl = ui->NewLabel(this->pnlProfile2, CSTR("Watermark"));
	lbl->SetRect(0, 121, 60, 19, false);
	this->txtWatermark = ui->NewTextBox(this->pnlProfile2, CSTR(""));
	this->txtWatermark->SetRect(60, 121, 140, 19, false);
	this->radTIFF = ui->NewRadioButton(this->pnlProfile2, CSTR("TIFF"), false);
	this->radTIFF->SetRect(0, 140, 120, 19, false);
	this->radJPEGQ = ui->NewRadioButton(this->pnlProfile2, CSTR("JPEG (% Quality)"), false);
	this->radJPEGQ->SetRect(0, 159, 120, 19, false);
	this->txtJPEGQuality = ui->NewTextBox(this->pnlProfile2, CSTR("100"));
	this->txtJPEGQuality->SetRect(120, 159, 60, 19, false);
	this->radJPEGSize = ui->NewRadioButton(this->pnlProfile2, CSTR("JPEG (1 / Size)"), false);
	this->radJPEGSize->SetRect(0, 178, 120, 19, false);
	this->txtJPEGSize = ui->NewTextBox(this->pnlProfile2, CSTR("20"));
	this->txtJPEGSize->SetRect(120, 178, 60, 19, false);
	this->radJPEGSize->Select();
	this->radPNG = ui->NewRadioButton(this->pnlProfile2, CSTR("PNG"), false);
	this->radPNG->SetRect(0, 197, 120, 19, false);
	this->radWEBPQ = ui->NewRadioButton(this->pnlProfile2, CSTR("WEBP (% Quality)"), false);
	this->radWEBPQ->SetRect(0, 216, 120, 19, false);
	this->txtWEBPQuality = ui->NewTextBox(this->pnlProfile2, CSTR("100"));
	this->txtWEBPQuality->SetRect(120, 216, 60, 19, false);

	this->btnProfileAdd = ui->NewButton(this->pnlProfile2, CSTR("&Add"));
	this->btnProfileAdd->SetRect(15, 235, 100, 22, false);
	this->btnProfileAdd->HandleButtonClick(OnClickedAddProfile, this);

	this->HandleDropFiles(OnFileDrop, this);

	UpdateProfileList();
	ChangeProfile();
}

SSWR::AVIRead::AVIRProfiledResizerForm::~AVIRProfiledResizerForm()
{
	this->resizer.Delete();
	this->ClearChildren();
	this->core->GetDrawEngine()->EndColorSess(this->colorSess);
	this->core->GetColorManager()->DeleteSess(this->colorSess);
}

void SSWR::AVIRead::AVIRProfiledResizerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
