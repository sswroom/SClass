#ifndef _SM_EXPORTER_PEMEXPORTER
#define _SM_EXPORTER_PEMEXPORTER
#include "Crypto/Cert/X509File.h"
#include "IO/FileExporter.h"

namespace Exporter
{
	class PEMExporter : public IO::FileExporter
	{
	public:
		PEMExporter();
		virtual ~PEMExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(NN<IO::ParsedObject> pobj);
		virtual Bool GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual Bool ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param);

		static Bool ExportStream(NN<IO::SeekableStream> stm, NN<Crypto::Cert::X509File> x509);
		static Bool ExportFile(Text::CStringNN fileName, NN<Crypto::Cert::X509File> x509);
	};
}
#endif
