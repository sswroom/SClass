#ifndef _SM_EXPORTER_GUIEXPORTER
#define _SM_EXPORTER_GUIEXPORTER
#include "AnyType.h"
#include "IO/FileExporter.h"

namespace Exporter
{
	class GUIExporter : public IO::FileExporter
	{
	private:
		struct ClassData;
		NN<ClassData> clsData;

	public:
		GUIExporter();
		virtual ~GUIExporter();

		virtual SupportType IsObjectSupported(NN<IO::ParsedObject> pobj);

		AnyType ToImage(NN<IO::ParsedObject> pobj, OutParam<UInt8 *> relBuff);
		Int32 GetEncoderClsid(UnsafeArray<const WChar> format, void *clsid);
	};
}
#endif
