#ifndef _SM_EXPORTER_GPXEXPORTER
#define _SM_EXPORTER_GPXEXPORTER
#include "IO/FileExporter.h"

namespace Exporter
{
	class GPXExporter : public IO::FileExporter
	{
	private:
		UInt32 codePage;
	public:
		GPXExporter();
		virtual ~GPXExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(NotNullPtr<IO::ParsedObject> pobj);
		virtual Bool GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual void SetCodePage(UInt32 codePage);
		virtual Bool ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CStringNN fileName, NotNullPtr<IO::ParsedObject> pobj, Optional<ParamData> param);
	};
}
#endif
