#include "Stdafx.h"
#include "Math/Math.h"
#include "SSWR/AVIRead/AVIRChartForm.h"

typedef enum
{
	MNU_LOG_SAVE = 101
} MenuItems;

void __stdcall SSWR::AVIRead::AVIRChartForm::OnSizeChanged(void *userObj)
{
	SSWR::AVIRead::AVIRChartForm *me = (SSWR::AVIRead::AVIRChartForm*)userObj;
	Math::Size2D<UOSInt> sz = me->pbMain->GetSizeP();
	if (sz.width == 0 || sz.height == 0)
		return;
	Media::DrawImage *gimg = me->core->GetDrawEngine()->CreateImage32(sz.width, sz.height, Media::AT_NO_ALPHA);
	Double dpi = me->core->GetMonitorHDPI(me->GetHMonitor());
	gimg->SetHDPI(dpi);
	gimg->SetVDPI(dpi);
	me->chart->Plot(gimg, 0, 0, UOSInt2Double(sz.width), UOSInt2Double(sz.height));
	me->pbMain->SetImageDImg(gimg);
	me->core->GetDrawEngine()->DeleteImage(gimg);
}

SSWR::AVIRead::AVIRChartForm::AVIRChartForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Data::IChart *chart) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("Chart Form"));
	this->SetFont(0, 0, 8.25, false);
	this->chart = chart;
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pbMain, UI::GUIPictureBoxSimple(ui, this, core->GetDrawEngine(), false));
	this->pbMain->SetNoBGColor(true);
	this->pbMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->pbMain->HandleSizeChanged(OnSizeChanged, this);
	OnSizeChanged(this);
}

SSWR::AVIRead::AVIRChartForm::~AVIRChartForm()
{
	DEL_CLASS(this->chart);
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
