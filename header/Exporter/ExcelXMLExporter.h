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
		virtual SupportType IsObjectSupported(NN<IO::ParsedObject> pobj);
		virtual Bool GetOutputName(UIntOS index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff);
		virtual void SetCodePage(UInt32 codePage);
		virtual Bool ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param);

	private:
		static void WriteBorderStyle(NN<IO::Writer> writer, UnsafeArray<const UTF8Char> position, Text::SpreadSheet::CellStyle::BorderStyle border);
	};
}
#endif
