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
		virtual SupportType IsObjectSupported(IO::ParsedObject *pobj);
		virtual Bool GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual Bool ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CString fileName, IO::ParsedObject *pobj, void *param);

		static UTF8Char *FixDouble(UTF8Char *sbuff, Double val, const Char *format, Int32 colSize);
	};
}
#endif
