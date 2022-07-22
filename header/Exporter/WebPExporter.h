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
		virtual SupportType IsObjectSupported(IO::ParsedObject *pobj);
		virtual Bool GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual Bool ExportFile(IO::SeekableStream *stm, Text::CString fileName, IO::ParsedObject *pobj, void *param);

		virtual UOSInt GetParamCnt();
		virtual void *CreateParam(IO::ParsedObject *pobj);
		virtual void DeleteParam(void *param);
		virtual Bool GetParamInfo(UOSInt index, ParamInfo *info);
		virtual Bool SetParamInt32(void *param, UOSInt index, Int32 val);
		virtual Int32 GetParamInt32(void *param, UOSInt index);
	};
}
#endif
