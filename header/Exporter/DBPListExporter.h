#ifndef _SM_EXPORTER_DBPLISTEXPORTER
#define _SM_EXPORTER_DBPLISTEXPORTER
#include "Data/ArrayListNN.h"
#include "Data/ArrayListStrUTF8.h"
#include "DB/ReadingDB.h"
#include "IO/FileExporter.h"
#include "Text/CString.h"

namespace Exporter
{
	class DBPListExporter : public IO::FileExporter
	{
	private:
		typedef struct
		{
			DB::ReadingDB *db;
			UOSInt tableIndex;
			Data::ArrayListNN<Text::String> *names;
		} DBParam;

	private:
		UInt32 codePage;

	public:
		DBPListExporter();
		virtual ~DBPListExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(IO::ParsedObject *pobj);
		virtual Bool GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual void SetCodePage(UInt32 codePage);
		virtual Bool ExportFile(IO::SeekableStream *stm, Text::CString fileName, IO::ParsedObject *pobj, void *param);

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
