#ifndef _SM_EXPORTER_ZIPEXPORTER
#define _SM_EXPORTER_ZIPEXPORTER
#include "IO/FileExporter.h"
#include "IO/PackageFile.h"
#include "IO/ZIPMTBuilder.h"

namespace Exporter
{
	class ZIPExporter : public IO::FileExporter
	{
	public:
		ZIPExporter();
		virtual ~ZIPExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(NN<IO::ParsedObject> pobj);
		virtual Bool GetOutputName(UOSInt index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff);
		virtual Bool ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param);

		Bool ExportPackage(NN<IO::ZIPMTBuilder> zip, UnsafeArray<UTF8Char> buffStart, UnsafeArray<UTF8Char> buffEnd, NN<IO::PackageFile> pkg);
	};
}
#endif
