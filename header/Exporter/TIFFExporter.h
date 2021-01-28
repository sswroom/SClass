#ifndef _SM_EXPORTER_TIFFEXPORTER
#define _SM_EXPORTER_TIFFEXPORTER
#include "IO/FileExporter.h"
#include "Media/EXIFData.h"

namespace Exporter
{
	class TIFFExporter : public IO::FileExporter
	{
	private:
		static void GenSubExifBuff(IO::SeekableStream *stm, Int64 buffOfst, Media::EXIFData *exif);
	public:
		TIFFExporter();
		virtual ~TIFFExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(IO::ParsedObject *pobj);
		virtual Bool GetOutputName(OSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual void SetCodePage(Int32 codePage);
		virtual Bool ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param);
	};
};
#endif
