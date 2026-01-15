#ifndef _SM_EXPORTER_SSHPUBEXPORTER
#define _SM_EXPORTER_SSHPUBEXPORTER
#include "IO/FileExporter.h"

namespace Exporter
{
	class SSHPubExporter : public IO::FileExporter
	{
	public:
		SSHPubExporter();
		virtual ~SSHPubExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(NN<IO::ParsedObject> pobj);
		virtual Bool GetOutputName(UIntOS index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff);
		virtual Bool ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param);
	};
}
#endif
