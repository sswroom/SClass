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
	if (sz.x == 0 || sz.y == 0)
		return;
	NotNullPtr<Media::DrawImage> gimg;
	if (gimg.Set(me->core->GetDrawEngine()->CreateImage32(sz, Media::AT_NO_ALPHA)))
	{
		Double dpi = me->core->GetMonitorHDPI(me->GetHMonitor());
		gimg->SetHDPI(dpi);
		gimg->SetVDPI(dpi);
		me->chart->Plot(gimg, 0, 0, UOSInt2Double(sz.x), UOSInt2Double(sz.y));
		me->pbMain->SetImageDImg(gimg.Ptr());
		me->core->GetDrawEngine()->DeleteImage(gimg);
	}
}

SSWR::AVIRead::AVIRChartForm::AVIRChartForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Data::Chart *chart) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("Chart Form"));
	this->SetFont(0, 0, 8.25, false);
	this->chart = chart;
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pbMain = ui->NewPictureBoxSimple(*this, core->GetDrawEngine(), false);
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
