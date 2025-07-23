#include "Stdafx.h"
#include "Data/ChartPlotter.h"
#include "DB/CSVFile.h"
#include "IO/FileStream.h"
#include "IO/StreamReader.h"
#include "Manage/HiResClock.h"
#include "Math/Math.h"
#include "Net/HTTPClient.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRCOVID19Form.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/GUIFileDialog.h"

void __stdcall SSWR::AVIRead::AVIRCOVID19Form::OnFileClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRCOVID19Form> me = userObj.GetNN<SSWR::AVIRead::AVIRCOVID19Form>();
	NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"COVID19", false);
	dlg->AddFilter(CSTR("*.csv"), CSTR("CSV File"));
	dlg->SetAllowMultiSel(false);
	if (dlg->ShowDialog(me->GetHandle()))
	{
		IO::FileStream fs(dlg->GetFileName(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		me->LoadCSV(fs);
	}
	dlg.Delete();
}

void __stdcall SSWR::AVIRead::AVIRCOVID19Form::OnDownloadClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRCOVID19Form> me = userObj.GetNN<SSWR::AVIRead::AVIRCOVID19Form>();
	UInt8 buff[2048];
	UOSInt i;
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(me->clif, me->ssl, CSTR("https://covid.ourworldindata.org/data/owid-covid-data.csv"), Net::WebUtil::RequestMethod::HTTP_GET, true);
	IO::MemoryStream mstm(1024);
	while (true)
	{
		i = cli->Read(BYTEARR(buff));
		if (i <= 0)
		{
			break;
		}
		mstm.Write(Data::ByteArrayR(buff, i));
	}
	cli.Delete();
	if (mstm.GetLength() > 100)
	{
		me->LoadCSV(mstm);
	}
}

void __stdcall SSWR::AVIRead::AVIRCOVID19Form::OnCountrySelChg(AnyType userObj)
{
	OnNewCasesSizeChanged(userObj);
}

void __stdcall SSWR::AVIRead::AVIRCOVID19Form::OnNewCasesSizeChanged(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRCOVID19Form> me = userObj.GetNN<SSWR::AVIRead::AVIRCOVID19Form>();
	NN<SSWR::AVIRead::AVIRCOVID19Form::CountryInfo> country;
	if (!me->lvCountry->GetSelectedItem().GetOpt<SSWR::AVIRead::AVIRCOVID19Form::CountryInfo>().SetTo(country))
		return;
	NN<Media::DrawEngine> deng = me->core->GetDrawEngine();
	Math::Size2D<UOSInt> sz = me->pbNewCases->GetSizeP();
	NN<Media::DrawImage> dimg;
	if (deng->CreateImage32(sz, Media::AT_ALPHA_ALL_FF).SetTo(dimg))
	{
		NN<SSWR::AVIRead::AVIRCOVID19Form::DailyRecord> record;
		UOSInt i;
		UOSInt j;
		Int64 lastCount = 0;
		Int32 *counts;
		Int64 *dates;
		UTF8Char sbuff[256];
		UnsafeArray<UTF8Char> sptr;
		{
			sptr = country->name->ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("New Cases in ")));
			Data::ChartPlotter chart(CSTRP(sbuff, sptr));
			chart.SetFontHeightPt(9.0);
			chart.SetDateFormat(CSTR("yyyy-MM-dd"));
			i = 0;
			j = country->records.GetCount();
			counts = MemAlloc(Int32, j);
			dates = MemAlloc(Int64, j);
			while (i < j)
			{
				record = country->records.GetItemNoCheck(i);
				counts[i] = (Int32)(record->totalCases - lastCount);
				lastCount = record->totalCases;
				dates[i] = record->timeTicks;
				i++;
			}
			chart.AddXDataDate(dates, j);
			chart.AddYData(CSTR("New Cases"), counts, j, 0xffff0000, Data::ChartPlotter::LineStyle::Line);
			chart.Plot(dimg, 0, 0, UOSInt2Double(sz.x), UOSInt2Double(sz.y));
			MemFree(counts);
			MemFree(dates);
		}
		me->pbNewCases->SetImageDImg(dimg.Ptr());
		deng->DeleteImage(dimg);
	}
}

void SSWR::AVIRead::AVIRCOVID19Form::ClearRecords()
{
	NN<SSWR::AVIRead::AVIRCOVID19Form::CountryInfo> country;
	NN<const Data::ArrayListNN<SSWR::AVIRead::AVIRCOVID19Form::CountryInfo>> countryList = this->countries.GetValues();
	UOSInt i = countryList->GetCount();
	UOSInt j;
	while (i-- > 0)
	{
		country = countryList->GetItemNoCheck(i);
		country->isoCode->Release();
		country->name->Release();
		j = country->records.GetCount();
		while (j-- > 0)
		{
			MemFreeNN(country->records.GetItemNoCheck(j));
		}
		country.Delete();
	}
	this->countries.Clear();
	this->lvCountry->ClearItems();
}

Bool SSWR::AVIRead::AVIRCOVID19Form::LoadCSV(NN<IO::SeekableStream> stm)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UOSInt colIsoCode = (UOSInt)-1;
	UOSInt colLocation = (UOSInt)-1;
	UOSInt colDate = (UOSInt)-1;
	UOSInt colTotalCases = (UOSInt)-1;
	UOSInt colTotalDeath = (UOSInt)-1;
	UOSInt colPopulation = (UOSInt)-1;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	NN<SSWR::AVIRead::AVIRCOVID19Form::CountryInfo> country;
	NN<SSWR::AVIRead::AVIRCOVID19Form::DailyRecord> record;
	this->ClearRecords();
	{
		DB::CSVFile csv(stm, 65001);
		NN<DB::DBReader> r;
		if (!csv.QueryTableData(CSTR_NULL, CSTR(""), 0, 0, 0, CSTR_NULL, 0).SetTo(r))
		{
			return false;
		}
		i = r->ColCount();
		while (i-- > 0)
		{
			if (r->GetName(i, sbuff).SetTo(sptr))
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

		Int64 t;
		while (r->ReadNext())
		{
			sptr = r->GetStr(colIsoCode, sbuff, sizeof(sbuff)).Or(sbuff);
			if (!this->countries.GetC(CSTRP(sbuff, sptr)).SetTo(country))
			{
				NEW_CLASSNN(country, SSWR::AVIRead::AVIRCOVID19Form::CountryInfo());
				country->isoCode = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
				sptr = r->GetStr(colLocation, sbuff, sizeof(sbuff)).Or(sbuff);
				country->name = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
				r->GetStr(colPopulation, sbuff, sizeof(sbuff));
				country->population = Text::StrToDoubleOrNAN(sbuff);
				this->countries.PutNN(country->isoCode, country);
			}
			t = r->GetTimestamp(colDate).ToTicks();
			if (country->records.Get(t).IsNull())
			{
				record = MemAllocNN(SSWR::AVIRead::AVIRCOVID19Form::DailyRecord);
				record->timeTicks = t;
				r->GetStr(colTotalCases, sbuff, sizeof(sbuff));
				record->totalCases = Text::StrToInt64(sbuff);
				r->GetStr(colTotalDeath, sbuff, sizeof(sbuff));
				record->totalDeaths = Text::StrToInt64(sbuff);
				country->records.Put(record->timeTicks, record);
			}
		}
		csv.CloseReader(r);
	}
	NN<const Data::ArrayListNN<SSWR::AVIRead::AVIRCOVID19Form::CountryInfo>> countryList;
	countryList = this->countries.GetValues();
	i = 0;
	j = countryList->GetCount();
	while (i < j)
	{
		country = countryList->GetItemNoCheck(i);
		k = this->lvCountry->AddItem(country->isoCode->ToCString(), country);
		this->lvCountry->SetSubItem(k, 1, country->name);
		record = country->records.GetItemNoCheck(country->records.GetCount() - 1);
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

SSWR::AVIRead::AVIRCOVID19Form::AVIRCOVID19Form(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("COVID-19"));

	this->core = core;
	this->clif = core->GetTCPClientFactory();
	this->ssl = Net::SSLEngineFactory::Create(this->clif, true);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlRequest = ui->NewPanel(*this);
	this->pnlRequest->SetRect(0, 0, 100, 31, false);
	this->pnlRequest->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnFile = ui->NewButton(this->pnlRequest, CSTR("&Load File"));
	this->btnFile->SetRect(4, 4, 75, 23, false);
	this->btnFile->HandleButtonClick(OnFileClicked, this);
	this->btnDownload = ui->NewButton(this->pnlRequest, CSTR("&Load Internet"));
	this->btnDownload->SetRect(84, 4, 75, 23, false);
	this->btnDownload->HandleButtonClick(OnDownloadClicked, this);
	this->pbNewCases = ui->NewPictureBoxSimple(*this, this->core->GetDrawEngine(), false);
	this->pbNewCases->SetRect(0, 0, 100, 150, false);
	this->pbNewCases->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->pbNewCases->HandleSizeChanged(OnNewCasesSizeChanged, this);
	this->vspNewCases = ui->NewVSplitter(*this, 3, true);
	this->lvCountry = ui->NewListView(*this, UI::ListViewStyle::Table, 5);
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
	this->ssl.Delete();
}

void SSWR::AVIRead::AVIRCOVID19Form::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
