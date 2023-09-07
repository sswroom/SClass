#ifndef _SM_EXPORTER_EXCELXMLEXPORTER
#define _SM_EXPORTER_EXCELXMLEXPORTER
#include "IO/FileExporter.h"
#include "IO/Writer.h"
#include "Text/SpreadSheet/CellStyle.h"

namespace Exporter
{
	class ExcelXMLExporter : public IO::FileExporter
	{
	private:
		UInt32 codePage;
	public:
		ExcelXMLExporter();
		virtual ~ExcelXMLExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(IO::ParsedObject *pobj);
		virtual Bool GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual void SetCodePage(UInt32 codePage);
		virtual Bool ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CStringNN fileName, IO::ParsedObject *pobj, void *param);

	private:
		static void WriteBorderStyle(IO::Writer *writer, const UTF8Char *position, Text::SpreadSheet::CellStyle::BorderStyle border);
	};
}
#endif
