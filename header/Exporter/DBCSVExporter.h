#ifndef _SM_EXPORTER_DBCSVEXPORTER
#define _SM_EXPORTER_DBCSVEXPORTER
#include "Data/ArrayListStringNN.h"
#include "Data/ArrayListStrUTF8.h"
#include "DB/ReadingDB.h"
#include "IO/FileExporter.h"
#include "Text/CString.h"

namespace Exporter
{
	class DBCSVExporter : public IO::FileExporter
	{
	private:
		struct DBParam
		{
			NotNullPtr<DB::ReadingDB> db;
			UOSInt tableIndex;
			Data::ArrayListStringNN names;
		};

	private:
		UInt32 codePage;

	public:
		DBCSVExporter();
		virtual ~DBCSVExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(NotNullPtr<IO::ParsedObject> pobj);
		virtual Bool GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual void SetCodePage(UInt32 codePage);
		virtual Bool ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CStringNN fileName, NotNullPtr<IO::ParsedObject> pobj, void *param);

		virtual UOSInt GetParamCnt();
		virtual void *CreateParam(NotNullPtr<IO::ParsedObject> pobj);
		virtual void DeleteParam(void *param);
		virtual Bool GetParamInfo(UOSInt index, NotNullPtr<ParamInfo> info);
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
