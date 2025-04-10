#ifndef _SM_EXPORTER_SFVEXPORTER
#define _SM_EXPORTER_SFVEXPORTER
#include "IO/FileExporter.h"

namespace Exporter
{
	class SFVExporter : public IO::FileExporter
	{
	public:
		SFVExporter();
		virtual ~SFVExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(NN<IO::ParsedObject> pobj);
		virtual Bool GetOutputName(UOSInt index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff);
		virtual Bool ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param);
	};
}
#endif
