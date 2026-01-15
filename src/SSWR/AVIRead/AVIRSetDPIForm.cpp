#include "Stdafx.h"
#include "Math/Math_C.h"
#include "Math/Unit/Distance.h"
#include "Media/DDCReader.h"
#include "Media/EDID.h"
#include "SSWR/AVIRead/AVIRSetDPIForm.h"
#include "Text/MyStringFloat.h"

void __stdcall SSWR::AVIRead::AVIRSetDPIForm::OnOKClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSetDPIForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSetDPIForm>();
	Text::StringBuilderUTF8 sb;
	Double hdpi;
	Double ddpi;
	me->txtDesktopDPI->GetText(sb);
	if (!Text::StrToDouble(sb.ToString(), ddpi))
	{
		me->ui->ShowMsgOK(CSTR("Desktop DPI is not number"), CSTR("Error"), me);
		return;
	}
	if (ddpi <= 0)
	{
		me->ui->ShowMsgOK(CSTR("Desktop DPI is not valid"), CSTR("Error"), me);
		return;
	}
	hdpi = UOSInt2Double(me->hsbDPI->GetPos()) * 0.1;
	if (ddpi > hdpi)
	{
		if (!me->ui->ShowMsgYesNo(CSTR("Are you sure to set larger desktop DPI (reducing object size)?"), CSTR("Confirm"), me))
		{
			return;
		}
	}
	me->core->SetMonitorDDPI(me->GetHMonitor(), ddpi);
	me->core->SetMonitorHDPI(me->GetHMonitor(), hdpi);
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRSetDPIForm::OnCancelClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSetDPIForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSetDPIForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

void __stdcall SSWR::AVIRead::AVIRSetDPIForm::OnPreviewChanged(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSetDPIForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSetDPIForm>();
	me->UpdatePreview();
}

void __stdcall SSWR::AVIRead::AVIRSetDPIForm::OnDPIChanged(AnyType userObj, UOSInt newVal)
{
	NN<SSWR::AVIRead::AVIRSetDPIForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSetDPIForm>();
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrDouble(sbuff, UOSInt2Double(newVal) * 0.1);
	me->lblDPIV->SetText(CSTRP(sbuff, sptr));
	me->UpdatePreview();
}

void __stdcall SSWR::AVIRead::AVIRSetDPIForm::OnStandardClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSetDPIForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSetDPIForm>();
	me->txtDesktopDPI->SetText(CSTR("96.0"));
}

void __stdcall SSWR::AVIRead::AVIRSetDPIForm::On1xClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSetDPIForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSetDPIForm>();
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	sbuff[0] = 0;
	sptr = me->lblDPIV->GetText(sbuff).Or(sbuff);
	me->txtDesktopDPI->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRSetDPIForm::OnLaptopClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSetDPIForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSetDPIForm>();
	me->txtDesktopDPI->SetText(CSTR("144.0"));
}

void __stdcall SSWR::AVIRead::AVIRSetDPIForm::OnEDIDClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSetDPIForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSetDPIForm>();
	UOSInt edidSize;
	UnsafeArray<UInt8> edidSrc;
	Optional<MonitorHandle> hMon = me->GetHMonitor();

	Media::EDID::EDIDInfo info;
	Media::DDCReader reader(hMon);
	if (reader.GetEDID(edidSize).SetTo(edidSrc) && Media::EDID::Parse(edidSrc, info))
	{
		Double hdpi = info.pixelW / Math::Unit::Distance::Convert(Math::Unit::Distance::DU_MILLIMETER, Math::Unit::Distance::DU_INCH, info.dispPhysicalW_mm);
		Double vdpi = info.pixelH / Math::Unit::Distance::Convert(Math::Unit::Distance::DU_MILLIMETER, Math::Unit::Distance::DU_INCH, info.dispPhysicalH_mm);
		Double dpi = (hdpi + vdpi) * 5;
		dpi = Math_Round(dpi);
		me->hsbDPI->SetPos((UOSInt)Double2OSInt(hdpi * 10));
		OnDPIChanged(me, me->hsbDPI->GetPos());
	}
}

void SSWR::AVIRead::AVIRSetDPIForm::UpdatePreview()
{
	Math::Size2D<UOSInt> usz;
	UOSInt v;
	Int32 currV;
	Double initX;
	Double currX;
	Double lastX;
	UTF8Char sbuff[10];
	UnsafeArray<UTF8Char> sptr;
	Math::Size2DDbl sz;
	NN<Media::DrawEngine> eng;
	NN<Media::DrawImage> gimg;
	NN<Media::DrawBrush> b;
	NN<Media::DrawFont> f;
	NN<Media::DrawPen> p;
	this->pbPreview->SetImage(nullptr);
	this->pimg.Delete();
	usz = this->pbPreview->GetSizeP();
	eng = this->core->GetDrawEngine();
	if (usz.x > 0 && usz.y > 0)
	{
		Double ddpi = this->core->GetMonitorDDPI(this->GetHMonitor());
		v = this->hsbDPI->GetPos();
		if (eng->CreateImage32(usz, Media::AT_ALPHA_ALL_FF).SetTo(gimg))
		{
			b = gimg->NewBrushARGB(0xffffffff);
			gimg->DrawRect(Math::Coord2DDbl(0, 0), usz.ToDouble(), nullptr, b);
			gimg->DelBrush(b);

			f = gimg->NewFontPx(CSTR("Arial"), 12 * UOSInt2Double(v) * 0.1 / ddpi, Media::DrawEngine::DFS_ANTIALIAS, 0);
			p = gimg->NewPenARGB(0xff000000, 1, nullptr, 0);
			b = gimg->NewBrushARGB(0xff000000);
			currV = 0;
			sz = gimg->GetTextSize(f, CSTR("0"));
			initX = sz.x * 0.5;
			lastX = initX - 20.0;
			while (true)
			{
				currX = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_CENTIMETER, Math::Unit::Distance::DU_INCH, currV) * UOSInt2Double(v) * 0.1 + initX;
				if (currX > UOSInt2Double(usz.x))
					break;

				if (currX >= lastX + 20)
				{
					sptr = Text::StrInt32(sbuff, currV);
					sz = gimg->GetTextSize(f, CSTRP(sbuff, sptr));
					gimg->DrawLine(currX, 0, currX, UOSInt2Double(usz.y) - sz.y, p);
					gimg->DrawString(Math::Coord2DDbl(currX - sz.x * 0.5, UOSInt2Double(usz.y) - sz.y), CSTRP(sbuff, sptr), f, b);
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
}

SSWR::AVIRead::AVIRSetDPIForm::AVIRSetDPIForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 174, ui)
{
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	this->SetFont(nullptr, 8.25, false);
	this->SetText(CSTR("Set Monitor DPI"));

	this->pimg = nullptr;
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlBtn = ui->NewPanel(*this);
	this->pnlBtn->SetRect(0, 0, 100, 56, false);
	this->pnlBtn->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->pnlDPI = ui->NewPanel(*this);
	this->pnlDPI->SetRect(0, 0, 100, ui->GetScrollBarSize() + 1, false);
	this->pnlDPI->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->pbPreview = ui->NewPictureBoxSimple(*this, core->GetDrawEngine(), false);
	this->pbPreview->SetDockType(UI::GUIControl::DOCK_FILL);
	this->pbPreview->HandleSizeChanged(OnPreviewChanged, this);
	this->lblDPI = ui->NewLabel(this->pnlDPI, CSTR("Monitor DPI"));
	this->lblDPI->SetRect(0, 0, 100, 23, false);
	this->lblDPI->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lblDPIV = ui->NewLabel(this->pnlDPI, CSTR(""));
	this->lblDPIV->SetRect(0, 0, 60, 23, false);
	this->lblDPIV->SetDockType(UI::GUIControl::DOCK_RIGHT);
	this->hsbDPI = ui->NewHScrollBar(this->pnlDPI, ui->GetScrollBarSize());
	this->hsbDPI->SetDockType(UI::GUIControl::DOCK_FILL);
	this->hsbDPI->InitScrollBar(100, 4010, (UOSInt)Double2OSInt(this->core->GetMonitorHDPI(this->GetHMonitor()) * 10), 10);
	this->hsbDPI->HandlePosChanged(OnDPIChanged, this);
	this->lblMagnifyRatio = ui->NewLabel(this->pnlBtn, CSTR("OS Magnify Ratio"));
	this->lblMagnifyRatio->SetRect(4, 4, 150, 23, false);
	this->txtMagnifyRatio = ui->NewTextBox(this->pnlBtn, CSTR(""));
	this->txtMagnifyRatio->SetRect(154, 4, 100, 23, false);
	this->txtMagnifyRatio->SetReadOnly(true);
	this->lblDesktopDPI = ui->NewLabel(this->pnlBtn, CSTR("Desktop DPI"));
	this->lblDesktopDPI->SetRect(304, 4, 100, 23, false);
	this->txtDesktopDPI = ui->NewTextBox(this->pnlBtn, CSTR(""));
	this->txtDesktopDPI->SetRect(404, 4, 100, 23, false);
	this->btnStandard = ui->NewButton(this->pnlBtn, CSTR("Standard Size"));
	this->btnStandard->SetRect(504, 4, 75, 23, false);
	this->btnStandard->HandleButtonClick(OnStandardClicked, this);
	this->btn1x = ui->NewButton(this->pnlBtn, CSTR("1x"));
	this->btn1x->SetRect(584, 4, 75, 23, false);
	this->btn1x->HandleButtonClick(On1xClicked, this);
	this->btnLaptop = ui->NewButton(this->pnlBtn, CSTR("Laptop/Tablet"));
	this->btnLaptop->SetRect(664, 4, 75, 23, false);
	this->btnLaptop->HandleButtonClick(OnLaptopClicked, this);
	this->btnEDID = ui->NewButton(this->pnlBtn, CSTR("From EDID"));
	this->btnEDID->SetRect(154, 28, 75, 23, false);
	this->btnEDID->HandleButtonClick(OnEDIDClicked, this);
	this->btnOK = ui->NewButton(this->pnlBtn, CSTR("OK"));
	this->btnOK->SetRect(300, 28, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	this->btnCancel = ui->NewButton(this->pnlBtn, CSTR("Cancel"));
	this->btnCancel->SetRect(400, 28, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);

	Double v = ui->GetMagnifyRatio(this->GetHMonitor());
	sptr = Text::StrDouble(sbuff, v);
	this->txtMagnifyRatio->SetText(CSTRP(sbuff, sptr));
	v = this->core->GetMonitorDDPI(this->GetHMonitor());
	sptr = Text::StrDouble(sbuff, v);
	this->txtDesktopDPI->SetText(CSTRP(sbuff, sptr));
	OnDPIChanged(this, this->hsbDPI->GetPos());
}

SSWR::AVIRead::AVIRSetDPIForm::~AVIRSetDPIForm()
{
	this->pimg.Delete();
}

void SSWR::AVIRead::AVIRSetDPIForm::OnMonitorChanged()
{
	Double hdpi = this->core->GetMonitorHDPI(this->GetHMonitor());
	Double ddpi = this->core->GetMonitorDDPI(this->GetHMonitor());
	Double r = this->ui->GetMagnifyRatio(this->GetHMonitor());
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrDouble(sbuff, ddpi);
	this->txtDesktopDPI->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrDouble(sbuff, r);
	this->txtMagnifyRatio->SetText(CSTRP(sbuff, sptr));
	this->hsbDPI->SetPos((UOSInt)Double2OSInt(hdpi * 10));
	this->SetDPI(hdpi, ddpi);
	OnDPIChanged(this, this->hsbDPI->GetPos());
}
