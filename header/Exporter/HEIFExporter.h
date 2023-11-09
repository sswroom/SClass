#ifndef _SM_EXPORTER_HEIFEXPORTER
#define _SM_EXPORTER_HEIFEXPORTER
#include "IO/FileExporter.h"

namespace Exporter
{
	class HEIFExporter : public IO::FileExporter
	{
	public:
		HEIFExporter();
		virtual ~HEIFExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(NotNullPtr<IO::ParsedObject> pobj);
		virtual Bool GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual Bool ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CStringNN fileName, NotNullPtr<IO::ParsedObject> pobj, void *param);

		virtual UOSInt GetParamCnt();
		virtual void *CreateParam(NotNullPtr<IO::ParsedObject> pobj);
		virtual void DeleteParam(void *param);
		virtual Bool GetParamInfo(UOSInt index, NotNullPtr<ParamInfo> info);
		virtual Bool SetParamInt32(void *param, UOSInt index, Int32 val);
		virtual Int32 GetParamInt32(void *param, UOSInt index);
	};
}
#endif
