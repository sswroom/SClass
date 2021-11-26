#include "Stdafx.h"
#include "Core/Core.h"
#include "Exporter/XLSXExporter.h"
#include "IO/ConsoleWriter.h"
#include "IO/Path.h"
#include "Math/Math.h"
#include "Math/Unit/Distance.h"
#include "Text/SpreadSheet/Workbook.h"

using namespace Text::SpreadSheet;

Int32 MyMain(Core::IProgControl *progCtrl)
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
		dataSheet->SetCellDouble(0, i + 1, numStyle, 112.0 + Math::UOSInt2Double(i) * 0.1);
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
					dataSheet->SetCellDouble(rowNum, i + 1, numStyle, Math::UOSInt2Double(i) * 0.1);
				}
				i++;
			}
			WorkbookDataSource *chainageSource = NEW_CLASS_D(WorkbookDataSource(dataSheet, 0, 0, 1, j));
			WorkbookDataSource *valSource = NEW_CLASS_D(WorkbookDataSource(dataSheet, rowNum, rowNum, 1, j));
			dt.ToString(sbuff2, "yyyy-MM-dd");
			chart->AddSeries(chainageSource, valSource, sbuff2, testRowCnt > 1);
		}
	//	chart.plot(lineChartData);
	}

	Exporter::XLSXExporter exporter;
	if (!exporter.ExportNewFile(fileName, wb, 0))
	{
		IO::ConsoleWriter console;
		console.WriteLine((const UTF8Char*)"Error in writing to file");
	}
	DEL_CLASS(wb);
	return 0;
}