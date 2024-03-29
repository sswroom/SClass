#ifndef _SM_EXPORTER_SFVEXPORTER
#define _SM_EXPORTER_SFVEXPORTER
#include "IO/FileExporter.h"

namespace Exporter
{
	class SFVExporter : public IO::FileExporter
	{
	public:
		SFVExporter();
		virtual ~SFVExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(NotNullPtr<IO::ParsedObject> pobj);
		virtual Bool GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual Bool ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CStringNN fileName, NotNullPtr<IO::ParsedObject> pobj, Optional<ParamData> param);
	};
}
#endif
