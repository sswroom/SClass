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
		virtual SupportType IsObjectSupported(NN<IO::ParsedObject> pobj);
		virtual Bool GetOutputName(UIntOS index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff);
		virtual void SetCodePage(UInt32 codePage);
		virtual Bool ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param);
	};
}
#endif
