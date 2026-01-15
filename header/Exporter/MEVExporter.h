#ifndef _SM_EXPORTER_MEVEXPORTER
#define _SM_EXPORTER_MEVEXPORTER
#include "Data/ArrayListUInt32.h"
#include "Data/ArrayListStrUTF8.h"
#include "Data/StringMapNN.hpp"
#include "IO/FileExporter.h"
#include "Map/MapEnv.h"

namespace Exporter
{
	class MEVExporter : public IO::FileExporter
	{
	private:
		struct MEVStrRecord
		{
			UInt8 *strBytes;
			UInt32 byteSize;
			Data::ArrayListUInt32 ofstList;
		};

	public:
		MEVExporter();
		virtual ~MEVExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(NN<IO::ParsedObject> pobj);
		virtual Bool GetOutputName(UIntOS index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff);
		virtual Bool ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param);

	private:
		static void GetMapDirs(NN<Map::MapEnv> env, Data::ArrayListString *dirArr, Optional<Map::MapEnv::GroupItem> group);
		static UInt32 AddString(NN<Data::StringMapNN<MEVStrRecord>> strArr, Optional<Text::String> strVal, UInt32 fileOfst);
		static UInt32 AddString(NN<Data::StringMapNN<MEVStrRecord>> strArr, UnsafeArray<const UTF8Char> strVal, UIntOS strLen, UInt32 fileOfst);
		static void WriteGroupItems(NN<Map::MapEnv> env, Optional<Map::MapEnv::GroupItem> group, UInt32 *stmPos, NN<IO::SeekableStream> stm, NN<Data::StringMapNN<Exporter::MEVExporter::MEVStrRecord>> strArr, Data::ArrayListString *dirArr);
	};
}
#endif
