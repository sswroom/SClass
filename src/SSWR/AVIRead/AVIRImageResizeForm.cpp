#include "Stdafx.h"
#include "IO/Registry.h"
#include "Math/Math.h"
#include "Media/Resizer/LanczosResizerRGB_C8.h"
#include "Media/Resizer/LanczosResizer16_C8.h"
#include "SSWR/AVIRead/AVIRImageResizeForm.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRImageResizeForm::OnOKClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRImageResizeForm> me = userObj.GetNN<SSWR::AVIRead::AVIRImageResizeForm>();
	Text::StringBuilderUTF8 sb;
	Math::Size2D<UOSInt> outSize = Math::Size2D<UOSInt>(0, 0);
	UOSInt nTap = 0;
	sb.ClearStr();
	me->txtOutW->GetText(sb);
	sb.ToUOSInt(outSize.x);
	sb.ClearStr();
	me->txtOutH->GetText(sb);
	sb.ToUOSInt(outSize.y);
	sb.ClearStr();
	me->txtNTap->GetText(sb);
	sb.ToUOSInt(nTap);
	if (outSize.x == 0 || outSize.y == 0 || nTap < 3 || nTap > 32)
	{
		me->ui->ShowMsgOK(CSTR("Invalid input"), CSTR("Error"), me);
		return;
	}
	Media::Resizer::LanczosResizerRGB_C8 rgbResizer(nTap, nTap, me->srcImg->info.color, me->srcImg->info.color, 0, Media::AT_NO_ALPHA);
	if (rgbResizer.IsSupported(me->srcImg->info))
	{
		rgbResizer.SetSrcPixelFormat(me->srcImg->info.pf, me->srcImg->pal);
		rgbResizer.SetResizeAspectRatio(Media::ImageResizer::RAR_IGNOREAR);
		rgbResizer.SetTargetSize(outSize);
		me->outImg = rgbResizer.ProcessToNew(me->srcImg);
	}
	else if (me->srcImg->info.pf == Media::PF_LE_B16G16R16A16)
	{
		Media::Resizer::LanczosResizer16_C8 resizer(nTap, nTap, me->srcImg->info.color, me->srcImg->info.color, 0, Media::AT_NO_ALPHA);
		resizer.SetResizeAspectRatio(Media::ImageResizer::RAR_IGNOREAR);
		resizer.SetTargetSize(outSize);
		me->outImg = resizer.ProcessToNew(me->srcImg);
	}
	else
	{
		me->ui->ShowMsgOK(CSTR("Unsupported image"), CSTR("Error"), me);
		return;
	}
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRImageResizeForm::OnCancelClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRImageResizeForm> me = userObj.GetNN<SSWR::AVIRead::AVIRImageResizeForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIRImageResizeForm::AVIRImageResizeForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Media::RasterImage> srcImg) : UI::GUIForm(parent, 480, 240, ui)
{
	this->SetText(CSTR("Image Resize"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->srcImg = srcImg->CreateStaticImage();
	this->outImg = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	
	this->lblOriW = ui->NewLabel(*this, CSTR("Ori Width"));
	this->lblOriW->SetRect(4, 4, 100, 23, false);
	this->txtOriW = ui->NewTextBox(*this, CSTR(""));
	this->txtOriW->SetRect(104, 4, 100, 23, false);
	this->txtOriW->SetReadOnly(true);
	this->lblOriH = ui->NewLabel(*this, CSTR("Ori Height"));
	this->lblOriH->SetRect(4, 28, 100, 23, false);
	this->txtOriH = ui->NewTextBox(*this, CSTR(""));
	this->txtOriH->SetRect(104, 28, 100, 23, false);
	this->txtOriH->SetReadOnly(true);
	this->lblOutW = ui->NewLabel(*this, CSTR("Out Width"));
	this->lblOutW->SetRect(4, 64, 100, 23, false);
	this->txtOutW = ui->NewTextBox(*this, CSTR(""));
	this->txtOutW->SetRect(104, 64, 100, 23, false);
	this->lblOutH = ui->NewLabel(*this, CSTR("Out Height"));
	this->lblOutH->SetRect(4, 88, 100, 23, false);
	this->txtOutH = ui->NewTextBox(*this, CSTR(""));
	this->txtOutH->SetRect(104, 88, 100, 23, false);
	this->lblNTap = ui->NewLabel(*this, CSTR("N-Tap"));
	this->lblNTap->SetRect(4, 112, 100, 23, false);
	this->txtNTap = ui->NewTextBox(*this, CSTR("16"));
	this->txtNTap->SetRect(104, 112, 100, 23, false);

	this->btnCancel = ui->NewButton(*this, CSTR("Cancel"));
	this->btnCancel->SetRect(104, 144, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	this->btnOK = ui->NewButton(*this, CSTR("OK"));
	this->btnOK->SetRect(184, 144, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);

	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);
	this->txtOutW->Focus();

	sptr = Text::StrUOSInt(sbuff, this->srcImg->info.dispSize.x);
	this->txtOriW->SetText(CSTRP(sbuff, sptr));
	this->txtOutW->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrUOSInt(sbuff, this->srcImg->info.dispSize.y);
	this->txtOriH->SetText(CSTRP(sbuff, sptr));
	this->txtOutH->SetText(CSTRP(sbuff, sptr));
}

SSWR::AVIRead::AVIRImageResizeForm::~AVIRImageResizeForm()
{
	this->srcImg.Delete();
}

void SSWR::AVIRead::AVIRImageResizeForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

Optional<Media::StaticImage> SSWR::AVIRead::AVIRImageResizeForm::GetNewImage()
{
	return this->outImg;
}
