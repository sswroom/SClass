#include "Stdafx.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Media/ImageList.h"
#include "Media/StaticImage.h"
#include "SSWR/AVIRead/AVIRImagePSNRForm.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/FileDialog.h"

void __stdcall SSWR::AVIRead::AVIRImagePSNRForm::OnOriFileClicked(void *userObj)
{
	SSWR::AVIRead::AVIRImagePSNRForm *me = (SSWR::AVIRead::AVIRImagePSNRForm*)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtOriFile->GetText(&sb);
	UI::FileDialog *dlg;
	NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"ImagePSNRO", false));
	me->core->GetParserList()->PrepareSelector(dlg, IO::ParserType::ImageList);
	if (sb.GetLength() > 0)
	{
		dlg->SetFileName(sb.ToString());
	}
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->txtOriFile->SetText(dlg->GetFileName());
	}
	DEL_CLASS(dlg);
}

void __stdcall SSWR::AVIRead::AVIRImagePSNRForm::OnLossyFileClicked(void *userObj)
{
	SSWR::AVIRead::AVIRImagePSNRForm *me = (SSWR::AVIRead::AVIRImagePSNRForm*)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtLossyFile->GetText(&sb);
	UI::FileDialog *dlg;
	NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"ImagePSNRL", false));
	me->core->GetParserList()->PrepareSelector(dlg, IO::ParserType::ImageList);
	if (sb.GetLength() > 0)
	{
		dlg->SetFileName(sb.ToString());
	}
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->txtLossyFile->SetText(dlg->GetFileName());
	}
	DEL_CLASS(dlg);
}

void __stdcall SSWR::AVIRead::AVIRImagePSNRForm::OnCompareClicked(void *userObj)
{
	SSWR::AVIRead::AVIRImagePSNRForm *me = (SSWR::AVIRead::AVIRImagePSNRForm*)userObj;
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	me->txtOriFile->GetText(&sb);
	me->txtLossyFile->GetText(&sb2);
	if (IO::Path::GetPathType(sb.ToString(), sb.GetLength()) != IO::Path::PathType::File)
	{
		return;
	}
	if (IO::Path::GetPathType(sb2.ToString(), sb2.GetLength()) != IO::Path::PathType::File)
	{
		return;
	}
	Media::ImageList *imgList1 = 0;
	Media::ImageList *imgList2 = 0;
	Media::StaticImage *simg1;
	Media::StaticImage *simg2;
	IO::StmData::FileData *fd;
	NEW_CLASS(fd, IO::StmData::FileData(sb.ToString(), false));
	imgList1 = (Media::ImageList*)me->core->GetParserList()->ParseFileType(fd, IO::ParserType::ImageList);
	DEL_CLASS(fd);
	NEW_CLASS(fd, IO::StmData::FileData(sb2.ToString(), false));
	imgList2 = (Media::ImageList*)me->core->GetParserList()->ParseFileType(fd, IO::ParserType::ImageList);
	DEL_CLASS(fd);
	if (imgList1 && imgList2)
	{
		imgList1->ToStaticImage(0);
		imgList2->ToStaticImage(0);
		simg1 = (Media::StaticImage*)imgList1->GetImage(0, 0);
		simg2 = (Media::StaticImage*)imgList2->GetImage(0, 0);
		if (simg1->info->storeWidth == simg2->info->storeWidth && simg1->info->dispHeight == simg2->info->dispHeight)
		{
			Double psnr;
			if (me->radMode16Bit->IsSelected())
			{
				simg1->To64bpp();
				simg2->To64bpp();
				psnr = simg1->CalcPSNR(simg2);
			}
			else
			{
				simg1->To32bpp();
				simg2->To32bpp();
				psnr = simg1->CalcPSNR(simg2);
			}
			sb.ClearStr();
			Text::SBAppendF64(&sb, psnr);
			me->txtPSNR->SetText(sb.ToString());
		}
	}
	SDEL_CLASS(imgList1);
	SDEL_CLASS(imgList2);
}

SSWR::AVIRead::AVIRImagePSNRForm::AVIRImagePSNRForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 640, 160, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText((const UTF8Char*)"Image PSNR");
	this->SetNoResize(true);
	
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblOriFile, UI::GUILabel(ui, this, (const UTF8Char*)"Ori File"));
	this->lblOriFile->SetRect(0, 0, 100, 23, false);
	NEW_CLASS(this->txtOriFile, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtOriFile->SetRect(100, 0, 450, 23, false);
	NEW_CLASS(this->btnOriFile, UI::GUIButton(ui, this, (const UTF8Char*)"&Browse"));
	this->btnOriFile->SetRect(550, 0, 75, 23, false);
	this->btnOriFile->HandleButtonClick(OnOriFileClicked, this);
	NEW_CLASS(this->lblLossyFile, UI::GUILabel(ui, this, (const UTF8Char*)"Lossy File"));
	this->lblLossyFile->SetRect(0, 24, 100, 23, false);
	NEW_CLASS(this->txtLossyFile, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtLossyFile->SetRect(100, 24, 450, 23, false);
	NEW_CLASS(this->btnLossyFile, UI::GUIButton(ui, this, (const UTF8Char*)"B&rowse"));
	this->btnLossyFile->SetRect(550, 24, 75, 23, false);
	this->btnLossyFile->HandleButtonClick(OnLossyFileClicked, this);
	NEW_CLASS(this->lblMode, UI::GUILabel(ui, this, (const UTF8Char*)"Mode"));
	this->lblMode->SetRect(0, 48, 100, 23, false);
	NEW_CLASS(this->radMode16Bit, UI::GUIRadioButton(ui, this, (const UTF8Char*)"16Bit", true));
	this->radMode16Bit->SetRect(100, 48, 100, 23, false);
	NEW_CLASS(this->radMode8Bit, UI::GUIRadioButton(ui, this, (const UTF8Char*)"8Bit", false));
	this->radMode8Bit->SetRect(200, 48, 100, 23, false);
	NEW_CLASS(this->btnCompare, UI::GUIButton(ui, this, (const UTF8Char*)"&Compare"));
	this->btnCompare->SetRect(100, 72, 75, 23, false);
	this->btnCompare->HandleButtonClick(OnCompareClicked, this);
	NEW_CLASS(this->lblPSNR, UI::GUILabel(ui, this, (const UTF8Char*)"PSNR"));
	this->lblPSNR->SetRect(0, 96, 100, 23, false);
	NEW_CLASS(this->txtPSNR, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtPSNR->SetRect(100, 96, 100, 23, false);
	this->txtPSNR->SetReadOnly(true);
	NEW_CLASS(this->lblPSNRDB, UI::GUILabel(ui, this, (const UTF8Char*)"dB"));
	this->lblPSNRDB->SetRect(200, 96, 100, 23, false);
}

SSWR::AVIRead::AVIRImagePSNRForm::~AVIRImagePSNRForm()
{
}

void SSWR::AVIRead::AVIRImagePSNRForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
