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
			NN<Data::ChartPlotter::Axis> axis;
			Data::ChartPlotter chart(0);
			chart.AddLineChart(CSTR(""), Data::ChartPlotter::NewDataFromValue<Data::Timestamp, Double>(groupResult), Data::ChartPlotter::NewDataFromKey<Data::Timestamp, Double>(groupResult), 0xff000000);
			if (chart.GetXAxis().SetTo(axis)) axis->SetLabelRotate(60);
			chart.SavePng(deng, {640, 480}, CSTR("Chapter1-2.png"));
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


Int32 TestPage28()
{
	IO::ConsoleWriter console;
	UOSInt tsCols[] = {0};
	NN<Data::TableData> dfInfo;
	NN<Data::DataSet> ds;
	NN<Media::DrawEngine> deng = Media::DrawEngineFactory::CreateDrawEngine();
	if (DB::CSVFile::LoadAsTableData(CSTR(DATAPATH "Chapter1/accomodation_info.csv"), 65001, 0, {tsCols, sizeof(tsCols) / sizeof(tsCols[0])}).SetTo(dfInfo))
	{
		if (dfInfo->GetKeyDataSet().SetTo(ds))
		{
			Data::ArrayList<Data::TwinItem<Data::Timestamp, UInt32>> groupResult;
			ds->GroupKeyByMonth().Count(groupResult);
			NN<Data::ChartPlotter::Axis> axis;
			Data::ChartPlotter chart(0);
			chart.AddLineChart(CSTR(""), Data::ChartPlotter::NewDataFromValue<Data::Timestamp, UInt32>(groupResult), Data::ChartPlotter::NewDataFromKey<Data::Timestamp, UInt32>(groupResult), 0xff000000);
			if (chart.GetXAxis().SetTo(axis)) axis->SetLabelRotate(60);
			chart.SavePng(deng, {640, 480}, CSTR("Chapter1-2-2.png"));
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

Int32 TestPage29_2()
{
	IO::ConsoleWriter console;
	UOSInt tsCols[] = {0};
	NN<Data::TableData> dfInfo;
	if (DB::CSVFile::LoadAsTableData(CSTR(DATAPATH "Chapter1/accomodation_info.csv"), 65001, 0, {tsCols, sizeof(tsCols) / sizeof(tsCols[0])}).SetTo(dfInfo))
	{
		NN<Data::DataSet> ds;
		if (dfInfo->GetDataSet(CSTR("顧客ID")).SetTo(ds))
		{
			Data::ArrayListUInt32 cnts;
			ds->ValueCounts(NN<Data::ArrayList<UInt32>>(cnts));
			console.WriteLine(Text::StringBuilderUTF8().Append(CSTR("最小值:"))->AppendDouble(cnts.Mean())->ToCString());
			console.WriteLine(Text::StringBuilderUTF8().Append(CSTR("中位數:"))->AppendU32(cnts.Median())->ToCString());
			console.WriteLine(Text::StringBuilderUTF8().Append(CSTR("最小值:"))->AppendU32(cnts.Min())->ToCString());
			console.WriteLine(Text::StringBuilderUTF8().Append(CSTR("最大值:"))->AppendU32(cnts.Max())->ToCString());
			ds.Delete();
		}
		dfInfo.Delete();
	}
	return 0;
}

Int32 TestPage31()
{
	IO::ConsoleWriter console;
	UOSInt tsCols[] = {0};
	NN<Data::TableData> dfInfo;
	if (DB::CSVFile::LoadAsTableData(CSTR(DATAPATH "Chapter1/accomodation_info.csv"), 65001, 0, {tsCols, sizeof(tsCols) / sizeof(tsCols[0])}).SetTo(dfInfo))
	{
		NN<Data::DataSet> ds;
		if (dfInfo->GetDataSet(CSTR("顧客ID")).SetTo(ds))
		{
			NN<Media::DrawEngine> deng = Media::DrawEngineFactory::CreateDrawEngine();
			Data::ArrayListUInt32 cnts;
			ds->ValueCounts(NN<Data::ArrayList<UInt32>>(cnts));

			NN<Data::ChartPlotter::Axis> axis;
			Data::ChartPlotter chart(0);
			chart.AddHistogramCount(CSTR(""), Data::ChartPlotter::NewData(cnts), 21, 0xff000000, 0xff000000);
			if (chart.GetXAxis().SetTo(axis)) axis->SetLabelRotate(0);
			chart.SetDblFormat(CSTR("0"));
			chart.SavePng(deng, {640, 480}, CSTR("Chapter1-4.png"));
			ds.Delete();
			deng.Delete();
		}
		dfInfo.Delete();
	}
	return 0;
}

Int32 TestPage37()
{
	IO::ConsoleWriter console;
	UOSInt tsCols[] = {0};
	NN<Data::TableData> dfInfo;
	if (DB::CSVFile::LoadAsTableData(CSTR(DATAPATH "Chapter1/accomodation_info.csv"), 65001, 0, {tsCols, sizeof(tsCols) / sizeof(tsCols[0])}).SetTo(dfInfo))
	{
		dfInfo->SetCondition(Data::QueryConditions::New()->StrEquals(CSTR("方案"), CSTR("A")));
		Text::TextWriteUtil::WriteTableDataPart(console, dfInfo, 5, 5);
		dfInfo.Delete();
	}
	return 0;
}

Int32 TestPage37_2()
{
	IO::ConsoleWriter console;
	UOSInt tsCols[] = {0};
	NN<Data::TableData> dfInfo;
	if (DB::CSVFile::LoadAsTableData(CSTR(DATAPATH "Chapter1/accomodation_info.csv"), 65001, 0, {tsCols, sizeof(tsCols) / sizeof(tsCols[0])}).SetTo(dfInfo))
	{
		dfInfo->SetCondition(Data::QueryConditions::New()->StrEquals(CSTR("方案"), CSTR("A")));
		NN<Data::DataSet> ds;
		if (dfInfo->GetDataSet(CSTR("顧客ID")).SetTo(ds))
		{
			NN<Media::DrawEngine> deng = Media::DrawEngineFactory::CreateDrawEngine();
			Data::ArrayListUInt32 cnts;
			ds->ValueCounts(NN<Data::ArrayList<UInt32>>(cnts));

			NN<Data::ChartPlotter::Axis> axis;
			Data::ChartPlotter chart(0);
			chart.AddHistogramCount(CSTR(""), Data::ChartPlotter::NewData(cnts), 21, 0xff000000, 0xff000000);
			if (chart.GetXAxis().SetTo(axis)) axis->SetLabelRotate(0);
			chart.SetDblFormat(CSTR("0"));
			chart.SavePng(deng, {640, 480}, CSTR("Chapter1-6.png"));
			ds.Delete();
			deng.Delete();
		}
		dfInfo.Delete();
	}
	return 0;
}

void Page38AddChart(NN<Data::ChartPlotter> chart, NN<Data::TableData> dfInfo, NN<Data::QueryConditions> cond, UInt32 lineColor)
{
	NN<Data::DataSet> ds;
	dfInfo->SetCondition(cond);
	if (dfInfo->GetDataSet(CSTR("顧客ID")).SetTo(ds))
	{
		Data::ArrayList<Data::TwinItem<Data::Timestamp, UInt32>> cnts;
		ds->GroupKeyByMonth().Count(cnts);
		chart->AddLineChart(CSTR(""), Data::ChartPlotter::NewDataFromValue<Data::Timestamp, UInt32>(cnts), Data::ChartPlotter::NewDataFromKey<Data::Timestamp, UInt32>(cnts), lineColor);
		ds.Delete();
	}
}

Int32 TestPage38()
{
	IO::ConsoleWriter console;
	UOSInt tsCols[] = {0};
	NN<Data::TableData> dfInfo;
	if (DB::CSVFile::LoadAsTableData(CSTR(DATAPATH "Chapter1/accomodation_info.csv"), 65001, 0, {tsCols, sizeof(tsCols) / sizeof(tsCols[0])}).SetTo(dfInfo))
	{
		NN<Media::DrawEngine> deng = Media::DrawEngineFactory::CreateDrawEngine();
		Data::ChartPlotter chart(0);
		Page38AddChart(chart, dfInfo, Data::QueryConditions::New()->StrEquals(CSTR("方案"), CSTR("A")), 0xff0000ff);
		Page38AddChart(chart, dfInfo, Data::QueryConditions::New()->StrEquals(CSTR("方案"), CSTR("B")), 0xff00ff00);
		Page38AddChart(chart, dfInfo, Data::QueryConditions::New()->StrEquals(CSTR("方案"), CSTR("C")), 0xffff0000);
		Page38AddChart(chart, dfInfo, Data::QueryConditions::New()->StrEquals(CSTR("方案"), CSTR("D")), 0xff000000);
		NN<Data::ChartPlotter::Axis> axis;
		if (chart.GetXAxis().SetTo(axis)) axis->SetLabelRotate(60);
		chart.SetDateFormat(CSTR("yyyy-MM"));
		chart.SavePng(deng, {640, 480}, CSTR("Chapter1-6-2.png"));
		deng.Delete();
		dfInfo.Delete();
	}
	return 0;
}

Int32 TestPage39()
{
	IO::ConsoleWriter console;
	UOSInt tsCols[] = {0};
	NN<Data::TableData> dfInfo;
	if (DB::CSVFile::LoadAsTableData(CSTR(DATAPATH "Chapter1/accomodation_info.csv"), 65001, 0, {tsCols, sizeof(tsCols) / sizeof(tsCols[0])}).SetTo(dfInfo))
	{
		NN<Data::DataSet> ds;
		NN<Data::DataSet> ds2;
		if (dfInfo->GetDataSet(CSTR("顧客ID")).SetTo(ds))
		{
			ds2 = ds->ValueCountsAsDS();
			//ds2->Sort()
			////////////////////////////////////////

			ds2.Delete();
			ds.Delete();
		}
		dfInfo.Delete();
	}
	return 0;
}

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UOSInt page = 38;
	switch (page)
	{
	case 26:
		return TestPage26();
	case 27:
		return TestPage27();
	case 28:
		return TestPage28();
	case 29:
		return TestPage29_2();
	case 31:
		return TestPage31();
	//case 33:
	//case 34:
	//case 36:
	case 37:
		return TestPage37();
	case 2037:
		return TestPage37_2();
	case 38:
		return TestPage38();
	case 39:
		return TestPage39();
	default:
		return 0;
	}
}
