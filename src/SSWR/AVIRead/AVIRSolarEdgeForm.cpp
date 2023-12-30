#include "Stdafx.h"
#include "Data/LineChart.h"
#include "Math/Unit/Count.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRSolarEdgeForm.h"
#include "UI/GUIComboBoxUtil.h"

void __stdcall SSWR::AVIRead::AVIRSolarEdgeForm::OnAPIKeyClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSolarEdgeForm *me = (SSWR::AVIRead::AVIRSolarEdgeForm*)userObj;
	UOSInt i;
	UOSInt j;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	if (me->seAPI)
	{
		me->seAPI->FreeSiteList(&me->siteList);
		DEL_CLASS(me->seAPI);
		me->seAPI = 0;
		me->btnAPIKey->SetText(CSTR("Start"));
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		me->txtAPIKey->GetText(sb);
		if (sb.GetLength() == 0)
		{
			me->ui->ShowMsgOK(CSTR("Please enter API Key"), CSTR("SolarEdge API"), me);
			return;
		}
		NEW_CLASS(me->seAPI, Net::SolarEdgeAPI(me->core->GetSocketFactory(), me->ssl, sb.ToCString()));
		NotNullPtr<Text::String> s;
		if (me->seAPI->GetCurrentVersion().SetTo(s))
		{
			me->txtCurrVer->SetText(s->ToCString());
			s->Release();

			Data::ArrayListStringNN vers;
			if (me->seAPI->GetSupportedVersions(&vers))
			{
				sb.ClearStr();
				Data::ArrayIterator<NotNullPtr<Text::String>> it = vers.Iterator();
				Bool found = false;
				while (it.HasNext())
				{
					if (found)
						sb.AppendC(UTF8STRC(", "));
					sb.Append(it.Next());
					found = true;
				}
				me->txtSuppVer->SetText(sb.ToCString());
				vers.FreeAll();
			}
			else
			{
				me->txtSuppVer->SetText(CSTR(""));
			}
			UOSInt totalCount;
			Net::SolarEdgeAPI::Site *site;
			me->lvSiteList->ClearItems();
			me->cboSiteEnergySite->ClearItems();
			me->cboSitePowerSite->ClearItems();
			if (me->seAPI->GetSiteList(&me->siteList, 20, 0, &totalCount))
			{
				i = 0;
				j = me->siteList.GetCount();
				while (i < j)
				{
					site = me->siteList.GetItem(i);
					sptr = Text::StrInt32(sbuff, site->id);
					me->lvSiteList->AddItem(CSTRP(sbuff, sptr), (void*)site);
					if (site->name.SetTo(s))
					{
						me->lvSiteList->SetSubItem(i, 1, s);
						*sptr++ = ' ';
						sptr = s->ConcatTo(sptr);
						me->cboSiteEnergySite->AddItem(CSTRP(sbuff, sptr), (void*)site);
						me->cboSitePowerSite->AddItem(CSTRP(sbuff, sptr), (void*)site);
					}
					sptr = Text::StrInt32(sbuff, site->accountId);
					me->lvSiteList->SetSubItem(i, 2, CSTRP(sbuff, sptr));
					if (site->status.SetTo(s))
						me->lvSiteList->SetSubItem(i, 3, s);
					sptr = Text::StrConcatC(Text::StrDouble(sbuff, site->peakPower_kWp), UTF8STRC(" kWp"));
					me->lvSiteList->SetSubItem(i, 4, CSTRP(sbuff, sptr));
					sptr = site->lastUpdateTime.ToStringNoZone(sbuff);
					me->lvSiteList->SetSubItem(i, 5, CSTRP(sbuff, sptr));
					if (site->currency.SetTo(s))
						me->lvSiteList->SetSubItem(i, 6, s);
					sptr = site->installationDate.ToStringNoZone(sbuff);
					me->lvSiteList->SetSubItem(i, 7, CSTRP(sbuff, sptr));
					sptr = site->ptoDate.ToStringNoZone(sbuff);
					me->lvSiteList->SetSubItem(i, 8, CSTRP(sbuff, sptr));
					if (site->notes.SetTo(s))
						me->lvSiteList->SetSubItem(i, 9, s);
					if (site->type.SetTo(s))
						me->lvSiteList->SetSubItem(i, 10, s);
					i++;
				}
				if (j > 0)
				{
					me->cboSiteEnergySite->SetSelectedIndex(0);
					me->cboSitePowerSite->SetSelectedIndex(0);
				}
			}

			me->btnAPIKey->SetText(CSTR("Stop"));
		}
		else
		{
			DEL_CLASS(me->seAPI);
			me->seAPI = 0;
			me->ui->ShowMsgOK(CSTR("API Key error"), CSTR("SolarEdge API"), me);
			return;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRSolarEdgeForm::OnSiteListSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRSolarEdgeForm *me = (SSWR::AVIRead::AVIRSolarEdgeForm*)userObj;
	Net::SolarEdgeAPI::Site *site = (Net::SolarEdgeAPI::Site*)me->lvSiteList->GetSelectedItem();
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	if (site)
	{
		me->txtSiteCountry->SetText(Text::String::OrEmpty(site->country)->ToCString());
		me->txtSiteCity->SetText(Text::String::OrEmpty(site->city)->ToCString());
		me->txtSiteAddress->SetText(Text::String::OrEmpty(site->address)->ToCString());
		me->txtSiteAddress2->SetText(Text::String::OrEmpty(site->address2)->ToCString());
		me->txtSiteZIP->SetText(Text::String::OrEmpty(site->zip)->ToCString());
		me->txtSiteTimeZone->SetText(Text::String::OrEmpty(site->timeZone)->ToCString());
		me->txtSiteCountryCode->SetText(Text::String::OrEmpty(site->countryCode)->ToCString());
		me->txtSiteIsPublic->SetText(site->isPublic?CSTR("true"):CSTR("false"));
		me->txtSitePublicName->SetText(Text::String::OrEmpty(site->publicName)->ToCString());

		Net::SolarEdgeAPI::SiteOverview overview;
		if (me->seAPI && me->seAPI->GetSiteOverview(site->id, &overview))
		{
			sptr = Text::StrConcatC(Math::Unit::Count::WellFormat(sbuff, overview.lifeTimeEnergy_Wh), UTF8STRC("Wh"));
			me->txtSiteLifetimeEnergy->SetText(CSTRP(sbuff, sptr));
			sptr = Math::Unit::Count::WellFormat(sbuff, overview.lifeTimeRevenue);
			me->txtSiteLifetimeRevenue->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrConcatC(Math::Unit::Count::WellFormat(sbuff, overview.yearlyEnergy_Wh), UTF8STRC("Wh"));
			me->txtSiteYearlyEnergy->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrConcatC(Math::Unit::Count::WellFormat(sbuff, overview.monthlyEnergy_Wh), UTF8STRC("Wh"));
			me->txtSiteMonthlyEnergy->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrConcatC(Math::Unit::Count::WellFormat(sbuff, overview.dailyEnergy_Wh), UTF8STRC("Wh"));
			me->txtSiteDailyEnergy->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrConcatC(Math::Unit::Count::WellFormat(sbuff, overview.currentPower_W), UTF8STRC("W"));
			me->txtSiteCurrentPower->SetText(CSTRP(sbuff, sptr));
		}
		else
		{
			me->txtSiteLifetimeEnergy->SetText(CSTR(""));
			me->txtSiteLifetimeRevenue->SetText(CSTR(""));
			me->txtSiteYearlyEnergy->SetText(CSTR(""));
			me->txtSiteMonthlyEnergy->SetText(CSTR(""));
			me->txtSiteDailyEnergy->SetText(CSTR(""));
			me->txtSiteCurrentPower->SetText(CSTR(""));
		}
	}
	else
	{
		me->txtSiteCountry->SetText(CSTR(""));
		me->txtSiteCity->SetText(CSTR(""));
		me->txtSiteAddress->SetText(CSTR(""));
		me->txtSiteAddress2->SetText(CSTR(""));
		me->txtSiteZIP->SetText(CSTR(""));
		me->txtSiteTimeZone->SetText(CSTR(""));
		me->txtSiteCountryCode->SetText(CSTR(""));
		me->txtSiteIsPublic->SetText(CSTR(""));
		me->txtSitePublicName->SetText(CSTR(""));

		me->txtSiteLifetimeEnergy->SetText(CSTR(""));
		me->txtSiteLifetimeRevenue->SetText(CSTR(""));
		me->txtSiteYearlyEnergy->SetText(CSTR(""));
		me->txtSiteMonthlyEnergy->SetText(CSTR(""));
		me->txtSiteDailyEnergy->SetText(CSTR(""));
		me->txtSiteCurrentPower->SetText(CSTR(""));
	}
}

void __stdcall SSWR::AVIRead::AVIRSolarEdgeForm::OnSiteEnergyClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSolarEdgeForm *me = (SSWR::AVIRead::AVIRSolarEdgeForm*)userObj;
	Net::SolarEdgeAPI::Site *site = (Net::SolarEdgeAPI::Site*)me->cboSiteEnergySite->GetSelectedItem();
	if (site && me->seAPI)
	{
		Net::SolarEdgeAPI::TimeUnit timeUnit = (Net::SolarEdgeAPI::TimeUnit)(UOSInt)me->cboSiteEnergyInterval->GetSelectedItem();
		Data::DateTimeUtil::TimeValue timeVal;
		timeVal.year = (UInt16)(UOSInt)me->cboSiteEnergyYear->GetSelectedItem();
		timeVal.month = (UInt8)(UOSInt)me->cboSiteEnergyMonth->GetSelectedItem();
		timeVal.day = (UInt8)(UOSInt)me->cboSiteEnergyDay->GetSelectedItem();
		timeVal.hour = 0;
		timeVal.minute = 0;
		timeVal.second = 0;
		Data::Timestamp startTime = GetDefaultStartTime(Data::Timestamp::FromTimeValue(timeVal, 0, Data::DateTimeUtil::GetLocalTzQhr()), timeUnit);
		Data::Timestamp endTime = GetDefaultEndTime(startTime, timeUnit);
		Data::ArrayList<Net::SolarEdgeAPI::TimedValue> values;
		if (me->seAPI->GetSiteEnergy(site->id, startTime, endTime, timeUnit, &values))
		{
			me->siteEnergyList.Clear();
			me->siteEnergyList.AddAll(values);
			me->UpdateSiteEnergyGraph();
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRSolarEdgeForm::OnSiteEnergySizeChg(void *userObj)
{
	SSWR::AVIRead::AVIRSolarEdgeForm *me = (SSWR::AVIRead::AVIRSolarEdgeForm*)userObj;
	me->UpdateSiteEnergyGraph();
}

void __stdcall SSWR::AVIRead::AVIRSolarEdgeForm::OnSitePowerClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSolarEdgeForm *me = (SSWR::AVIRead::AVIRSolarEdgeForm*)userObj;
	Net::SolarEdgeAPI::Site *site = (Net::SolarEdgeAPI::Site*)me->cboSitePowerSite->GetSelectedItem();
	if (site && me->seAPI)
	{
		Net::SolarEdgeAPI::TimeUnit timeUnit = Net::SolarEdgeAPI::TimeUnit::QUARTER_OF_AN_HOUR;
		Data::DateTimeUtil::TimeValue timeVal;
		timeVal.year = (UInt16)(UOSInt)me->cboSitePowerYear->GetSelectedItem();
		timeVal.month = (UInt8)(UOSInt)me->cboSitePowerMonth->GetSelectedItem();
		timeVal.day = (UInt8)(UOSInt)me->cboSitePowerDay->GetSelectedItem();
		timeVal.hour = 0;
		timeVal.minute = 0;
		timeVal.second = 0;
		Data::Timestamp startTime = GetDefaultStartTime(Data::Timestamp::FromTimeValue(timeVal, 0, Data::DateTimeUtil::GetLocalTzQhr()), timeUnit);
		Data::Timestamp endTime = startTime.AddMinute(24 * 60 - 15);
		Data::ArrayList<Net::SolarEdgeAPI::TimedValue> values;
		if (me->seAPI->GetSitePower(site->id, startTime, endTime, &values))
		{
			me->sitePowerList.Clear();
			me->sitePowerList.AddAll(values);
			me->UpdateSitePowerGraph();
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRSolarEdgeForm::OnSitePowerSizeChg(void *userObj)
{
	SSWR::AVIRead::AVIRSolarEdgeForm *me = (SSWR::AVIRead::AVIRSolarEdgeForm*)userObj;
	me->UpdateSitePowerGraph();
}

Data::Timestamp SSWR::AVIRead::AVIRSolarEdgeForm::GetDefaultStartTime(const Data::Timestamp &startTime, Net::SolarEdgeAPI::TimeUnit timeUnit)
{
	switch (timeUnit)
	{
	case Net::SolarEdgeAPI::TimeUnit::DAY:
		return startTime.ClearDayOfMonth();
	case Net::SolarEdgeAPI::TimeUnit::QUARTER_OF_AN_HOUR:
		return startTime.ClearTimeLocal();
	case Net::SolarEdgeAPI::TimeUnit::HOUR:
		return startTime.ClearTimeLocal();
	case Net::SolarEdgeAPI::TimeUnit::WEEK:
		return startTime.ClearMonthAndDay();
	case Net::SolarEdgeAPI::TimeUnit::MONTH:
		return startTime.ClearMonthAndDay();
	case Net::SolarEdgeAPI::TimeUnit::YEAR:
		return startTime.ClearMonthAndDay();
	default:
		return startTime;
	}
}

Data::Timestamp SSWR::AVIRead::AVIRSolarEdgeForm::GetDefaultEndTime(const Data::Timestamp &startTime, Net::SolarEdgeAPI::TimeUnit timeUnit)
{
	switch (timeUnit)
	{
	case Net::SolarEdgeAPI::TimeUnit::DAY:
		return startTime.AddMonth(1).AddDay(-1);
	case Net::SolarEdgeAPI::TimeUnit::QUARTER_OF_AN_HOUR:
		return startTime;
	case Net::SolarEdgeAPI::TimeUnit::HOUR:
		return startTime;
	case Net::SolarEdgeAPI::TimeUnit::WEEK:
		return startTime.AddMonth(1).AddDay(-1);
	case Net::SolarEdgeAPI::TimeUnit::MONTH:
		return startTime.AddMonth(1).AddDay(-1);
	case Net::SolarEdgeAPI::TimeUnit::YEAR:
		return startTime.AddMonth(1).AddDay(-1);
	default:
		return startTime;
	}
}

void SSWR::AVIRead::AVIRSolarEdgeForm::UpdateSiteEnergyGraph()
{
	NotNullPtr<Media::DrawEngine> deng = this->core->GetDrawEngine();
	Math::Size2D<UOSInt> size = this->pbSiteEnergy->GetSizeP();
	if (this->siteEnergyList.GetCount() > 0 && size.x > 0 && size.y > 0)
	{
		NotNullPtr<Media::DrawImage> dimg;
		if (dimg.Set(deng->CreateImage32(size, Media::AlphaType::AT_NO_ALPHA)))
		{
			dimg->SetHDPI(this->GetHDPI() * 96.0 / this->GetDDPI());
			dimg->SetVDPI(this->GetHDPI() * 96.0 / this->GetDDPI());
			Data::LineChart chart(CSTR("Site Energy"));
			chart.SetFontHeightPt(9.0);
			UOSInt i = 0;
			UOSInt j = this->siteEnergyList.GetCount();
			Data::Timestamp *tsList = MemAlloc(Data::Timestamp, j);
			Double *valList = MemAlloc(Double, j);
			while (i < j)
			{
				tsList[i] = this->siteEnergyList.GetItem(i).ts;
				valList[i] = this->siteEnergyList.GetItem(i).value;
				i++;
			}
			chart.AddXData(tsList, j);
			chart.AddYData(CSTR("Wh"), valList, j, 0xffff0000, Data::LineChart::LS_LINE);
			chart.Plot(dimg, 0, 0, UOSInt2Double(size.x), UOSInt2Double(size.y));
			MemFree(tsList);
			MemFree(valList);
			Media::StaticImage *simg = dimg->ToStaticImage();
			this->pbSiteEnergy->SetImage(simg);
			SDEL_CLASS(this->imgSiteEnergy);
			this->imgSiteEnergy = simg;
			deng->DeleteImage(dimg);
		}
	}
}

void SSWR::AVIRead::AVIRSolarEdgeForm::UpdateSitePowerGraph()
{
	NotNullPtr<Media::DrawEngine> deng = this->core->GetDrawEngine();
	Math::Size2D<UOSInt> size = this->pbSitePower->GetSizeP();
	if (this->sitePowerList.GetCount() > 0 && size.x > 0 && size.y > 0)
	{
		NotNullPtr<Media::DrawImage> dimg;
		if (dimg.Set(deng->CreateImage32(size, Media::AlphaType::AT_NO_ALPHA)))
		{
			dimg->SetHDPI(this->GetHDPI() * 96.0 / this->GetDDPI());
			dimg->SetVDPI(this->GetHDPI() * 96.0 / this->GetDDPI());
			Data::LineChart chart(CSTR("Site Power"));
			chart.SetFontHeightPt(9.0);
			UOSInt i = 0;
			UOSInt j = this->sitePowerList.GetCount();
			Data::Timestamp *tsList = MemAlloc(Data::Timestamp, j);
			Double *valList = MemAlloc(Double, j);
			while (i < j)
			{
				tsList[i] = this->sitePowerList.GetItem(i).ts;
				valList[i] = this->sitePowerList.GetItem(i).value;
				i++;
			}
			chart.AddXData(tsList, j);
			chart.AddYData(CSTR("W"), valList, j, 0xffff0000, Data::LineChart::LS_LINE);
			chart.Plot(dimg, 0, 0, UOSInt2Double(size.x), UOSInt2Double(size.y));
			MemFree(tsList);
			MemFree(valList);
			Media::StaticImage *simg = dimg->ToStaticImage();
			this->pbSitePower->SetImage(simg);
			SDEL_CLASS(this->imgSitePower);
			this->imgSitePower = simg;
			deng->DeleteImage(dimg);
		}
	}
}

SSWR::AVIRead::AVIRSolarEdgeForm::AVIRSolarEdgeForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("SolarEdge API"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetSocketFactory(), true);
	this->seAPI = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->imgSiteEnergy = 0;
	this->imgSitePower = 0;

	this->pnlAPIKey = ui->NewPanel(*this);
	this->pnlAPIKey->SetRect(0, 0, 100, 31, false);
	this->pnlAPIKey->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblAPIKey = ui->NewLabel(this->pnlAPIKey, CSTR("API Key"));
	this->lblAPIKey->SetRect(4, 4, 100, 23, false);
	this->txtAPIKey = ui->NewTextBox(this->pnlAPIKey, CSTR(""));
	this->txtAPIKey->SetRect(104, 4, 300, 23, false);
	this->btnAPIKey = ui->NewButton(this->pnlAPIKey, CSTR("Start"));
	this->btnAPIKey->SetRect(404, 4, 75, 23, false);
	this->btnAPIKey->HandleButtonClick(OnAPIKeyClicked, this);
	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpVersion = this->tcMain->AddTabPage(CSTR("Version"));
	this->lblCurrVer = ui->NewLabel(this->tpVersion, CSTR("Current Version"));
	this->lblCurrVer->SetRect(4, 4, 100, 23, false);
	this->txtCurrVer = ui->NewTextBox(this->tpVersion, CSTR(""));
	this->txtCurrVer->SetRect(104, 4, 100, 23, false);
	this->txtCurrVer->SetReadOnly(true);
	this->lblSuppVer = ui->NewLabel(this->tpVersion, CSTR("Supported Versions"));
	this->lblSuppVer->SetRect(4, 28, 100, 23, false);
	this->txtSuppVer = ui->NewTextBox(this->tpVersion, CSTR(""));
	this->txtSuppVer->SetRect(104, 28, 300, 23, false);
	this->txtSuppVer->SetReadOnly(true);

	this->tpSiteList = this->tcMain->AddTabPage(CSTR("Site List"));
	this->pnlSiteList = ui->NewPanel(this->tpSiteList);
	this->pnlSiteList->SetRect(0, 0, 100, 216, false);
	this->pnlSiteList->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lvSiteList = ui->NewListView(this->tpSiteList, UI::ListViewStyle::Table, 11);
	this->lvSiteList->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvSiteList->SetFullRowSelect(true);
	this->lvSiteList->SetShowGrid(true);
	this->lvSiteList->AddColumn(CSTR("Id"), 60);
	this->lvSiteList->AddColumn(CSTR("name"), 100);
	this->lvSiteList->AddColumn(CSTR("accountId"), 60);
	this->lvSiteList->AddColumn(CSTR("status"), 60);
	this->lvSiteList->AddColumn(CSTR("peakPower"), 100);
	this->lvSiteList->AddColumn(CSTR("lastUpdateTime"), 150);
	this->lvSiteList->AddColumn(CSTR("currency"), 60);
	this->lvSiteList->AddColumn(CSTR("installationDate"), 150);
	this->lvSiteList->AddColumn(CSTR("ptoDate"), 150);
	this->lvSiteList->AddColumn(CSTR("notes"), 150);
	this->lvSiteList->AddColumn(CSTR("type"), 100);
	this->lvSiteList->HandleSelChg(OnSiteListSelChg, this);
	this->lblSiteCountry = ui->NewLabel(this->pnlSiteList, CSTR("Country"));
	this->lblSiteCountry->SetRect(0, 0, 100, 23, false);
	this->txtSiteCountry = ui->NewTextBox(this->pnlSiteList, CSTR(""));
	this->txtSiteCountry->SetRect(100, 0, 200, 23, false);
	this->txtSiteCountry->SetReadOnly(true);
	this->lblSiteCity = ui->NewLabel(this->pnlSiteList, CSTR("City"));
	this->lblSiteCity->SetRect(0, 24, 100, 23, false);
	this->txtSiteCity = ui->NewTextBox(this->pnlSiteList, CSTR(""));
	this->txtSiteCity->SetRect(100, 24, 200, 23, false);
	this->txtSiteCity->SetReadOnly(true);
	this->lblSiteAddress = ui->NewLabel(this->pnlSiteList, CSTR("Address"));
	this->lblSiteAddress->SetRect(0, 48, 100, 23, false);
	this->txtSiteAddress = ui->NewTextBox(this->pnlSiteList, CSTR(""));
	this->txtSiteAddress->SetRect(100, 48, 200, 23, false);
	this->txtSiteAddress->SetReadOnly(true);
	this->lblSiteAddress2 = ui->NewLabel(this->pnlSiteList, CSTR("Address2"));
	this->lblSiteAddress2->SetRect(0, 72, 100, 23, false);
	this->txtSiteAddress2 = ui->NewTextBox(this->pnlSiteList, CSTR(""));
	this->txtSiteAddress2->SetRect(100, 72, 200, 23, false);
	this->txtSiteAddress2->SetReadOnly(true);
	this->lblSiteZIP = ui->NewLabel(this->pnlSiteList, CSTR("ZIP"));
	this->lblSiteZIP->SetRect(0, 96, 100, 23, false);
	this->txtSiteZIP = ui->NewTextBox(this->pnlSiteList, CSTR(""));
	this->txtSiteZIP->SetRect(100, 96, 200, 23, false);
	this->txtSiteZIP->SetReadOnly(true);
	this->lblSiteTimeZone = ui->NewLabel(this->pnlSiteList, CSTR("TimeZone"));
	this->lblSiteTimeZone->SetRect(0, 120, 100, 23, false);
	this->txtSiteTimeZone = ui->NewTextBox(this->pnlSiteList, CSTR(""));
	this->txtSiteTimeZone->SetRect(100, 120, 200, 23, false);
	this->txtSiteTimeZone->SetReadOnly(true);
	this->lblSiteCountryCode = ui->NewLabel(this->pnlSiteList, CSTR("CountryCode"));
	this->lblSiteCountryCode->SetRect(0, 144, 100, 23, false);
	this->txtSiteCountryCode = ui->NewTextBox(this->pnlSiteList, CSTR(""));
	this->txtSiteCountryCode->SetRect(100, 144, 200, 23, false);
	this->txtSiteCountryCode->SetReadOnly(true);
	this->lblSiteIsPublic = ui->NewLabel(this->pnlSiteList, CSTR("IsPublic"));
	this->lblSiteIsPublic->SetRect(0, 168, 100, 23, false);
	this->txtSiteIsPublic = ui->NewTextBox(this->pnlSiteList, CSTR(""));
	this->txtSiteIsPublic->SetRect(100, 168, 200, 23, false);
	this->txtSiteIsPublic->SetReadOnly(true);
	this->lblSitePublicName = ui->NewLabel(this->pnlSiteList, CSTR("Public Name"));
	this->lblSitePublicName->SetRect(0, 192, 100, 23, false);
	this->txtSitePublicName = ui->NewTextBox(this->pnlSiteList, CSTR(""));
	this->txtSitePublicName->SetRect(100, 192, 200, 23, false);
	this->txtSitePublicName->SetReadOnly(true);
	this->lblSiteLifetimeEnergy = ui->NewLabel(this->pnlSiteList, CSTR("Lifetime Energy"));
	this->lblSiteLifetimeEnergy->SetRect(350, 0, 100, 23, false);
	this->txtSiteLifetimeEnergy = ui->NewTextBox(this->pnlSiteList, CSTR(""));
	this->txtSiteLifetimeEnergy->SetRect(450, 0, 100, 23, false);
	this->txtSiteLifetimeEnergy->SetReadOnly(true);
	this->lblSiteLifetimeRevenue = ui->NewLabel(this->pnlSiteList, CSTR("Lifetime Revenue"));
	this->lblSiteLifetimeRevenue->SetRect(350, 24, 100, 23, false);
	this->txtSiteLifetimeRevenue = ui->NewTextBox(this->pnlSiteList, CSTR(""));
	this->txtSiteLifetimeRevenue->SetRect(450, 24, 100, 23, false);
	this->txtSiteLifetimeRevenue->SetReadOnly(true);
	this->lblSiteYearlyEnergy = ui->NewLabel(this->pnlSiteList, CSTR("Yearly Energy"));
	this->lblSiteYearlyEnergy->SetRect(350, 48, 100, 23, false);
	this->txtSiteYearlyEnergy = ui->NewTextBox(this->pnlSiteList, CSTR(""));
	this->txtSiteYearlyEnergy->SetRect(450, 48, 100, 23, false);
	this->txtSiteYearlyEnergy->SetReadOnly(true);
	this->lblSiteMonthlyEnergy = ui->NewLabel(this->pnlSiteList, CSTR("Monthly Energy"));
	this->lblSiteMonthlyEnergy->SetRect(350, 72, 100, 23, false);
	this->txtSiteMonthlyEnergy = ui->NewTextBox(this->pnlSiteList, CSTR(""));
	this->txtSiteMonthlyEnergy->SetRect(450, 72, 100, 23, false);
	this->txtSiteMonthlyEnergy->SetReadOnly(true);
	this->lblSiteDailyEnergy = ui->NewLabel(this->pnlSiteList, CSTR("Daily Energy"));
	this->lblSiteDailyEnergy->SetRect(350, 96, 100, 23, false);
	this->txtSiteDailyEnergy = ui->NewTextBox(this->pnlSiteList, CSTR(""));
	this->txtSiteDailyEnergy->SetRect(450, 96, 100, 23, false);
	this->txtSiteDailyEnergy->SetReadOnly(true);
	this->lblSiteCurrentPower = ui->NewLabel(this->pnlSiteList, CSTR("Current Power"));
	this->lblSiteCurrentPower->SetRect(350, 120, 100, 23, false);
	this->txtSiteCurrentPower = ui->NewTextBox(this->pnlSiteList, CSTR(""));
	this->txtSiteCurrentPower->SetRect(450, 120, 100, 23, false);
	this->txtSiteCurrentPower->SetReadOnly(true);

	this->tpSiteEnergy = this->tcMain->AddTabPage(CSTR("Site Energy"));
	this->pnlSiteEnergy = ui->NewPanel(this->tpSiteEnergy);
	this->pnlSiteEnergy->SetRect(0, 0, 100, 103, false);
	this->pnlSiteEnergy->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblSiteEnergySite = ui->NewLabel(this->pnlSiteEnergy, CSTR("Site"));
	this->lblSiteEnergySite->SetRect(4, 4, 100, 23, false);
	this->cboSiteEnergySite = ui->NewComboBox(this->pnlSiteEnergy, false);
	this->cboSiteEnergySite->SetRect(104, 4, 150, 23, false);
	this->lblSiteEnergyInterval = ui->NewLabel(this->pnlSiteEnergy, CSTR("Time Interval"));
	this->lblSiteEnergyInterval->SetRect(4, 28, 100, 23, false);
	this->cboSiteEnergyInterval = ui->NewComboBox(this->pnlSiteEnergy, false);
	this->cboSiteEnergyInterval->SetRect(104, 28, 150, 23, false);
	CBOADDENUM(this->cboSiteEnergyInterval, Net::SolarEdgeAPI::TimeUnit, DAY);
	CBOADDENUM(this->cboSiteEnergyInterval, Net::SolarEdgeAPI::TimeUnit, QUARTER_OF_AN_HOUR);
	CBOADDENUM(this->cboSiteEnergyInterval, Net::SolarEdgeAPI::TimeUnit, HOUR);
	this->cboSiteEnergyInterval->SetSelectedIndex(0);
	this->lblSiteEnergyRange = ui->NewLabel(this->pnlSiteEnergy, CSTR("Time Range"));
	this->lblSiteEnergyRange->SetRect(4, 52, 100, 23, false);
	this->cboSiteEnergyYear = ui->NewComboBox(this->pnlSiteEnergy, false);
	this->cboSiteEnergyYear->SetRect(104, 52, 100, 23, false);
	UI::GUIComboBoxUtil::AddYearItems(this->cboSiteEnergyYear, 5);
	this->cboSiteEnergyMonth = ui->NewComboBox(this->pnlSiteEnergy, false);
	this->cboSiteEnergyMonth->SetRect(204, 52, 60, 23, false);
	UI::GUIComboBoxUtil::AddMonthItems(this->cboSiteEnergyMonth);
	this->cboSiteEnergyDay = ui->NewComboBox(this->pnlSiteEnergy, false);
	this->cboSiteEnergyDay->SetRect(264, 52, 60, 23, false);
	UI::GUIComboBoxUtil::AddDayItems(this->cboSiteEnergyDay);
	this->btnSiteEnergy = ui->NewButton(this->pnlSiteEnergy, CSTR("Query"));
	this->btnSiteEnergy->SetRect(104, 76, 75, 23, false);
	this->btnSiteEnergy->HandleButtonClick(OnSiteEnergyClicked, this);
	this->pbSiteEnergy = ui->NewPictureBox(this->tpSiteEnergy, this->core->GetDrawEngine(), true, false);
	this->pbSiteEnergy->SetDockType(UI::GUIControl::DOCK_FILL);
	this->pbSiteEnergy->HandleSizeChanged(OnSiteEnergySizeChg, this);

	this->tpSitePower = this->tcMain->AddTabPage(CSTR("Site Power"));
	this->pnlSitePower = ui->NewPanel(this->tpSitePower);
	this->pnlSitePower->SetRect(0, 0, 100, 79, false);
	this->pnlSitePower->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblSitePowerSite = ui->NewLabel(this->pnlSitePower, CSTR("Site"));
	this->lblSitePowerSite->SetRect(4, 4, 100, 23, false);
	this->cboSitePowerSite = ui->NewComboBox(this->pnlSitePower, false);
	this->cboSitePowerSite->SetRect(104, 4, 150, 23, false);
	this->lblSitePowerRange = ui->NewLabel(this->pnlSitePower, CSTR("Time Range"));
	this->lblSitePowerRange->SetRect(4, 28, 100, 23, false);
	this->cboSitePowerYear = ui->NewComboBox(this->pnlSitePower, false);
	this->cboSitePowerYear->SetRect(104, 28, 100, 23, false);
	UI::GUIComboBoxUtil::AddYearItems(this->cboSitePowerYear, 5);
	this->cboSitePowerMonth = ui->NewComboBox(this->pnlSitePower, false);
	this->cboSitePowerMonth->SetRect(204, 28, 60, 23, false);
	UI::GUIComboBoxUtil::AddMonthItems(this->cboSitePowerMonth);
	this->cboSitePowerDay = ui->NewComboBox(this->pnlSitePower, false);
	this->cboSitePowerDay->SetRect(264, 28, 60, 23, false);
	UI::GUIComboBoxUtil::AddDayItems(this->cboSitePowerDay);
	this->btnSitePower = ui->NewButton(this->pnlSitePower, CSTR("Query"));
	this->btnSitePower->SetRect(104, 52, 75, 23, false);
	this->btnSitePower->HandleButtonClick(OnSitePowerClicked, this);
	this->pbSitePower = ui->NewPictureBox(this->tpSitePower, this->core->GetDrawEngine(), true, false);
	this->pbSitePower->SetDockType(UI::GUIControl::DOCK_FILL);
	this->pbSitePower->HandleSizeChanged(OnSitePowerSizeChg, this);
}

SSWR::AVIRead::AVIRSolarEdgeForm::~AVIRSolarEdgeForm()
{
	if (this->seAPI)
	{
		this->seAPI->FreeSiteList(&this->siteList);
		DEL_CLASS(this->seAPI);
		this->seAPI = 0;
	}
	this->ClearChildren();
	this->ssl.Delete();
	SDEL_CLASS(this->imgSiteEnergy);
	SDEL_CLASS(this->imgSitePower);
}

void SSWR::AVIRead::AVIRSolarEdgeForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
