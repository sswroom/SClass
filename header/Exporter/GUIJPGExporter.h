#ifndef _SM_EXPORTER_GUIJPGEXPORTER
#define _SM_EXPORTER_GUIJPGEXPORTER
#include "Exporter/GUIExporter.h"

namespace Exporter
{
	class GUIJPGExporter : public Exporter::GUIExporter
	{
	public:
		GUIJPGExporter();
		virtual ~GUIJPGExporter();

		virtual Int32 GetName();
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
