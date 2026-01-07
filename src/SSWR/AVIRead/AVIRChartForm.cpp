#include "Stdafx.h"
#include "Math/Math_C.h"
#include "SSWR/AVIRead/AVIRChartForm.h"

typedef enum
{
	MNU_LOG_SAVE = 101
} MenuItems;

void __stdcall SSWR::AVIRead::AVIRChartForm::SizeChanged(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRChartForm> me = userObj.GetNN<SSWR::AVIRead::AVIRChartForm>();
	Math::Size2D<UOSInt> sz = me->pbMain->GetSizeP();
	if (sz.x == 0 || sz.y == 0)
		return;
	NN<Media::DrawImage> gimg;
	if (me->core->GetDrawEngine()->CreateImage32(sz, Media::AT_ALPHA_ALL_FF).SetTo(gimg))
	{
		Double dpi = me->core->GetMonitorHDPI(me->GetHMonitor());
		gimg->SetHDPI(dpi);
		gimg->SetVDPI(dpi);
		me->chart->Plot(gimg, 0, 0, UOSInt2Double(sz.x), UOSInt2Double(sz.y));
		me->pbMain->SetImageDImg(gimg.Ptr());
		me->core->GetDrawEngine()->DeleteImage(gimg);
	}
}

SSWR::AVIRead::AVIRChartForm::AVIRChartForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Data::ChartPlotter> chart) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("Chart Form"));
	this->SetFont(nullptr, 8.25, false);
	this->chart = chart;
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pbMain = ui->NewPictureBoxSimple(*this, core->GetDrawEngine(), false);
	this->pbMain->SetNoBGColor(true);
	this->pbMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->pbMain->HandleSizeChanged(SizeChanged, this);
	SizeChanged(this);
}

SSWR::AVIRead::AVIRChartForm::~AVIRChartForm()
{
	this->chart.Delete();
}

void SSWR::AVIRead::AVIRChartForm::EventMenuClicked(UInt16 cmdId)
{
	switch (cmdId)
	{
	case MNU_LOG_SAVE:
		break;
	}
}

void SSWR::AVIRead::AVIRChartForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
