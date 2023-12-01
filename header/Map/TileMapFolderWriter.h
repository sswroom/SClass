#ifndef _SM_MAP_TILEMAPFOLDERWRITER
#define _SM_MAP_TILEMAPFOLDERWRITER
#include "Map/TileMap.h"
#include "Map/TileMapWriter.h"

namespace Map
{
	class TileMapFolderWriter : public Map::TileMapWriter
	{
	private:
		NotNullPtr<Text::String> folderName;
		Map::TileMap::ImageType imgType;
		UOSInt minLev;
		UOSInt maxLev;
		Math::RectAreaDbl bounds;
		NotNullPtr<Text::String> name;
	public:
		TileMapFolderWriter(Text::CStringNN folderName, Map::TileMap::ImageType imgType, UOSInt minLev, UOSInt maxLev, Math::RectAreaDbl bounds);
		virtual ~TileMapFolderWriter();

		virtual void BeginLevel(UOSInt level);
		virtual void AddX(Int32 x);
		virtual void AddImage(UOSInt level, Int32 x, Int32 y, Data::ByteArrayR imgData, Map::TileMap::ImageType imgType);
	};
}
#endif
