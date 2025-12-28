#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRGISLineEditForm.h"
#include "SSWR/AVIRead/AVIRGISLineForm.h"
#include "Text/MyString.h"
#include "UtilUI/ColorDialog.h"

#define MNU_SET_DEFAULT 101

void __stdcall SSWR::AVIRead::AVIRGISLineForm::OnThickChanged(AnyType userObj, UOSInt newPos)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	NN<SSWR::AVIRead::AVIRGISLineForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISLineForm>();
	me->lineThick = UOSInt2Double(newPos) * 0.1;
	sptr = Text::StrDouble(sbuff, me->lineThick);
	me->lblThickV->SetText(CSTRP(sbuff, sptr));
	me->UpdatePreview();
}

Bool __stdcall SSWR::AVIRead::AVIRGISLineForm::OnColorDown(AnyType userObj, Math::Coord2D<OSInt> scnPos, MouseButton btn)
{
	NN<SSWR::AVIRead::AVIRGISLineForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISLineForm>();
	if (btn == UI::GUIControl::MBTN_LEFT)
	{
		Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
		UtilUI::ColorDialog dlg(0, me->ui, me->core->GetColorManager(), me->core->GetDrawEngine(), UtilUI::ColorDialog::CCT_PHOTO, color, me->core->GetMonitorMgr());
		dlg.SetColor32(me->lineColor);
		if (dlg.ShowDialog(me) == UI::GUIForm::DR_OK)
		{
			me->lineColor = dlg.GetColor32();
			me->pbColor->SetBGColor(me->colorConv->ConvRGB8(me->lineColor));
			me->pbColor->Redraw();
			me->UpdatePreview();
		}
	}
	return false;
}

void __stdcall SSWR::AVIRead::AVIRGISLineForm::OnOKClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISLineForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISLineForm>();
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRGISLineForm::OnCancelClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISLineForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISLineForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

void SSWR::AVIRead::AVIRGISLineForm::UpdatePreview()
{
	Math::Size2D<UOSInt> sz;
	NN<Media::DrawImage> dimg;
	sz = this->pbPreview->GetSizeP();
	if (this->eng->CreateImage32(sz, Media::AT_ALPHA_ALL_FF).SetTo(dimg))
	{
		dimg->SetHDPI(this->GetHDPI());
		dimg->SetVDPI(this->GetHDPI());
		this->core->GenLinePreview(dimg, this->eng, this->lineThick, this->lineColor, this->colorConv);
		this->prevImg.Delete();
		this->prevImg = dimg->ToStaticImage();
		this->eng->DeleteImage(dimg);
		this->pbPreview->SetImage(this->prevImg);
	}
}

SSWR::AVIRead::AVIRGISLineForm::AVIRGISLineForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Media::DrawEngine> eng, Double lineThick, UInt32 lineColor) : UI::GUIForm(parent, 462, 334, ui)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	this->core = core;
	this->eng = eng;
	this->lineThick = lineThick;
	this->lineColor = lineColor;
	this->prevImg = 0;
	this->colorSess = this->core->GetColorManager()->CreateSess(this->GetHMonitor());
	this->colorSess->AddHandler(*this);
	Media::ColorProfile srcColor(Media::ColorProfile::CPT_SRGB);
	Media::ColorProfile destColor(Media::ColorProfile::CPT_PDISPLAY);
	NEW_CLASSNN(this->colorConv, Media::ColorConv(srcColor, destColor, this->colorSess.Ptr()));
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->SetText(CSTR("Line Modify"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	this->pbPreview = ui->NewPictureBox(*this, this->eng, true, false);
	this->pbPreview->SetRect(0, 0, 100, 60, false);
	this->pbPreview->SetDockType(UI::GUIControl::DOCK_TOP);
	this->pnlMain = ui->NewPanel(*this);
	this->pnlMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lblThick = ui->NewLabel(this->pnlMain, CSTR("Thick"));
	this->lblThick->SetRect(4, 4, 100, 23, false);
	this->hsbThick = ui->NewHScrollBar(this->pnlMain, 16);
	this->hsbThick->InitScrollBar(0, 600, (UOSInt)Double2OSInt(this->lineThick * 10), 10);
	this->hsbThick->SetRect(104, 4, 150, 20, false);
	this->hsbThick->HandlePosChanged(OnThickChanged, this);
	sptr = Text::StrDouble(sbuff, this->lineThick);
	this->lblThickV = ui->NewLabel(this->pnlMain, CSTRP(sbuff, sptr));
	this->lblThickV->SetRect(254, 4, 100, 23, false);
	this->lblColor = ui->NewLabel(this->pnlMain, CSTR("Color"));
	this->lblColor->SetRect(4, 28, 100, 23, false);
	this->pbColor = ui->NewPictureBox(this->pnlMain, this->eng, true, false);
	this->pbColor->SetRect(104, 28, 100, 20, false);
	this->pbColor->HandleMouseDown(OnColorDown, this);
	this->pbColor->SetBGColor(this->colorConv->ConvRGB8(this->lineColor));

	this->btnOK = ui->NewButton(this->pnlMain, CSTR("OK"));
	this->btnOK->SetRect(104, 64, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	this->btnCancel = ui->NewButton(this->pnlMain, CSTR("Cancel"));
	this->btnCancel->SetRect(184, 64, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);

	this->UpdatePreview();
}

SSWR::AVIRead::AVIRGISLineForm::~AVIRGISLineForm()
{
	this->prevImg.Delete();
	this->colorConv.Delete();
	this->colorSess->RemoveHandler(*this);
	this->ClearChildren();
	this->eng->EndColorSess(this->colorSess);
	this->core->GetColorManager()->DeleteSess(this->colorSess);
}

void SSWR::AVIRead::AVIRGISLineForm::OnMonitorChanged()
{
	this->colorSess->ChangeMonitor(this->GetHMonitor());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRGISLineForm::YUVParamChanged(NN<const Media::ColorHandler::YUVPARAM> yuvParam)
{
}

void SSWR::AVIRead::AVIRGISLineForm::RGBParamChanged(NN<const Media::ColorHandler::RGBPARAM2> rgbParam)
{
	this->colorConv->RGBParamChanged(rgbParam);
	this->pbColor->SetBGColor(this->colorConv->ConvRGB8(this->lineColor));
	this->pbColor->Redraw();
	this->UpdatePreview();
}

Double SSWR::AVIRead::AVIRGISLineForm::GetLineThick()
{
	return this->lineThick;
}

UInt32 SSWR::AVIRead::AVIRGISLineForm::GetLineColor()
{
	return this->lineColor;
}
