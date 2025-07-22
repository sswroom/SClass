#include "Stdafx.h"
#include "Core/Core.h"
#include "Data/ArrayListUInt32.h"
#include "DB/CSVFile.h"
#include "IO/ConsoleWriter.h"
#include "Text/TextWriteUtil.h"

#define DATAPATH ""

Int32 TestPage26()
{
	IO::ConsoleWriter console;
	UOSInt tsCols[] = {0};
	NN<Data::TableData> dfInfo;
	if (DB::CSVFile::LoadAsTableData(CSTR(DATAPATH "Chapter1/accomodation_info.csv"), 65001, 0, {tsCols, sizeof(tsCols) / sizeof(tsCols[0])}).SetTo(dfInfo))
	{
		Text::TextWriteUtil::WriteTableDataPart(console, dfInfo, 5, 5);
		dfInfo.Delete();
	}
	return 0;
}

Int32 TestPage27()
{
	IO::ConsoleWriter console;
	UOSInt tsCols[] = {0};
	NN<Data::TableData> dfInfo;
	NN<Data::DataSet> ds;
	if (DB::CSVFile::LoadAsTableData(CSTR(DATAPATH "Chapter1/accomodation_info.csv"), 65001, 0, {tsCols, sizeof(tsCols) / sizeof(tsCols[0])}).SetTo(dfInfo))
	{
		if (dfInfo->GetDataSet(CSTR("金額")).SetTo(ds))
		{
			console.WriteLine(Text::StringBuilderUTF8().Append(CSTR("Item Count:"))->AppendUOSInt(ds->GetCount())->ToCString());
			ds.Delete();
		}
		else
		{
			console.WriteLine(CSTR("Error in getting DataSet"));
		}
		dfInfo.Delete();
	}
	return 0;
}

Int32 TestPage29()
{
	IO::ConsoleWriter console;
	UOSInt tsCols[] = {0};
	NN<Data::TableData> dfInfo;
	if (DB::CSVFile::LoadAsTableData(CSTR(DATAPATH "Chapter1/accomodation_info.csv"), 65001, 0, {tsCols, sizeof(tsCols) / sizeof(tsCols[0])}).SetTo(dfInfo))
	{
		Data::ArrayListStringNN strs;
		Data::FastStringMap<UInt32> cnts;
		Data::ArrayListUInt32 cnts2;
		dfInfo->GetColumnDataStr(CSTR("顧客ID"), strs);
		strs.ValueCounts(cnts);
		cnts2.AddAll(cnts);
		console.WriteLine(Text::StringBuilderUTF8().Append(CSTR("最小值:"))->AppendDouble(cnts2.Mean())->ToCString());
		console.WriteLine(Text::StringBuilderUTF8().Append(CSTR("中位數:"))->AppendU32(cnts2.Median())->ToCString());
		console.WriteLine(Text::StringBuilderUTF8().Append(CSTR("最小值:"))->AppendU32(cnts2.Min())->ToCString());
		console.WriteLine(Text::StringBuilderUTF8().Append(CSTR("最大值:"))->AppendU32(cnts2.Max())->ToCString());
		dfInfo.Delete();
		strs.FreeAll();
	}
	return 0;
}

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UOSInt page = 27;
	switch (page)
	{
	case 26:
		return TestPage26();
	case 27:
		return TestPage27();
	case 29:
		return TestPage29();
	default:
		return 0;
	}
}
