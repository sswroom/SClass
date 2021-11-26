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
	UTF8Char sbuff[512];
	IO::Path::GetRealPath(sbuff, (const UTF8Char*)"~/Progs/Temp/XLSXTest.xlsx");

	Workbook *wb;
	NEW_CLASS(wb, Workbook());
	WorkbookFont *font10 = wb->NewFont((const UTF8Char*)"Arial", 10, false);
	CellStyle *dateStyle = wb->NewCellStyle(font10, HAlignment::Left, VAlignment::Center, (const UTF8Char*)"yyyy-MM-dd");
	CellStyle *numStyle = wb->NewCellStyle(font10, HAlignment::Left, VAlignment::Center, (const UTF8Char*)"0.###");
	Worksheet *graphSheet = wb->AddWorksheet();
	Worksheet *dataSheet = wb->AddWorksheet();
	OfficeChart *chart = graphSheet->CreateChart(Math::Unit::Distance::DU_INCH, 0.64, 1.61, 13.10, 5.53, (const UTF8Char*)"SETTLEMENT VS CHAINAGE");

//	XDDFLineChartData lineChartData = XlsxUtil.lineChart(chart, "ACCUMULATED SETTLEMENT", "CHAINAGE", AxisType.AT_CATEGORY);
	if (testRowCnt > 1)
	{
		chart->AddLegend(LegendPos::Bottom);
	}

	UOSInt rowNum;
	Data::DateTime dt;
	dataSheet->SetCellString(0, 0, dateStyle, (const UTF8Char*)"Date");
	UOSInt i = 0;
	UOSInt j = 10;
	while (i < j)
	{
		dataSheet->SetCellDouble(0, i + 1, numStyle, 112.0 + Math::UOSInt2Double(i) * 0.1);
		i++;
	}
//	XDDFCategoryDataSource chainageSource = XDDFDataSourcesFactory.fromStringCellRange((XSSFSheet)dataSheet, new CellRangeAddress(0, 0, 1, j));

//	SimpleDateFormat dateFmt = new SimpleDateFormat("yyyy-MM-dd");
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
			dataSheet->SetCellDouble(rowNum, i + 1, numStyle, Math::UOSInt2Double(i) * 0.1);
			i++;
		}
//		XDDFNumericalDataSource<Double> valSource = XDDFDataSourcesFactory.fromNumericCellRange((XSSFSheet)dataSheet, new CellRangeAddress(rowNum, rowNum, 1, j));
//		XlsxUtil.addLineChartSeries(lineChartData, chainageSource, valSource, dateFmt.format(ts), testRowCnt > 1);
	}
//	chart.plot(lineChartData);

	Exporter::XLSXExporter exporter;
	if (!exporter.ExportNewFile(sbuff, wb, 0))
	{
		IO::ConsoleWriter console;
		console.WriteLine((const UTF8Char*)"Error in writing to file");
	}
	DEL_CLASS(wb);
	return 0;
}