#include "Stdafx.h"
#include "Core/Core.h"
#include "Exporter/XLSXExporter.h"
#include "IO/Path.h"
#include "Math/Math.h"
#include "Text/SpreadSheet/Workbook.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UOSInt testRowCnt = 2;
	UTF8Char sbuff[512];
	IO::Path::GetRealPath(sbuff, (const UTF8Char*)"~/Progs/Temp/XLSXTest.xlsx");

	Text::SpreadSheet::Workbook *wb;
	NEW_CLASS(wb, Text::SpreadSheet::Workbook());
	Text::SpreadSheet::WorkbookFont *font10 = wb->NewFont((const UTF8Char*)"Arial", 10, false);
	Text::SpreadSheet::CellStyle *dateStyle = wb->NewCellStyle(font10, Text::SpreadSheet::HAlignment::Left, Text::SpreadSheet::VAlignment::Center, (const UTF8Char*)"yyyy-MM-dd");
	Text::SpreadSheet::CellStyle *numStyle = wb->NewCellStyle(font10, Text::SpreadSheet::HAlignment::Left, Text::SpreadSheet::VAlignment::Center, (const UTF8Char*)"0.###");
	Text::SpreadSheet::Worksheet *graphSheet = wb->AddWorksheet();
	Text::SpreadSheet::Worksheet *dataSheet = wb->AddWorksheet();
/*	XSSFChart chart = XlsxUtil.createChart(graphSheet, DistanceUnit.Inch, 0.64, 1.61, 13.10, 5.53, "SETTLEMENT VS CHAINAGE");
	XDDFLineChartData lineChartData = XlsxUtil.lineChart(chart, "ACCUMULATED SETTLEMENT", "CHAINAGE", AxisType.AT_CATEGORY);
	if (testRowCnt > 1)
	{
		XlsxUtil.chartAddLegend(chart, LegendPosition.BOTTOM);
	}*/

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
	exporter.ExportNewFile(sbuff, wb, 0);
	DEL_CLASS(wb);
	return 0;
}