#ifndef _SM_EXPORTER_PPKEXPORTER
#define _SM_EXPORTER_PPKEXPORTER
#include "IO/FileExporter.h"

namespace Exporter
{
	class PPKExporter : public IO::FileExporter
	{
	public:
		PPKExporter();
		virtual ~PPKExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(NN<IO::ParsedObject> pobj);
		virtual Bool GetOutputName(UOSInt index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff);
		virtual Bool ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param);
	};
}
#endif
