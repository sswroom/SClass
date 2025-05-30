#include "Stdafx.h"
#include "IO/Path.h"
#include "Math/Math.h"
#include "SSWR/AVIRead/AVIRImageBatchForm.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "UI/GUIFolderDialog.h"

typedef enum
{
	MNU_EXPORT_SEL = 101,
	MNU_EXPORT_TIF,
	MNU_MOVE_UP,
	MNU_MOVE_DOWN,
	MNU_SEL_ALL
} MenuItems;

void __stdcall SSWR::AVIRead::AVIRImageBatchForm::OnFolderClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRImageBatchForm> me = userObj.GetNN<SSWR::AVIRead::AVIRImageBatchForm>();
	NN<Text::String> path;
	NN<UI::GUIFolderDialog> dlg = me->ui->NewFolderDialog();
	if (me->icMain->GetFolder().SetTo(path))
	{
		dlg->SetFolder(path->ToCString());
	}
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->OpenFolder(dlg->GetFolder());
	}
	dlg.Delete();
}

void __stdcall SSWR::AVIRead::AVIRImageBatchForm::OnImageChanged(AnyType userObj, Text::CString fileName, Optional<const SSWR::AVIRead::AVIRImageControl::ImageSetting> setting)
{
	NN<SSWR::AVIRead::AVIRImageBatchForm> me = userObj.GetNN<SSWR::AVIRead::AVIRImageBatchForm>();
	Text::CStringNN nnfileName;
	NN<const SSWR::AVIRead::AVIRImageControl::ImageSetting> nnsetting;
	if (!fileName.SetTo(nnfileName) || nnfileName.leng == 0 || !setting.SetTo(nnsetting))
	{
		me->dispImage.Delete();
		me->previewImage.Delete();
		me->filteredImage.Delete();
		me->pbMain->SetImage(0, false);
	}
	else
	{
		Optional<Media::StaticImage> img = me->icMain->LoadImage(nnfileName);
		me->pbMain->SetImage(0, false);
		me->dispImage.Delete();
		me->previewImage.Delete();
		me->filteredImage.Delete();
		me->dispImage = img;
		NN<Media::StaticImage> simg;
		if (img.SetTo(simg))
		{
			if (!me->resizer->IsSupported(simg->info))
			{
				printf("AVIRImageBatchForm: Pixel Format not supported\r\n");
				simg->ToB8G8R8A8();
			}
			Math::Size2D<UOSInt> sz = me->pbMain->GetSizeP();
			me->resizer->SetTargetSize(sz);
			me->resizer->SetDestProfile(simg->info.color);
			me->previewImage = me->resizer->ProcessToNew(simg);
			if (me->previewImage.SetTo(simg))
			{
				me->filteredImage = simg->CreateStaticImage();
			}
			me->initPos = true;
			me->hsbBright->SetPos((UOSInt)Double2OSInt(nnsetting->brightness * 1000));
			me->hsbContr->SetPos((UOSInt)Double2OSInt(nnsetting->contrast * 100));
			me->hsbGamma->SetPos((UOSInt)Double2OSInt(nnsetting->gamma * 100));
			me->hsbHDRLev->SetPos((nnsetting->flags & 240) >> 4);
			me->initPos = false;
			me->UpdatePreview();
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRImageBatchForm::OnColorChg(AnyType userObj, UOSInt newPos)
{
	NN<SSWR::AVIRead::AVIRImageBatchForm> me = userObj.GetNN<SSWR::AVIRead::AVIRImageBatchForm>();
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;

	Double bvalue = UOSInt2Double(me->hsbBright->GetPos()) * 0.1;
	Double cvalue = UOSInt2Double(me->hsbContr->GetPos());
	Double gvalue = UOSInt2Double(me->hsbGamma->GetPos());
	UOSInt hdrLev = me->hsbHDRLev->GetPos();
	sptr = Text::StrConcatC(Text::StrDouble(sbuff, bvalue), UTF8STRC("%"));
	me->lblBrightV->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrConcatC(Text::StrDouble(sbuff, cvalue), UTF8STRC("%"));
	me->lblContrV->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrConcatC(Text::StrDouble(sbuff, gvalue), UTF8STRC("%"));
	me->lblGammaV->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrUOSInt(sbuff, hdrLev);
	me->lblHDRLevV->SetText(CSTRP(sbuff, sptr));

	if (!me->initPos)
	{
		SSWR::AVIRead::AVIRImageControl::ImageSetting setting;
		setting.brightness = bvalue * 0.01;
		setting.contrast = cvalue * 0.01;
		setting.gamma = gvalue * 0.01;
		setting.flags = (Int32)(hdrLev << 4);
		me->icMain->UpdateImgSetting(setting);
		me->UpdatePreview();
	}
}

Bool __stdcall SSWR::AVIRead::AVIRImageBatchForm::OnFormClosing(AnyType userObj, CloseReason reason)
{
	NN<SSWR::AVIRead::AVIRImageBatchForm> me = userObj.GetNN<SSWR::AVIRead::AVIRImageBatchForm>();
	if (me->icMain->IsLoadingDir())
	{
		return !me->ui->ShowMsgYesNo(CSTR("Still loading directory, setting will be lost. Continue?"), CSTR("Question"), me);
	}
	return false;
}

void __stdcall SSWR::AVIRead::AVIRImageBatchForm::OnProgressUpdated(AnyType userObj, UOSInt finCnt)
{
	NN<SSWR::AVIRead::AVIRImageBatchForm> me = userObj.GetNN<SSWR::AVIRead::AVIRImageBatchForm>();
	me->prgMain->ProgressUpdate(finCnt, me->selCnt);
	if (finCnt >= me->selCnt)
	{
		me->prgMain->ProgressEnd();
	}
}

void __stdcall SSWR::AVIRead::AVIRImageBatchForm::OnKeyDown(AnyType userObj, UI::GUIControl::GUIKey key)
{
	NN<SSWR::AVIRead::AVIRImageBatchForm> me = userObj.GetNN<SSWR::AVIRead::AVIRImageBatchForm>();
	if (key == UI::GUIControl::GK_LEFT)
	{
		UOSInt currPos = me->hsbContr->GetPos();
		if (currPos < 10)
		{
			currPos = 0;
		}
		else
		{
			currPos -= 10;
		}
		me->hsbContr->SetPos(currPos);
	}
	else if (key == UI::GUIControl::GK_RIGHT)
	{
		UOSInt currPos = me->hsbContr->GetPos();
		currPos += 10;
		me->hsbContr->SetPos(currPos);
	}
	else if (key == UI::GUIControl::GK_HOME)
	{
		UOSInt currPos = me->hsbContr->GetPos();
		if (currPos < 100)
		{
			currPos = 0;
		}
		else
		{
			currPos -= 100;
		}
		me->hsbContr->SetPos(currPos);
	}
	else if (key == UI::GUIControl::GK_END)
	{
		UOSInt currPos = me->hsbContr->GetPos();
		currPos += 100;
		me->hsbContr->SetPos(currPos);
	}
}

void __stdcall SSWR::AVIRead::AVIRImageBatchForm::OnBrightResetClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRImageBatchForm> me = userObj.GetNN<SSWR::AVIRead::AVIRImageBatchForm>();
	me->hsbBright->SetPos(1000);
}

void __stdcall SSWR::AVIRead::AVIRImageBatchForm::OnGammaResetClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRImageBatchForm> me = userObj.GetNN<SSWR::AVIRead::AVIRImageBatchForm>();
	me->hsbGamma->SetPos(100);
}

void __stdcall SSWR::AVIRead::AVIRImageBatchForm::OnFilesDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIRImageBatchForm> me = userObj.GetNN<SSWR::AVIRead::AVIRImageBatchForm>();
	if (files.GetCount() == 1 && IO::Path::GetPathType(files[0]->ToCString()) == IO::Path::PathType::Directory)
	{
		me->OpenFolder(files[0]);
	}
}

void SSWR::AVIRead::AVIRImageBatchForm::OpenFolder(NN<Text::String> folder)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	sptr = folder->ConcatTo(sbuff);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	UOSInt fileCnt = 0;
	NN<IO::Path::FindFileSession> sess;
	if (IO::Path::FindFile(CSTRP(sbuff, sptr2)).SetTo(sess))
	{
		IO::Path::PathType pt;
		while (IO::Path::FindNextFile(sptr, sess, 0, pt, 0).NotNull())
		{
			if (pt == IO::Path::PathType::File)
			{
				fileCnt++;
			}
		}
		IO::Path::FindFileClose(sess);
	}
	this->selCnt = fileCnt;
	this->prgMain->ProgressStart(CSTR("Loading"), fileCnt);
	this->icMain->SetFolder(folder->ToCString());
	this->lblFolder->SetText(folder->ToCString());
	this->pbMain->SetImage(0, false);
	this->dispImage.Delete();
	this->previewImage.Delete();
	this->filteredImage.Delete();
}

void SSWR::AVIRead::AVIRImageBatchForm::UpdatePreview()
{
	NN<Media::StaticImage> filteredImage;
	NN<Media::StaticImage> previewImage;
	if (this->filteredImage.SetTo(filteredImage) && this->previewImage.SetTo(previewImage))
	{
		SSWR::AVIRead::AVIRImageControl::ImageSetting setting;

		setting.brightness = UOSInt2Double(this->hsbBright->GetPos()) * 0.001;
		setting.contrast = UOSInt2Double(this->hsbContr->GetPos()) * 0.01;
		setting.gamma = UOSInt2Double(this->hsbGamma->GetPos()) * 0.01;
		setting.flags = (Int32)(this->hsbHDRLev->GetPos() << 4);
		this->icMain->ApplySetting(previewImage, filteredImage, setting);
		this->pbMain->SetImage(filteredImage.Ptr(), true);
	}
}

SSWR::AVIRead::AVIRImageBatchForm::AVIRImageBatchForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Image batch tool"));
	this->SetFormState(UI::GUIForm::FS_MAXIMIZED);

	this->core = core;
	this->colorSess = this->core->GetColorMgr()->CreateSess(this->GetHMonitor());
	this->dispImage = 0;
	this->previewImage = 0;
	this->filteredImage = 0;
	this->selCnt = 0;
	this->initPos = false;
	Media::ColorProfile srcProfile(Media::ColorProfile::CPT_SRGB);
	Media::ColorProfile destProfile(Media::ColorProfile::CPT_SRGB);
	NEW_CLASSNN(this->resizer, Media::Resizer::LanczosResizerRGB_C8(4, 3, srcProfile, destProfile, this->colorSess.Ptr(), Media::AT_IGNORE_ALPHA));
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlCtrl = ui->NewPanel(*this);
	this->pnlCtrl->SetRect(0, 0, 100, 24, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnFolder = ui->NewButton(this->pnlCtrl, CSTR("Folder..."));
	this->btnFolder->SetRect(0, 0, 75, 23, false);
	this->btnFolder->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->btnFolder->HandleButtonClick(OnFolderClicked, this);
	this->lblFolder = ui->NewLabel(this->pnlCtrl, CSTR(""));
	this->lblFolder->SetRect(0, 0, 480, 23, false);
	this->lblFolder->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->prgMain = ui->NewProgressBar(this->pnlCtrl, 1);
	this->prgMain->SetDockType(UI::GUIControl::DOCK_FILL);

	NEW_CLASSNN(this->icMain, SSWR::AVIRead::AVIRImageControl(ui, *this, this->core, *this, this->colorSess));
	this->icMain->SetRect(0, 0, 200, 100, false);
	this->icMain->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->icMain->SetDispImageHandler(OnImageChanged, this);
	this->icMain->SetProgressHandler(OnProgressUpdated, this);
	this->icMain->HandleKeyDown(OnKeyDown, this);
//	NEW_CLASS(this->hspLeft = ui->NewHSplitter(*this, 3, false));
	this->pnlImage = ui->NewPanel(*this);
	this->pnlImage->SetRect(0, 0, 200, 100, false);
	this->pnlImage->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->pbMain = ui->NewPictureBoxDD(*this, this->colorSess, true, false);
	this->pbMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->lblBright = ui->NewLabel(this->pnlImage, CSTR("Brightness"));
	this->lblBright->SetRect(0, 0, 100, 23, false);
	this->hsbBright = ui->NewHScrollBar(this->pnlImage, 16);
	this->hsbBright->SetRect(100, 0, 400, 23, false);
	this->hsbBright->InitScrollBar(0, 10009, 1, 10);
	this->hsbBright->SetPos(1000);
	this->hsbBright->HandlePosChanged(OnColorChg, this);
	this->lblBrightV = ui->NewLabel(this->pnlImage, CSTR(""));
	this->lblBrightV->SetRect(500, 0,100, 23, false);
	this->btnBrightReset = ui->NewButton(this->pnlImage, CSTR("Reset"));
	this->btnBrightReset->SetRect(600, 0, 75, 23, false);
	this->btnBrightReset->HandleButtonClick(OnBrightResetClicked, this);
	this->lblContr = ui->NewLabel(this->pnlImage, CSTR("Contrast"));
	this->lblContr->SetRect(0, 24, 100, 23, false);
	this->hsbContr = ui->NewHScrollBar(this->pnlImage, 16);
	this->hsbContr->SetRect(100, 24, 400, 23, false);
	this->hsbContr->InitScrollBar(0, 1609, 1, 10);
	this->hsbContr->SetPos(100);
	this->hsbContr->HandlePosChanged(OnColorChg, this);
	this->lblContrV = ui->NewLabel(this->pnlImage, CSTR(""));
	this->lblContrV->SetRect(500, 24,100, 23, false);
	this->lblGamma = ui->NewLabel(this->pnlImage, CSTR("Gamma"));
	this->lblGamma->SetRect(0, 48, 100, 23, false);
	this->hsbGamma = ui->NewHScrollBar(this->pnlImage, 16);
	this->hsbGamma->SetRect(100, 48, 400, 23, false);
	this->hsbGamma->InitScrollBar(0, 1009, 1, 10);
	this->hsbGamma->SetPos(100);
	this->hsbGamma->HandlePosChanged(OnColorChg, this);
	this->lblGammaV = ui->NewLabel(this->pnlImage, CSTR(""));
	this->lblGammaV->SetRect(500, 48, 100, 23, false);
	this->btnGammaReset = ui->NewButton(this->pnlImage, CSTR("Reset"));
	this->btnGammaReset->SetRect(600, 48, 75, 23, false);
	this->btnGammaReset->HandleButtonClick(OnGammaResetClicked, this);
	this->lblHDRLev = ui->NewLabel(this->pnlImage, CSTR("HDR Lev"));
	this->lblHDRLev->SetRect(0, 72, 100, 23, false);
	this->hsbHDRLev = ui->NewHScrollBar(this->pnlImage, 16);
	this->hsbHDRLev->SetRect(100, 72, 400, 23, false);
	this->hsbHDRLev->InitScrollBar(0, 15, 1, 1);
	this->hsbHDRLev->SetPos(0);
	this->hsbHDRLev->HandlePosChanged(OnColorChg, this);
	this->lblHDRLevV = ui->NewLabel(this->pnlImage, CSTR(""));
	this->lblHDRLevV->SetRect(500, 72, 100, 23, false);
/*	this->btnCrop = ui->NewButton(this->pnlImage, L"Crop");
	this->btnCrop->SetRect(632, 72, 75, 23, false);*/

	NN<UI::GUIMainMenu> mmnu;
	NN<UI::GUIMenu> mnu;
	NEW_CLASSNN(mmnu, UI::GUIMainMenu());
	mnu = mmnu->AddSubMenu(CSTR("&Export"));
	mnu->AddItem(CSTR("Export Selected"), MNU_EXPORT_SEL, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_S);
	mnu->AddItem(CSTR("Export as TIF"), MNU_EXPORT_TIF, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_T);
	mnu = mmnu->AddSubMenu(CSTR("&Navigate"));
	mnu->AddItem(CSTR("Move Up"), MNU_MOVE_UP, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_UP);
	mnu->AddItem(CSTR("Move Down"), MNU_MOVE_DOWN, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_DOWN);
	mnu->AddSeperator();
	mnu->AddItem(CSTR("Select All"), MNU_SEL_ALL, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_A);
	this->SetMenu(mmnu);
	this->SetClosingHandler(OnFormClosing, this);

	this->HandleDropFiles(OnFilesDrop, this);
}

SSWR::AVIRead::AVIRImageBatchForm::~AVIRImageBatchForm()
{
	this->dispImage.Delete();
	this->previewImage.Delete();
	this->filteredImage.Delete();
	this->resizer.Delete();
	this->ClearChildren();
	this->core->GetDrawEngine()->EndColorSess(this->colorSess);
	this->core->GetColorMgr()->DeleteSess(this->colorSess);
}

void SSWR::AVIRead::AVIRImageBatchForm::EventMenuClicked(UInt16 cmdId)
{
	switch (cmdId)
	{
	case MNU_EXPORT_SEL:
		this->icMain->SetExportFormat(SSWR::AVIRead::AVIRImageControl::EF_JPG);
		this->prgMain->ProgressStart(CSTR("Test"), this->selCnt = this->icMain->ExportSelected());
		break;
	case MNU_EXPORT_TIF:
		this->icMain->SetExportFormat(SSWR::AVIRead::AVIRImageControl::EF_TIF);
		this->prgMain->ProgressStart(CSTR("Test"), this->selCnt = this->icMain->ExportSelected());
		break;
	case MNU_MOVE_UP:
		this->icMain->MoveUp();
		break;
	case MNU_MOVE_DOWN:
		this->icMain->MoveDown();
		break;
	case MNU_SEL_ALL:
		this->icMain->SelectAll();
		break;
	}
}

void SSWR::AVIRead::AVIRImageBatchForm::OnMonitorChanged()
{
	this->colorSess->ChangeMonitor(this->GetHMonitor());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
