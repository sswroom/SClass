#ifndef _SM_EXPORTER_CIPEXPORTER
#define _SM_EXPORTER_CIPEXPORTER
#include "Map/IMapDrawLayer.h"
#include "IO/FileExporter.h"
#include "Data/ArrayList.h"

namespace Exporter
{
	class CIPExporter : public IO::FileExporter
	{
	private:
		typedef struct
		{
			Map::IMapDrawLayer *layer;
			Int32 dispCol;
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
		virtual Bool GetOutputName(OSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual Bool ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param);

		virtual OSInt GetParamCnt();
		virtual void *CreateParam(IO::ParsedObject *pobj);
		virtual void DeleteParam(void *param);
		virtual Bool GetParamInfo(OSInt index, ParamInfo *info);
		virtual Bool SetParamStr(void *param, OSInt index, const UTF8Char *val);
		virtual Bool SetParamInt32(void *param, OSInt index, Int32 val);
		virtual Bool SetParamSel(void *param, OSInt index, Int32 selCol);
		virtual UTF8Char *GetParamStr(void *param, OSInt index, UTF8Char *buff);
		virtual Int32 GetParamInt32(void *param, OSInt index);
		virtual Int32 GetParamSel(void *param, OSInt index);
		virtual UTF8Char *GetParamSelItems(void *param, OSInt index, OSInt itemIndex, UTF8Char *buff);
	};
}
#endif
