#include "Stdafx.h"
#include "Core/Core.h"
#include "Exporter/ExcelXMLExporter.h"
#include "Exporter/XLSXExporter.h"
#include "IO/FileStream.h"
#include "Text/SpreadSheet/Workbook.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	Exporter::ExcelXMLExporter exporterXML;
	Exporter::XLSXExporter exporterXLSX;
	Text::CStringNN fileName;
	Text::SpreadSheet::Workbook workbook;
	NN<Text::SpreadSheet::Worksheet> sheet = workbook.AddWorksheet(CSTR("Sheet"));
	Text::SpreadSheet::CellStyle style(0);
	style.SetDataFormat(CSTR("0.0"));
	sheet->SetCellDouble(0, 0, workbook.FindOrCreateStyle(style).Ptr(), 0.123456);
	Data::Timestamp ts = Data::Timestamp::Now();
	style.SetDataFormat(CSTR("YYYY-MM-DD"));
	sheet->SetCellTS(0, 1, workbook.FindOrCreateStyle(style).Ptr(), ts);
	style.SetDataFormat(CSTR("HH:MM:SS.000"));
	sheet->SetCellTS(0, 2, workbook.FindOrCreateStyle(style).Ptr(), ts);

	fileName = CSTR("Workbook.xml");
	{
		IO::FileStream fs(fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		exporterXML.ExportFile(fs, fileName, workbook, 0);
	}

	fileName = CSTR("Workbook.xlsx");
	{
		IO::FileStream fs(fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		exporterXLSX.ExportFile(fs, fileName, workbook, 0);
	}
	return 0;
}
