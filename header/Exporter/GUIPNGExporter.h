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
		virtual Bool GetOutputName(UOSInt index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff);
		virtual Bool ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param);
	};
}
#endif
