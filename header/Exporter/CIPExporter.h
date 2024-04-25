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
			NN<Map::MapDrawLayer> layer;
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
		virtual SupportType IsObjectSupported(NN<IO::ParsedObject> pobj);
		virtual Bool GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual Bool ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param);

		virtual UOSInt GetParamCnt();
		virtual Optional<ParamData> CreateParam(NN<IO::ParsedObject> pobj);
		virtual void DeleteParam(Optional<ParamData> param);
		virtual Bool GetParamInfo(UOSInt index, NN<ParamInfo> info);
		virtual Bool SetParamStr(Optional<ParamData> param, UOSInt index, const UTF8Char *val);
		virtual Bool SetParamInt32(Optional<ParamData> param, UOSInt index, Int32 val);
		virtual Bool SetParamSel(Optional<ParamData> param, UOSInt index, UOSInt selCol);
		virtual UTF8Char *GetParamStr(Optional<ParamData> param, UOSInt index, UTF8Char *buff);
		virtual Int32 GetParamInt32(Optional<ParamData> param, UOSInt index);
		virtual Int32 GetParamSel(Optional<ParamData> param, UOSInt index);
		virtual UTF8Char *GetParamSelItems(Optional<ParamData> param, UOSInt index, UOSInt itemIndex, UTF8Char *buff);
	};
}
#endif
