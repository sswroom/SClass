#include "Stdafx.h"
#include "Core/Core.h"
#include "Exporter/ExcelXMLExporter.h"
#include "Exporter/XLSXExporter.h"
#include "IO/FileStream.h"
#include "Text/SpreadSheet/Workbook.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Exporter::ExcelXMLExporter exporterXML;
	Exporter::XLSXExporter exporterXLSX;
	Text::SpreadSheet::Workbook *workbook;
	IO::FileStream *fs;
	Text::CString fileName;
	NEW_CLASS(workbook, Text::SpreadSheet::Workbook());

	fileName = CSTR("Workbook.xml");
	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	exporterXML.ExportFile(fs, fileName.v, workbook, 0);
	DEL_CLASS(fs);

	fileName = CSTR("Workbook.xlsx");
	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	exporterXLSX.ExportFile(fs, fileName.v, workbook, 0);
	DEL_CLASS(fs);

	DEL_CLASS(workbook);
	return 0;
}
