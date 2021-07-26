#include "Stdafx.h"
#include "Math/Math.h"
#include "SSWR/AVIRead/AVIRSetDPIForm.h"
#include "Text/MyStringFloat.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRSetDPIForm::OnOKClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSetDPIForm *me = (SSWR::AVIRead::AVIRSetDPIForm*)userObj;
	Text::StringBuilderUTF8 sb;
	Double hdpi;
	Double ddpi;
	me->txtDesktopDPI->GetText(&sb);
	if (!Text::StrToDouble(sb.ToString(), &ddpi))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char *)"Desktop DPI is not number", (const UTF8Char *)"Error", me);
		return;
	}
	if (ddpi <= 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char *)"Desktop DPI is not valid", (const UTF8Char *)"Error", me);
		return;
	}
	hdpi = Math::UOSInt2Double(me->hsbDPI->GetPos()) * 0.1;
	if (ddpi > hdpi)
	{
		if (!UI::MessageDialog::ShowYesNoDialog((const UTF8Char *)"Are you sure to set larger desktop DPI (reducing object size)?", (const UTF8Char *)"Confirm", me))
		{
			return;
		}
	}
	me->core->SetMonitorDDPI(me->GetHMonitor(), ddpi);
	me->core->SetMonitorHDPI(me->GetHMonitor(), hdpi);
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRSetDPIForm::OnCancelClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSetDPIForm *me = (SSWR::AVIRead::AVIRSetDPIForm*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

void __stdcall SSWR::AVIRead::AVIRSetDPIForm::OnPreviewChanged(void *userObj)
{
	SSWR::AVIRead::AVIRSetDPIForm *me = (SSWR::AVIRead::AVIRSetDPIForm*)userObj;
	me->UpdatePreview();
}

void __stdcall SSWR::AVIRead::AVIRSetDPIForm::OnDPIChanged(void *userObj, UOSInt newVal)
{
	SSWR::AVIRead::AVIRSetDPIForm *me = (SSWR::AVIRead::AVIRSetDPIForm*)userObj;
	UTF8Char sbuff[32];
	Text::StrDouble(sbuff, Math::UOSInt2Double(newVal) * 0.1);
	me->lblDPIV->SetText(sbuff);
	me->UpdatePreview();
}

void __stdcall SSWR::AVIRead::AVIRSetDPIForm::OnStandardClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSetDPIForm *me = (SSWR::AVIRead::AVIRSetDPIForm*)userObj;
	me->txtDesktopDPI->SetText((const UTF8Char*)"96.0");
}

void __stdcall SSWR::AVIRead::AVIRSetDPIForm::On1xClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSetDPIForm *me = (SSWR::AVIRead::AVIRSetDPIForm*)userObj;
	UTF8Char sbuff[32];
	me->lblDPIV->GetText(sbuff);
	me->txtDesktopDPI->SetText(sbuff);
}

void __stdcall SSWR::AVIRead::AVIRSetDPIForm::OnLaptopClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSetDPIForm *me = (SSWR::AVIRead::AVIRSetDPIForm*)userObj;
	me->txtDesktopDPI->SetText((const UTF8Char*)"144.0");
}

void SSWR::AVIRead::AVIRSetDPIForm::UpdatePreview()
{
	UOSInt w;
	UOSInt h;
	UOSInt v;
	Int32 currV;
	Double initX;
	Double currX;
	Double lastX;
	UTF8Char sbuff[10];
	Double sz[2];
	Media::DrawEngine *eng;
	Media::DrawImage *gimg;
	Media::DrawBrush *b;
	Media::DrawFont *f;
	Media::DrawPen *p;
	this->pbPreview->SetImage(0);
	SDEL_CLASS(this->pimg);
	this->pbPreview->GetSizeP(&w, &h);
	eng = this->core->GetDrawEngine();
	if (w > 0 && h > 0)
	{
		Double ddpi = this->core->GetMonitorDDPI(this->GetHMonitor());
		v = this->hsbDPI->GetPos();
		gimg = eng->CreateImage32(w, h, Media::AT_NO_ALPHA);
		b = gimg->NewBrushARGB(0xffffffff);
		gimg->DrawRect(0, 0, Math::UOSInt2Double(w), Math::UOSInt2Double(h), 0, b);
		gimg->DelBrush(b);

		f = gimg->NewFontPx((const UTF8Char*)"Arial", 12 * Math::UOSInt2Double(v) * 0.1 / ddpi, Media::DrawEngine::DFS_ANTIALIAS, 0);
		p = gimg->NewPenARGB(0xff000000, 1, 0, 0);
		b = gimg->NewBrushARGB(0xff000000);
		currV = 0;
		gimg->GetTextSize(f, (const UTF8Char*)"0", sz);
		initX = sz[0] * 0.5;
		lastX = initX - 20.0;
		while (true)
		{
			currX = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_CENTIMETER, Math::Unit::Distance::DU_INCH, currV) * Math::UOSInt2Double(v) * 0.1 + initX;
			if (currX > Math::UOSInt2Double(w))
				break;

			if (currX >= lastX + 20)
			{
				Text::StrInt32(sbuff, currV);
				gimg->GetTextSize(f, sbuff, sz);
				gimg->DrawLine(currX, 0, currX, Math::UOSInt2Double(h) - sz[1], p);
				gimg->DrawString(currX - sz[0] * 0.5, Math::UOSInt2Double(h) - sz[1], sbuff, f, b);
				lastX = currX;
			}
			currV++;
		}

		this->pimg = gimg->ToStaticImage();

		gimg->DelFont(f);
		gimg->DelPen(p);
		gimg->DelBrush(b);
		eng->DeleteImage(gimg);
		this->pbPreview->SetImage(this->pimg);
	}	
}

SSWR::AVIRead::AVIRSetDPIForm::AVIRSetDPIForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 174, ui)
{
	UTF8Char sbuff[128];
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"Set Monitor DPI");

	this->pimg = 0;
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlBtn, UI::GUIPanel(ui, this));
	this->pnlBtn->SetRect(0, 0, 100, 56, false);
	this->pnlBtn->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->pnlDPI, UI::GUIPanel(ui, this));
	this->pnlDPI->SetRect(0, 0, 100, UI::GUIHScrollBar::GetSystemSize() + 1, false);
	this->pnlDPI->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->pbPreview, UI::GUIPictureBoxSimple(ui, this, core->GetDrawEngine(), false));
	this->pbPreview->SetDockType(UI::GUIControl::DOCK_FILL);
	this->pbPreview->HandleSizeChanged(OnPreviewChanged, this);
	NEW_CLASS(this->lblDPI, UI::GUILabel(ui, this->pnlDPI, (const UTF8Char*)"Monitor DPI"));
	this->lblDPI->SetRect(0, 0, 100, 23, false);
	this->lblDPI->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->lblDPIV, UI::GUILabel(ui, this->pnlDPI, (const UTF8Char*)""));
	this->lblDPIV->SetRect(0, 0, 60, 23, false);
	this->lblDPIV->SetDockType(UI::GUIControl::DOCK_RIGHT);
	NEW_CLASS(this->hsbDPI, UI::GUIHScrollBar(ui, this->pnlDPI, UI::GUIHScrollBar::GetSystemSize()));
	this->hsbDPI->SetDockType(UI::GUIControl::DOCK_FILL);
	this->hsbDPI->InitScrollBar(100, 3010, (UOSInt)Math::Double2OSInt(this->core->GetMonitorHDPI(this->GetHMonitor()) * 10), 10);
	this->hsbDPI->HandlePosChanged(OnDPIChanged, this);
	NEW_CLASS(this->lblMagnifyRatio, UI::GUILabel(ui, this->pnlBtn, (const UTF8Char*)"OS Magnify Ratio"));
	this->lblMagnifyRatio->SetRect(4, 4, 150, 23, false);
	NEW_CLASS(this->txtMagnifyRatio, UI::GUITextBox(ui, this->pnlBtn, (const UTF8Char*)""));
	this->txtMagnifyRatio->SetRect(154, 4, 100, 23, false);
	this->txtMagnifyRatio->SetReadOnly(true);
	NEW_CLASS(this->lblDesktopDPI, UI::GUILabel(ui, this->pnlBtn, (const UTF8Char*)"Desktop DPI"));
	this->lblDesktopDPI->SetRect(304, 4, 100, 23, false);
	NEW_CLASS(this->txtDesktopDPI, UI::GUITextBox(ui, this->pnlBtn, (const UTF8Char*)""));
	this->txtDesktopDPI->SetRect(404, 4, 100, 23, false);
	NEW_CLASS(this->btnStandard, UI::GUIButton(ui, this->pnlBtn, (const UTF8Char*)"Standard Size"));
	this->btnStandard->SetRect(504, 4, 75, 23, false);
	this->btnStandard->HandleButtonClick(OnStandardClicked, this);
	NEW_CLASS(this->btn1x, UI::GUIButton(ui, this->pnlBtn, (const UTF8Char*)"1x"));
	this->btn1x->SetRect(584, 4, 75, 23, false);
	this->btn1x->HandleButtonClick(On1xClicked, this);
	NEW_CLASS(this->btnLaptop, UI::GUIButton(ui, this->pnlBtn, (const UTF8Char*)"Laptop/Tablet"));
	this->btnLaptop->SetRect(664, 4, 75, 23, false);
	this->btnLaptop->HandleButtonClick(OnLaptopClicked, this);
	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this->pnlBtn, (const UTF8Char*)"OK"));
	this->btnOK->SetRect(300, 28, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this->pnlBtn, (const UTF8Char*)"Cancel"));
	this->btnCancel->SetRect(400, 28, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);

	Double v = ui->GetMagnifyRatio(this->GetHMonitor());
	Text::StrDouble(sbuff, v);
	this->txtMagnifyRatio->SetText(sbuff);
	v = this->core->GetMonitorDDPI(this->GetHMonitor());
	Text::StrDouble(sbuff, v);
	this->txtDesktopDPI->SetText(sbuff);
	OnDPIChanged(this, this->hsbDPI->GetPos());
}

SSWR::AVIRead::AVIRSetDPIForm::~AVIRSetDPIForm()
{
	SDEL_CLASS(this->pimg);
}

void SSWR::AVIRead::AVIRSetDPIForm::OnMonitorChanged()
{
	Double hdpi = this->core->GetMonitorHDPI(this->GetHMonitor());
	Double ddpi = this->core->GetMonitorDDPI(this->GetHMonitor());
	Double r = this->ui->GetMagnifyRatio(this->GetHMonitor());
	UTF8Char sbuff[32];
	Text::StrDouble(sbuff, ddpi);
	this->txtDesktopDPI->SetText(sbuff);
	Text::StrDouble(sbuff, r);
	this->txtMagnifyRatio->SetText(sbuff);
	this->hsbDPI->SetPos((UOSInt)Math::Double2OSInt(hdpi * 10));
	this->SetDPI(hdpi, ddpi);
	OnDPIChanged(this, this->hsbDPI->GetPos());
}
