#ifndef _SM_EXPORTER_ICOEXPORTER
#define _SM_EXPORTER_ICOEXPORTER
#include "IO/FileExporter.h"
#include "Media/ImageList.h"

namespace Exporter
{
	class ICOExporter : public IO::FileExporter
	{
	public:
		ICOExporter();
		virtual ~ICOExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(IO::ParsedObject *pobj);
		virtual Bool GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual void SetCodePage(UInt32 codePage);
		virtual Bool ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CStringNN fileName, IO::ParsedObject *pobj, void *param);
	};
}
#endif
