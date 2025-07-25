#include "Stdafx.h"
#include "Core/Core.h"
#include "Data/ArrayListUInt32.h"
#include "Data/ChartPlotter.h"
#include "DB/CSVFile.h"
#include "IO/ConsoleWriter.h"
#include "Media/DrawEngineFactory.h"
#include "Text/TextWriteUtil.h"

#define DATAPATH "/home/sswroom/Progs/VCClass/Book/GotopDataSci/MathProgramming/"

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
	NN<Media::DrawEngine> deng = Media::DrawEngineFactory::CreateDrawEngine();
	if (DB::CSVFile::LoadAsTableData(CSTR(DATAPATH "Chapter1/accomodation_info.csv"), 65001, 0, {tsCols, sizeof(tsCols) / sizeof(tsCols[0])}).SetTo(dfInfo))
	{
		if (dfInfo->GetDataSet(CSTR("金額")).SetTo(ds))
		{
			Data::ArrayList<Data::TwinItem<Data::Timestamp, Double>> groupResult;
			ds->GroupKeyByMonth().Sum(groupResult);
//			UOSInt i = 0;
//			UOSInt j = groupResult.GetCount();
//			while (i < j)
//			{
//				console.WriteLine(Text::StringBuilderUTF8().Append(CSTR("TS: "))->AppendTSNoZone(groupResult.GetItem(i).key)->Append(CSTR(", Sum: "))->AppendDouble(groupResult.GetItem(i).value)->ToCString());
//				i++;
//			}
			NN<Data::ChartPlotter::Axis> axis;
			Data::ChartPlotter chart(0);
			chart.AddLineChart(CSTR(""), Data::ChartPlotter::NewDataFromValue<Data::Timestamp, Double>(groupResult), Data::ChartPlotter::NewDataFromKey<Data::Timestamp, Double>(groupResult), 0xff000000);
			if (chart.GetXAxis().SetTo(axis)) axis->SetLabelRotate(60);
			chart.SavePng(deng, {640, 480}, CSTR("Chapter1-2.png"));
			//console.WriteLine(Text::StringBuilderUTF8().Append(CSTR("Item Count:"))->AppendUOSInt(groupResult.GetCount())->ToCString());
			ds.Delete();
		}
		else
		{
			console.WriteLine(CSTR("Error in getting DataSet"));
		}
		dfInfo.Delete();
	}
	deng.Delete();
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
