#ifndef _SM_EXPORTER_ORUXMAPEXPORTER
#define _SM_EXPORTER_ORUXMAPEXPORTER
#include "IO/FileExporter.h"

namespace Exporter
{
	class OruxMapExporter : public IO::FileExporter
	{
	public:
		OruxMapExporter();
		virtual ~OruxMapExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(IO::ParsedObject *pobj);
		virtual Bool GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual Bool ExportFile(IO::SeekableStream *stm, Text::CString fileName, IO::ParsedObject *pobj, void *param);
	};
}
#endif
