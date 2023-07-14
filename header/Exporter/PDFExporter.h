#ifndef _SM_EXPORTER_PDFEXPORTER
#define _SM_EXPORTER_PDFEXPORTER
#include "IO/FileExporter.h"

namespace Exporter
{
	class PDFExporter : public IO::FileExporter
	{
	public:
		PDFExporter();
		virtual ~PDFExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(IO::ParsedObject *pobj);
		virtual Bool GetOutputName(OSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual Bool ExportFile(NotNullPtr<IO::SeekableStream> stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param);
	};
};
#endif
