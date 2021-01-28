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
		virtual Bool GetOutputName(OSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual Bool ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param);

		virtual OSInt GetParamCnt();
		virtual void *CreateParam(IO::ParsedObject *pobj);
		virtual void DeleteParam(void *param);
		virtual Bool GetParamInfo(OSInt index, ParamInfo *info);
		virtual Bool SetParamInt32(void *param, OSInt index, Int32 val);
		virtual Int32 GetParamInt32(void *param, OSInt index);
	};
};
#endif
