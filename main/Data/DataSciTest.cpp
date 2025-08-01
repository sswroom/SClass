#include "Stdafx.h"
#include "Core/Core.h"
#include "Data/ArrayListUInt32.h"
#include "Data/ChartPlotter.h"
#include "Data/RandomMT19937.h"
#include "DB/CSVFile.h"
#include "DB/TextDB.h"
#include "IO/ConsoleWriter.h"
#include "Math/NumArrayTool.h"
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
		console.WriteLine(Text::StringBuilderUTF8().Str(CSTR("最小值:"))->F64(cnts2.Mean())->ToCString());
		console.WriteLine(Text::StringBuilderUTF8().Str(CSTR("中位數:"))->U32(cnts2.Median())->ToCString());
		console.WriteLine(Text::StringBuilderUTF8().Str(CSTR("最小值:"))->U32(cnts2.Min())->ToCString());
		console.WriteLine(Text::StringBuilderUTF8().Str(CSTR("最大值:"))->U32(cnts2.Max())->ToCString());
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
			console.WriteLine(Text::StringBuilderUTF8().Str(CSTR("最小值:"))->F64(cnts.Mean())->ToCString());
			console.WriteLine(Text::StringBuilderUTF8().Str(CSTR("中位數:"))->U32(cnts.Median())->ToCString());
			console.WriteLine(Text::StringBuilderUTF8().Str(CSTR("最小值:"))->U32(cnts.Min())->ToCString());
			console.WriteLine(Text::StringBuilderUTF8().Str(CSTR("最大值:"))->U32(cnts.Max())->ToCString());
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
			ds2->SortByValueInv();
			Data::VariItem item;
			UOSInt i = 0;
			UOSInt j = 10;
			while (i < j)
			{
				if (ds2->GetKey(i, item))
				{
					NN<Text::String> id;
					if (item.GetAsNewString().SetTo(id))
					{
						dfInfo->SetCondition(Data::QueryConditions::New()->StrEquals(CSTR("顧客ID"), id->ToCString()));
						Text::TextWriteUtil::WriteTableDataPart(console, dfInfo, 5, 5);
						id->Release();
					}
				}
				i++;
			}

			ds2.Delete();
			ds.Delete();
		}
		dfInfo.Delete();
	}
	return 0;
}

Int32 TestPage40()
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
			ds2->SortByValueInv();
			Data::ChartPlotter chart(0);
			NN<Media::DrawEngine> deng = Media::DrawEngineFactory::CreateDrawEngine();

			Data::VariItem item;
			UOSInt i = 0;
			UOSInt j = 10;
			while (i < j)
			{
				if (ds2->GetKey(i, item))
				{
					NN<Text::String> id;
					if (item.GetAsNewString().SetTo(id))
					{
						Page38AddChart(chart, dfInfo, Data::QueryConditions::New()->StrEquals(CSTR("顧客ID"), id->ToCString()), chart.GetRndColor());
						id->Release();
					}
				}
				i++;
			}
			NN<Data::ChartPlotter::Axis> axis;
			if (chart.GetXAxis().SetTo(axis)) axis->SetLabelRotate(60);
			chart.SetDateFormat(CSTR("yyyy-MM"));
			chart.SavePng(deng, {640, 480}, CSTR("Chapter1-7.png"));

			ds2.Delete();
			ds.Delete();
			deng.Delete();
		}
		dfInfo.Delete();
	}
	return 0;
}

Int32 TestPage41()
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
			ds2->SortByValueInv();
			Data::ChartPlotter chart(0);
			NN<Media::DrawEngine> deng = Media::DrawEngineFactory::CreateDrawEngine();

			Data::VariItem item;
			UOSInt i = 11;
			UOSInt j = 20;
			while (i < j)
			{
				if (ds2->GetKey(i, item))
				{
					NN<Text::String> id;
					if (item.GetAsNewString().SetTo(id))
					{
						Page38AddChart(chart, dfInfo, Data::QueryConditions::New()->StrEquals(CSTR("顧客ID"), id->ToCString()), chart.GetRndColor());
						id->Release();
					}
				}
				i++;
			}
			NN<Data::ChartPlotter::Axis> axis;
			if (chart.GetXAxis().SetTo(axis)) axis->SetLabelRotate(60);
			chart.SetDateFormat(CSTR("yyyy-MM"));
			chart.SavePng(deng, {640, 480}, CSTR("Chapter1-7-2.png"));

			ds2.Delete();
			ds.Delete();
			deng.Delete();
		}
		dfInfo.Delete();
	}
	return 0;
}

Int32 TestPage42()
{
	IO::ConsoleWriter console;
	UOSInt tsCols[] = {0};
	NN<Data::TableData> dfInfo;
	if (DB::CSVFile::LoadAsTableData(CSTR(DATAPATH "Chapter1/accomodation_info.csv"), 65001, 0, {tsCols, sizeof(tsCols) / sizeof(tsCols[0])}).SetTo(dfInfo))
	{
		Data::Timestamp targetDate = Data::Timestamp::FromTimeValue(2020, 3, 1, 0, 0, 0, 0, Data::DateTimeUtil::GetLocalTzQhr());
		dfInfo->SetCondition(Data::QueryConditions::New()->TimeBefore(CSTR("日期"), targetDate));
		Text::TextWriteUtil::WriteTableDataPart(console, dfInfo, 5, 5);
		UOSInt rowCntPre = dfInfo->GetRowCount();
		dfInfo->SetCondition(Data::QueryConditions::New()->TimeOnOrAfter(CSTR("日期"), targetDate));
		UOSInt rowCntPost = dfInfo->GetRowCount();
		dfInfo->SetCondition(0);
		UOSInt rowCntAll = dfInfo->GetRowCount();
		console.WriteLine(Text::StringBuilderUTF8().UOS(rowCntPre + rowCntPost)->AppendUTF8Char(' ')->UOS(rowCntAll)->ToCString());
		dfInfo.Delete();
	}
	return 0;
}

Int32 TestPage43()
{
	IO::ConsoleWriter console;
	UOSInt tsCols[] = {0};
	NN<Data::TableData> dfInfo;
	NN<Data::TableData> dfInfoPre;
	NN<Data::TableData> dfInfoPost;
	if (DB::CSVFile::LoadAsTableData(CSTR(DATAPATH "Chapter1/accomodation_info.csv"), 65001, 0, {tsCols, sizeof(tsCols) / sizeof(tsCols[0])}).SetTo(dfInfo))
	{
		Data::Timestamp targetDate = Data::Timestamp::FromTimeValue(2020, 3, 1, 0, 0, 0, 0, Data::DateTimeUtil::GetLocalTzQhr());
		dfInfoPre = dfInfo->CreateSubTable(Data::QueryConditions::New()->TimeBefore(CSTR("日期"), targetDate));
		dfInfoPost = dfInfo->CreateSubTable(Data::QueryConditions::New()->TimeOnOrAfter(CSTR("日期"), targetDate));
		NN<Data::DataSet> ds;
		NN<Data::DataSet> ds2;
		if (dfInfo->GetDataSet(CSTR("顧客ID")).SetTo(ds))
		{
			UInt32 preCnts[200];
			UInt32 postCnts[200];
			Optional<Text::String> labels[200];
			Data::ChartPlotter chart(0);
			NN<Media::DrawEngine> deng = Media::DrawEngineFactory::CreateDrawEngine();

			ds2 = ds->ValueCountsAsDS();
			ds2->SortByValueInv();
			UOSInt i = 0;
			UOSInt j = 200;
			Data::VariItem item;
			while (i < j)
			{
				preCnts[i] = 0;
				postCnts[i] = 0;
				labels[i] = 0;
				if (ds2->GetKey(i, item))
				{
					NN<Text::String> id;
					if (item.GetAsNewString().SetTo(id))
					{
						preCnts[i] = (UInt32)dfInfoPre->GetRowCount(Data::QueryConditions::New()->StrEquals(CSTR("顧客ID"), id->ToCString()));
						postCnts[i] = (UInt32)dfInfoPost->GetRowCount(Data::QueryConditions::New()->StrEquals(CSTR("顧客ID"), id->ToCString()));
						labels[i] = id;
					}
				}
				i++;
			}

			chart.AddScatter(CSTR(""), Data::ChartPlotter::NewData(postCnts, j), Data::ChartPlotter::NewData(preCnts, j), labels, 0xff000000);
			chart.SetXAxisName(CSTR("pre epidemic"));
			chart.SetY1AxisName(CSTR("post epidemic"));
			chart.SavePng(deng, {640, 480}, CSTR("Chapter1-8.png"));

			deng.Delete();
			ds2.Delete();
			ds.Delete();
		}

		dfInfoPre.Delete();
		dfInfoPost.Delete();
		dfInfo.Delete();
	}
	return 0;
}

Int32 TestPage44()
{
	IO::ConsoleWriter console;
	UOSInt tsCols[] = {0};
	NN<Data::TableData> dfInfo;
	NN<Data::TableData> dfInfoPre;
	NN<Data::TableData> dfInfoPost;
	if (DB::CSVFile::LoadAsTableData(CSTR(DATAPATH "Chapter1/accomodation_info.csv"), 65001, 0, {tsCols, sizeof(tsCols) / sizeof(tsCols[0])}).SetTo(dfInfo))
	{
		Data::Timestamp targetDate = Data::Timestamp::FromTimeValue(2020, 3, 1, 0, 0, 0, 0, Data::DateTimeUtil::GetLocalTzQhr());
		dfInfoPre = dfInfo->CreateSubTable(Data::QueryConditions::New()->TimeBefore(CSTR("日期"), targetDate));
		dfInfoPost = dfInfo->CreateSubTable(Data::QueryConditions::New()->TimeOnOrAfter(CSTR("日期"), targetDate));
		NN<Data::DataSet> ds;
		NN<Data::DataSet> ds2;
		if (dfInfo->GetDataSet(CSTR("顧客ID")).SetTo(ds))
		{
			UInt32 preCnts[200];
			UInt32 postCnts[200];
			Data::ChartPlotter chart(0);
			NN<Media::DrawEngine> deng = Media::DrawEngineFactory::CreateDrawEngine();

			ds2 = ds->ValueCountsAsDS();
			ds2->SortByValueInv();
			UOSInt i = 0;
			UOSInt j = 200;
			Data::VariItem item;
			while (i < j)
			{
				preCnts[i] = 0;
				postCnts[i] = 0;
				if (ds2->GetKey(i, item))
				{
					NN<Text::String> id;
					if (item.GetAsNewString().SetTo(id))
					{
						preCnts[i] = (UInt32)dfInfoPre->GetRowCount(Data::QueryConditions::New()->StrEquals(CSTR("顧客ID"), id->ToCString()));
						postCnts[i] = (UInt32)dfInfoPost->GetRowCount(Data::QueryConditions::New()->StrEquals(CSTR("顧客ID"), id->ToCString()));
						id->Release();
					}
				}
				i++;
			}

			chart.AddScatter(CSTR(""), Data::ChartPlotter::NewData(postCnts, j), Data::ChartPlotter::NewData(preCnts, j), 0xff000000);
			chart.SetXAxisName(CSTR("pre epidemic"));
			chart.SetY1AxisName(CSTR("post epidemic"));
			chart.SavePng(deng, {640, 480}, CSTR("Chapter1-8-2.png"));

			deng.Delete();
			ds2.Delete();
			ds.Delete();
		}

		dfInfoPre.Delete();
		dfInfoPost.Delete();
		dfInfo.Delete();
	}
	return 0;
}

Int32 TestPage46()
{
	IO::ConsoleWriter console;
	UOSInt tsCols[] = {0};
	NN<Data::TableData> dfInfo;
	NN<Data::TableData> dfInfoPre;
	NN<Data::TableData> dfInfoPost;
	if (DB::CSVFile::LoadAsTableData(CSTR(DATAPATH "Chapter1/accomodation_info.csv"), 65001, 0, {tsCols, sizeof(tsCols) / sizeof(tsCols[0])}).SetTo(dfInfo))
	{
		Data::Timestamp targetDate = Data::Timestamp::FromTimeValue(2020, 3, 1, 0, 0, 0, 0, Data::DateTimeUtil::GetLocalTzQhr());
		dfInfoPre = dfInfo->CreateSubTable(Data::QueryConditions::New()->TimeBefore(CSTR("日期"), targetDate));
		dfInfoPost = dfInfo->CreateSubTable(Data::QueryConditions::New()->TimeOnOrAfter(CSTR("日期"), targetDate));
		NN<Data::DataSet> ds;
		NN<Data::DataSet> ds2;
		if (dfInfo->GetDataSet(CSTR("顧客ID")).SetTo(ds))
		{
			UInt32 preCnts[200];
			UInt32 postCnts[200];
			Data::ChartPlotter chart(0);
			NN<Media::DrawEngine> deng = Media::DrawEngineFactory::CreateDrawEngine();

			ds2 = ds->ValueCountsAsDS();
			ds2->SortByValueInv();
			UInt32 thresholdPost = 50;
			UOSInt i = 0;
			UOSInt j = 200;
			Data::VariItem item;
			NN<Text::String> id;
			UInt32 tempColor;
			while (i < j)
			{
				preCnts[i] = 0;
				postCnts[i] = 0;
				if (ds2->GetKey(i, item))
				{
					if (item.GetAsNewString().SetTo(id))
					{
						preCnts[i] = (UInt32)dfInfoPre->GetRowCount(Data::QueryConditions::New()->StrEquals(CSTR("顧客ID"), id->ToCString()));
						postCnts[i] = (UInt32)dfInfoPost->GetRowCount(Data::QueryConditions::New()->StrEquals(CSTR("顧客ID"), id->ToCString()));
						id->Release();
					}
				}
				i++;
			}
			i = 0;
			while (i < j)
			{
				if (ds2->GetKey(i, item))
				{
					if (item.GetAsNewString().SetTo(id))
					{
						if (postCnts[i] > thresholdPost)
							tempColor = 0xffff0000;
						else
							tempColor = 0xff000000;
						chart.AddScatter(CSTR(""), Data::ChartPlotter::NewData(&postCnts[i], 1), Data::ChartPlotter::NewData(&preCnts[i], 1), tempColor);
						id->Release();
					}
				}
				i++;
			}

			chart.SetXAxisName(CSTR("pre epidemic"));
			chart.SetY1AxisName(CSTR("post epidemic"));
			chart.SavePng(deng, {640, 480}, CSTR("Chapter1-9.png"));

			deng.Delete();
			ds2.Delete();
			ds.Delete();
		}

		dfInfoPre.Delete();
		dfInfoPost.Delete();
		dfInfo.Delete();
	}
	return 0;
}

Int32 TestPage48()
{
	IO::ConsoleWriter console;
	UOSInt tsCols[] = {0};
	NN<Data::TableData> dfInfo;
	NN<Data::TableData> dfInfoPre;
	NN<Data::TableData> dfInfoPost;
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	if (DB::CSVFile::LoadAsTableData(CSTR(DATAPATH "Chapter1/accomodation_info.csv"), 65001, 0, {tsCols, sizeof(tsCols) / sizeof(tsCols[0])}).SetTo(dfInfo))
	{
		Data::Timestamp targetDate = Data::Timestamp::FromTimeValue(2020, 3, 1, 0, 0, 0, 0, Data::DateTimeUtil::GetLocalTzQhr());
		dfInfoPre = dfInfo->CreateSubTable(Data::QueryConditions::New()->TimeBefore(CSTR("日期"), targetDate));
		dfInfoPost = dfInfo->CreateSubTable(Data::QueryConditions::New()->TimeOnOrAfter(CSTR("日期"), targetDate));
		NN<Data::DataSet> ds;
		NN<Data::DataSet> ds2;
		if (dfInfo->GetDataSet(CSTR("顧客ID")).SetTo(ds))
		{
			UInt32 preCnts[200];
			UInt32 postCnts[200];
			ds2 = ds->ValueCountsAsDS();
			ds2->SortByValueInv();
			UInt32 thresholdPost = 50;
			UOSInt i = 0;
			UOSInt j = 200;
			Data::VariItem item;
			Data::VariItem item2;
			NN<Text::String> id;
			Optional<Text::String> cusName;
			while (i < j)
			{
				preCnts[i] = 0;
				postCnts[i] = 0;
				if (ds2->GetKey(i, item))
				{
					if (item.GetAsNewString().SetTo(id))
					{
						preCnts[i] = (UInt32)dfInfoPre->GetRowCount(Data::QueryConditions::New()->StrEquals(CSTR("顧客ID"), id->ToCString()));
						postCnts[i] = (UInt32)dfInfoPost->GetRowCount(Data::QueryConditions::New()->StrEquals(CSTR("顧客ID"), id->ToCString()));
						id->Release();
					}
				}
				i++;
			}
			DB::TextDB newDB(CSTR("Temp"));
			Text::CStringNN cols[4] = {CSTR("顧客ID"), CSTR("住宿者姓名"), CSTR("住宿天數（爆發前）"), CSTR("住宿天數（爆發後）")};
			newDB.AddTable(CSTR("Temp"), cols, 4);
			Text::CString vals[4];
			i = 0;
			while (i < j)
			{
				if (postCnts[i] > thresholdPost)
				{
					if (ds2->GetKey(i, item))
					{
						if (item.GetAsNewString().SetTo(id))
						{
							cusName = 0;
							vals[0] = id->ToCString();
							if (dfInfo->GetFirstData(CSTR("住宿者姓名"), Data::QueryConditions::New()->StrEquals(CSTR("顧客ID"), id->ToCString()), item2))
							{
								cusName = item2.GetAsNewString();
							}
							vals[1] = OPTSTR_CSTR(cusName);
							sptr = Text::StrUInt32(sbuff, preCnts[i]);
							vals[2] = CSTRP(sbuff, sptr);
							sptr++;
							sptr2 = Text::StrUInt32(sptr, postCnts[i]);
							vals[3] = CSTRP(sptr, sptr2);
							newDB.AddTableData(vals, 4);
							id->Release();
							OPTSTR_DEL(cusName);
						}
					}
				}
				i++;
			}
			Data::TableData data2(newDB, false, 0, CSTR("Temp"));
			Text::TextWriteUtil::WriteTableData(console, data2);
			ds2.Delete();
			ds.Delete();
		}

		dfInfoPre.Delete();
		dfInfoPost.Delete();
		dfInfo.Delete();
	}
	return 0;
}

Int32 TestPage52()
{
	IO::ConsoleWriter console;
	UOSInt tsCols[] = {0};
	NN<Data::TableData> dfInfo;
	if (DB::CSVFile::LoadAsTableData(CSTR(DATAPATH "Chapter2/accomodation_info.csv"), 65001, 0, {tsCols, sizeof(tsCols) / sizeof(tsCols[0])}).SetTo(dfInfo))
	{
		Text::TextWriteUtil::WriteTableDataPart(console, dfInfo, 5, 5);
		dfInfo.Delete();
	}
	return 0;
}

Int32 TestPage53()
{
	IO::ConsoleWriter console;
	UOSInt tsCols[] = {0};
	NN<Data::TableData> dfInfo;
	NN<Data::DataSet> ds;
	NN<Data::DataSet> ds2;
	if (DB::CSVFile::LoadAsTableData(CSTR(DATAPATH "Chapter2/accomodation_info.csv"), 65001, 0, {tsCols, sizeof(tsCols) / sizeof(tsCols[0])}).SetTo(dfInfo))
	{
		Data::ArrayList<Data::TwinItem<Data::Timestamp, UInt32>> x0;
		if (dfInfo->GetKeyDataSet().SetTo(ds))
		{
			ds->GroupKeyByMonth().Count(x0);
			ds.Delete();
		}
		Data::VariItem item;
		UOSInt iRank = 1;
		UOSInt jRank = 2;
		Optional<Text::String> iId = 0;
		Optional<Text::String> jId = 0;
		NN<Text::String> s;
		if (dfInfo->GetDataSet(CSTR("顧客ID")).SetTo(ds))
		{
			ds2 = ds->ValueCountsAsDS();
			ds2->SortByValueInv();
			if (ds2->GetKey(iRank, item)) iId = item.GetAsNewString();
			if (ds2->GetKey(jRank, item)) jId = item.GetAsNewString();
			ds2.Delete();
			ds.Delete();
		}
		Data::ChartPlotter chart(0);
		NN<Media::DrawEngine> deng = Media::DrawEngineFactory::CreateDrawEngine();
		if (iId.SetTo(s))
		{
			dfInfo->SetCondition(Data::QueryConditions::New()->StrEquals(CSTR("顧客ID"), s->ToCString()));
			if (dfInfo->GetKeyDataSet().SetTo(ds))
			{
				Data::ArrayList<Data::TwinItem<Data::Timestamp, UInt32>> cnts;
				ds->GroupKeyByMonth().Count(cnts);
				chart.AddLineChart(CSTR(""), Data::ChartPlotter::NewDataFromValue<Data::Timestamp, UInt32>(cnts), Data::ChartPlotter::NewDataFromKey<Data::Timestamp, UInt32>(cnts), 0xffff0000);
				ds.Delete();
			}
			s->Release();
		}
		if (jId.SetTo(s))
		{
			dfInfo->SetCondition(Data::QueryConditions::New()->StrEquals(CSTR("顧客ID"), s->ToCString()));
			if (dfInfo->GetKeyDataSet().SetTo(ds))
			{
				Data::ArrayList<Data::TwinItem<Data::Timestamp, UInt32>> cnts;
				ds->GroupKeyByMonth().Count(cnts);
				chart.AddLineChart(CSTR(""), Data::ChartPlotter::NewDataFromValue<Data::Timestamp, UInt32>(cnts), Data::ChartPlotter::NewDataFromKey<Data::Timestamp, UInt32>(cnts), 0xff000000);
				ds.Delete();
			}
			s->Release();
		}
		NN<Data::ChartPlotter::Axis> axis;
		if (chart.GetXAxis().SetTo(axis))
		{
			NN<Data::ChartPlotter::TimeAxis>::ConvertFrom(axis)->ExtendRange(x0.GetItem(0).key.inst);
			NN<Data::ChartPlotter::TimeAxis>::ConvertFrom(axis)->ExtendRange(x0.GetItem(x0.GetCount() - 1).key.inst);
			axis->SetLabelRotate(60);
		}
		chart.SavePng(deng, {640, 480}, CSTR("Chapter2-1.png"));

		dfInfo.Delete();
		deng.Delete();
	}
	return 0;
}


Int32 TestPage54()
{
	IO::ConsoleWriter console;
	UOSInt tsCols[] = {0};
	NN<Data::TableData> dfInfo;
	NN<Data::DataSet> ds;
	NN<Data::DataSet> ds2;
	if (DB::CSVFile::LoadAsTableData(CSTR(DATAPATH "Chapter2/accomodation_info.csv"), 65001, 0, {tsCols, sizeof(tsCols) / sizeof(tsCols[0])}).SetTo(dfInfo))
	{
		Data::ArrayList<Data::TwinItem<Data::Timestamp, UInt32>> x0;
		if (dfInfo->GetKeyDataSet().SetTo(ds))
		{
			ds->GroupKeyByMonth().Count(x0);
			ds.Delete();
		}
		Data::VariItem item;
		UOSInt iRank = 1;
		UOSInt jRank = 2;
		Optional<Text::String> iId = 0;
		Optional<Text::String> jId = 0;
		NN<Text::String> s;
		if (dfInfo->GetDataSet(CSTR("顧客ID")).SetTo(ds))
		{
			ds2 = ds->ValueCountsAsDS();
			ds2->SortByValueInv();
			if (ds2->GetKey(iRank, item)) iId = item.GetAsNewString();
			if (ds2->GetKey(jRank, item)) jId = item.GetAsNewString();
			ds2.Delete();
			ds.Delete();
		}
		Data::ArrayList<Data::TwinItem<Data::Timestamp, UInt32>> cntsI;
		Data::ArrayList<Data::TwinItem<Data::Timestamp, UInt32>> cntsJ;
		if (iId.SetTo(s))
		{
			dfInfo->SetCondition(Data::QueryConditions::New()->StrEquals(CSTR("顧客ID"), s->ToCString()));
			if (dfInfo->GetKeyDataSet().SetTo(ds))
			{
				ds->GroupKeyByMonth().Count(cntsI);
				ds.Delete();
			}
			s->Release();
		}
		if (jId.SetTo(s))
		{
			dfInfo->SetCondition(Data::QueryConditions::New()->StrEquals(CSTR("顧客ID"), s->ToCString()));
			if (dfInfo->GetKeyDataSet().SetTo(ds))
			{
				ds->GroupKeyByMonth().Count(cntsJ);
				ds.Delete();
			}
			s->Release();
		}
		if (cntsI.GetCount() > 0 && cntsI.GetCount() == cntsJ.GetCount())
		{
			Data::ArrayListDbl dx;
			UOSInt i = 0;
			UOSInt j = cntsI.GetCount();
			while (i < j)
			{
				Double v = (Double)cntsI.GetItem(i).value - (Double)cntsJ.GetItem(i).value;
				dx.Add(v);
				i++;
			}
			Double norm = dx.FrobeniusNorm();
			console.WriteLine(Text::StringBuilderUTF8().Str(CSTR("相似度:"))->F64(norm/(Double)j)->ToCString());
		}

		dfInfo.Delete();
	}
	return 0;
}


Int32 TestPage81()
{
	IO::ConsoleWriter console;
	UOSInt num = 365*2;
	Double ave = 0.0;
	Double std = 1.0;
	Data::RandomMT19937 random(0);
	Data::ArrayListDbl x;
	Math::NumArrayTool::GenerateNormalRandom(x, NN<Data::RandomMT19937>(random), ave, std, num);
//	Math::NumArrayTool::GenerateExponentialRandom(x, NN<Data::RandomMT19937>(random), 0.5, num);
	Double xAve = x.Average();
	Double xStd = x.StdDev();
	console.WriteLine(Text::StringBuilderUTF8().Str(CSTR("平均值:"))->F64(xAve)->ToCString());
	console.WriteLine(Text::StringBuilderUTF8().Str(CSTR("標準差:"))->F64(xStd)->ToCString());
	UOSInt numBin = 21;
	Data::ChartPlotter chart(0);
	NN<Media::DrawEngine> deng = Media::DrawEngineFactory::CreateDrawEngine();
	chart.AddHistogramCount(CSTR(""), Data::ChartPlotter::NewData(x), numBin, 0xff000000, 0xff000000);
	NN<Data::ChartPlotter::Axis> axis;
	if (chart.GetXAxis().SetTo(axis))
	{
		NN<Data::ChartPlotter::DoubleAxis>::ConvertFrom(axis)->ExtendRange(-5);
		NN<Data::ChartPlotter::DoubleAxis>::ConvertFrom(axis)->ExtendRange(5);
	}
	if (chart.GetY1Axis().SetTo(axis))
	{
		NN<Data::ChartPlotter::UInt32Axis>::ConvertFrom(axis)->ExtendRange(0);
	}
	chart.SavePng(deng, {640, 480}, CSTR("Chapter3-1.png"));

	UOSInt numSample = 30;
	UOSInt numTrial = 10000;
	Data::ArrayListDbl xTrial;
	Data::ArrayListDbl xSample;
	UOSInt i = 0;
	while (i < numTrial)
	{
		xSample.Clear();
		Math::NumArrayTool::RandomChoice<Double>(xSample, NN<Data::RandomMT19937>(random), x, numSample);
		xTrial.Add(xSample.Average());
		i++;
	}
	Double xTrialAve = xTrial.Average();
	Double xTrialStd = xTrial.StdDev();
	console.WriteLine(Text::StringBuilderUTF8().Str(CSTR("平均值:"))->F64(xTrialAve)->ToCString());
	console.WriteLine(Text::StringBuilderUTF8().Str(CSTR("標準差:"))->F64(xTrialStd)->ToCString());
	Data::ChartPlotter chart2(0);
	chart2.AddHistogramCount(CSTR(""), Data::ChartPlotter::NewData(xTrial), numBin, 0xff000000, 0xff000000);
	if (chart2.GetXAxis().SetTo(axis))
	{
		NN<Data::ChartPlotter::DoubleAxis>::ConvertFrom(axis)->ExtendRange(-5);
		NN<Data::ChartPlotter::DoubleAxis>::ConvertFrom(axis)->ExtendRange(5);
	}
	if (chart2.GetY1Axis().SetTo(axis))
	{
		NN<Data::ChartPlotter::UInt32Axis>::ConvertFrom(axis)->ExtendRange(0);
	}
	chart2.SavePng(deng, {640, 480}, CSTR("Chapter3-1-2.png"));

	deng.Delete();
	return 0;
}

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UOSInt page = 81;
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
	//case 33: //1-5 (polyfit)
	//case 34: //1-5 (polyfit)
	//case 36: //1-5 (polyfit)
	case 37:
		return TestPage37();
	case 2037:
		return TestPage37_2();
	case 38:
		return TestPage38();
	case 39:
		return TestPage39();
	case 40:
		return TestPage40();
	case 41:
		return TestPage41();
	case 42:
		return TestPage42();
	case 43:
		return TestPage43();
	case 44:
		return TestPage44();
	case 46: //1-9
		return TestPage46();
	case 48: //1-10
		return TestPage48();
	case 52: //2-1-1
		return TestPage52();
	case 53: //2-1-2
		return TestPage53();
	case 54: //2-1-3
		return TestPage54();
/*	case 58: //2-3 (PCA)
		return TestPage58();
	case 60: //2-4 (PCA extract display)
		return TestPage60();
	case 63: //2-5 (KMean)
		return TestPage63();*/

	case 81: //3-1
		return TestPage81();
	default:
		return 0;
	}
}
