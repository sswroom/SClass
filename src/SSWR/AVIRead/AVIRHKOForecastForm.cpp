#include "Stdafx.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRHKOForecastForm.h"

void __stdcall SSWR::AVIRead::AVIRHKOForecastForm::OnReloadClicked(void *userObj)
{
	SSWR::AVIRead::AVIRHKOForecastForm *me = (SSWR::AVIRead::AVIRHKOForecastForm*)userObj;
	Net::HKOWeather::Language lang = (Net::HKOWeather::Language)(OSInt)me->cboLang->GetSelectedItem();
	me->Reload(lang);
}

void SSWR::AVIRead::AVIRHKOForecastForm::Reload(Net::HKOWeather::Language lang)
{
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	Net::HKOWeather::WeatherForecast weather;
	Net::HKOWeather::DayForecast *forecast;
	Data::Timestamp reqTime = Data::Timestamp::Now();
	sptr = reqTime.ToStringNoZone(sbuff);
	this->txtReqTime->SetText(CSTRP(sbuff, sptr));
	this->lvForecast->ClearItems();
	if (Net::HKOWeather::GetWeatherForecast(this->core->GetSocketFactory(), this->ssl, lang, &weather))
	{
		sptr = weather.updateTime.ToStringNoZone(sbuff);
		this->txtUpdateTime->SetText(CSTRP(sbuff, sptr));
		this->txtGeneralSituation->SetText(weather.generalSituation->ToCString());
		sptr = Text::StrInt32(sbuff, weather.seaTemp);
		this->txtSeaTemp->SetText(CSTRP(sbuff, sptr));
		this->txtSeaTempPlace->SetText(weather.seaTempPlace->ToCString());
		sptr = weather.seaTempTime.ToStringNoZone(sbuff);
		this->txtSeaTempTime->SetText(CSTRP(sbuff, sptr));

		UOSInt i = 0;
		UOSInt j = weather.forecast.GetCount();
		while (i < j)
		{
			forecast = weather.forecast.GetItem(i);
			sptr = Data::DateTimeUtil::DispYear(sbuff, forecast->date.year);
			*sptr++ = '-';
			sptr = Text::StrUInt16(sptr, forecast->date.month);
			*sptr++ = '-';
			sptr = Text::StrUInt16(sptr, forecast->date.day);
			this->lvForecast->AddItem(CSTRP(sbuff, sptr), 0);
			this->lvForecast->SetSubItem(i, 1, Data::DateTimeUtil::WeekdayGetName(forecast->weekday));
			sptr = Text::StrInt32(sbuff, forecast->minTemp);
			sptr = Text::StrConcatC(sptr, UTF8STRC(" - "));
			sptr = Text::StrInt32(sptr, forecast->maxTemp);
			this->lvForecast->SetSubItem(i, 2, CSTRP(sbuff, sptr));
			sptr = Text::StrInt32(sbuff, forecast->minRH);
			sptr = Text::StrConcatC(sptr, UTF8STRC(" - "));
			sptr = Text::StrInt32(sptr, forecast->maxRH);
			this->lvForecast->SetSubItem(i, 3, CSTRP(sbuff, sptr));
			this->lvForecast->SetSubItem(i, 4, Net::HKOWeather::ForecastIconGetName(forecast->weatherIcon));
			this->lvForecast->SetSubItem(i, 5, Net::HKOWeather::PSRGetName(forecast->psr));
			this->lvForecast->SetSubItem(i, 6, forecast->wind);
			this->lvForecast->SetSubItem(i, 7, forecast->weather);
			
			i++;
		}
		Net::HKOWeather::FreeWeatherForecast(&weather);
	}
	else
	{
		this->txtUpdateTime->SetText(CSTR(""));
		this->txtGeneralSituation->SetText(CSTR(""));
		this->txtSeaTemp->SetText(CSTR(""));
		this->txtSeaTempPlace->SetText(CSTR(""));
		this->txtSeaTempTime->SetText(CSTR(""));
	}
}

SSWR::AVIRead::AVIRHKOForecastForm::AVIRHKOForecastForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 800, 600, ui)
{
	this->SetText(CSTR("HK Observatory 9-day Weather Forecast"));
	this->SetFont(0, 0, 8.25, false);
	
	this->core = core;
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetSocketFactory(), true);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASSNN(this->pnlMain, UI::GUIPanel(ui, *this));
	this->pnlMain->SetRect(0, 0, 100, 248, false);
	this->pnlMain->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblLang, UI::GUILabel(ui, this->pnlMain, CSTR("Language")));
	this->lblLang->SetRect(4, 4, 100, 23, false);
	this->cboLang = ui->NewComboBox(this->pnlMain, false);
	this->cboLang->SetRect(104, 4, 150, 23, false);
	this->cboLang->AddItem(CSTR("English"), (void*)Net::HKOWeather::Language::En);
	this->cboLang->AddItem(CSTR("Chinese (Traditional)"), (void*)Net::HKOWeather::Language::TC);
	this->cboLang->AddItem(CSTR("Chinese (Simplified)"), (void*)Net::HKOWeather::Language::SC);
	this->cboLang->SetSelectedIndex(0);
	this->btnReload = ui->NewButton(this->pnlMain, CSTR("Reload"));
	this->btnReload->SetRect(254, 4, 75, 23, false);
	this->btnReload->HandleButtonClick(OnReloadClicked, this);

	NEW_CLASS(this->lblReqTime, UI::GUILabel(ui, this->pnlMain, CSTR("Request Time")));
	this->lblReqTime->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtReqTime, UI::GUITextBox(ui, this->pnlMain, CSTR("")));
	this->txtReqTime->SetRect(104, 52, 180, 23, false);
	this->txtReqTime->SetReadOnly(true);
	NEW_CLASS(this->lblUpdateTime, UI::GUILabel(ui, this->pnlMain, CSTR("Update Time")));
	this->lblUpdateTime->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtUpdateTime, UI::GUITextBox(ui, this->pnlMain, CSTR("")));
	this->txtUpdateTime->SetRect(104, 76, 180, 23, false);
	this->txtUpdateTime->SetReadOnly(true);
	NEW_CLASS(this->lblSeaTemp, UI::GUILabel(ui, this->pnlMain, CSTR("Sea Temperature")));
	this->lblSeaTemp->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtSeaTemp, UI::GUITextBox(ui, this->pnlMain, CSTR("")));
	this->txtSeaTemp->SetRect(104, 100, 100, 23, false);
	this->txtSeaTemp->SetReadOnly(true);
	NEW_CLASS(this->lblSeaTempPlace, UI::GUILabel(ui, this->pnlMain, CSTR("Place of Sea record")));
	this->lblSeaTempPlace->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->txtSeaTempPlace, UI::GUITextBox(ui, this->pnlMain, CSTR("")));
	this->txtSeaTempPlace->SetRect(104, 124, 100, 23, false);
	this->txtSeaTempPlace->SetReadOnly(true);
	NEW_CLASS(this->lblSeaTempTime, UI::GUILabel(ui, this->pnlMain, CSTR("Time of Sea record")));
	this->lblSeaTempTime->SetRect(4, 148, 100, 23, false);
	NEW_CLASS(this->txtSeaTempTime, UI::GUITextBox(ui, this->pnlMain, CSTR("")));
	this->txtSeaTempTime->SetRect(104, 148, 100, 23, false);
	this->txtSeaTempTime->SetReadOnly(true);
	NEW_CLASS(this->lblGeneralSituation, UI::GUILabel(ui, this->pnlMain, CSTR("General Situation")));
	this->lblGeneralSituation->SetRect(4, 172, 100, 23, false);
	NEW_CLASS(this->txtGeneralSituation, UI::GUITextBox(ui, this->pnlMain, CSTR(""), true));
	this->txtGeneralSituation->SetRect(104, 172, 650, 71, false);
	this->txtGeneralSituation->SetReadOnly(true);
	this->txtGeneralSituation->SetWordWrap(true);

	NEW_CLASS(this->lvForecast, UI::GUIListView(ui, *this, UI::GUIListView::LVSTYLE_TABLE, 8));
	this->lvForecast->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvForecast->SetFullRowSelect(true);
	this->lvForecast->SetShowGrid(true);
	this->lvForecast->AddColumn(CSTR("Date"), 80);
	this->lvForecast->AddColumn(CSTR("Weekday"), 60);
	this->lvForecast->AddColumn(CSTR("Temp Range"), 80);
	this->lvForecast->AddColumn(CSTR("RH Range"), 80);
	this->lvForecast->AddColumn(CSTR("Weather"), 100);
	this->lvForecast->AddColumn(CSTR("PSR"), 50);
	this->lvForecast->AddColumn(CSTR("Wind"), 150);
	this->lvForecast->AddColumn(CSTR("Description"), 150);

	this->Reload(Net::HKOWeather::Language::En);
}

SSWR::AVIRead::AVIRHKOForecastForm::~AVIRHKOForecastForm()
{
	this->ssl.Delete();
}

void SSWR::AVIRead::AVIRHKOForecastForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
