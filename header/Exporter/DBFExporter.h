#ifndef _SM_EXPORTER_DBFEXPORTER
#define _SM_EXPORTER_DBFEXPORTER
#include "IO/FileExporter.h"

namespace Exporter
{
	class DBFExporter : public IO::FileExporter
	{
	private:
		Int32 codePage;

	public:
		DBFExporter();
		virtual ~DBFExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(IO::ParsedObject *pobj);
		virtual Bool GetOutputName(OSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual void SetCodePage(Int32 codePage);
		virtual Bool ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param);
	};
};
#endif
