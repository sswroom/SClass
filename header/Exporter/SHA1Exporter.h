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
		virtual SupportType IsObjectSupported(NN<IO::ParsedObject> pobj);
		virtual Bool GetOutputName(UOSInt index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff);
		virtual void SetCodePage(UInt32 codePage);
		virtual Bool ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param);
	};
}
#endif
