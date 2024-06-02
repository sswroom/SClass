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
		virtual SupportType IsObjectSupported(NN<IO::ParsedObject> pobj);
		virtual Bool GetOutputName(OSInt index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff);
		virtual Bool ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param);
	};
};
#endif
