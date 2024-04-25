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
		virtual SupportType IsObjectSupported(NN<IO::ParsedObject> pobj);
		virtual Bool GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual Bool ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param);
	};
}
#endif
