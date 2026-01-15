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
		virtual Bool GetOutputName(UIntOS index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff);
		virtual Bool ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param);
	};
}
#endif
