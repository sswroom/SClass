#ifndef _SM_EXPORTER_DBHTMLEXPORTER
#define _SM_EXPORTER_DBHTMLEXPORTER
#include "Data/ArrayListNN.hpp"
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
			UIntOS tableIndex;
			Data::ArrayListStringNN names;
		} DBParam;

	private:
		UInt32 codePage;

	public:
		DBHTMLExporter();
		virtual ~DBHTMLExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(NN<IO::ParsedObject> pobj);
		virtual Bool GetOutputName(UIntOS index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff);
		virtual void SetCodePage(UInt32 codePage);
		virtual Bool ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param);

		virtual UIntOS GetParamCnt();
		virtual Optional<ParamData> CreateParam(NN<IO::ParsedObject> pobj);
		virtual void DeleteParam(Optional<ParamData> param);
		virtual Bool GetParamInfo(UIntOS index, NN<ParamInfo> info);
		virtual Bool SetParamStr(Optional<ParamData> param, UIntOS index, UnsafeArrayOpt<const UTF8Char> val);
		virtual Bool SetParamInt32(Optional<ParamData> param, UIntOS index, Int32 val);
		virtual Bool SetParamSel(Optional<ParamData> param, UIntOS index, UIntOS selCol);
		virtual UnsafeArrayOpt<UTF8Char> GetParamStr(Optional<ParamData> param, UIntOS index, UnsafeArray<UTF8Char> buff);
		virtual Int32 GetParamInt32(Optional<ParamData> param, UIntOS index);
		virtual Int32 GetParamSel(Optional<ParamData> param, UIntOS index);
		virtual UnsafeArrayOpt<UTF8Char> GetParamSelItems(Optional<ParamData> param, UIntOS index, UIntOS itemIndex, UnsafeArray<UTF8Char> buff);
	};
}
#endif
