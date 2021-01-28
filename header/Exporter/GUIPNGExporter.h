#ifndef _SM_EXPORTER_GUIPNGEXPORTER
#define _SM_EXPORTER_GUIPNGEXPORTER
#include "Exporter/GUIExporter.h"

namespace Exporter
{
	class GUIPNGExporter : public Exporter::GUIExporter
	{
	public:
		GUIPNGExporter();
		virtual ~GUIPNGExporter();

		virtual Int32 GetName();
		virtual Bool GetOutputName(OSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual Bool ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param);
	};
};
#endif
