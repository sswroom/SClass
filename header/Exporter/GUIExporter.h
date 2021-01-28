#ifndef _SM_EXPORTER_GUIEXPORTER
#define _SM_EXPORTER_GUIEXPORTER
#include "IO/FileExporter.h"

namespace Exporter
{
	class GUIExporter : public IO::FileExporter
	{
	private:
		void *clsData;

	public:
		GUIExporter();
		virtual ~GUIExporter();

		virtual SupportType IsObjectSupported(IO::ParsedObject *pobj);

		void *ToImage(IO::ParsedObject *pobj, UInt8 **relBuff);
		Int32 GetEncoderClsid(const WChar *format, void *clsid);
	};
};
#endif
