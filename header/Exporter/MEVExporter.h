#ifndef _SM_EXPORTER_MEVEXPORTER
#define _SM_EXPORTER_MEVEXPORTER
#include "Data/ArrayList.h"
#include "Data/ArrayListUInt32.h"
#include "Data/ArrayListStrUTF8.h"
#include "Data/StringUTF8Map.h"
#include "IO/FileExporter.h"
#include "Map/MapEnv.h"

namespace Exporter
{
	class MEVExporter : public IO::FileExporter
	{
	private:
		typedef struct
		{
			UInt8 *strBytes;
			UInt32 byteSize;
			Data::ArrayListUInt32 *ofstList;
		} MEVStrRecord;

	public:
		MEVExporter();
		virtual ~MEVExporter();

		virtual Int32 GetName();
		virtual SupportType IsObjectSupported(IO::ParsedObject *pobj);
		virtual Bool GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff);
		virtual Bool ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param);

	private:
		static void GetMapDirs(Map::MapEnv *env, Data::ArrayListString *dirArr, Map::MapEnv::GroupItem *group);
		static UInt32 AddString(Data::StringMap<MEVStrRecord*> *strArr, Text::String *strVal, UInt32 fileOfst);
		static UInt32 AddString(Data::StringMap<MEVStrRecord*> *strArr, const UTF8Char *strVal, UOSInt strLen, UInt32 fileOfst);
		static void WriteGroupItems(Map::MapEnv *env, Map::MapEnv::GroupItem *group, UInt32 *stmPos, IO::SeekableStream *stm, Data::StringMap<Exporter::MEVExporter::MEVStrRecord*> *strArr, Data::ArrayListString *dirArr);
	};
}
#endif
