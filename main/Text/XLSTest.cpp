#include "Stdafx.h"
#include "Core/Core.h"
#include "Exporter/XLSXExporter.h"
#include "IO/ConsoleWriter.h"
#include "IO/Path.h"
#include "Math/Math_C.h"
#include "Math/Unit/Distance.h"
#include "Text/MyStringFloat.h"
#include "Text/SpreadSheet/Workbook.h"

using namespace Text::SpreadSheet;

void TestEmpty()
{
	UTF8Char fileName[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = IO::Path::GetRealPath(fileName, UTF8STRC("~/Progs/Temp/EmptyMe.xlsx"));
	Workbook wb;
	wb.AddWorksheet(CSTR("Sheet1"));
	wb.AddWorksheet(CSTR("Sheet2"));
	Exporter::XLSXExporter exporter;
	if (!exporter.ExportNewFile({fileName, (UOSInt)(sptr - fileName)}, wb, nullptr))
	{
		IO::ConsoleWriter console;
		console.WriteLine(CSTR("Error in writing to file"));
	}
}

void TestChart()
{
	UOSInt testRowCnt = 2;
	UTF8Char fileName[512];
	UnsafeArray<UTF8Char> sptr;
	UTF8Char sbuff2[32];
	UnsafeArray<UTF8Char> sptr2;
	sptr = IO::Path::GetRealPath(fileName, UTF8STRC("~/Progs/Temp/XLSXTest.xlsx"));

	Workbook wb;
	NN<WorkbookFont> font10 = wb.NewFont(CSTR("Arial"), 10, false);
	NN<CellStyle> dateStyle = wb.NewCellStyle(font10, Text::HAlignment::Left, Text::VAlignment::Center, CSTR("yyyy-MM-dd"));
	NN<CellStyle> numStyle = wb.NewCellStyle(font10, Text::HAlignment::Left, Text::VAlignment::Center, CSTR("0.###"));
	NN<Worksheet> graphSheet = wb.AddWorksheet();
	NN<Worksheet> dataSheet = wb.AddWorksheet();
	NN<OfficeChart> chart = graphSheet->CreateChart(Math::Unit::Distance::DU_INCH, 0.64, 1.61, 13.10, 5.53, CSTR("\nSETTLEMENT VS CHAINAGE"));
	chart->InitLineChart(CSTR("ACCUMULATED SETTLEMENT"), CSTR("CHAINAGE"), AxisType::Category);
	chart->SetDisplayBlankAs(BlankAs::Gap);
	if (testRowCnt > 1)
	{
		chart->AddLegend(LegendPos::Bottom);
	}

	UOSInt rowNum;
	Data::DateTime dt;
	dataSheet->SetCellString(0, 0, dateStyle.Ptr(), CSTR("Date"));
	UOSInt i = 0;
	UOSInt j = 20;
	while (i < j)
	{
		dataSheet->SetCellDouble(0, i + 1, numStyle.Ptr(), 112.0 + UOSInt2Double(i) * 0.1);
		
		sbuff2[0] = (UTF8Char)('A' + i);
		sbuff2[1] = 0;
		graphSheet->SetCellString(0, i, wb.GetStyle(0), {sbuff2, 1});
		i++;
	}
	if (testRowCnt > 0)
	{
		rowNum = 0;
		while (rowNum < testRowCnt)
		{
			dt.SetCurrTime();
			dt.AddDay((OSInt)(rowNum - testRowCnt));
			rowNum++;
			dataSheet->SetCellDateTime(rowNum, 0, dateStyle.Ptr(), dt);
			i = 0;
			while (i < j)
			{
				if (i != 5)
				{
					dataSheet->SetCellDouble(rowNum, i + 1, numStyle.Ptr(), UOSInt2Double(rowNum) * 0.2 + UOSInt2Double(i) * 0.1);
				}
				i++;
			}
			NN<WorkbookDataSource> chainageSource;
			NEW_CLASSNN(chainageSource, WorkbookDataSource(dataSheet, 0, 0, 1, j))
			NN<WorkbookDataSource> valSource;
			NEW_CLASSNN(valSource, WorkbookDataSource(dataSheet, rowNum, rowNum, 1, j))
			sptr2 = dt.ToString(sbuff2, "yyyy-MM-dd");
			chart->AddSeries(chainageSource, valSource, CSTRP(sbuff2, sptr2), testRowCnt > 1);
		}
	}

	Exporter::XLSXExporter exporter;
	if (!exporter.ExportNewFile({fileName, (UOSInt)(sptr - fileName)}, wb, nullptr))
	{
		IO::ConsoleWriter console;
		console.WriteLine(CSTR("Error in writing to file"));
	}
}

void TestCols()
{
	UTF8Char fileName[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = IO::Path::GetRealPath(fileName, UTF8STRC("~/Progs/Temp/ColsMe.xlsx"));
	Workbook wb;
	NN<WorkbookFont> font10 = wb.NewFont(CSTR("Arial"), 10, false);
	NN<CellStyle> numStyle = wb.NewCellStyle(font10, Text::HAlignment::Left, Text::VAlignment::Center, CSTR("0.###"));
	NN<Worksheet> sheet = wb.AddWorksheet(CSTR("Sheet1"));
	UOSInt i = 0;
	UOSInt j = 2000;
	while (i < j)
	{
		sheet->SetCellDouble(0, i + 1, numStyle.Ptr(), 112.0 + UOSInt2Double(i) * 0.1);

		i++;
	}

	Exporter::XLSXExporter exporter;
	if (!exporter.ExportNewFile({fileName, (UOSInt)(sptr - fileName)}, wb, nullptr))
	{
		IO::ConsoleWriter console;
		console.WriteLine(CSTR("Error in writing to file"));
	}
}

void TestColWidth()
{
	UTF8Char fileName[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = IO::Path::GetRealPath(fileName, UTF8STRC("~/Progs/Temp/ColWidthMe.xlsx"));
	Workbook wb;
	NN<Worksheet> sheet = wb.AddWorksheet(CSTR("Sheet1"));
	sheet->SetColWidth(0, 123.75, Math::Unit::Distance::DU_POINT);	//23.5714285714286 * 5.25
	sheet->SetColWidth(1, 75.75, Math::Unit::Distance::DU_POINT);	//14.4285714285714
	sheet->SetColWidth(2, 303.75, Math::Unit::Distance::DU_POINT);	//57.8571428571429
	sheet->SetColWidth(3, 75.75, Math::Unit::Distance::DU_POINT);	//14.4285714285714
	sheet->SetColWidth(4, 75.75, Math::Unit::Distance::DU_POINT);	//14.4285714285714
	sheet->SetColWidth(5, 75.75, Math::Unit::Distance::DU_POINT);	// * 0.1904761905
	sheet->SetColWidth(6, 75.75, Math::Unit::Distance::DU_POINT);
	sheet->SetColWidth(7, 75.75, Math::Unit::Distance::DU_POINT);
	sheet->SetColWidth(8, 75.75, Math::Unit::Distance::DU_POINT);

	Exporter::XLSXExporter exporter;
	if (!exporter.ExportNewFile({fileName, (UOSInt)(sptr - fileName)}, wb, nullptr))
	{
		IO::ConsoleWriter console;
		console.WriteLine(CSTR("Error in writing to file"));
	}
}

void TestBorder()
{
	UTF8Char fileName[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = IO::Path::GetRealPath(fileName, UTF8STRC("~/Progs/Temp/BorderMe.xlsx"));
	Workbook wb;
	NN<Worksheet> sheet = wb.AddWorksheet(CSTR("Sheet1"));
	NN<WorkbookFont> font = wb.NewFont(CSTR("Arial"), 10.0, false);
	NN<CellStyle> borderStyle = wb.NewCellStyle(font, Text::HAlignment::Center, Text::VAlignment::Bottom, CSTR("0.0"));
	CellStyle::BorderStyle border(0xFF000000, BorderType::Medium);
	borderStyle->SetBorderBottom(border);
	NN<CellStyle> normalStyle = wb.NewCellStyle(font, Text::HAlignment::Center, Text::VAlignment::Bottom, CSTR("0.0"));
	sheet->SetCellInt32(0, 0, borderStyle.Ptr(), 1);
	sheet->SetCellInt32(0, 1, borderStyle.Ptr(), 2);
	sheet->SetCellInt32(0, 2, borderStyle.Ptr(), 3);
	sheet->SetCellInt32(0, 3, borderStyle.Ptr(), 4);
	sheet->SetCellInt32(1, 0, normalStyle.Ptr(), 5);
	sheet->SetCellInt32(1, 1, normalStyle.Ptr(), 6);
	sheet->SetCellInt32(1, 2, normalStyle.Ptr(), 7);
	sheet->SetCellInt32(1, 3, normalStyle.Ptr(), 8);

	Exporter::XLSXExporter exporter;
	if (!exporter.ExportNewFile({fileName, (UOSInt)(sptr - fileName)}, wb, nullptr))
	{
		IO::ConsoleWriter console;
		console.WriteLine(CSTR("Error in writing to file"));
	}
}

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	TestEmpty();
	TestChart();
	TestCols();
	TestColWidth();
	TestBorder();
	return 0;
}