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
	const UTF8Char *fileName;
	NEW_CLASS(workbook, Text::SpreadSheet::Workbook());

	fileName = (const UTF8Char*)"Workbook.xml";
	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	exporterXML.ExportFile(fs, fileName, workbook, 0);
	DEL_CLASS(fs);

	fileName = (const UTF8Char*)"Workbook.xlsx";
	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	exporterXLSX.ExportFile(fs, fileName, workbook, 0);
	DEL_CLASS(fs);

	DEL_CLASS(workbook);
	return 0;
}
