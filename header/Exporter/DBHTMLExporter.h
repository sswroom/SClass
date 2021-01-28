#ifndef _SM_EXPORTER_DBHTMLEXPORTER
#define _SM_EXPORTER_DBHTMLEXPORTER
#include "IO/FileExporter.h"
#include "Data/ArrayListStrUTF8.h"
#include "DB/ReadingDB.h"

namespace Exporter
{
	class DBHTMLExporter : public IO::FileExporter
	{
	private:
		typedef struct
		{
			DB::ReadingDB *db;
			OSInt tableIndex;
			Data::ArrayListStrUTF8 *names;
		} DBParam;

	private:
		Int32 codePage;

	public:
		DBHTMLExporter();
		virtual ~DBHTMLExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(IO::ParsedObject *pobj);
		virtual Bool GetOutputName(OSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual void SetCodePage(Int32 codePage);
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
