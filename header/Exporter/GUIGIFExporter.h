#ifndef _SM_EXPORTER_GUIGIFEXPORTER
#define _SM_EXPORTER_GUIGIFEXPORTER
#include "Exporter/GUIExporter.h"

namespace Exporter
{
	class GUIGIFExporter : public Exporter::GUIExporter
	{
	public:
		GUIGIFExporter();
		virtual ~GUIGIFExporter();

		virtual Int32 GetName();
		virtual Bool GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual Bool ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param);
	};
}
#endif
