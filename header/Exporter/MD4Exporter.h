#ifndef _SM_EXPORTER_MD4EXPORTER
#define _SM_EXPORTER_MD4EXPORTER
#include "IO/FileExporter.h"

namespace Exporter
{
	class MD4Exporter : public IO::FileExporter
	{
	private:
		Int32 codePage;
	public:
		MD4Exporter();
		virtual ~MD4Exporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(IO::ParsedObject *pobj);
		virtual Bool GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual void SetCodePage(UInt32 codePage);
		virtual Bool ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param);
	};
}
#endif
