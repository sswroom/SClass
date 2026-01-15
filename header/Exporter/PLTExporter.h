#ifndef _SM_EXPORTER_PLTEXPORTER
#define _SM_EXPORTER_PLTEXPORTER
#include "IO/FileExporter.h"

namespace Exporter
{
	class PLTExporter : public IO::FileExporter
	{
	public:
		PLTExporter();
		virtual ~PLTExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(NN<IO::ParsedObject> pobj);
		virtual Bool GetOutputName(UIntOS index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff);
		virtual Bool ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param);

		static UnsafeArray<UTF8Char> FixDouble(UnsafeArray<UTF8Char> sbuff, Double val, const Char *format, Int32 colSize);
	};
}
#endif
