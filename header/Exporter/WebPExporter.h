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
		virtual Bool GetOutputName(UOSInt index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff);
		virtual Bool ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param);

		virtual UOSInt GetParamCnt();
		virtual Optional<ParamData> CreateParam(NN<IO::ParsedObject> pobj);
		virtual void DeleteParam(Optional<ParamData> param);
		virtual Bool GetParamInfo(UOSInt index, NN<ParamInfo> info);
		virtual Bool SetParamInt32(Optional<ParamData> param, UOSInt index, Int32 val);
		virtual Int32 GetParamInt32(Optional<ParamData> param, UOSInt index);
	};
}
#endif
