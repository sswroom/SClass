#include "Stdafx.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRHKOWarningSummaryForm.h"

void SSWR::AVIRead::AVIRHKOWarningSummaryForm::Reload()
{
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	Data::Timestamp reqTime = Data::Timestamp::Now();
	sptr = reqTime.ToStringNoZone(sbuff);
	this->txtReqTime->SetText(CSTRP(sbuff, sptr));

	Net::HKOWeather::WarningSummary *warning;
	Data::ArrayList<Net::HKOWeather::WarningSummary*> warnings;
	this->lvWarning->ClearItems();
	if (Net::HKOWeather::GetWarningSummary(this->core->GetSocketFactory(), this->ssl, &warnings))
	{
		UOSInt i = 0;
		UOSInt j = warnings.GetCount();
		while (i < j)
		{
			warning = warnings.GetItem(i);
			this->lvWarning->AddItem(Net::HKOWeather::WeatherWarningGetCode(warning->code), 0);
			this->lvWarning->SetSubItem(i, 1, Net::HKOWeather::WeatherWarningGetName(warning->code));
			this->lvWarning->SetSubItem(i, 2, Net::HKOWeather::SignalActionGetName(warning->actionCode));
			sptr = warning->issueTime.ToString(sbuff);
			this->lvWarning->SetSubItem(i, 3, CSTRP(sbuff, sptr));
			sptr = warning->updateTime.ToString(sbuff);
			this->lvWarning->SetSubItem(i, 4, CSTRP(sbuff, sptr));
			sptr = warning->expireTime.ToString(sbuff);
			this->lvWarning->SetSubItem(i, 5, CSTRP(sbuff, sptr));
			i++;
		}
		Net::HKOWeather::FreeWarningSummary(&warnings);
	}
}

SSWR::AVIRead::AVIRHKOWarningSummaryForm::AVIRHKOWarningSummaryForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 800, 600, ui)
{
	this->SetText(CSTR("HK Observatory Warning Summary"));
	this->SetFont(0, 0, 8.25, false);
	
	this->core = core;
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetSocketFactory(), true);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASSNN(this->pnlReqTime, UI::GUIPanel(ui, *this));
	this->pnlReqTime->SetRect(0, 0, 100, 31, false);
	this->pnlReqTime->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblReqTime = ui->NewLabel(this->pnlReqTime, CSTR("Request Time"));
	this->lblReqTime->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtReqTime, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtReqTime->SetRect(104, 4, 160, 23, false);
	this->txtReqTime->SetReadOnly(true);
	NEW_CLASS(this->lvWarning, UI::GUIListView(ui, *this, UI::GUIListView::LVSTYLE_TABLE, 6));
	this->lvWarning->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvWarning->SetFullRowSelect(true);
	this->lvWarning->SetShowGrid(true);
	this->lvWarning->AddColumn(CSTR("Code"), 50);
	this->lvWarning->AddColumn(CSTR("Name"), 150);
	this->lvWarning->AddColumn(CSTR("Action"), 60);
	this->lvWarning->AddColumn(CSTR("Issue Time"), 150);
	this->lvWarning->AddColumn(CSTR("Update Time"), 150);
	this->lvWarning->AddColumn(CSTR("Expire Time"), 150);

	this->Reload();
}

SSWR::AVIRead::AVIRHKOWarningSummaryForm::~AVIRHKOWarningSummaryForm()
{
	this->ssl.Delete();
}

void SSWR::AVIRead::AVIRHKOWarningSummaryForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
