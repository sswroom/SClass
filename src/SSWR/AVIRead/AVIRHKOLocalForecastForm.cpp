#include "Stdafx.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRHKOLocalForecastForm.h"

void __stdcall SSWR::AVIRead::AVIRHKOLocalForecastForm::OnReloadClicked(void *userObj)
{
	SSWR::AVIRead::AVIRHKOLocalForecastForm *me = (SSWR::AVIRead::AVIRHKOLocalForecastForm*)userObj;
	Net::HKOWeather::Language lang = (Net::HKOWeather::Language)(OSInt)me->cboLang->GetSelectedItem();
	me->Reload(lang);
}

void SSWR::AVIRead::AVIRHKOLocalForecastForm::Reload(Net::HKOWeather::Language lang)
{
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	Net::HKOWeather::LocalForecast forecast;
	Data::Timestamp reqTime = Data::Timestamp::Now();
	sptr = reqTime.ToStringNoZone(sbuff);
	this->txtReqTime->SetText(CSTRP(sbuff, sptr));
	if (Net::HKOWeather::GetLocalForecast(this->core->GetSocketFactory(), this->ssl, lang, &forecast))
	{
		sptr = forecast.updateTime.ToStringNoZone(sbuff);
		this->txtUpdateTime->SetText(CSTRP(sbuff, sptr));
		this->txtGeneralSituation->SetText(forecast.generalSituation->ToCString());
		this->txtTCInfo->SetText(forecast.tcInfo->ToCString());
		this->txtFireDangerWarning->SetText(forecast.fireDangerWarning->ToCString());
		this->txtForecastPeriod->SetText(forecast.forecastPeriod->ToCString());
		this->txtForecastDesc->SetText(forecast.forecastDesc->ToCString());
		this->txtOutlook->SetText(forecast.outlook->ToCString());
		Net::HKOWeather::FreeLocalForecast(&forecast);
	}
	else
	{
		this->txtUpdateTime->SetText(CSTR(""));
		this->txtGeneralSituation->SetText(CSTR(""));
		this->txtTCInfo->SetText(CSTR(""));
		this->txtFireDangerWarning->SetText(CSTR(""));
		this->txtForecastPeriod->SetText(CSTR(""));
		this->txtForecastDesc->SetText(CSTR(""));
		this->txtOutlook->SetText(CSTR(""));
	}
}

SSWR::AVIRead::AVIRHKOLocalForecastForm::AVIRHKOLocalForecastForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 800, 600, ui)
{
	this->SetText(CSTR("HK Observatory Local Weather Forecast"));
	this->SetFont(0, 0, 8.25, false);
	
	this->core = core;
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetSocketFactory(), true);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblLang, UI::GUILabel(ui, *this, CSTR("Language")));
	this->lblLang->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->cboLang, UI::GUIComboBox(ui, *this, false));
	this->cboLang->SetRect(104, 4, 150, 23, false);
	this->cboLang->AddItem(CSTR("English"), (void*)Net::HKOWeather::Language::En);
	this->cboLang->AddItem(CSTR("Chinese (Traditional)"), (void*)Net::HKOWeather::Language::TC);
	this->cboLang->AddItem(CSTR("Chinese (Simplified)"), (void*)Net::HKOWeather::Language::SC);
	this->cboLang->SetSelectedIndex(0);
	NEW_CLASS(this->btnReload, UI::GUIButton(ui, *this, CSTR("Reload")));
	this->btnReload->SetRect(254, 4, 75, 23, false);
	this->btnReload->HandleButtonClick(OnReloadClicked, this);

	NEW_CLASS(this->lblReqTime, UI::GUILabel(ui, *this, CSTR("Request Time")));
	this->lblReqTime->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtReqTime, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtReqTime->SetRect(104, 52, 180, 23, false);
	this->txtReqTime->SetReadOnly(true);
	NEW_CLASS(this->lblUpdateTime, UI::GUILabel(ui, *this, CSTR("Update Time")));
	this->lblUpdateTime->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtUpdateTime, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtUpdateTime->SetRect(104, 76, 180, 23, false);
	this->txtUpdateTime->SetReadOnly(true);
	NEW_CLASS(this->lblGeneralSituation, UI::GUILabel(ui, *this, CSTR("General Situation")));
	this->lblGeneralSituation->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtGeneralSituation, UI::GUITextBox(ui, *this, CSTR(""), true));
	this->txtGeneralSituation->SetRect(104, 100, 650, 47, false);
	this->txtGeneralSituation->SetReadOnly(true);
	this->txtGeneralSituation->SetWordWrap(true);
	NEW_CLASS(this->lblTCInfo, UI::GUILabel(ui, *this, CSTR("TC Info")));
	this->lblTCInfo->SetRect(4, 148, 100, 23, false);
	NEW_CLASS(this->txtTCInfo, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtTCInfo->SetRect(104, 148, 650, 23, false);
	this->txtTCInfo->SetReadOnly(true);
	NEW_CLASS(this->lblFireDangerWarning, UI::GUILabel(ui, *this, CSTR("Fire Danger Warning")));
	this->lblFireDangerWarning->SetRect(4, 172, 100, 23, false);
	NEW_CLASS(this->txtFireDangerWarning, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtFireDangerWarning->SetRect(104, 172, 650, 23, false);
	this->txtFireDangerWarning->SetReadOnly(true);
	NEW_CLASS(this->lblForecastPeriod, UI::GUILabel(ui, *this, CSTR("Forecast Period")));
	this->lblForecastPeriod->SetRect(4, 196, 100, 23, false);
	NEW_CLASS(this->txtForecastPeriod, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtForecastPeriod->SetRect(104, 196, 650, 23, false);
	this->txtForecastPeriod->SetReadOnly(true);
	NEW_CLASS(this->lblForecastDesc, UI::GUILabel(ui, *this, CSTR("Forecast Desc")));
	this->lblForecastDesc->SetRect(4, 220, 100, 23, false);
	NEW_CLASS(this->txtForecastDesc, UI::GUITextBox(ui, *this, CSTR(""), true));
	this->txtForecastDesc->SetRect(104, 220, 650, 71, false);
	this->txtForecastDesc->SetReadOnly(true);
	this->txtForecastDesc->SetWordWrap(true);
	NEW_CLASS(this->lblOutlook, UI::GUILabel(ui, *this, CSTR("Outlook")));
	this->lblOutlook->SetRect(4, 292, 100, 23, false);
	NEW_CLASS(this->txtOutlook, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtOutlook->SetRect(104, 292, 650, 23, false);
	this->txtOutlook->SetReadOnly(true);

	this->Reload(Net::HKOWeather::Language::En);
}

SSWR::AVIRead::AVIRHKOLocalForecastForm::~AVIRHKOLocalForecastForm()
{
	this->ssl.Delete();
}

void SSWR::AVIRead::AVIRHKOLocalForecastForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
