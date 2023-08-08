#include "Stdafx.h"
#include "Core/Core.h"
#include "Exporter/ExcelXMLExporter.h"
#include "Exporter/XLSXExporter.h"
#include "IO/FileStream.h"
#include "Text/SpreadSheet/Workbook.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	Exporter::ExcelXMLExporter exporterXML;
	Exporter::XLSXExporter exporterXLSX;
	Text::CStringNN fileName;
	Text::SpreadSheet::Workbook workbook;

	fileName = CSTR("Workbook.xml");
	{
		IO::FileStream fs(fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		exporterXML.ExportFile(fs, fileName, &workbook, 0);
	}

	fileName = CSTR("Workbook.xlsx");
	{
		IO::FileStream fs(fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		exporterXLSX.ExportFile(fs, fileName, &workbook, 0);
	}
	return 0;
}
