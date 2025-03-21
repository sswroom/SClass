#include "Stdafx.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Media/ImageList.h"
#include "Media/StaticImage.h"
#include "SSWR/AVIRead/AVIRImagePSNRForm.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/GUIFileDialog.h"

void __stdcall SSWR::AVIRead::AVIRImagePSNRForm::OnOriFileClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRImagePSNRForm> me = userObj.GetNN<SSWR::AVIRead::AVIRImagePSNRForm>();
	Text::StringBuilderUTF8 sb;
	me->txtOriFile->GetText(sb);
	NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"ImagePSNRO", false);
	me->core->GetParserList()->PrepareSelector(dlg, IO::ParserType::ImageList);
	if (sb.GetLength() > 0)
	{
		dlg->SetFileName(sb.ToCString());
	}
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->txtOriFile->SetText(dlg->GetFileName()->ToCString());
	}
	dlg.Delete();
}

void __stdcall SSWR::AVIRead::AVIRImagePSNRForm::OnLossyFileClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRImagePSNRForm> me = userObj.GetNN<SSWR::AVIRead::AVIRImagePSNRForm>();
	Text::StringBuilderUTF8 sb;
	me->txtLossyFile->GetText(sb);
	NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"ImagePSNRL", false);
	me->core->GetParserList()->PrepareSelector(dlg, IO::ParserType::ImageList);
	if (sb.GetLength() > 0)
	{
		dlg->SetFileName(sb.ToCString());
	}
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->txtLossyFile->SetText(dlg->GetFileName()->ToCString());
	}
	dlg.Delete();
}

void __stdcall SSWR::AVIRead::AVIRImagePSNRForm::OnCompareClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRImagePSNRForm> me = userObj.GetNN<SSWR::AVIRead::AVIRImagePSNRForm>();
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	me->txtOriFile->GetText(sb);
	me->txtLossyFile->GetText(sb2);
	if (IO::Path::GetPathType(sb.ToCString()) != IO::Path::PathType::File)
	{
		return;
	}
	if (IO::Path::GetPathType(sb2.ToCString()) != IO::Path::PathType::File)
	{
		return;
	}
	Optional<Media::ImageList> imgList1 = 0;
	Optional<Media::ImageList> imgList2 = 0;
	NN<Media::ImageList> nnimgList1;
	NN<Media::ImageList> nnimgList2;
	NN<Media::StaticImage> simg1;
	NN<Media::StaticImage> simg2;
	{
		IO::StmData::FileData fd(sb.ToCString(), false);
		imgList1 = Optional<Media::ImageList>::ConvertFrom(me->core->GetParserList()->ParseFileType(fd, IO::ParserType::ImageList));
	}
	{
		IO::StmData::FileData fd(sb2.ToCString(), false);
		imgList2 = Optional<Media::ImageList>::ConvertFrom(me->core->GetParserList()->ParseFileType(fd, IO::ParserType::ImageList));
	}
	if (imgList1.SetTo(nnimgList1) && imgList2.SetTo(nnimgList2))
	{
		nnimgList1->ToStaticImage(0);
		nnimgList2->ToStaticImage(0);
		if (Optional<Media::StaticImage>::ConvertFrom(nnimgList1->GetImage(0, 0)).SetTo(simg1) && 
			Optional<Media::StaticImage>::ConvertFrom(nnimgList2->GetImage(0, 0)).SetTo(simg2) &&
			simg1->info.storeSize.x == simg2->info.storeSize.x &&
			simg1->info.dispSize.y == simg2->info.dispSize.y)
		{
			Double psnr;
			if (me->radMode16Bit->IsSelected())
			{
				simg1->ToB16G16R16A16();
				simg2->ToB16G16R16A16();
				psnr = simg1->CalcPSNR(simg2);
			}
			else
			{
				simg1->ToB8G8R8A8();
				simg2->ToB8G8R8A8();
				psnr = simg1->CalcPSNR(simg2);
			}
			sb.ClearStr();
			sb.AppendDouble(psnr);
			me->txtPSNR->SetText(sb.ToCString());
		}
	}
	imgList1.Delete();
	imgList2.Delete();
}

SSWR::AVIRead::AVIRImagePSNRForm::AVIRImagePSNRForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 640, 160, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Image PSNR"));
	this->SetNoResize(true);
	
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblOriFile = ui->NewLabel(*this, CSTR("Ori File"));
	this->lblOriFile->SetRect(0, 0, 100, 23, false);
	this->txtOriFile = ui->NewTextBox(*this, CSTR(""));
	this->txtOriFile->SetRect(100, 0, 450, 23, false);
	this->btnOriFile = ui->NewButton(*this, CSTR("&Browse"));
	this->btnOriFile->SetRect(550, 0, 75, 23, false);
	this->btnOriFile->HandleButtonClick(OnOriFileClicked, this);
	this->lblLossyFile = ui->NewLabel(*this, CSTR("Lossy File"));
	this->lblLossyFile->SetRect(0, 24, 100, 23, false);
	this->txtLossyFile = ui->NewTextBox(*this, CSTR(""));
	this->txtLossyFile->SetRect(100, 24, 450, 23, false);
	this->btnLossyFile = ui->NewButton(*this, CSTR("B&rowse"));
	this->btnLossyFile->SetRect(550, 24, 75, 23, false);
	this->btnLossyFile->HandleButtonClick(OnLossyFileClicked, this);
	this->lblMode = ui->NewLabel(*this, CSTR("Mode"));
	this->lblMode->SetRect(0, 48, 100, 23, false);
	this->radMode16Bit = ui->NewRadioButton(*this, CSTR("16Bit"), true);
	this->radMode16Bit->SetRect(100, 48, 100, 23, false);
	this->radMode8Bit = ui->NewRadioButton(*this, CSTR("8Bit"), false);
	this->radMode8Bit->SetRect(200, 48, 100, 23, false);
	this->btnCompare = ui->NewButton(*this, CSTR("&Compare"));
	this->btnCompare->SetRect(100, 72, 75, 23, false);
	this->btnCompare->HandleButtonClick(OnCompareClicked, this);
	this->lblPSNR = ui->NewLabel(*this, CSTR("PSNR"));
	this->lblPSNR->SetRect(0, 96, 100, 23, false);
	this->txtPSNR = ui->NewTextBox(*this, CSTR(""));
	this->txtPSNR->SetRect(100, 96, 100, 23, false);
	this->txtPSNR->SetReadOnly(true);
	this->lblPSNRDB = ui->NewLabel(*this, CSTR("dB"));
	this->lblPSNRDB->SetRect(200, 96, 100, 23, false);
}

SSWR::AVIRead::AVIRImagePSNRForm::~AVIRImagePSNRForm()
{
}

void SSWR::AVIRead::AVIRImagePSNRForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
