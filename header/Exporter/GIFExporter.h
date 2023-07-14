#ifndef _SM_EXPORTER_GIFEXPORTER
#define _SM_EXPORTER_GIFEXPORTER
#include "IO/FileExporter.h"

namespace Exporter
{
	class GIFExporter : public IO::FileExporter
	{
	public:
		GIFExporter();
		virtual ~GIFExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(IO::ParsedObject *pobj);
		virtual Bool GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual void SetCodePage(UInt32 codePage);
		virtual Bool ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CString fileName, IO::ParsedObject *pobj, void *param);
	};
}
#endif
