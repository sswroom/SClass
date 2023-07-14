#ifndef _SM_EXPORTER_SQLITEEXPORTER
#define _SM_EXPORTER_SQLITEEXPORTER
#include "IO/FileExporter.h"

namespace Exporter
{
	class SQLiteExporter : public IO::FileExporter
	{
	public:
		SQLiteExporter();
		virtual ~SQLiteExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(IO::ParsedObject *pobj);
		virtual Bool GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual Bool ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CString fileName, IO::ParsedObject *pobj, void *param);
	};
}
#endif
