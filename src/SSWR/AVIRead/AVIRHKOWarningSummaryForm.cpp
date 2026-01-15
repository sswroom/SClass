#include "Stdafx.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRHKOWarningSummaryForm.h"

void SSWR::AVIRead::AVIRHKOWarningSummaryForm::Reload()
{
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	Data::Timestamp reqTime = Data::Timestamp::Now();
	sptr = reqTime.ToStringNoZone(sbuff);
	this->txtReqTime->SetText(CSTRP(sbuff, sptr));

	NN<Net::HKOWeather::WarningSummary> warning;
	Data::ArrayListNN<Net::HKOWeather::WarningSummary> warnings;
	this->lvWarning->ClearItems();
	if (Net::HKOWeather::GetWarningSummary(this->core->GetTCPClientFactory(), this->ssl, warnings))
	{
		UIntOS i = 0;
		UIntOS j = warnings.GetCount();
		while (i < j)
		{
			warning = warnings.GetItemNoCheck(i);
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
		Net::HKOWeather::FreeWarningSummary(warnings);
	}
}

SSWR::AVIRead::AVIRHKOWarningSummaryForm::AVIRHKOWarningSummaryForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 800, 600, ui)
{
	this->SetText(CSTR("HK Observatory Warning Summary"));
	this->SetFont(nullptr, 8.25, false);
	
	this->core = core;
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetTCPClientFactory(), true);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlReqTime = ui->NewPanel(*this);
	this->pnlReqTime->SetRect(0, 0, 100, 31, false);
	this->pnlReqTime->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblReqTime = ui->NewLabel(this->pnlReqTime, CSTR("Request Time"));
	this->lblReqTime->SetRect(4, 4, 100, 23, false);
	this->txtReqTime = ui->NewTextBox(*this, CSTR(""));
	this->txtReqTime->SetRect(104, 4, 160, 23, false);
	this->txtReqTime->SetReadOnly(true);
	this->lvWarning = ui->NewListView(*this, UI::ListViewStyle::Table, 6);
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
