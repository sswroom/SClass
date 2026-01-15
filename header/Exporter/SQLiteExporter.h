#ifndef _SM_EXPORTER_SQLITEEXPORTER
#define _SM_EXPORTER_SQLITEEXPORTER
#include "IO/FileExporter.h"

namespace Exporter
{
	class SQLiteExporter : public IO::FileExporter
	{
	public:
		SQLiteExporter();
		virtual ~SQLiteExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(NN<IO::ParsedObject> pobj);
		virtual Bool GetOutputName(UIntOS index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff);
		virtual Bool ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param);
	};
}
#endif
