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
		MapLayerData(Text::CString filePath);
		virtual ~MapLayerData();

		virtual Bool IsError();
		virtual UTF8Char *GetPGLabel(UTF8Char *buff, UOSInt buffSize, Math::Coord2DDbl coord, Math::Coord2DDbl *outCoord, UOSInt strIndex);
		virtual UTF8Char *GetPLLabel(UTF8Char *buff, UOSInt buffSize, Math::Coord2DDbl coord, Math::Coord2DDbl *outCoord, UOSInt strIndex);
	};
}
#endif
