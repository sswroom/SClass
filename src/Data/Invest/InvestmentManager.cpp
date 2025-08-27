#include "Stdafx.h"
#include "Data/Invest/InvestmentManager.h"
#include "DB/CSVFile.h"
#include "DB/DBReader.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"

#define SETTINGFILE CSTR("Setting.txt")
#define ACCOUNTFILE CSTR("Accounts.txt")
#define ASSETSFILE CSTR("Assets.csv")

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
	sb.Append(ACCOUNTFILE);
	{
		IO::FileStream fs(sb.ToCString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		Text::UTF8Reader reader(fs);
		sb.ClearStr();
		while (reader.ReadLine(sb, 512))
		{
			if (sb.leng > 0)
			{
				this->accounts.Add(Text::String::New(sb.ToCString()));
			}
			sb.ClearStr();
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
	this->accounts.FreeAll();
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
	this->tradeList.MemFreeAll();
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

Bool Data::Invest::InvestmentManager::SaveAccounts() const
{
	Text::StringBuilderUTF8 sb;
	sb.Append(this->path);
	sb.Append(ACCOUNTFILE);
	IO::FileStream fs(sb.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.IsError())
	{
		return false;
	}
	Bool succ = true;
	Text::UTF8Writer writer(fs);
	UOSInt i = 0;
	UOSInt j = this->accounts.GetCount();
	while (i < j)
	{
		succ = succ && writer.WriteLine(this->accounts.GetItemNoCheck(i)->ToCString());
		i++;
	}
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

Bool Data::Invest::InvestmentManager::AddAccount(NN<Text::String> accountName)
{
	this->accounts.Add(accountName->Clone());
	return this->SaveAccounts();
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
