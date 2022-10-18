#ifndef _SM_MAP_HKROADNETWORK2
#define _SM_MAP_HKROADNETWORK2
#include "DB/ReadingDB.h"
#include "Map/IMapDrawLayer.h"
#include "Math/CoordinateSystem.h"
#include "Text/CString.h"

namespace Map
{
	class HKSpeedLimit;

	class HKRoadNetwork2
	{
	private:
		DB::ReadingDB *fgdb;
	public:
		HKRoadNetwork2(Text::CString fgdbPath);
		~HKRoadNetwork2();

		Bool IsError();
		DB::ReadingDB *GetDB();
		Math::CoordinateSystem *CreateCoordinateSystem();
		Map::HKSpeedLimit *CreateSpeedLimit();
		Map::IMapDrawLayer *CreateTonnesSignLayer();

		static Text::CString GetDownloadURL();
		static Text::CString GetDefFileName();
	};
}
#endif
