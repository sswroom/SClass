#ifndef _SM_EXPORTER_SHA1EXPORTER
#define _SM_EXPORTER_SHA1EXPORTER
#include "IO/FileExporter.h"

namespace Exporter
{
	class SHA1Exporter : public IO::FileExporter
	{
	private:
		UInt32 codePage;
	public:
		SHA1Exporter();
		virtual ~SHA1Exporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(IO::ParsedObject *pobj);
		virtual Bool GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual void SetCodePage(UInt32 codePage);
		virtual Bool ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CStringNN fileName, IO::ParsedObject *pobj, void *param);
	};
}
#endif
