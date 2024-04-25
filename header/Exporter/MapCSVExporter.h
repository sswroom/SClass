#ifndef _SM_EXPORTER_MAPCSVEXPORTER
#define _SM_EXPORTER_MAPCSVEXPORTER
#include "IO/FileExporter.h"

namespace Exporter
{
	class MapCSVExporter : public IO::FileExporter
	{
	private:
		UInt32 codePage;
	public:
		MapCSVExporter();
		virtual ~MapCSVExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(NN<IO::ParsedObject> pobj);
		virtual Bool GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual void SetCodePage(UInt32 codePage);
		virtual Bool ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param);
	};
}
#endif
