#ifndef _SM_EXPORTER_WEBPEXPORTER
#define _SM_EXPORTER_WEBPEXPORTER
#include "IO/FileExporter.h"

namespace Exporter
{
	class WebPExporter : public IO::FileExporter
	{
	public:
		WebPExporter();
		virtual ~WebPExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(NN<IO::ParsedObject> pobj);
		virtual Bool GetOutputName(UIntOS index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff);
		virtual Bool ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param);

		virtual UIntOS GetParamCnt();
		virtual Optional<ParamData> CreateParam(NN<IO::ParsedObject> pobj);
		virtual void DeleteParam(Optional<ParamData> param);
		virtual Bool GetParamInfo(UIntOS index, NN<ParamInfo> info);
		virtual Bool SetParamInt32(Optional<ParamData> param, UIntOS index, Int32 val);
		virtual Int32 GetParamInt32(Optional<ParamData> param, UIntOS index);
	};
}
#endif
