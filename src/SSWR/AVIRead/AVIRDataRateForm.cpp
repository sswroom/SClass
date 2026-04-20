#include "Stdafx.h"
#include "IO/FileStream.h"
#include "SSWR/AVIRead/AVIRDataRateForm.h"
#include "Text/CharUtil.h"
#include "UI/Clipboard.h"
#include "UI/GUIFileDialog.h"

void __stdcall SSWR::AVIRead::AVIRDataRateForm::OnSaveClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDataRateForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDataRateForm>();
	NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"DataRateSave", true);
	dlg->AddFilter(CSTR("*.csv"), CSTR("CSV File"));
	if (dlg->ShowDialog(me->GetHandle()))
	{
		UTF8Char sbuff[256];
		UnsafeArray<UTF8Char> sptr;
		UIntOS i = 0;
		UIntOS j = me->dataRate->GetCount();
		NN<IO::DataRateCalc::DataStat> stat;
		Int64 lastTime = 0;
		IO::FileStream fs(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		while (i < j)
		{
			if (me->dataRate->GetItem(i).SetTo(stat))
			{
				if (lastTime != 0 && lastTime + 1 != stat->sec)
				{
					sptr = Data::Timestamp::FromEpochSec(lastTime + 1, Data::DateTimeUtil::GetLocalTzQhr()).ToStringNoZone(sbuff);
					*sptr++ = ',';
					*sptr++ = '0';
					*sptr++ = '\r';
					*sptr++ = '\n';
					fs.Write(Data::ByteArrayR(sbuff, (UIntOS)(sptr - sbuff)));
				}
				sptr = Data::Timestamp::FromEpochSec(stat->sec, Data::DateTimeUtil::GetLocalTzQhr()).ToStringNoZone(sbuff);
				*sptr++ = ',';
				sptr = Text::StrUInt64(sptr, stat->dataSize);
				*sptr++ = '\r';
				*sptr++ = '\n';
				fs.Write(Data::ByteArrayR(sbuff, (UIntOS)(sptr - sbuff)));
				lastTime = stat->sec;
			}
			i++;
		}
	}
	dlg.Delete();
}

SSWR::AVIRead::AVIRDataRateForm::AVIRDataRateForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<IO::DataRateCalc> dataRate) : UI::GUIForm(parent, 800, 600, ui)
{
	this->SetText(CSTR("Data Rate"));
	this->SetFont(nullptr, 8.25, false);
	
	this->core = core;
	this->dataRate = dataRate;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlControl = ui->NewPanel(*this);
	this->pnlControl->SetRect(0, 0, 100, 31, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->btnSave = ui->NewButton(this->pnlControl, CSTR("Save"));
	this->btnSave->SetRect(4, 4, 75, 23, false);
	this->btnSave->HandleButtonClick(OnSaveClicked, this);
	this->lvDataRates = ui->NewListView(*this, UI::ListViewStyle::Table, 2);
	this->lvDataRates->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDataRates->AddColumn(CSTR("Time"), 150);
	this->lvDataRates->AddColumn(CSTR("Data Rate"), 150);
	this->lvDataRates->SetFullRowSelect(true);
	this->lvDataRates->SetShowGrid(true);

	NN<IO::DataRateCalc::DataStat> stat;
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	UIntOS k;
	UIntOS i = 0;
	UIntOS j = this->dataRate->GetCount();
	while (i < j)
	{
		if (this->dataRate->GetItem(i).SetTo(stat))
		{
			sptr = Data::Timestamp::FromEpochSec(stat->sec, Data::DateTimeUtil::GetLocalTzQhr()).ToString(sbuff);
			k = this->lvDataRates->AddItem(CSTRP(sbuff, sptr), 0);
			sptr = Text::StrUInt64(sbuff, stat->dataSize);
			this->lvDataRates->SetSubItem(k, 1, CSTRP(sbuff, sptr));
		}
		i++;
	}
}

SSWR::AVIRead::AVIRDataRateForm::~AVIRDataRateForm()
{
	this->dataRate.Delete();
}

void SSWR::AVIRead::AVIRDataRateForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
