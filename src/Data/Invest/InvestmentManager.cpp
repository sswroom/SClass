#include "Stdafx.h"
#include "Data/Invest/InvestmentManager.h"
#include "DB/CSVFile.h"
#include "DB/DBReader.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"

#define SETTINGFILE CSTR("Setting.txt")
#define ASSETSFILE CSTR("Assets.csv")
#define TRADEFILE CSTR("Trade.csv")

NN<Data::Invest::Currency> Data::Invest::InvestmentManager::LoadCurrency(UInt32 c)
{
	NN<Currency> curr;
	if (this->currMap.Get(c).SetTo(curr))
	{
		return curr;
	}
	NEW_CLASSNN(curr, Currency());
	curr->c = c;
	curr->current = 0;
	this->currMap.Put(c, curr);
	
	Text::StringBuilderUTF8 sb;
	sb.Append(this->path);
	sb.Append(CURRENCYSTR(c));
	sb.Append(CSTR(".csv"));
	DB::CSVFile csv(sb.ToCString(), 65001);
	NN<DB::DBReader> r;
	if (csv.QueryTableData(nullptr, CSTR(""), 0, 0, 0, nullptr, nullptr).SetTo(r))
	{
		Int64 ts;
		Double val;
		while (r->ReadNext())
		{
			ts = r->GetInt64(0);
			val = r->GetDblOrNAN(1);
			if (ts != 0 && !Math::IsNAN(val))
			{
				curr->tsList.Add(Data::Timestamp(ts, Data::DateTimeUtil::GetLocalTzQhr()));
				curr->valList.Add(val);
				curr->current = val;
			}
		}
		csv.CloseReader(r);
	}
	return curr;
}

Bool Data::Invest::InvestmentManager::LoadAsset(NN<Asset> ass)
{
	Text::StringBuilderUTF8 sb;
	sb.Append(this->path);
	sb.Append(CSTR("Asset_"));
	sb.AppendUOSInt(ass->index);
	sb.Append(CSTR(".csv"));
	DB::CSVFile csv(sb.ToCString(), 65001);
	NN<DB::DBReader> r;
	if (csv.QueryTableData(nullptr, CSTR(""), 0, 0, 0, nullptr, nullptr).SetTo(r))
	{
		Int64 ts;
		Double val;
		Double divVal;
		while (r->ReadNext())
		{
			ts = r->GetInt64(0);
			val = r->GetDblOrNAN(1);
			divVal = r->GetDblOrNAN(2);
			if (ts != 0 && !Math::IsNAN(val) && !Math::IsNAN(divVal))
			{
				ass->tsList.Add(Data::Timestamp(ts, Data::DateTimeUtil::GetLocalTzQhr()));
				ass->valList.Add(val);
				ass->divList.Add(divVal);
				ass->current = val;
			}
		}
		csv.CloseReader(r);
		return true;
	}
	return false;
}

Data::Timestamp Data::Invest::InvestmentManager::ParseTime(NN<Text::String> s, DateFormat fmt)
{
	Text::PString sarr[4];
	if (Text::StrSplitP(sarr, 4, s.Ptr()[0], '/') == 3 || Text::StrSplitP(sarr, 4, s.Ptr()[0], '-') == 3)
	{
		UInt32 year;
		UInt32 month;
		UInt32 day;
		if (fmt == DateFormat::DDMMYYYY)
		{
			if (sarr[0].ToUInt32(day) && sarr[1].ToUInt32(month) && sarr[2].ToUInt32(year))
			{
				return Data::Timestamp::FromDate(Data::Date((Int32)year, (UInt8)month, (UInt8)day), Data::DateTimeUtil::GetLocalTzQhr());
			}
		}
		else
		{
			if (sarr[0].ToUInt32(month) && sarr[1].ToUInt32(day) && sarr[2].ToUInt32(year))
			{
				return Data::Timestamp::FromDate(Data::Date((Int32)year, (UInt8)month, (UInt8)day), Data::DateTimeUtil::GetLocalTzQhr());
			}
		}
	}
	return nullptr;
}

void Data::Invest::InvestmentManager::AddTradeEntry(NN<TradeEntry> ent)
{
	OSInt i = 0;
	OSInt j = (OSInt)this->tradeList.GetCount() - 1;
	OSInt k; 
	NN<TradeEntry> e;
	while (i <= j)
	{
		k = (i + j) >> 1;
		e = this->tradeList.GetItemNoCheck((UOSInt)k);
		if (e->fromDetail.tranBeginDate > ent->fromDetail.tranBeginDate)
		{
			j = k - 1;
		}
		else if (e->fromDetail.tranBeginDate < ent->fromDetail.tranBeginDate)
		{
			i = k + 1;
		}
		else
		{
			i++;
			while (i <= j)
			{
				e = this->tradeList.GetItemNoCheck((UOSInt)i);
				if (e->fromDetail.tranBeginDate > ent->fromDetail.tranBeginDate)
				{
					break;
				}
				i++;
			}
			this->tradeList.Insert((UOSInt)i, ent);
			return;
		}
	}
	this->tradeList.Insert((UOSInt)i, ent);
	return;
}

Data::Invest::InvestmentManager::InvestmentManager(Text::CStringNN path)
{
	Text::StringBuilderUTF8 sb;
	sb.Append(path);
	if (!sb.EndsWith(IO::Path::PATH_SEPERATOR))
	{
		sb.AppendUTF8Char(IO::Path::PATH_SEPERATOR);
	}
	this->path = Text::String::New(sb.ToCString());
	this->localCurrency = 0;
	this->refCurrency = 0;
	this->inited = false;
	sb.Append(SETTINGFILE);
	IO::Path::PathType pt = IO::Path::GetPathType(sb.ToCString());
	if (pt == IO::Path::PathType::Unknown)
	{
		this->localCurrency = CURRENCY("HKD");
		this->refCurrency = CURRENCY("USD");
		if (!this->SaveSettings())
		{
			this->localCurrency = 0;
			this->refCurrency = 0;
		}
		else
		{
			this->LoadCurrency(this->localCurrency);
			this->LoadCurrency(this->refCurrency);
		}
	}
	else
	{
		IO::FileStream fs(sb.ToCString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		Text::UTF8Reader reader(fs);
		sb.ClearStr();
		if (reader.ReadLine(sb, 20) && sb.leng == 3)
		{
			this->localCurrency = CURRENCY(sb.v.Ptr());
			this->LoadCurrency(this->localCurrency);
		}
		sb.ClearStr();
		if (reader.ReadLine(sb, 20) && sb.leng == 3)
		{
			this->refCurrency = CURRENCY(sb.v.Ptr());
			this->LoadCurrency(this->refCurrency);
		}
	}

	sb.ClearStr();
	sb.Append(this->path);
	sb.Append(ASSETSFILE);
	{
		DB::CSVFile csv(sb.ToCString(), 65001);
		NN<DB::DBReader> r;
		if (csv.QueryTableData(nullptr, CSTR(""), 0, 0, 0, nullptr, nullptr).SetTo(r))
		{
			NN<Asset> ass;
			NN<Text::String> currency;
			NN<Text::String> shortName;
			NN<Text::String> fullName;
			while (r->ReadNext())
			{
				currency = r->GetNewStrNN(0);
				shortName = r->GetNewStrNN(1);
				fullName = r->GetNewStrNN(2);
				if (currency->leng == 3 && shortName->leng > 0 && fullName->leng > 0)
				{
					NEW_CLASSNN(ass, Asset());
					ass->index = this->assetList.GetCount();
					ass->currency = CURRENCY(currency->v.Ptr());
					ass->shortName = shortName;
					ass->fullName = fullName;
					currency->Release();
					ass->current = 0;
					this->assetList.Add(ass);
					LoadCurrency(ass->currency);
					this->LoadAsset(ass);
				}
				else
				{
					currency->Release();
					shortName->Release();
					fullName->Release();
				}
			}
			csv.CloseReader(r);
		}
	}

	sb.ClearStr();
	sb.Append(this->path);
	sb.Append(TRADEFILE);
	{
		DB::CSVFile csv(sb.ToCString(), 65001);
		NN<DB::DBReader> r;
		if (csv.QueryTableData(nullptr, CSTR(""), 0, 0, 0, nullptr, nullptr).SetTo(r))
		{
			TradeType type;
			while (r->ReadNext())
			{
				type = (TradeType)r->GetInt32(0);
				//	0   , 1      , 2    , 3       , 4           , 5        , 6     , 7         , 8      , 9
				//	Type,FromDate,ToDate,FromIndex,FromPriceDate,FromAmount,ToIndex,ToPriceDate,ToAmount,RefRate

				if (type == TradeType::ForeignExchange)
				{
					this->AddTransactionFX(Data::Timestamp(r->GetInt64(1), Data::DateTimeUtil::GetLocalTzQhr()), (UInt32)r->GetInt64(3), r->GetDblOrNAN(5), (UInt32)r->GetInt64(6), r->GetDblOrNAN(8), r->GetDblOrNAN(9));
				}
				else if (type == TradeType::FixedDeposit)
				{
					this->AddTransactionDeposit(Data::Timestamp(r->GetInt64(1), Data::DateTimeUtil::GetLocalTzQhr()), Data::Timestamp(r->GetInt64(2), Data::DateTimeUtil::GetLocalTzQhr()), (UInt32)r->GetInt64(3), -r->GetDblOrNAN(5), r->GetDblOrNAN(8));
				}
				else if (type == TradeType::CashToAsset)
				{
					Data::Timestamp startTime = Data::Timestamp(r->GetInt64(1), Data::DateTimeUtil::GetLocalTzQhr());
					Data::Timestamp endTime = Data::Timestamp(r->GetInt64(2), Data::DateTimeUtil::GetLocalTzQhr());
					Data::Timestamp priceTime = Data::Timestamp(r->GetInt64(7), Data::DateTimeUtil::GetLocalTzQhr());
					if (endTime.ToTicks() == 0)
					{
						endTime = 0;
					}
					if (priceTime.ToTicks() == 0)
					{
						priceTime = 0;
					}
					this->AddTransactionAsset(startTime, endTime, priceTime, (UOSInt)r->GetInt64(6), r->GetDblOrNAN(8), r->GetDblOrNAN(5));
				}
				else if (type == TradeType::AssetInterest)
				{
					Data::Timestamp startTime = Data::Timestamp(r->GetInt64(1), Data::DateTimeUtil::GetLocalTzQhr());
					Data::Timestamp endTime = Data::Timestamp(r->GetInt64(2), Data::DateTimeUtil::GetLocalTzQhr());
					if (endTime.ToTicks() == 0)
					{
						endTime = 0;
					}
					this->AddTransactionAInterest(startTime, endTime, (UOSInt)r->GetInt64(3), r->GetDblOrNAN(8));
				}
				else if (type == TradeType::AccountInterest)
				{
					Data::Timestamp startTime = Data::Timestamp(r->GetInt64(1), Data::DateTimeUtil::GetLocalTzQhr());
					this->AddTransactionCInterest(startTime, (UInt32)r->GetInt64(6), r->GetDblOrNAN(8));
				}
			}
			csv.CloseReader(r);
		}
	}
	this->inited = true;
}

Data::Invest::InvestmentManager::~InvestmentManager()
{
	this->path->Release();
	UOSInt i = this->currMap.GetCount();
	NN<Currency> curr;
	while (i-- > 0)
	{
		curr = this->currMap.GetItemNoCheck(i);
		curr.Delete();
	}
	this->currMap.Clear();
	NN<Asset> ass;
	i = this->assetList.GetCount();
	while (i-- > 0)
	{
		ass = this->assetList.GetItemNoCheck(i);
		ass->shortName->Release();
		ass->fullName->Release();
		ass.Delete();
	}
	this->assetList.Clear();
	this->tradeList.DeleteAll();
}

Bool Data::Invest::InvestmentManager::IsError() const
{
	return this->localCurrency == 0 || this->refCurrency == 0;
}

Bool Data::Invest::InvestmentManager::SaveSettings() const
{
	Text::StringBuilderUTF8 sb;
	sb.Append(this->path);
	sb.Append(SETTINGFILE);
	IO::FileStream fs(sb.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.IsError())
	{
		return false;
	}
	Bool succ = true;
	Text::UTF8Writer writer(fs);
	succ = succ && writer.WriteLine(CURRENCYSTR(this->localCurrency));
	succ = succ && writer.WriteLine(CURRENCYSTR(this->refCurrency));
	return succ;
}

Bool Data::Invest::InvestmentManager::SaveAssets() const
{
	Text::StringBuilderUTF8 sb;
	sb.Append(this->path);
	sb.Append(ASSETSFILE);
	IO::FileStream fs(sb.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.IsError())
		return false;
	sb.ClearStr();
	sb.Append(CSTR("Currency,ShortName,FullName\r\n"));
	NN<Asset> ass;
	NN<Text::String> s;
	UOSInt i = 0;
	UOSInt j = this->assetList.GetCount();;
	while (i < j)
	{
		ass = this->assetList.GetItemNoCheck(i);
		s = Text::String::NewCSVRec((const UInt8*)&ass->currency);
		sb.Append(s);
		sb.AppendUTF8Char(',');
		s->Release();
		s = Text::String::NewCSVRec(ass->shortName->v);
		sb.Append(s);
		sb.AppendUTF8Char(',');
		s->Release();
		s = Text::String::NewCSVRec(ass->fullName->v);
		sb.Append(s);
		s->Release();
		sb.Append(CSTR("\r\n"));
		if (sb.leng >= 4096)
		{
			fs.Write(sb.ToByteArray());
			sb.ClearStr();
		}
		i++;
	}
	if (sb.leng > 0)
	{
		fs.Write(sb.ToByteArray());
	}
	return true;
}

Bool Data::Invest::InvestmentManager::SaveCurrency(NN<Currency> curr) const
{
	Text::StringBuilderUTF8 sb;
	sb.Append(this->path);
	sb.Append(CURRENCYSTR(curr->c));
	sb.Append(CSTR(".csv"));
	IO::FileStream fs(sb.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.IsError())
		return false;
	sb.ClearStr();
	sb.Append(CSTR("Time,Value\r\n"));
	UOSInt i = 0;
	UOSInt j = curr->tsList.GetCount();
	while (i < j)
	{
		sb.AppendI64(curr->tsList.GetItem(i).ToTicks());
		sb.AppendUTF8Char(',');
		sb.AppendDouble(curr->valList.GetItem(i));
		sb.Append(CSTR("\r\n"));
		if (sb.leng >= 4096)
		{
			fs.Write(sb.ToByteArray());
			sb.ClearStr();
		}
		i++;
	}
	if (sb.leng > 0)
	{
		fs.Write(sb.ToByteArray());
	}
	return true;
}

Bool Data::Invest::InvestmentManager::SaveAsset(NN<Asset> ass) const
{
	Text::StringBuilderUTF8 sb;
	sb.Append(this->path);
	sb.Append(CSTR("Asset_"));
	sb.AppendUOSInt(ass->index);
	sb.Append(CSTR(".csv"));
	IO::FileStream fs(sb.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.IsError())
		return false;
	sb.ClearStr();
	sb.Append(CSTR("Time,Value,Div\r\n"));
	UOSInt i = 0;
	UOSInt j = ass->tsList.GetCount();
	while (i < j)
	{
		sb.AppendI64(ass->tsList.GetItem(i).ToTicks());
		sb.AppendUTF8Char(',');
		sb.AppendDouble(ass->valList.GetItem(i));
		sb.AppendUTF8Char(',');
		sb.AppendDouble(ass->divList.GetItem(i));
		sb.Append(CSTR("\r\n"));
		if (sb.leng >= 4096)
		{
			fs.Write(sb.ToByteArray());
			sb.ClearStr();
		}
		i++;
	}
	if (sb.leng > 0)
	{
		fs.Write(sb.ToByteArray());
	}
	return true;
}

Bool Data::Invest::InvestmentManager::SaveTransactions() const
{
	if (!inited)
	{
		return false;
	}
	Text::StringBuilderUTF8 sb;
	sb.Append(this->path);
	sb.Append(TRADEFILE);
	IO::FileStream fs(sb.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.IsError())
		return false;
	sb.ClearStr();
	sb.Append(CSTR("Type,FromDate,ToDate,FromIndex,FromPriceDate,FromAmount,ToIndex,ToPriceDate,ToAmount,RefRate\r\n"));
	NN<TradeEntry> ent;
	UOSInt i = 0;
	UOSInt j = this->tradeList.GetCount();;
	while (i < j)
	{
		ent = this->tradeList.GetItemNoCheck(i);
		sb.AppendUOSInt((UOSInt)ent->type);
		sb.AppendUTF8Char(',');
		sb.AppendI64(ent->fromDetail.tranBeginDate.ToTicks());
		sb.AppendUTF8Char(',');
		sb.AppendI64(ent->toDetail.tranEndDate.ToTicks());
		sb.AppendUTF8Char(',');
		sb.AppendUOSInt(ent->fromIndex);
		sb.AppendUTF8Char(',');
		sb.AppendI64(ent->fromDetail.priceDate.ToTicks());
		sb.AppendUTF8Char(',');
		sb.AppendDouble(ent->fromDetail.amount);
		sb.AppendUTF8Char(',');
		sb.AppendUOSInt(ent->toIndex);
		sb.AppendUTF8Char(',');
		sb.AppendI64(ent->toDetail.priceDate.ToTicks());
		sb.AppendUTF8Char(',');
		sb.AppendDouble(ent->toDetail.amount);
		sb.AppendUTF8Char(',');
		sb.AppendDouble(ent->refRate);
		sb.Append(CSTR("\r\n"));
		if (sb.leng >= 4096)
		{
			fs.Write(sb.ToByteArray());
			sb.ClearStr();
		}
		i++;
	}
	if (sb.leng > 0)
	{
		fs.Write(sb.ToByteArray());
	}
	return true;
}

Bool Data::Invest::InvestmentManager::CurrencyImport(NN<Currency> curr, NN<DB::ReadingDB> db, UOSInt timeCol, UOSInt valueCol, DateFormat fmt) const
{
	NN<DB::DBReader> r;
	if (!db->QueryTableData(nullptr, CSTR(""), 0, 0, 0, nullptr, nullptr).SetTo(r))
		return false;
	UOSInt colCnt = r->ColCount();
	if (timeCol >= colCnt || valueCol >= colCnt)
	{
		db->CloseReader(r);
		return false;
	}
	Bool found = false;
	NN<Text::String> tsStr;
	NN<Text::String> valStr;
	Data::Timestamp ts;
	Double value;
	while (r->ReadNext())
	{
		tsStr = r->GetNewStrNN(timeCol);
		valStr = r->GetNewStrNN(valueCol);
		tsStr->Trim();
		valStr->Trim();
		ts = ParseTime(tsStr, fmt);
		value = valStr->ToDoubleOrNAN();
		if (!ts.IsNull() && !Math::IsNAN(value))
		{
			OSInt index = curr->tsList.SortedIndexOf(ts);
			UOSInt i;
			if (index >= 0)
			{
				curr->valList.SetItem((UOSInt)index, value);
				if ((UOSInt)index == curr->valList.GetCount() - 1)
				{
					curr->current = value;
				}
			}
			else
			{
				curr->valList.Insert(i = curr->tsList.SortedInsert(ts), value);
				if (i == curr->valList.GetCount() - 1)
				{
					curr->current = value;
				}
			}
			found = true;
		}
		tsStr->Release();
		valStr->Release();
	}
	db->CloseReader(r);
	if (found)
	{
		this->SaveCurrency(curr);
	}
	return found;
}

Bool Data::Invest::InvestmentManager::UpdateCurrency(NN<Currency> curr, Data::Timestamp ts, Double value)
{
	OSInt i = curr->tsList.SortedIndexOf(ts);
	if (i >= 0)
	{
		curr->valList.SetItem((UOSInt)i, value);
		if ((UOSInt)i == curr->valList.GetCount() - 1)
		{
			curr->current = value;
		}
	}
	else
	{
		UOSInt ui = curr->tsList.SortedInsert(ts);
		curr->valList.Insert(ui, value);
		if (ui == curr->valList.GetCount() - 1)
		{
			curr->current = value;
		}
	}
	this->SaveCurrency(curr);
	return true;
}

void Data::Invest::InvestmentManager::CurrencyCalcValues(NN<Currency> curr, Data::Date startDate, Data::Date endDate, NN<Data::ArrayListTS> dateList, NN<Data::ArrayList<Double>> valueList)
{
	Data::DateTimeUtil::Weekday wd = startDate.GetWeekday();
	if (wd == Data::DateTimeUtil::Weekday::Saturday)
	{
		startDate = startDate.AddDay(2);
	}
	else if (wd == Data::DateTimeUtil::Weekday::Sunday)
	{
		startDate = startDate.AddDay(1);
	}
	Data::Timestamp startTS = Data::Timestamp::FromDate(startDate, Data::DateTimeUtil::GetLocalTzQhr());
	Data::Timestamp endTS = Data::Timestamp::FromDate(endDate, Data::DateTimeUtil::GetLocalTzQhr());
	Data::ArrayListNN<TradeEntry> depositList;
	NN<TradeEntry> ent;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	i = 0;
	j = this->tradeList.GetCount();
	while (i < j)
	{
		ent = this->tradeList.GetItemNoCheck(i);
		if (ent->type == TradeType::FixedDeposit && ent->fromIndex == curr->c && ent->fromDetail.tranBeginDate < endTS && ent->toDetail.tranBeginDate > startTS)
		{
			depositList.Add(ent);
		}
		i++;
	}
	Double totalValue = 0;
	Double depositValue;
	NN<TradeDetail> t;
	i = 0;
	j = curr->trades.GetCount();
	while (i < j)
	{
		t = curr->trades.GetItemNoCheck(i);
		if (t->tranBeginDate > startTS)
		{
			break;
		}
		totalValue += t->amount;
		i++;
	}
	while (startDate < endDate)
	{
		startTS = Data::Timestamp::FromDate(startDate, Data::DateTimeUtil::GetLocalTzQhr());
		while (i < j)
		{
			t = curr->trades.GetItemNoCheck(i);
			if (t->tranBeginDate > startTS)
			{
				break;
			}
			totalValue += t->amount;
			i++;
		}
		depositValue = 0;
		k = depositList.GetCount();
		while (k-- > 0)
		{
			ent = depositList.GetItemNoCheck(k);
			if (ent->fromDetail.tranBeginDate <= startTS && ent->toDetail.tranBeginDate > startTS)
			{
				depositValue += -ent->fromDetail.amount + (ent->fromDetail.amount + ent->toDetail.amount) * startTS.Diff(ent->fromDetail.tranBeginDate).GetTotalDays() / ent->toDetail.tranBeginDate.Diff(ent->fromDetail.tranBeginDate).GetTotalDays();
			}
		}
		dateList->Add(startTS);
		valueList->Add(totalValue + depositValue);
		startDate = startDate.AddDay(1);
		wd = startDate.GetWeekday();
		if (wd == Data::DateTimeUtil::Weekday::Saturday)
		{
			startDate = startDate.AddDay(2);
		}
		else if (wd == Data::DateTimeUtil::Weekday::Sunday)
		{
			startDate = startDate.AddDay(1);
		}
	}
}

Double Data::Invest::InvestmentManager::CurrencyGetRate(NN<Currency> curr, Data::Timestamp ts)
{
	OSInt i = curr->tsList.SortedIndexOf(ts);
	if (i >= 0)
	{
		return curr->valList.GetItem((UOSInt)i);
	}
	else if (i == -1)
	{
		if (curr->valList.GetCount() == 0)
			return 1.0;
		else
			return curr->valList.GetItem(0);
	}
	else
	{
		return curr->valList.GetItem((UOSInt)~i - 1);
	}
}

Optional<Data::Invest::Asset> Data::Invest::InvestmentManager::AddAsset(NN<Text::String> shortName, NN<Text::String> fullName, UInt32 currency)
{
	NN<Asset> ass;
	NEW_CLASSNN(ass, Asset());
	ass->index = this->assetList.GetCount();
	ass->shortName = shortName->Clone();
	ass->fullName = fullName->Clone();
	ass->currency = currency;
	ass->current = 0;
	this->assetList.Add(ass);
	this->LoadCurrency(currency);
	this->SaveAssets();
	return ass;
}

Bool Data::Invest::InvestmentManager::AssetImport(NN<Asset> ass, NN<DB::ReadingDB> db, UOSInt timeCol, UOSInt valueCol, DateFormat fmt) const
{
	NN<DB::DBReader> r;
	if (!db->QueryTableData(nullptr, CSTR(""), 0, 0, 0, nullptr, nullptr).SetTo(r))
		return false;
	UOSInt colCnt = r->ColCount();
	if (timeCol >= colCnt || valueCol >= colCnt)
	{
		db->CloseReader(r);
		return false;
	}
	Bool found = false;
	NN<Text::String> tsStr;
	NN<Text::String> valStr;
	Data::Timestamp ts;
	Double value;
	while (r->ReadNext())
	{
		tsStr = r->GetNewStrNN(timeCol);
		valStr = r->GetNewStrNN(valueCol);
		tsStr->Trim();
		valStr->Trim();
		ts = ParseTime(tsStr, fmt);
		value = valStr->ToDoubleOrNAN();
		if (!ts.IsNull() && !Math::IsNAN(value))
		{
			OSInt index = ass->tsList.SortedIndexOf(ts);
			UOSInt i;
			if (index >= 0)
			{
				ass->valList.SetItem((UOSInt)index, value);
				if ((UOSInt)index == ass->valList.GetCount() - 1)
				{
					ass->current = value;
				}
			}
			else
			{
				ass->valList.Insert(i = ass->tsList.SortedInsert(ts), value);
				ass->divList.Insert(i, 0.0);
				if (i == ass->valList.GetCount() - 1)
				{
					ass->current = value;
				}
			}
			found = true;
		}
		tsStr->Release();
		valStr->Release();
	}
	db->CloseReader(r);
	if (found)
	{
		this->SaveAsset(ass);
	}
	return found;
}


Bool Data::Invest::InvestmentManager::AssetImportDiv(NN<Asset> ass, NN<DB::ReadingDB> db, UOSInt timeCol, UOSInt valueCol, DateFormat fmt) const
{
	NN<DB::DBReader> r;
	if (!db->QueryTableData(nullptr, CSTR(""), 0, 0, 0, nullptr, nullptr).SetTo(r))
		return false;
	UOSInt colCnt = r->ColCount();
	if (timeCol >= colCnt || valueCol >= colCnt)
	{
		db->CloseReader(r);
		return false;
	}
	Bool found = false;
	NN<Text::String> tsStr;
	NN<Text::String> valStr;
	Data::Timestamp ts;
	Double value;
	while (r->ReadNext())
	{
		tsStr = r->GetNewStrNN(timeCol);
		valStr = r->GetNewStrNN(valueCol);
		tsStr->Trim();
		valStr->Trim();
		ts = ParseTime(tsStr, fmt);
		value = valStr->ToDoubleOrNAN();
		if (!ts.IsNull() && !Math::IsNAN(value))
		{
			OSInt index = ass->tsList.SortedIndexOf(ts);
			if (index >= 0)
			{
				ass->divList.SetItem((UOSInt)index, value);
				found = true;
			}
		}
		tsStr->Release();
		valStr->Release();
	}
	db->CloseReader(r);
	if (found)
	{
		this->SaveAsset(ass);
	}
	return found;
}

Bool Data::Invest::InvestmentManager::UpdateAsset(NN<Asset> ass, Data::Timestamp ts, Double value, Double divValue)
{
	OSInt i = ass->tsList.SortedIndexOf(ts);
	if (i >= 0)
	{
		ass->valList.SetItem((UOSInt)i, value);
		ass->divList.SetItem((UOSInt)i, divValue);
		if ((UOSInt)i == ass->valList.GetCount() - 1)
		{
			ass->current = value;
		}
	}
	else
	{
		UOSInt ui = ass->tsList.SortedInsert(ts);
		ass->valList.Insert(ui, value);
		ass->divList.Insert(ui, divValue);
		if (ui == ass->valList.GetCount() - 1)
		{
			ass->current = value;
		}
	}
	this->SaveAsset(ass);
	return true;
}

Double Data::Invest::InvestmentManager::AssetGetPrice(NN<Asset> ass, Data::Timestamp ts) const
{
	OSInt i = ass->tsList.SortedIndexOf(ts);
	if (i >= 0)
	{
		return ass->valList.GetItem((UOSInt)i);
	}
	else
	{
		i = ~i;
		if (i == 0)
		{
			return 0;
		}
		return ass->valList.GetItem((UOSInt)i - 1);
	}
}

Double Data::Invest::InvestmentManager::AssetGetAmount(NN<Asset> ass, Data::Timestamp ts) const
{
	NN<TradeDetail> t;
	Double total = 0;
	UOSInt i = 0;
	UOSInt j = ass->trades.GetCount();
	while (i < j)
	{
		t = ass->trades.GetItemNoCheck(i);
		if (t->tranBeginDate.NotNull() && t->tranBeginDate <= ts)
		{
			total += t->amount;
		}
		i++;
	}
	return total;
}

void Data::Invest::InvestmentManager::AssetCalcValues(NN<Asset> ass, Data::Date startDate, Data::Date endDate, NN<Data::ArrayListTS> dateList, NN<Data::ArrayList<Double>> valueList)
{
	Data::DateTimeUtil::Weekday wd = startDate.GetWeekday();
	if (wd == Data::DateTimeUtil::Weekday::Saturday)
	{
		startDate = startDate.AddDay(2);
	}
	else if (wd == Data::DateTimeUtil::Weekday::Sunday)
	{
		startDate = startDate.AddDay(1);
	}
	Data::Timestamp startTS = Data::Timestamp::FromDate(startDate, Data::DateTimeUtil::GetLocalTzQhr());
	Data::Timestamp endTS = Data::Timestamp::FromDate(endDate, Data::DateTimeUtil::GetLocalTzQhr());
	Double totalAmount = 0;
	NN<TradeDetail> t;
	OSInt si;
	UOSInt i = 0;
	UOSInt j = ass->trades.GetCount();
	while (i < j)
	{
		t = ass->trades.GetItemNoCheck(i);
		if (t->tranBeginDate > startTS)
		{
			break;
		}
		totalAmount += t->amount;
		i++;
	}
	while (startDate < endDate)
	{
		startTS = Data::Timestamp::FromDate(startDate, Data::DateTimeUtil::GetLocalTzQhr());
		while (i < j)
		{
			t = ass->trades.GetItemNoCheck(i);
			if (t->tranBeginDate > startTS)
			{
				break;
			}
			totalAmount += t->amount;
			i++;
		}
		dateList->Add(startTS);
		si = ass->tsList.SortedIndexOf(startTS);
		if (si >= 0)
		{
			valueList->Add(totalAmount * ass->valList.GetItem((UOSInt)si));
		}
		else if (si == -1)
		{
			valueList->Add(totalAmount);
		}
		else if ((UOSInt)~si >= ass->valList.GetCount())
		{
			valueList->Add(totalAmount * ass->valList.GetItem(ass->valList.GetCount() - 1));
		}
		else
		{
			valueList->Add(totalAmount * ass->valList.GetItem((UOSInt)~si - 1));
		}
		startDate = startDate.AddDay(1);
		wd = startDate.GetWeekday();
		if (wd == Data::DateTimeUtil::Weekday::Saturday)
		{
			startDate = startDate.AddDay(2);
		}
		else if (wd == Data::DateTimeUtil::Weekday::Sunday)
		{
			startDate = startDate.AddDay(1);
		}
	}
}

Bool Data::Invest::InvestmentManager::AddTransactionFX(Data::Timestamp ts, UInt32 curr1, Double value1, UInt32 curr2, Double value2, Double refRate)
{
	UOSInt negCnt = 0;
	if (value1 < 0)
	{
		negCnt++;
	}
	if (value2 < 0)
	{
		negCnt++;
	}
	if (negCnt != 1)
	{
		return false;
	}
	NN<Currency> c;
	NN<TradeDetail> t;
	if (value1 < 0)
	{
		if (curr1 != this->localCurrency)
		{
			c = this->LoadCurrency(curr1);
			Double sum = 0;
			UOSInt i = 0;
			UOSInt j = c->trades.GetCount();
			while (i < j)
			{

				t = c->trades.GetItemNoCheck(i);
				if (!t->tranEndDate.IsNull() && t->tranEndDate <= ts)
				{
					sum += t->amount;
				}
				i++;
			}
			if (sum + value1 < 0)
			{
				return false;
			}
		}
	}
	else
	{
		if (curr2 != this->localCurrency)
		{
			c = this->LoadCurrency(curr2);
			Double sum = 0;
			UOSInt i = 0;
			UOSInt j = c->trades.GetCount();
			while (i < j)
			{

				t = c->trades.GetItemNoCheck(i);
				if (!t->tranEndDate.IsNull() && t->tranEndDate <= ts)
				{
					sum += t->amount;
				}
				i++;
			}
			if (sum + value2 < 0)
			{
				return false;
			}
		}
	}
	NN<TradeEntry> ent;
	NEW_CLASSNN(ent, TradeEntry());
	ent->type = TradeType::ForeignExchange;
	ent->fromIndex = curr1;
	ent->fromDetail.tranBeginDate = ts;
	ent->fromDetail.tranEndDate = ts;
	ent->fromDetail.priceDate = ts;
	ent->fromDetail.amount = value1;
	if (curr1 == this->localCurrency)
	{
		if (curr2 == this->refCurrency)
		{
			ent->fromDetail.cost = -value1 / value2;
		}
		else
		{
			ent->fromDetail.cost = refRate;
		}
	}
	else if (curr1 == this->refCurrency)
	{
		ent->fromDetail.cost = 1.0;
	}
	else
	{
		if (curr2 == this->refCurrency)
		{
			ent->fromDetail.cost = -value1 / value2;
		}
		else
		{
			ent->fromDetail.cost = -value1 / value2 * refRate;
		}
	}
	ent->toIndex = curr2;
	ent->toDetail.tranBeginDate = ts;
	ent->toDetail.tranEndDate = ts;
	ent->toDetail.priceDate = ts;
	ent->toDetail.amount = value2;
	if (curr2 == this->localCurrency)
	{
		if (curr1 == this->refCurrency)
		{
			ent->fromDetail.cost = -value2 / value1;
		}
		else
		{
			ent->toDetail.cost = refRate;
		}
	}
	else if (curr2 == this->refCurrency)
	{
		ent->toDetail.cost = 1.0;
	}
	else
	{
		if (curr1 == this->refCurrency)
		{
			ent->toDetail.cost = -value2 / value1;
		}
		else
		{
			ent->toDetail.cost = -value2 / value1 * refRate;
		}
	}
	ent->refRate = refRate;
	c = this->LoadCurrency(curr1);
	c->trades.Add(ent->fromDetail);
	c = this->LoadCurrency(curr2);
	c->trades.Add(ent->toDetail);
	this->AddTradeEntry(ent);
	this->SaveTransactions();
	return true;
}

Bool Data::Invest::InvestmentManager::AddTransactionDeposit(Data::Timestamp startTime, Data::Timestamp endTime, UInt32 curr, Double startValue, Double endValue)
{
	if (startTime.IsNull() || endTime.IsNull() || startValue <= 0 || endValue <= startValue)
	{
		return false;
	}
	NN<Currency> c;
	if (!this->FindCurrency(curr).SetTo(c))
	{
		return false;
	}
	UOSInt i;
	UOSInt j;
	if (curr != this->localCurrency)
	{
		NN<TradeDetail> t;
		Double totalVal = 0;
		i = 0;
		j = c->trades.GetCount();
		while (i < j)
		{
			t = c->trades.GetItemNoCheck(i);
			if (!t->tranEndDate.IsNull() && t->tranEndDate <= startTime)
			{
				totalVal += c->trades.GetItemNoCheck(i)->amount;
			}
			i++;
		}
		if (startValue > totalVal)
		{
			return false;
		}
	}
	NN<TradeEntry> ent;
	NEW_CLASSNN(ent, TradeEntry());
	ent->type = TradeType::FixedDeposit;
	ent->fromIndex = curr;
	ent->fromDetail.tranBeginDate = startTime;
	ent->fromDetail.tranEndDate = startTime;
	ent->fromDetail.priceDate = startTime;
	ent->fromDetail.amount = -startValue;
	ent->fromDetail.cost = 1.0;
	ent->toIndex = curr;
	ent->toDetail.tranBeginDate = endTime;
	ent->toDetail.tranEndDate = endTime;
	ent->toDetail.priceDate = startTime;
	ent->toDetail.amount = endValue;
	ent->toDetail.cost = 1.0;
	ent->refRate = (endValue / startValue - 1) * 36500 / endTime.Diff(startTime).GetTotalDays();
	c = this->LoadCurrency(curr);
	c->trades.Add(ent->fromDetail);
	c->trades.Add(ent->toDetail);
	this->AddTradeEntry(ent);
	this->SaveTransactions();
	return true;
}

Bool Data::Invest::InvestmentManager::AddTransactionAsset(Data::Timestamp startTime, Data::Timestamp endTime, Data::Timestamp priceTime, UOSInt assetIndex, Double assetAmount, Double currencyValue)
{
	if (endTime.NotNull())
	{
		if (startTime > endTime || priceTime > endTime || priceTime < startTime)
		{
			return false;
		}
	}
	else if (priceTime.NotNull())
	{
		if (priceTime < startTime)
		{
			return false;
		}
	}
	NN<Asset> ass;
	if (!this->assetList.GetItem(assetIndex).SetTo(ass))
	{
		return false;
	}
	UOSInt negCnt = 0;
	if (assetAmount < 0)
	{
		negCnt++;
	}
	if (currencyValue < 0)
	{
		negCnt++;
	}
	if (negCnt != 1)
	{
		return false;
	}

	NN<TradeDetail> t;
	NN<Currency> c;
	UOSInt i;
	UOSInt j;
	if (assetAmount < 0)
	{
		Double totalAmount = 0;
		i = 0;
		j = ass->trades.GetCount();
		while (i < j)
		{
			t = ass->trades.GetItemNoCheck(i);
			if (t->tranEndDate.NotNull() && t->tranEndDate <= startTime)
			{
				totalAmount += t->amount;
			}
			i++;
		}
		if (assetAmount + totalAmount < 0)
		{
			return false;
		}
	}
	else
	{
		if (ass->currency != this->localCurrency)
		{
			Double totalValue = 0;
			c = this->LoadCurrency(ass->currency);
			i = 0;
			j = c->trades.GetCount();
			while (i < j)
			{
				t = c->trades.GetItemNoCheck(i);
				if (t->tranEndDate.NotNull() && t->tranEndDate <= startTime)
				{
					totalValue += t->amount;
				}
				i++;
			}
			if (currencyValue + totalValue < 0)
			{
				return false;
			}
		}
	}
	NN<TradeEntry> ent;
	NEW_CLASSNN(ent, TradeEntry());
	ent->type = TradeType::CashToAsset;
	ent->fromIndex = ass->currency;
	ent->fromDetail.tranBeginDate = startTime;
	ent->fromDetail.tranEndDate = startTime;
	ent->fromDetail.priceDate = startTime;
	ent->fromDetail.amount = currencyValue;
	ent->fromDetail.cost = 1.0;
	ent->toIndex = ass->index;
	ent->toDetail.tranBeginDate = startTime;
	ent->toDetail.tranEndDate = endTime;
	ent->toDetail.priceDate = priceTime;
	ent->toDetail.amount = assetAmount;
	ent->toDetail.cost = -currencyValue / assetAmount;
	ent->refRate = -currencyValue / assetAmount;
	c = this->LoadCurrency(ass->currency);
	c->trades.Add(ent->fromDetail);
	ass->trades.Add(ent->toDetail);
	this->AddTradeEntry(ent);
	this->SaveTransactions();
	return true;
}

Bool Data::Invest::InvestmentManager::AddTransactionAInterest(Data::Timestamp startTime, Data::Timestamp endTime, UOSInt assetIndex, Double currencyValue)
{
	if (endTime.NotNull())
	{
		if (startTime > endTime)
		{
			return false;
		}
	}
	NN<Asset> ass;
	if (!this->assetList.GetItem(assetIndex).SetTo(ass))
	{
		return false;
	}
	if (currencyValue <= 0)
	{
		return false;
	}
	NN<TradeEntry> ent;
	NEW_CLASSNN(ent, TradeEntry());
	ent->type = TradeType::AssetInterest;
	ent->fromIndex = ass->index;
	ent->fromDetail.tranBeginDate = startTime;
	ent->fromDetail.tranEndDate = startTime;
	ent->fromDetail.priceDate = startTime;
	ent->fromDetail.amount = 0;
	ent->fromDetail.cost = -currencyValue / AssetGetAmount(ass, startTime);
	ent->toIndex = ass->currency;
	ent->toDetail.tranBeginDate = startTime;
	ent->toDetail.tranEndDate = endTime;
	ent->toDetail.priceDate = startTime;
	ent->toDetail.amount = currencyValue;
	ent->toDetail.cost = 1.0;
	ent->refRate = 0.0;
	ass->trades.Add(ent->fromDetail);
	NN<Currency> c = this->LoadCurrency(ass->currency);
	c->trades.Add(ent->toDetail);
	this->AddTradeEntry(ent);
	this->SaveTransactions();
	return true;
}

Bool Data::Invest::InvestmentManager::AddTransactionCInterest(Data::Timestamp ts, UInt32 curr, Double currencyValue)
{
	NN<Currency> c;
	if (!this->FindCurrency(curr).SetTo(c))
	{
		return false;
	}
	if (currencyValue <= 0)
	{
		return false;
	}
	if (ts.IsNull())
	{
		return false;
	}
	NN<TradeEntry> ent;
	NEW_CLASSNN(ent, TradeEntry());
	ent->type = TradeType::AccountInterest;
	ent->fromIndex = curr;
	ent->fromDetail.tranBeginDate = ts;
	ent->fromDetail.tranEndDate = ts;
	ent->fromDetail.priceDate = ts;
	ent->fromDetail.amount = 0;
	ent->fromDetail.cost = 0;
	ent->toIndex = curr;
	ent->toDetail.tranBeginDate = ts;
	ent->toDetail.tranEndDate = ts;
	ent->toDetail.priceDate = ts;
	ent->toDetail.amount = currencyValue;
	ent->toDetail.cost = 1.0;
	ent->refRate = 0.0;
	c->trades.Add(ent->toDetail);
	this->AddTradeEntry(ent);
	this->SaveTransactions();
	return true;
}

Data::Timestamp Data::Invest::InvestmentManager::GetFirstTradeTime() const
{
	if (this->tradeList.GetCount() > 0)
	{
		return this->tradeList.GetItemNoCheck(0)->fromDetail.tranBeginDate;
	}
	return Data::Timestamp::Now();
}

Text::CStringNN Data::Invest::InvestmentManager::TradeTypeGetName(TradeType type)
{
	switch (type)
	{
	case TradeType::ForeignExchange:
		return CSTR("FX");
	case TradeType::FixedDeposit:
		return CSTR("Deposit");
	case TradeType::CashToAsset:
		return CSTR("Asset");
	case TradeType::AssetInterest:
		return CSTR("AInterest");
	case TradeType::AccountInterest:
		return CSTR("Interest");
	default:
		return CSTR("?");
	}
}
