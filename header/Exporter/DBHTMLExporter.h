#ifndef _SM_EXPORTER_DBHTMLEXPORTER
#define _SM_EXPORTER_DBHTMLEXPORTER
#include "Data/ArrayListNN.h"
#include "Data/ArrayListStrUTF8.h"
#include "DB/ReadingDB.h"
#include "IO/FileExporter.h"
#include "Text/CString.h"

namespace Exporter
{
	class DBHTMLExporter : public IO::FileExporter
	{
	private:
		typedef struct
		{
			NN<DB::ReadingDB> db;
			UOSInt tableIndex;
			Data::ArrayListStringNN names;
		} DBParam;

	private:
		UInt32 codePage;

	public:
		DBHTMLExporter();
		virtual ~DBHTMLExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(NN<IO::ParsedObject> pobj);
		virtual Bool GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual void SetCodePage(UInt32 codePage);
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
