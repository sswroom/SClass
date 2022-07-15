#include "Stdafx.h"
#include "Data/LineChart.h"
#include "DB/CSVFile.h"
#include "IO/FileStream.h"
#include "IO/StreamReader.h"
#include "Manage/HiResClock.h"
#include "Math/Math.h"
#include "Net/HTTPClient.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRCOVID19Form.h"
#include "Sync/Thread.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/FileDialog.h"

void __stdcall SSWR::AVIRead::AVIRCOVID19Form::OnFileClicked(void *userObj)
{
	SSWR::AVIRead::AVIRCOVID19Form *me = (SSWR::AVIRead::AVIRCOVID19Form*)userObj;
	UI::FileDialog dlg(L"SSWR", L"AVIRead", L"COVID19", false);
	dlg.AddFilter(CSTR("*.csv"), CSTR("CSV File"));
	dlg.SetAllowMultiSel(false);
	if (dlg.ShowDialog(me->GetHandle()))
	{
		IO::FileStream fs(dlg.GetFileName(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		me->LoadCSV(&fs);
	}
}

void __stdcall SSWR::AVIRead::AVIRCOVID19Form::OnDownloadClicked(void *userObj)
{
	SSWR::AVIRead::AVIRCOVID19Form *me = (SSWR::AVIRead::AVIRCOVID19Form*)userObj;
	UInt8 buff[2048];
	UOSInt i;
	Net::HTTPClient *cli = Net::HTTPClient::CreateConnect(me->sockf, me->ssl, CSTR("https://covid.ourworldindata.org/data/owid-covid-data.csv"), Net::WebUtil::RequestMethod::HTTP_GET, true);
	IO::MemoryStream mstm(1024, UTF8STRC("SSWR.AVIRead.AVIRCOVID19Form.OnDownloadClicked.mstm"));
	while (true)
	{
		i = cli->Read(buff, 2048);
		if (i <= 0)
		{
			break;
		}
		mstm.Write(buff, i);
	}
	DEL_CLASS(cli);
	if (mstm.GetLength() > 100)
	{
		me->LoadCSV(&mstm);
	}
}

void __stdcall SSWR::AVIRead::AVIRCOVID19Form::OnCountrySelChg(void *userObj)
{
	OnNewCasesSizeChanged(userObj);
}

void __stdcall SSWR::AVIRead::AVIRCOVID19Form::OnNewCasesSizeChanged(void *userObj)
{
	SSWR::AVIRead::AVIRCOVID19Form *me = (SSWR::AVIRead::AVIRCOVID19Form*)userObj;
	SSWR::AVIRead::AVIRCOVID19Form::CountryInfo *country = (SSWR::AVIRead::AVIRCOVID19Form::CountryInfo*)me->lvCountry->GetSelectedItem();
	if (country == 0)
		return;
	Media::DrawEngine *deng = me->core->GetDrawEngine();
	Math::Size2D<UOSInt> sz = me->pbNewCases->GetSizeP();
	Media::DrawImage *dimg = deng->CreateImage32(sz.width, sz.height, Media::AT_NO_ALPHA);
	const Data::ArrayList<SSWR::AVIRead::AVIRCOVID19Form::DailyRecord*> *recordList = country->records->GetValues();
	SSWR::AVIRead::AVIRCOVID19Form::DailyRecord *record;
	UOSInt i;
	UOSInt j;
	Int64 lastCount = 0;
	Int32 *counts;
	Int64 *dates;
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	{
		sptr = country->name->ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("New Cases in ")));
		Data::LineChart chart(CSTRP(sbuff, sptr));
		chart.SetFontHeightPt(9.0);
		chart.SetDateFormat(CSTR("yyyy-MM-dd"));
		i = 0;
		j = recordList->GetCount();
		counts = MemAlloc(Int32, j);
		dates = MemAlloc(Int64, j);
		while (i < j)
		{
			record = recordList->GetItem(i);
			counts[i] = (Int32)(record->totalCases - lastCount);
			lastCount = record->totalCases;
			dates[i] = record->timeTicks;
			i++;
		}
		chart.AddXDataDate(dates, j);
		chart.AddYData(CSTR("New Cases"), counts, j, 0xffff0000, Data::LineChart::LS_LINE);
		chart.Plot(dimg, 0, 0, UOSInt2Double(sz.width), UOSInt2Double(sz.height));
		MemFree(counts);
		MemFree(dates);
	}
	me->pbNewCases->SetImageDImg(dimg);
	deng->DeleteImage(dimg);
}

void SSWR::AVIRead::AVIRCOVID19Form::ClearRecords()
{
	SSWR::AVIRead::AVIRCOVID19Form::CountryInfo *country;
	const Data::ArrayList<SSWR::AVIRead::AVIRCOVID19Form::CountryInfo*> *countryList = this->countries.GetValues();
	const Data::ArrayList<SSWR::AVIRead::AVIRCOVID19Form::DailyRecord*> *recordList;
	UOSInt i = countryList->GetCount();
	UOSInt j;
	while (i-- > 0)
	{
		country = countryList->GetItem(i);
		country->isoCode->Release();
		country->name->Release();
		recordList = country->records->GetValues();
		j = recordList->GetCount();
		while (j-- > 0)
		{
			MemFree(recordList->GetItem(j));
		}
		DEL_CLASS(country->records);
		MemFree(country);
	}
	this->countries.Clear();
	this->lvCountry->ClearItems();
}

Bool SSWR::AVIRead::AVIRCOVID19Form::LoadCSV(IO::SeekableStream *stm)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	UOSInt colIsoCode = (UOSInt)-1;
	UOSInt colLocation = (UOSInt)-1;
	UOSInt colDate = (UOSInt)-1;
	UOSInt colTotalCases = (UOSInt)-1;
	UOSInt colTotalDeath = (UOSInt)-1;
	UOSInt colPopulation = (UOSInt)-1;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	SSWR::AVIRead::AVIRCOVID19Form::CountryInfo *country;
	SSWR::AVIRead::AVIRCOVID19Form::DailyRecord *record;
	this->ClearRecords();
	{
		DB::CSVFile csv(stm, 65001);
		DB::DBReader *r = csv.QueryTableData(0, 0, 0, 0, CSTR_NULL, 0);
		if (r == 0)
		{
			return false;
		}
		i = r->ColCount();
		while (i-- > 0)
		{
			if ((sptr = r->GetName(i, sbuff)) != 0)
			{
				if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("iso_code")))
				{
					colIsoCode = i;
				}
				else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("location")))
				{
					colLocation = i;
				}
				else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("date")))
				{
					colDate = i;
				}
				else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("total_cases")))
				{
					colTotalCases = i;
				}
				else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("total_deaths")))
				{
					colTotalDeath = i;
				}
				else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("population")))
				{
					colPopulation = i;
				}
			}
		}

		if (colIsoCode == (UOSInt)-1 || colLocation == (UOSInt)-1 || colDate == (UOSInt)-1 || colTotalCases == (UOSInt)-1 || colTotalDeath == (UOSInt)-1 || colPopulation == (UOSInt)-1)
		{
			csv.CloseReader(r);
			return false;
		}

		Data::DateTime dt;
		Int64 t;
		while (r->ReadNext())
		{
			sptr = r->GetStr(colIsoCode, sbuff, sizeof(sbuff));
			country = this->countries.Get(CSTRP(sbuff, sptr));
			if (country == 0)
			{
				country = MemAlloc(SSWR::AVIRead::AVIRCOVID19Form::CountryInfo, 1);
				country->isoCode = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
				sptr = r->GetStr(colLocation, sbuff, sizeof(sbuff));
				country->name = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
				r->GetStr(colPopulation, sbuff, sizeof(sbuff));
				country->population = Text::StrToDouble(sbuff);
				NEW_CLASS(country->records, Data::Int64Map<SSWR::AVIRead::AVIRCOVID19Form::DailyRecord *>());
				this->countries.Put(country->isoCode, country);
			}
			r->GetDate(colDate, &dt);
			t = dt.ToTicks();
			if (country->records->Get(t) == 0)
			{
				record = MemAlloc(SSWR::AVIRead::AVIRCOVID19Form::DailyRecord, 1);
				record->timeTicks = t;
				r->GetStr(colTotalCases, sbuff, sizeof(sbuff));
				record->totalCases = Text::StrToInt64(sbuff);
				r->GetStr(colTotalDeath, sbuff, sizeof(sbuff));
				record->totalDeaths = Text::StrToInt64(sbuff);
				country->records->Put(record->timeTicks, record);
			}
		}
		csv.CloseReader(r);
	}
	const Data::ArrayList<SSWR::AVIRead::AVIRCOVID19Form::CountryInfo *> *countryList;
	countryList = this->countries.GetValues();
	i = 0;
	j = countryList->GetCount();
	while (i < j)
	{
		country = countryList->GetItem(i);
		k = this->lvCountry->AddItem(country->isoCode->ToCString(), country);
		this->lvCountry->SetSubItem(k, 1, country->name);
		record = country->records->GetValues()->GetItem(country->records->GetCount() - 1);
		sptr = Text::StrInt64(sbuff, record->totalCases);
		this->lvCountry->SetSubItem(k, 2, CSTRP(sbuff, sptr));
		sptr = Text::StrInt64(sbuff, record->totalDeaths);
		this->lvCountry->SetSubItem(k, 3, CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, country->population / (Double)record->totalCases);
		this->lvCountry->SetSubItem(k, 4, CSTRP(sbuff, sptr));

		i++;
	}
	return true;
}

SSWR::AVIRead::AVIRCOVID19Form::AVIRCOVID19Form(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("COVID-19"));

	this->core = core;
	this->sockf = core->GetSocketFactory();
	this->ssl = Net::SSLEngineFactory::Create(this->sockf, true);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlRequest, UI::GUIPanel(ui, this));
	this->pnlRequest->SetRect(0, 0, 100, 31, false);
	this->pnlRequest->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->btnFile, UI::GUIButton(ui, this->pnlRequest, CSTR("&Load File")));
	this->btnFile->SetRect(4, 4, 75, 23, false);
	this->btnFile->HandleButtonClick(OnFileClicked, this);
	NEW_CLASS(this->btnDownload, UI::GUIButton(ui, this->pnlRequest, CSTR("&Load Internet")));
	this->btnDownload->SetRect(84, 4, 75, 23, false);
	this->btnDownload->HandleButtonClick(OnDownloadClicked, this);
	NEW_CLASS(this->pbNewCases, UI::GUIPictureBoxSimple(ui, this, this->core->GetDrawEngine(), false));
	this->pbNewCases->SetRect(0, 0, 100, 150, false);
	this->pbNewCases->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->pbNewCases->HandleSizeChanged(OnNewCasesSizeChanged, this);
	NEW_CLASS(this->vspNewCases, UI::GUIVSplitter(ui, this, 3, true));
	NEW_CLASS(this->lvCountry, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 5));
	this->lvCountry->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvCountry->SetShowGrid(true);
	this->lvCountry->SetFullRowSelect(true);
	this->lvCountry->HandleSelChg(OnCountrySelChg, this);
	this->lvCountry->AddColumn(CSTR("Code"), 50);
	this->lvCountry->AddColumn(CSTR("Name"), 120);
	this->lvCountry->AddColumn(CSTR("Total Cases"), 100);
	this->lvCountry->AddColumn(CSTR("Total Death"), 100);
	this->lvCountry->AddColumn(CSTR("Population per case"), 100);
}

SSWR::AVIRead::AVIRCOVID19Form::~AVIRCOVID19Form()
{
	this->ClearRecords();
	SDEL_CLASS(this->ssl);
}

void SSWR::AVIRead::AVIRCOVID19Form::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
