#ifndef _SM_EXPORTER_DBEXCELXMLEXPORTER
#define _SM_EXPORTER_DBEXCELXMLEXPORTER
#include "IO/FileExporter.h"
#include "DB/ReadingDB.h"

namespace Exporter
{
	class DBExcelXMLExporter : public IO::FileExporter
	{
	private:
		Int32 codePage;

	public:
		DBExcelXMLExporter();
		virtual ~DBExcelXMLExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(IO::ParsedObject *pobj);
		virtual Bool GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual void SetCodePage(UInt32 codePage);
		virtual Bool ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param);
	};
}
#endif
