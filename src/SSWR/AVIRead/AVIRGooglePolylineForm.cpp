#include "Stdafx.h"
#include "Map/GoogleMap/GoogleMapsUtil.h"
#include "SSWR/AVIRead/AVIRGooglePolylineForm.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRGooglePolylineForm::OnOKClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGooglePolylineForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGooglePolylineForm>();
	Text::StringBuilderUTF8 sb;
	me->txtPolylineText->GetText(sb);
	me->polyline = Map::GoogleMap::GoogleMapsUtil::ParsePolylineText(sb.ToString());
	if (me->polyline)
	{
		me->SetDialogResult(UI::GUIForm::DR_OK);
	}
}

void __stdcall SSWR::AVIRead::AVIRGooglePolylineForm::OnCancelClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGooglePolylineForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGooglePolylineForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIRGooglePolylineForm::AVIRGooglePolylineForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 120, ui)
{
	this->SetText(CSTR("Google Polyline"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->polyline = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblPolylineText = ui->NewLabel(*this, CSTR("Polyline Text"));
	this->lblPolylineText->SetRect(4, 4, 100, 23, false);
	this->txtPolylineText = ui->NewTextBox(*this, CSTR(""));
	this->txtPolylineText->SetRect(104, 4, 800, 23, false);
	this->btnCancel = ui->NewButton(*this, CSTR("Cancel"));
	this->btnCancel->SetRect(104, 28, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	this->btnOK = ui->NewButton(*this, CSTR("OK"));
	this->btnOK->SetRect(184, 28, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
}

SSWR::AVIRead::AVIRGooglePolylineForm::~AVIRGooglePolylineForm()
{
}

void SSWR::AVIRead::AVIRGooglePolylineForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

Math::Geometry::LineString *SSWR::AVIRead::AVIRGooglePolylineForm::GetPolyline()
{
	return this->polyline;
}
