#ifndef _SM_EXPORTER_CIPEXPORTER
#define _SM_EXPORTER_CIPEXPORTER
#include "Map/MapDrawLayer.h"
#include "IO/FileExporter.h"
#include "Data/ArrayList.h"

namespace Exporter
{
	class CIPExporter : public IO::FileExporter
	{
	private:
		typedef struct
		{
			Map::MapDrawLayer *layer;
			UInt32 dispCol;
			Int32 scale;
		} CIPParam;

		typedef struct
		{
			Int32 recId;
			const UTF8Char *str;
		} CIPStrRecord;

		typedef struct
		{
			Int32 blockX;
			Int32 blockY;
			Data::ArrayList<CIPStrRecord*> *records;
		} CIPBlock;

	public:
		CIPExporter();
		virtual ~CIPExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(IO::ParsedObject *pobj);
		virtual Bool GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual Bool ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CStringNN fileName, IO::ParsedObject *pobj, void *param);

		virtual UOSInt GetParamCnt();
		virtual void *CreateParam(IO::ParsedObject *pobj);
		virtual void DeleteParam(void *param);
		virtual Bool GetParamInfo(UOSInt index, ParamInfo *info);
		virtual Bool SetParamStr(void *param, UOSInt index, const UTF8Char *val);
		virtual Bool SetParamInt32(void *param, UOSInt index, Int32 val);
		virtual Bool SetParamSel(void *param, UOSInt index, UOSInt selCol);
		virtual UTF8Char *GetParamStr(void *param, UOSInt index, UTF8Char *buff);
		virtual Int32 GetParamInt32(void *param, UOSInt index);
		virtual Int32 GetParamSel(void *param, UOSInt index);
		virtual UTF8Char *GetParamSelItems(void *param, UOSInt index, UOSInt itemIndex, UTF8Char *buff);
	};
}
#endif
