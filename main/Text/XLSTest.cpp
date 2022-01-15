#include "Stdafx.h"
#include "Core/Core.h"
#include "Exporter/XLSXExporter.h"
#include "IO/ConsoleWriter.h"
#include "IO/Path.h"
#include "Math/Math.h"
#include "Math/Unit/Distance.h"
#include "Text/MyStringFloat.h"
#include "Text/SpreadSheet/Workbook.h"

using namespace Text::SpreadSheet;

void TestEmpty()
{
	UTF8Char fileName[512];
	IO::Path::GetRealPath(fileName, (const UTF8Char*)"~/Progs/Temp/EmptyMe.xlsx");
	Workbook *wb;
	NEW_CLASS(wb, Workbook());
	wb->AddWorksheet((const UTF8Char*)"Sheet1");
	wb->AddWorksheet((const UTF8Char*)"Sheet2");
	Exporter::XLSXExporter exporter;
	if (!exporter.ExportNewFile(fileName, wb, 0))
	{
		IO::ConsoleWriter console;
		console.WriteLineC(UTF8STRC("Error in writing to file"));
	}
	DEL_CLASS(wb);
}

void TestChart()
{
	UOSInt testRowCnt = 2;
	UTF8Char fileName[512];
	UTF8Char sbuff2[32];
	IO::Path::GetRealPath(fileName, (const UTF8Char*)"~/Progs/Temp/XLSXTest.xlsx");

	Workbook *wb;
	NEW_CLASS(wb, Workbook());
	WorkbookFont *font10 = wb->NewFont((const UTF8Char*)"Arial", 10, false);
	CellStyle *dateStyle = wb->NewCellStyle(font10, HAlignment::Left, VAlignment::Center, (const UTF8Char*)"yyyy-MM-dd");
	CellStyle *numStyle = wb->NewCellStyle(font10, HAlignment::Left, VAlignment::Center, (const UTF8Char*)"0.###");
	Worksheet *graphSheet = wb->AddWorksheet();
	Worksheet *dataSheet = wb->AddWorksheet();
	OfficeChart *chart = graphSheet->CreateChart(Math::Unit::Distance::DU_INCH, 0.64, 1.61, 13.10, 5.53, (const UTF8Char*)"\nSETTLEMENT VS CHAINAGE");
	chart->InitLineChart((const UTF8Char*)"ACCUMULATED SETTLEMENT", (const UTF8Char*)"CHAINAGE", AxisType::Category);
	chart->SetDisplayBlankAs(BlankAs::Gap);
	if (testRowCnt > 1)
	{
		chart->AddLegend(LegendPos::Bottom);
	}

	UOSInt rowNum;
	Data::DateTime dt;
	dataSheet->SetCellString(0, 0, dateStyle, (const UTF8Char*)"Date");
	UOSInt i = 0;
	UOSInt j = 20;
	while (i < j)
	{
		dataSheet->SetCellDouble(0, i + 1, numStyle, 112.0 + UOSInt2Double(i) * 0.1);
		
		sbuff2[0] = (UTF8Char)('A' + i);
		sbuff2[1] = 0;
		graphSheet->SetCellString(0, i, wb->GetStyle(0), sbuff2);
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
			dataSheet->SetCellDate(rowNum, 0, dateStyle, &dt);
			i = 0;
			while (i < j)
			{
				if (i != 5)
				{
					dataSheet->SetCellDouble(rowNum, i + 1, numStyle, UOSInt2Double(i) * 0.1);
				}
				i++;
			}
			WorkbookDataSource *chainageSource = NEW_CLASS_D(WorkbookDataSource(dataSheet, 0, 0, 1, j));
			WorkbookDataSource *valSource = NEW_CLASS_D(WorkbookDataSource(dataSheet, rowNum, rowNum, 1, j));
			dt.ToString(sbuff2, "yyyy-MM-dd");
			chart->AddSeries(chainageSource, valSource, sbuff2, testRowCnt > 1);
		}
	}

	Exporter::XLSXExporter exporter;
	if (!exporter.ExportNewFile(fileName, wb, 0))
	{
		IO::ConsoleWriter console;
		console.WriteLineC(UTF8STRC("Error in writing to file"));
	}
	DEL_CLASS(wb);
}

void TestCols()
{
	UTF8Char fileName[512];
	IO::Path::GetRealPath(fileName, (const UTF8Char*)"~/Progs/Temp/ColsMe.xlsx");
	Workbook *wb;
	NEW_CLASS(wb, Workbook());
	WorkbookFont *font10 = wb->NewFont((const UTF8Char*)"Arial", 10, false);
	CellStyle *numStyle = wb->NewCellStyle(font10, HAlignment::Left, VAlignment::Center, (const UTF8Char*)"0.###");
	Worksheet *sheet = wb->AddWorksheet((const UTF8Char*)"Sheet1");
	UOSInt i = 0;
	UOSInt j = 2000;
	while (i < j)
	{
		sheet->SetCellDouble(0, i + 1, numStyle, 112.0 + UOSInt2Double(i) * 0.1);

		i++;
	}

	Exporter::XLSXExporter exporter;
	if (!exporter.ExportNewFile(fileName, wb, 0))
	{
		IO::ConsoleWriter console;
		console.WriteLineC(UTF8STRC("Error in writing to file"));
	}
	DEL_CLASS(wb);
}

void TestColWidth()
{
	UTF8Char fileName[512];
	IO::Path::GetRealPath(fileName, (const UTF8Char*)"~/Progs/Temp/ColWidthMe.xlsx");
	Workbook *wb;
	NEW_CLASS(wb, Workbook());
	Worksheet *sheet = wb->AddWorksheet((const UTF8Char*)"Sheet1");
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
	if (!exporter.ExportNewFile(fileName, wb, 0))
	{
		IO::ConsoleWriter console;
		console.WriteLineC(UTF8STRC("Error in writing to file"));
	}
	DEL_CLASS(wb);
}

void TestBorder()
{
	UTF8Char fileName[512];
	IO::Path::GetRealPath(fileName, (const UTF8Char*)"~/Progs/Temp/BorderMe.xlsx");
	Workbook *wb;
	NEW_CLASS(wb, Workbook());
	Worksheet *sheet = wb->AddWorksheet((const UTF8Char*)"Sheet1");
	WorkbookFont *font = wb->NewFont((const UTF8Char*)"Arial", 10.0, false);
	CellStyle *borderStyle = wb->NewCellStyle(font, HAlignment::Center, VAlignment::Bottom, (const UTF8Char*)"0.0");
	CellStyle::BorderStyle border;
	border.borderType = BorderType::Medium;
	border.borderColor = 0xFF000000;
	borderStyle->SetBorderBottom(&border);
	CellStyle *normalStyle = wb->NewCellStyle(font, HAlignment::Center, VAlignment::Bottom, (const UTF8Char*)"0.0");
	sheet->SetCellInt32(0, 0, borderStyle, 1);
	sheet->SetCellInt32(0, 1, borderStyle, 2);
	sheet->SetCellInt32(0, 2, borderStyle, 3);
	sheet->SetCellInt32(0, 3, borderStyle, 4);
	sheet->SetCellInt32(1, 0, normalStyle, 5);
	sheet->SetCellInt32(1, 1, normalStyle, 6);
	sheet->SetCellInt32(1, 2, normalStyle, 7);
	sheet->SetCellInt32(1, 3, normalStyle, 8);

	Exporter::XLSXExporter exporter;
	if (!exporter.ExportNewFile(fileName, wb, 0))
	{
		IO::ConsoleWriter console;
		console.WriteLineC(UTF8STRC("Error in writing to file"));
	}
	DEL_CLASS(wb);
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	TestEmpty();
	TestChart();
	TestCols();
	TestColWidth();
	TestBorder();
	return 0;
}