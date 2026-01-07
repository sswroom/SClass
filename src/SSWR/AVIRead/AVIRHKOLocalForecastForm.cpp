#include "Stdafx.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRHKOLocalForecastForm.h"

void __stdcall SSWR::AVIRead::AVIRHKOLocalForecastForm::OnReloadClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHKOLocalForecastForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHKOLocalForecastForm>();
	Net::HKOWeather::Language lang = (Net::HKOWeather::Language)me->cboLang->GetSelectedItem().GetOSInt();
	me->Reload(lang);
}

void SSWR::AVIRead::AVIRHKOLocalForecastForm::Reload(Net::HKOWeather::Language lang)
{
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	Net::HKOWeather::LocalForecast forecast;
	Data::Timestamp reqTime = Data::Timestamp::Now();
	sptr = reqTime.ToStringNoZone(sbuff);
	this->txtReqTime->SetText(CSTRP(sbuff, sptr));
	if (Net::HKOWeather::GetLocalForecast(this->core->GetTCPClientFactory(), this->ssl, lang, forecast))
	{
		sptr = forecast.updateTime.ToStringNoZone(sbuff);
		this->txtUpdateTime->SetText(CSTRP(sbuff, sptr));
		this->txtGeneralSituation->SetText(Text::String::OrEmpty(forecast.generalSituation)->ToCString());
		this->txtTCInfo->SetText(Text::String::OrEmpty(forecast.tcInfo)->ToCString());
		this->txtFireDangerWarning->SetText(Text::String::OrEmpty(forecast.fireDangerWarning)->ToCString());
		this->txtForecastPeriod->SetText(Text::String::OrEmpty(forecast.forecastPeriod)->ToCString());
		this->txtForecastDesc->SetText(Text::String::OrEmpty(forecast.forecastDesc)->ToCString());
		this->txtOutlook->SetText(Text::String::OrEmpty(forecast.outlook)->ToCString());
		Net::HKOWeather::FreeLocalForecast(forecast);
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

SSWR::AVIRead::AVIRHKOLocalForecastForm::AVIRHKOLocalForecastForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 800, 600, ui)
{
	this->SetText(CSTR("HK Observatory Local Weather Forecast"));
	this->SetFont(nullptr, 8.25, false);
	
	this->core = core;
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetTCPClientFactory(), true);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblLang = ui->NewLabel(*this, CSTR("Language"));
	this->lblLang->SetRect(4, 4, 100, 23, false);
	this->cboLang = ui->NewComboBox(*this, false);
	this->cboLang->SetRect(104, 4, 150, 23, false);
	this->cboLang->AddItem(CSTR("English"), (void*)Net::HKOWeather::Language::En);
	this->cboLang->AddItem(CSTR("Chinese (Traditional)"), (void*)Net::HKOWeather::Language::TC);
	this->cboLang->AddItem(CSTR("Chinese (Simplified)"), (void*)Net::HKOWeather::Language::SC);
	this->cboLang->SetSelectedIndex(0);
	this->btnReload = ui->NewButton(*this, CSTR("Reload"));
	this->btnReload->SetRect(254, 4, 75, 23, false);
	this->btnReload->HandleButtonClick(OnReloadClicked, this);

	this->lblReqTime = ui->NewLabel(*this, CSTR("Request Time"));
	this->lblReqTime->SetRect(4, 52, 100, 23, false);
	this->txtReqTime = ui->NewTextBox(*this, CSTR(""));
	this->txtReqTime->SetRect(104, 52, 180, 23, false);
	this->txtReqTime->SetReadOnly(true);
	this->lblUpdateTime = ui->NewLabel(*this, CSTR("Update Time"));
	this->lblUpdateTime->SetRect(4, 76, 100, 23, false);
	this->txtUpdateTime = ui->NewTextBox(*this, CSTR(""));
	this->txtUpdateTime->SetRect(104, 76, 180, 23, false);
	this->txtUpdateTime->SetReadOnly(true);
	this->lblGeneralSituation = ui->NewLabel(*this, CSTR("General Situation"));
	this->lblGeneralSituation->SetRect(4, 100, 100, 23, false);
	this->txtGeneralSituation = ui->NewTextBox(*this, CSTR(""), true);
	this->txtGeneralSituation->SetRect(104, 100, 650, 47, false);
	this->txtGeneralSituation->SetReadOnly(true);
	this->txtGeneralSituation->SetWordWrap(true);
	this->lblTCInfo = ui->NewLabel(*this, CSTR("TC Info"));
	this->lblTCInfo->SetRect(4, 148, 100, 23, false);
	this->txtTCInfo = ui->NewTextBox(*this, CSTR(""));
	this->txtTCInfo->SetRect(104, 148, 650, 23, false);
	this->txtTCInfo->SetReadOnly(true);
	this->lblFireDangerWarning = ui->NewLabel(*this, CSTR("Fire Danger Warning"));
	this->lblFireDangerWarning->SetRect(4, 172, 100, 23, false);
	this->txtFireDangerWarning = ui->NewTextBox(*this, CSTR(""));
	this->txtFireDangerWarning->SetRect(104, 172, 650, 23, false);
	this->txtFireDangerWarning->SetReadOnly(true);
	this->lblForecastPeriod = ui->NewLabel(*this, CSTR("Forecast Period"));
	this->lblForecastPeriod->SetRect(4, 196, 100, 23, false);
	this->txtForecastPeriod = ui->NewTextBox(*this, CSTR(""));
	this->txtForecastPeriod->SetRect(104, 196, 650, 23, false);
	this->txtForecastPeriod->SetReadOnly(true);
	this->lblForecastDesc = ui->NewLabel(*this, CSTR("Forecast Desc"));
	this->lblForecastDesc->SetRect(4, 220, 100, 23, false);
	this->txtForecastDesc = ui->NewTextBox(*this, CSTR(""), true);
	this->txtForecastDesc->SetRect(104, 220, 650, 71, false);
	this->txtForecastDesc->SetReadOnly(true);
	this->txtForecastDesc->SetWordWrap(true);
	this->lblOutlook = ui->NewLabel(*this, CSTR("Outlook"));
	this->lblOutlook->SetRect(4, 292, 100, 23, false);
	this->txtOutlook = ui->NewTextBox(*this, CSTR(""));
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
