#ifndef _SM_EXPORTER_XLSXEXPORTER
#define _SM_EXPORTER_XLSXEXPORTER
#include "Data/DateTime.h"
#include "IO/FileExporter.h"

namespace Exporter
{
	class XLSXExporter : public IO::FileExporter
	{
	public:
		XLSXExporter();
		virtual ~XLSXExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(IO::ParsedObject *pobj);
		virtual Bool GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual Bool ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param);

		static Double Date2Number(Data::DateTime *dt);
		static void Number2Date(Data::DateTime *dt, Double v);
	};
}
#endif
