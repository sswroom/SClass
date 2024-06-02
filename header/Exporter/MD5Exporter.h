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
		virtual SupportType IsObjectSupported(NN<IO::ParsedObject> pobj);
		virtual Bool GetOutputName(UOSInt index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff);
		virtual void SetCodePage(UInt32 codePage);
		virtual Bool ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param);
	};
}
#endif
