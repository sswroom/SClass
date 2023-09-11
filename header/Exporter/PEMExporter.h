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
		virtual SupportType IsObjectSupported(IO::ParsedObject *pobj);
		virtual Bool GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual Bool ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CStringNN fileName, IO::ParsedObject *pobj, void *param);

		static Bool ExportStream(NotNullPtr<IO::SeekableStream> stm, Crypto::Cert::X509File *x509);
		static Bool ExportFile(Text::CStringNN fileName, Crypto::Cert::X509File *x509);
	};
}
#endif
