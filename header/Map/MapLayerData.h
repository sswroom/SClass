#ifndef _SM_MAP_MAPLAYERDATA
#define _SM_MAP_MAPLAYERDATA
#include "IO/FileBuffer.h"
#include "Map/MapSearchLayer.h"
#include "Text/CString.h"

namespace Map
{
	class MapLayerData : public Map::MapSearchLayer
	{
	public:
		UnsafeArrayOpt<UInt8> cipFile;
		UnsafeArrayOpt<UInt8> cixFile;
		UnsafeArrayOpt<UInt8> ciuFile;
		UnsafeArrayOpt<UInt8> blkFile;
		NN<IO::FileBuffer> cipFileObj;
		NN<IO::FileBuffer> cixFileObj;
		NN<IO::FileBuffer> ciuFileObj;
		NN<IO::FileBuffer> blkFileObj;
	public:
		MapLayerData(Text::CStringNN filePath);
		virtual ~MapLayerData();

		virtual Bool IsError() const;
		virtual Bool GetPGLabel(NN<Text::StringBuilderUTF8> sb, Math::Coord2DDbl coord, OptOut<Math::Coord2DDbl> outCoord, UOSInt strIndex);
		virtual Bool GetPLLabel(NN<Text::StringBuilderUTF8> sb, Math::Coord2DDbl coord, OutParam<Math::Coord2DDbl> outCoord, UOSInt strIndex);
	};
}
#endif
