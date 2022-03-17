#ifndef _SM_EXPORTER_MD5EXPORTER
#define _SM_EXPORTER_MD5EXPORTER
#include "IO/FileExporter.h"

namespace Exporter
{
	class MD5Exporter : public IO::FileExporter
	{
	private:
		UInt32 codePage;
	public:
		MD5Exporter();
		virtual ~MD5Exporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(IO::ParsedObject *pobj);
		virtual Bool GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual void SetCodePage(UInt32 codePage);
		virtual Bool ExportFile(IO::SeekableStream *stm, Text::CString fileName, IO::ParsedObject *pobj, void *param);
	};
}
#endif
