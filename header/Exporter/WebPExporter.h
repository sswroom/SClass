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
		virtual SupportType IsObjectSupported(NotNullPtr<IO::ParsedObject> pobj);
		virtual Bool GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual Bool ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CStringNN fileName, NotNullPtr<IO::ParsedObject> pobj, Optional<ParamData> param);

		virtual UOSInt GetParamCnt();
		virtual Optional<ParamData> CreateParam(NotNullPtr<IO::ParsedObject> pobj);
		virtual void DeleteParam(Optional<ParamData> param);
		virtual Bool GetParamInfo(UOSInt index, NotNullPtr<ParamInfo> info);
		virtual Bool SetParamInt32(Optional<ParamData> param, UOSInt index, Int32 val);
		virtual Int32 GetParamInt32(Optional<ParamData> param, UOSInt index);
	};
}
#endif
