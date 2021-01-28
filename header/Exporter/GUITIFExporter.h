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
		virtual Bool GetOutputName(OSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual Bool ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param);

		virtual OSInt GetParamCnt();
		virtual void *CreateParam(IO::ParsedObject *pobj);
		virtual void DeleteParam(void *param);
		virtual Bool GetParamInfo(OSInt index, ParamInfo *info);
		virtual Bool SetParamBool(void *param, OSInt index, Bool val);
		virtual Bool GetParamBool(void *param, OSInt index);
	};
};
#endif
