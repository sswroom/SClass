#ifndef _SM_EXPORTER_PNGEXPORTER
#define _SM_EXPORTER_PNGEXPORTER
#include "IO/FileExporter.h"

namespace Exporter
{
	class PNGExporter : public IO::FileExporter
	{
	public:
		PNGExporter();
		virtual ~PNGExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(IO::ParsedObject *pobj);
		virtual Bool GetOutputName(OSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual Bool ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param);
	};
};
#endif
