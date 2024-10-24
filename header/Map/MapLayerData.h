#ifndef _SM_MAP_MAPLAYERDATA
#define _SM_MAP_MAPLAYERDATA
#include "IO/IFileBuffer.h"
#include "Map/MapSearchLayer.h"
#include "Text/CString.h"

namespace Map
{
	class MapLayerData : public Map::MapSearchLayer
	{
	public:
		UInt8 *cipFile;
		UInt8 *cixFile;
		UInt8 *ciuFile;
		UInt8 *blkFile;
		IO::IFileBuffer *cipFileObj;
		IO::IFileBuffer *cixFileObj;
		IO::IFileBuffer *ciuFileObj;
		IO::IFileBuffer *blkFileObj;
	public:
		MapLayerData(Text::CStringNN filePath);
		virtual ~MapLayerData();

		virtual Bool IsError() const;
		virtual Bool GetPGLabel(NN<Text::StringBuilderUTF8> sb, Math::Coord2DDbl coord, OptOut<Math::Coord2DDbl> outCoord, UOSInt strIndex);
		virtual Bool GetPLLabel(NN<Text::StringBuilderUTF8> sb, Math::Coord2DDbl coord, OutParam<Math::Coord2DDbl> outCoord, UOSInt strIndex);
	};
}
#endif
