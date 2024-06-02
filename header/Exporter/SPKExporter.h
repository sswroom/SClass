#ifndef _SM_EXPORTER_SPKEXPORTER
#define _SM_EXPORTER_SPKEXPORTER
#include "IO/FileExporter.h"
#include "IO/PackageFile.h"
#include "IO/SPackageFile.h"

namespace Exporter
{
	class SPKExporter : public IO::FileExporter
	{
	public:
		SPKExporter();
		virtual ~SPKExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(NN<IO::ParsedObject> pobj);
		virtual Bool GetOutputName(UOSInt index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff);
		virtual void SetCodePage(UInt32 codePage);
		virtual Bool ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param);

	private:
		void ExportPackageFile(IO::SPackageFile *spkg, NN<IO::PackageFile> pkgFile, UnsafeArray<UTF8Char> buff, UnsafeArray<UTF8Char> buffEnd);
	};
}
#endif
