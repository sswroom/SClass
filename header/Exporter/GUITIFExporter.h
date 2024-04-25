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
		virtual Bool ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param);

		virtual UOSInt GetParamCnt();
		virtual Optional<ParamData> CreateParam(NN<IO::ParsedObject> pobj);
		virtual void DeleteParam(Optional<ParamData> param);
		virtual Bool GetParamInfo(UOSInt index, NN<ParamInfo> info);
		virtual Bool SetParamBool(Optional<ParamData> param, UOSInt index, Bool val);
		virtual Bool GetParamBool(Optional<ParamData> param, UOSInt index);
	};
}
#endif
