#ifndef _SM_EXPORTER_MDBEXPORTER
#define _SM_EXPORTER_MDBEXPORTER
#include "IO/FileExporter.h"

namespace Exporter
{
	class MDBExporter : public IO::FileExporter
	{
	public:
		MDBExporter();
		virtual ~MDBExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(IO::ParsedObject *pobj);
		virtual Bool GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual Bool ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CString fileName, IO::ParsedObject *pobj, void *param);
	};
}
#endif
