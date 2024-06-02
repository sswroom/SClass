#ifndef _SM_EXPORTER_TIFFEXPORTER
#define _SM_EXPORTER_TIFFEXPORTER
#include "IO/FileExporter.h"
#include "Media/EXIFData.h"

namespace Exporter
{
	class TIFFExporter : public IO::FileExporter
	{
	private:
		static void GenSubExifBuff(NN<IO::SeekableStream> stm, UInt64 buffOfst, NN<Media::EXIFData> exif);
	public:
		TIFFExporter();
		virtual ~TIFFExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(NN<IO::ParsedObject> pobj);
		virtual Bool GetOutputName(UOSInt index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff);
		virtual void SetCodePage(UInt32 codePage);
		virtual Bool ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param);
	};
}
#endif
