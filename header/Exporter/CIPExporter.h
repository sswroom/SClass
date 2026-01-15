#ifndef _SM_EXPORTER_CIPEXPORTER
#define _SM_EXPORTER_CIPEXPORTER
#include "Data/ArrayListNN.hpp"
#include "IO/FileExporter.h"
#include "Map/MapDrawLayer.h"

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
			Data::ArrayListNN<CIPStrRecord> records;
		} CIPBlock;

	public:
		CIPExporter();
		virtual ~CIPExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(NN<IO::ParsedObject> pobj);
		virtual Bool GetOutputName(UIntOS index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff);
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
