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
		virtual SupportType IsObjectSupported(IO::ParsedObject *pobj);
		virtual Bool GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual Bool ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CStringNN fileName, IO::ParsedObject *pobj, void *param);

		Bool ExportPackage(IO::ZIPMTBuilder *zip, UTF8Char *buffStart, UTF8Char *buffEnd, IO::PackageFile *pkg);
	};
}
#endif
