#ifndef _SM_EXPORTER_GUITIFEXPORTER
#define _SM_EXPORTER_GUITIFEXPORTER
#include "Exporter/GUIExporter.h"

namespace Exporter
{
	class GUITIFExporter : public Exporter::GUIExporter
	{
	public:
		GUITIFExporter();
		virtual ~GUITIFExporter();

		virtual Int32 GetName();
		virtual Bool GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual Bool ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CStringNN fileName, NotNullPtr<IO::ParsedObject> pobj, void *param);

		virtual UOSInt GetParamCnt();
		virtual void *CreateParam(NotNullPtr<IO::ParsedObject> pobj);
		virtual void DeleteParam(void *param);
		virtual Bool GetParamInfo(UOSInt index, NotNullPtr<ParamInfo> info);
		virtual Bool SetParamBool(void *param, UOSInt index, Bool val);
		virtual Bool GetParamBool(void *param, UOSInt index);
	};
}
#endif
