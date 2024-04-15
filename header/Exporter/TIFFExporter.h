#ifndef _SM_EXPORTER_TIFFEXPORTER
#define _SM_EXPORTER_TIFFEXPORTER
#include "IO/FileExporter.h"
#include "Media/EXIFData.h"

namespace Exporter
{
	class TIFFExporter : public IO::FileExporter
	{
	private:
		static void GenSubExifBuff(NotNullPtr<IO::SeekableStream> stm, UInt64 buffOfst, NotNullPtr<Media::EXIFData> exif);
	public:
		TIFFExporter();
		virtual ~TIFFExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(NotNullPtr<IO::ParsedObject> pobj);
		virtual Bool GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual void SetCodePage(UInt32 codePage);
		virtual Bool ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CStringNN fileName, NotNullPtr<IO::ParsedObject> pobj, Optional<ParamData> param);
	};
}
#endif
