#ifndef _SM_EXPORTER_TIFFEXPORTER
#define _SM_EXPORTER_TIFFEXPORTER
#include "IO/FileExporter.h"
#include "Media/EXIFData.h"

namespace Exporter
{
	class TIFFExporter : public IO::FileExporter
	{
	private:
		static void GenSubExifBuff(IO::SeekableStream *stm, UInt64 buffOfst, Media::EXIFData *exif);
	public:
		TIFFExporter();
		virtual ~TIFFExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(IO::ParsedObject *pobj);
		virtual Bool GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual void SetCodePage(UInt32 codePage);
		virtual Bool ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CString fileName, IO::ParsedObject *pobj, void *param);
	};
}
#endif
